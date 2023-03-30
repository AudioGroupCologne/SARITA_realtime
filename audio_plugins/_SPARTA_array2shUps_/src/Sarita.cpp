//
//  sarita.cpp
//  sparta_array2sh
//
//  Created by Gary Grutzek on 11.05.22.
//  Copyright © 2022 TH Köln. All rights reserved.

#include "Sarita.h"


//#define VDSP_CONV

#ifdef VDSP_CONV
Sarita::Sarita() : logFile("~/logConv.txt"), logger(logFile, "Loggedilog", 0)
#else
Sarita::Sarita() : logFile("~/logFFT.txt"), logger(logFile, "Loggedilog", 0)
#endif
{
    #ifdef SAF_USE_APPLE_ACCELERATE
    inputBuffer1.realp = NULL;
    inputBuffer1.imagp = NULL;
    inputBuffer2.realp = NULL;
    inputBuffer2.imagp = NULL;
    outputBuffer3.realp = NULL;
    outputBuffer3.imagp = NULL;
    #endif
}

/*
* read config data from matlab workspace dump
*/
int Sarita::readConfigFile(const char* path)
{
    FILE* configFile = fopen(path, "rb");
    
    if (!configFile)
        return -1;
    
    /* read config file header data
     * fs, order, maxShiftOverall, ...
     */
    size_t result = fread(&fs, sizeof(int), 1, configFile);
    result = fread(&N, sizeof(int), 1, configFile);
    result = fread(&NUpsampling, sizeof(int), 1, configFile);
    result = fread(&radius, sizeof(int), 1, configFile);
    result = fread(&denseGridSize, sizeof(int), 1, configFile);
    result = fread(&maxShiftOverall, sizeof(int), 1, configFile);
    result = fread(&neighborCombLength, sizeof(int), 1, configFile);
    result = fread(&idxNeighborsDenseLen, sizeof(int), 1, configFile);
    result = fread(&combinationsPtrLen, sizeof(int), 1, configFile);
    
    // poor man's plausibility check
    if (denseGridSize > ARRAY2SH_MAX_NUM_SENSORS)
        return -1;
    
    /* read config file data */
    // neighbor combinations
    neighborCombinations = (uint8_t**)calloc2d(neighborCombLength, 2, sizeof(uint8_t));
    result = readArrayUint8(configFile, neighborCombinations, neighborCombLength, 2);

    // num of neighbors dense grid
    numNeighborsDense = (uint8_t*)malloc(denseGridSize * sizeof(uint8_t));
    result = fread(numNeighborsDense, sizeof(uint8_t), denseGridSize, configFile);

    // dense grid neighbors index
    idxNeighborsDense = (uint8_t**)calloc2d(idxNeighborsDenseLen, denseGridSize, sizeof(uint8_t));
    result = readArrayUint8(configFile, idxNeighborsDense, idxNeighborsDenseLen, denseGridSize);

    // dense grid neighbors weights
    weightsNeighborsDense = (float**)calloc2d(idxNeighborsDenseLen, denseGridSize, sizeof(float));
    result = readArrayFloat(configFile, weightsNeighborsDense, idxNeighborsDenseLen, denseGridSize);

    // max shift dense
    maxShiftDense = (uint8_t**)calloc2d(idxNeighborsDenseLen-1, denseGridSize, sizeof(uint8_t));
    result = readArrayUint8(configFile, maxShiftDense, idxNeighborsDenseLen-1, denseGridSize);
//    for (int x=0; x< (idxNeighborsDenseLen-1); x++) {
//        for (int y=0; y<denseGridSize; y++) {
//            int z = maxShiftDense[x][y];
//            DBG(String(x) + " " + String(y) + " = " + String(z));
//        }
//    }

    // combination Pointer
    combinationsPtr = (int8_t**)calloc2d(combinationsPtrLen, 2, sizeof(int8_t));
    result = readArrayUint8(configFile, (uint8_t**)combinationsPtr, combinationsPtrLen, 2);

    // dense grid (Az, El, weight)
    denseGrid = (float**)calloc2d(3, denseGridSize, sizeof(float));
    result = readArrayFloat(configFile, denseGrid, 3, denseGridSize);

    logger.writeToLog("Config\n");
    logger.writeToLog("Grid: " + String(denseGridSize));
    
    return result;
}


/*
 *  custom window: window ramp up and down only in overlapping parts
 */
void Sarita::hannWindow(int len, int overlap)
{
    if (hannWin)
        free(hannWin);
    hannWin = (float*) calloc(len, sizeof(float));
    float *tmpWin = (float*) calloc((1+overlap*2), sizeof(float));

    #ifdef SAF_USE_APPLE_ACCELERATE
    vDSP_hann_window(tmpWin, overlap*2+1, 0);
    float value = 1.f;
    vDSP_vfill(&value, hannWin, 1, len);
    cblas_scopy(overlap, tmpWin, 1, hannWin, 1);
    cblas_scopy(overlap, &tmpWin[overlap+1], 1, &hannWin[len-overlap], 1);
    #else
    // ippsSet_32f & ippsWinHann_32f_I was added to custom saf ipp list!
    ippsSet_32f(1, tmpWin, overlap*2);
    ippsWinHann_32f_I(tmpWin, overlap*2);

    ippsSet_32f(1, hannWin, len);
    ippsCopy_32f(tmpWin, hannWin, overlap); // ramp up
    ippsCopy_32f(&tmpWin[overlap], &hannWin[len-overlap], overlap); // ramp down
	#endif
	
	free(tmpWin);
}

#ifdef SAF_USE_APPLE_ACCELERATE
void Sarita::setupFFT(int blocksize) 
{
    log2n = log2(blocksize);
    fftSetup = vDSP_create_fftsetup(log2n, kFFTRadix2);
    
    float *inBufReal1 = (float*)malloc(blocksize/2*sizeof(float));
    float *inBufImag1 = (float*)malloc(blocksize/2*sizeof(float));
    inputBuffer1.realp = inBufReal1;
    inputBuffer1.imagp = inBufImag1;

    float *inBufReal2 = (float*)malloc(blocksize/2*sizeof(float));
    float *inBufImag2 = (float*)malloc(blocksize/2*sizeof(float));
    inputBuffer2.realp = inBufReal2;
    inputBuffer2.imagp = inBufImag2;
    
    float *outBufReal3 = (float*)malloc(blocksize*sizeof(float));
    float *outBufImag3 = (float*)malloc(blocksize*sizeof(float));
    outputBuffer3.realp = outBufReal3;
    outputBuffer3.imagp = outBufImag3;
}

void Sarita::fftXcorr(float* buf1, float* buf2, float* xcorr, int blocksize)
{
    // pack input data to split complex array 
    vDSP_ctoz((DSPComplex *) buf1, 2, &inputBuffer1, 1, blocksize/2);
    vDSP_ctoz((DSPComplex *) buf2, 2, &inputBuffer2, 1, blocksize/2);
    
    vDSP_fft_zrip(fftSetup, &inputBuffer1, 1, log2n, FFT_FORWARD);
    vDSP_fft_zrip(fftSetup, &inputBuffer2, 1, log2n, FFT_FORWARD);

    // complex conjugate
    vDSP_zvconj(&inputBuffer1, 1, &inputBuffer1, 1, blocksize/2);
    // multiply with complex conjugate. Hence -1!
    vDSP_zvmul(&inputBuffer1, 1, &inputBuffer2, 1, &outputBuffer3, 1, blocksize/2, 1);
    // inverse fft
    vDSP_fft_zrip(fftSetup, &outputBuffer3, 1, log2n, FFT_INVERSE);
    // scale fft output
    float scale = 1.f/(2.f*blocksize);
    vDSP_vsmul(outputBuffer3.realp, 1, &scale, outputBuffer3.realp, 1, blocksize/2);
    vDSP_vsmul(outputBuffer3.imagp, 1, &scale, outputBuffer3.imagp, 1, blocksize/2);
    // unpack data to normal array
    vDSP_ztoc(&outputBuffer3, 1, (DSPComplex *) xcorr, 2, blocksize/2);
}
#endif

void Sarita::deallocBuffers()
{
    if (sparseBuffer != NULL) {
        free(sparseBuffer);
        sparseBuffer = nullptr;
        #ifdef SAF_USE_APPLE_ACCELERATE
        free(tmpBuf);
        free(correlation);
        #else
        ippsFree(tmpXcorrBuffer);
        ippsFree(correlation);
        ippsFree(currentBlock);
        #endif
        free(currentTimeShift);
        free(denseBuffer);
        free(outputBuffer);
        free(xcorrBuffer);
        free(shiftBuffer);
        free(outData);
        delete(input);
        delete(output);
    }
    
    if (numNeighborsDense != NULL)
    {
        free(numNeighborsDense);
        free(idxNeighborsDense);
        free(weightsNeighborsDense);
        free(maxShiftDense);
        free(combinationsPtr);
        free(denseGrid);
    }
    
    #ifdef SAF_USE_APPLE_ACCELERATE
    if (fftSetup)
        vDSP_destroy_fftsetup(fftSetup);
    
    if (inputBuffer1.realp) free(inputBuffer1.realp);
    if (inputBuffer1.imagp) free(inputBuffer1.imagp);
    if (inputBuffer2.realp) free(inputBuffer2.realp);
    if (inputBuffer2.imagp) free(inputBuffer2.imagp);
    if (outputBuffer3.realp) free(outputBuffer3.realp);
    if (outputBuffer3.imagp) free(outputBuffer3.imagp);
    #endif
    
}

void Sarita::setOverlap(float newOverlap)
{
    overlapPercent = newOverlap;
    overlapChanged = true;
}

void Sarita::updateOverlap(int blocksize)
{
    overlapSize = (int)(blocksize * overlapPercent * 0.01);
    hannWindow(blocksize, overlapSize);
    overlapChanged = false;
}

int Sarita::setupSarita(const char* path, int blocksize, int numInputCount)
{
    configError = true;
    
    deallocBuffers();

    if(readConfigFile(path) < 0)
        return -1;
    
    testBuffer = (float**)calloc2d(numInputCount, blocksize, sizeof(float));
    for (int ch=0; ch<numInputCount; ch++) {
        for (int i=ch; i<ch+10; i++) {
            testBuffer[ch][i] = 1.f;
        }
    }
    
    bufferSize = 2*blocksize; // (2 - SARITA_OVERLAP)*blockSize;
    updateOverlap(blocksize);

    // allocate buffers
    #ifdef SAF_USE_APPLE_ACCELERATE
    xcorrLen = blocksize;
    // fft for vDSP XCorr
    setupFFT(blocksize);
    correlation = (float*)malloc(xcorrLen * sizeof(float));
    // padded buffer for vDSP_conv
    xcorrBufferPadded = (float*)calloc((blocksize-1 + blocksize + blocksize-1), sizeof(float));
    #else
    xcorrLen = 2*blocksize-1;
    IppStatus stat;
    IppEnum funCfgNormNo = (IppEnum)(ippAlgAuto | ippsNormNone);
    stat = ippsCrossCorrNormGetBufferSize(blocksize, blocksize, xcorrLen, -(blocksize-1), ipp32f, funCfgNormNo, &tmpXcorrBufferSize);
    tmpXcorrBuffer = ippsMalloc_8u(tmpXcorrBufferSize);
    correlation = ippsMalloc_32f(blocksize * 2);
	#endif
	
    sparseBuffer = (float**)calloc2d(64 /* max input count */, blocksize, sizeof(float));
    // over sized for shifted samples
    denseBuffer = (float***)calloc3d(2, denseGridSize, blocksize+maxShiftOverall, sizeof(float));
    outputBuffer = (float***)calloc3d(2, denseGridSize, blocksize, sizeof(float));
    xcorrBuffer = (float**)calloc2d(neighborCombLength, blocksize, sizeof(float));
    // stores samples which are shifted out of the frame
    shiftBuffer = (float**)calloc2d(denseGridSize, maxShiftOverall, sizeof(float));
    outData = (float**)calloc2d(denseGridSize, blocksize, sizeof(float));

    input = new RingBuffer(numInputCount, bufferSize); // FIXME: matching channel num
    output = new RingBuffer(denseGridSize, bufferSize);

    currentTimeShift = (int*)malloc(idxNeighborsDenseLen * sizeof(int)); // TODO: correct size?
	#ifdef SAF_USE_APPLE_ACCELERATE
	currentBlock = (float*)malloc(blocksize * sizeof(float));
	tmpBuf = (float*)malloc(blocksize * sizeof(float));
	#else
    currentBlock = ippsMalloc_32f(blocksize);
	#endif
	
    configError = false;
    return 0;
}

void Sarita::updateArrayData(void* const hA2sh)
{
    int num_sensors = denseGridSize;
    /* update with the new configuration  */
    array2sh_setNumSensors(hA2sh, num_sensors);
    for (int i=0; i<num_sensors; i++) {
        array2sh_setSensorAzi_rad(hA2sh, i, denseGrid[0][i]);
        array2sh_setSensorElev_rad(hA2sh, i, denseGrid[1][i]);
        // TODO: set weights ?
    }
    
    array2sh_setr(hA2sh, radius);
    array2sh_setR(hA2sh, radius);
    array2sh_setDiffEQpastAliasing(hA2sh, 0);
    array2sh_setArrayType(hA2sh, ARRAY_SPHERICAL);
    array2sh_setWeightType(hA2sh, WEIGHT_RIGID_OMNI);
    array2sh_setc(hA2sh, 343.0);
    
//    array2sh_data *pData = (array2sh_data*)(hA2sh);
//    array2sh_arrayPars* pars = (array2sh_arrayPars*)(pData->arraySpecs);
//    pData->reinitSHTmatrixFLAG = 1;
//    array2sh_setEvalStatus(hA2sh, EVAL_STATUS_NOT_EVALUATED);
}

/*
 * process all channels of a frame
 * read from input ringbuffer and write to denseBuffer
 */
void Sarita::processFrame (int blocksize, int numInputChannels)
{
    // apply hann window
    for (int ch=0; ch<numInputChannels; ch++) {
        input->popWithOverlap(sparseBuffer[ch], ch, blocksize, overlapSize);
        cblas_scopy(blocksize, testBuffer[ch], 1, sparseBuffer[ch], 1);
		#ifdef SAF_USE_APPLE_ACCELERATE
//		vDSP_vmul(hannWin, 1, sparseBuffer[ch], 1, tmpBuf, 1, blocksize);
//		memcpy(sparseBuffer[ch], tmpBuf, blocksize * sizeof(float));
//		cblas_scopy(blocksize, tmpBuf, 1, sparseBuffer[ch], 1);
		#else
		ippsMul_32f_I(hannWin, sparseBuffer[ch], blocksize);
		#endif
//        logger.logMessage("In " + String(ch));
//        String st;
//        for (int i=0; i<64; i++) {
//            st.append(String(sparseBuffer[ch][i]), 10);
//            st.append(", ", 2);
////            logger.logMessage(String(sparseBuffer[ch][i]));
//        }
//        logger.logMessage("in" + String(ch) + " = [" + st + "];");
    }

    // in each frame the cross-correlation required for the upsampling are determined
    uint8_t n1, n2;
    int maxSensors = denseGridSize;
    for (uint32_t n=0; n<neighborCombLength; n++) {
        n1 = neighborCombinations[n][0] - 1;
        n2 = neighborCombinations[n][1] - 1;
        // safety limit to max num channels. TODO: Assert?
        n1 = n1 >= maxSensors? maxSensors-1 : n1;
        n2 = n2 >= maxSensors? maxSensors-1 : n2;
        // cxcorr(processingBuffer[BufferNum][n1], processingBuffer[BufferNum][n2], xcorrBuffer[n], blocksize, blocksize); // cpu hog
        #ifdef SAF_USE_APPLE_ACCELERATE
         #ifdef VDSP_CONV
         memset(xcorrBufferPadded, 0, (blocksize-1 + blocksize + blocksize-1)*sizeof(float));
         // copy to padded buffer
         cblas_scopy(blocksize, sparseBuffer[n1], 1, &xcorrBufferPadded[blocksize-1], 1);
         // correlate with padded buffer
         vDSP_conv(xcorrBufferPadded, 1, sparseBuffer[n2], 1, xcorrBuffer[n], 1, (xcorrLen), (blocksize)); // cpu hog at higher block sizes
         #else
         // vDSP_vclr(xcorrBuffer[n], 1, xcorrLen);
        fftXcorr(sparseBuffer[n2], sparseBuffer[n1], xcorrBuffer[n], blocksize); // M1: 0.1% vs 0.9% CPU (Debug)
         #endif
        #else
        IppEnum funCfgNormNo = (IppEnum)(ippAlgAuto | ippsNormNone);
        // ipp correlates the reverse way compared to Matlab
        ippsCrossCorrNorm_32f(sparseBuffer[n2], blocksize, sparseBuffer[n1], blocksize, xcorrBuffer[n], xcorrLen, -blocksize+1, funCfgNormNo, tmpXcorrBuffer); // performs best, switches to fft calc at higher block sizes
        #endif
        
        logger.logMessage("N " + String(n1) + " & " + String(n2));

//        String str;
//        for (int i=0; i<2*blocksize; i++) {
//            int f = round(tmpBuf2[i]);
//            str.append(String(f), 20);
//            str.append(", ", 2);
//        }
//        logger.logMessage("xc-tmp " + String(n-1) + " = [" + str + "];");
        
        String st;
        for (int i=0; i<blocksize; i++) {
            int f = round(xcorrBuffer[n][i]);
            st.append(String(f), 20);
            st.append(", ", 2);
        }
        logger.logMessage("xc" + String(n) + " = [" + st + "];");
    }
    
// logger.logMessage("FRAME " + String((float)sparseBuffer[0][0]));
    int neighborsIndexCounter=0; // Counter which entry in combination_ptr is to be assessed
    for (uint32_t dirIdx=0; dirIdx<denseGridSize; dirIdx++) {
        currentTimeShift[0] = 0;
        float timeShiftMean = 0;
//        logger.logMessage("DIR IDX: " + String(dirIdx));
        int numNeighbors = numNeighborsDense[dirIdx];
        for(int nodeIndex=1; nodeIndex<numNeighbors; nodeIndex++) {
            // correlation=correlationsFrame(:,combination_ptr(1,neighborsIndexCounter));
            int x = combinationsPtr[neighborsIndexCounter][0] - 1;
            #ifdef SAF_USE_APPLE_ACCELERATE
            #ifdef VDSP_CONV
                cblas_scopy(xcorrLen, xcorrBuffer[x], 1, correlation, 1);
            #else
            // manual fft shift
            vDSP_vclr(correlation, 1, blocksize);
            cblas_scopy(blocksize/2, &xcorrBuffer[x][blocksize/2], 1, &correlation[0], 1);
            cblas_scopy(blocksize/2, &xcorrBuffer[x][0], 1, &correlation[blocksize/2], 1);
//            cblas_scopy(2*blocksize, xcorrBuffer[x], 1, correlation, 1);
            #endif

            
            #else
            ippsCopy_32f(xcorrBuffer[x], correlation, xcorrLen);
            #endif
            int reverse = 0;
            if (combinationsPtr[neighborsIndexCounter][1] == -1) {
                #ifdef SAF_USE_APPLE_ACCELERATE
                #ifdef VDSP_CONV
                vDSP_vrvrs(correlation, 1, xcorrLen); // reverse vector
                #else
                vDSP_vrvrs(correlation, 1, blocksize); // reverse vector
                reverse = 1;
                #endif
                #else
                ippsFlip_32f_I(correlation, xcorrLen);
                #endif
            }
            neighborsIndexCounter++;
            
            String st;
            for (int i=0; i<blocksize; i++) {
                int f = round(correlation[i]);
                st.append(String(f), 5);
                st.append(", ", 2);
            }
            logger.logMessage("dir " + String(dirIdx) + " Node " + String(nodeIndex) + " xc" + String(x) + " = [" + st + "];");

            // look for maximal value in the crosscorrelated IRs only in the relevant area
            // correlation = correlation(frame_length-maxShift(nodeIndex-1):frame_length+maxShift(nodeIndex-1));
            uint8_t maxShift = maxShiftDense[nodeIndex-1][dirIdx];
            int lowestLagIdx = blocksize-maxShift-1;
            int maxPos;
            int shiftLen = 2*maxShift+1; // TODO: correct?
            #ifdef SAF_USE_APPLE_ACCELERATE
            float maxVal;
            vDSP_Length pos;
            #ifdef VDSP_CONV
            vDSP_maxvi(&correlation[lowestLagIdx], 1, &maxVal, &pos, shiftLen);
            maxPos = (int) pos;
            currentTimeShift[nodeIndex] = (1 + maxPos - (shiftLen + 1) / 2); // conv
            #else
            lowestLagIdx = blocksize/2-maxShift-1;
//            vDSP_maxvi(correlation, 1, &maxVal, &pos, blocksize);
            vDSP_maxvi(&correlation[lowestLagIdx], 1, &maxVal, &pos, shiftLen);
            maxPos = (int) pos;
//            currentTimeShift[nodeIndex] = ((int)(reverse + maxPos - blocksize/2)); // fft
            currentTimeShift[nodeIndex] = (int)(reverse + maxPos - (shiftLen + 1) / 2); // fft
            #endif
            #else
            Ipp32f maxVal; // unused
            ippsMaxIndx_32f(&correlation[lowestLagIdx], shiftLen, &maxVal, &maxPos);
            currentTimeShift[nodeIndex] = (1 + maxPos - (shiftLen + 1) / 2);
            #endif
            logger.logMessage("x" + String(x) + "d" + String(dirIdx) + "n" + String(nodeIndex) +  "r" + String(reverse) + " Shift: " + String(currentTimeShift[nodeIndex]) + " MaxPos: " + String(maxPos) + " MaxVal: " + String(maxVal));
            timeShiftMean += currentTimeShift[nodeIndex] * weightsNeighborsDense[nodeIndex][dirIdx];
        }
        
        // memzero fixes crackle
        memset(denseBuffer[bufferNum][dirIdx], 0, overlapSize);
        
        #ifdef SAF_USE_APPLE_ACCELERATE
        // zero dense buffer at the end
        vDSP_vclr(&denseBuffer[bufferNum][dirIdx][blocksize-maxShiftOverall], 1, maxShiftOverall);
        // copy last out-of-frame samples to denseBuffer
        cblas_scopy(maxShiftOverall, shiftBuffer[dirIdx], 1, denseBuffer[bufferNum][dirIdx], 1);
        // zero shift buffer
        vDSP_vclr(shiftBuffer[dirIdx], 1, maxShiftOverall);
        // align every block according to the calculated time shift, weight and sum up
        for (int nodeIndex=0; nodeIndex<numNeighbors; nodeIndex++) {
            // currentBlock = neighborsIRs(nodeIndex, :) * weights(nodeIndex);
            int idx = idxNeighborsDense[nodeIndex][dirIdx]-1;
            const float w = weightsNeighborsDense[nodeIndex][dirIdx];
            vDSP_vsmul(sparseBuffer[idx], 1, &w, currentBlock, 1, blocksize);

            int timeShiftFinal = round(-timeShiftMean + currentTimeShift[nodeIndex] + maxShiftOverall); // As maxShiftOverall is added, timeShiftFinal will always be positive
            timeShiftFinal = (timeShiftFinal < 0) ? 0: timeShiftFinal; // Added 22.12.2021 to assure that timeShiftFinal does not become negative

            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) = ...
            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) + currentBlock;
            if (nodeIndex == 0) {
                cblas_scopy(blocksize, currentBlock, 1, &denseBuffer[bufferNum][dirIdx][timeShiftFinal], 1);
            }
            else {
                // vDSP_vadd(currentBlock, 1, &denseBuffer[bufferNum][dirIdx][timeShiftFinal] , 1, tmpBuf , 1, blocksize);
                // cblas_scopy(blocksize, tmpBuf, 1, &denseBuffer[bufferNum][dirIdx][timeShiftFinal], 1);
                cblas_saxpy(blocksize, 1.f, currentBlock, 1, &denseBuffer[bufferNum][dirIdx][timeShiftFinal], 1);
            }
        }

        // save out-of-frame samples to shift buffer
        cblas_scopy(maxShiftOverall, &denseBuffer[bufferNum][dirIdx][blocksize], 1, shiftBuffer[dirIdx], 1);
        #else
        // zero dense buffer at the end
        ippsZero_32f(&denseBuffer[bufferNum][dirIdx][blocksize-maxShiftOverall], maxShiftOverall);
        // copy last out-of-frame samples to denseBuffer
        ippsCopy_32f(shiftBuffer[dirIdx], denseBuffer[bufferNum][dirIdx], maxShiftOverall);
        // zero shift buffer
        ippsZero_32f(shiftBuffer[dirIdx], maxShiftOverall);

        // align every block according to the calculated time shift, weight and sum up
        for (int nodeIndex=0; nodeIndex<numNeighbors; nodeIndex++) {
            // currentBlock = neighborsIRs(nodeIndex, :) * weights(nodeIndex);
            int idx = idxNeighborsDense[nodeIndex][dirIdx]-1;
            float w = weightsNeighborsDense[nodeIndex][dirIdx];
            ippsMulC_32f(sparseBuffer[idx], w, currentBlock, blocksize);

            int timeShiftFinal = round(-timeShiftMean + currentTimeShift[nodeIndex] + maxShiftOverall); // As maxShiftOverall is added, timeShiftFinal will always be positive
            timeShiftFinal = (timeShiftFinal < 0) ? 0: timeShiftFinal; // Added 22.12.2021 to assure that timeShiftFinal does not become negative

            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) = ...
            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) + currentBlock;
            if (nodeIndex == 0) {
                ippsCopy_32f(currentBlock, &denseBuffer[bufferNum][dirIdx][timeShiftFinal], blocksize);
            }
            else
                ippsAdd_32f_I(currentBlock, &denseBuffer[bufferNum][dirIdx][timeShiftFinal], blocksize);
        }

        // save out-of-frame samples to shift buffer
        ippsCopy_32f(&denseBuffer[bufferNum][dirIdx][blocksize], shiftBuffer[dirIdx], maxShiftOverall);
		#endif
    }
}

