//
//  sarita.cpp
//  sparta_array2sh
//
//  Created by Gary Grutzek on 11.05.22.
//  Copyright © 2022 TH Köln. All rights reserved.

#include "Sarita.h"
/*
* read config data from matlab workspace dump
*/
int Sarita::readConfigFile(const char* path)
{
    FILE* configFile = fopen(path, "rb");

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

    return result;
}


/*
 *  custom window: window ramp up and down only in overlapping parts
 */
void Sarita::hannWindow(int len, int overlap)
{
    if (hannWin)
        free(hannWin);
    hannWin = (float*) malloc(len * sizeof(float));
    float *tmpWin = (float*) malloc(overlap*2 * sizeof(float));

    // ippsSet_32f & ippsWinHann_32f_I was added to custom saf ipp list!
    ippsSet_32f(1, tmpWin, overlap*2);
    ippsWinHann_32f_I(tmpWin, overlap*2);

    ippsSet_32f(1, hannWin, len);
    ippsCopy_32f(tmpWin, hannWin, overlap); // ramp up
    ippsCopy_32f(&tmpWin[overlap], &hannWin[len-overlap], overlap); // ramp down
    free(tmpWin);
}

void Sarita::deallocBuffers()
{
    if (sparseBuffer != NULL) {
        ippsFree(tmpXcorrBuffer);
        ippsFree(correlation);
        ippsFree(currentBlock);
        free(currentTimeShift);
        free(sparseBuffer);
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
    
    bufferSize = 2*blocksize; //  (2 - SARITA_OVERLAP)*blockSize;
    updateOverlap(blocksize);

    // allocate buffers
    xcorrLen = 2*blocksize-1;
    IppStatus stat;
    IppEnum funCfgNormNo = (IppEnum)(ippAlgAuto | ippsNormNone);
    stat = ippsCrossCorrNormGetBufferSize(blocksize, blocksize, xcorrLen, -blocksize-1, ipp32f, funCfgNormNo, &tmpXcorrBufferSize);
    tmpXcorrBuffer = ippsMalloc_8u(tmpXcorrBufferSize);
    correlation = ippsMalloc_32f(blocksize * 2);

    sparseBuffer = (float***)calloc3d(2, 64/*FIXME*/, blocksize, sizeof(float));
    // over sized for shifted samples
    denseBuffer = (float***)calloc3d(2, denseGridSize, blocksize+maxShiftOverall, sizeof(float));
    outputBuffer = (float***)calloc3d(2, denseGridSize, blocksize, sizeof(float));
    xcorrBuffer = (float**)calloc2d(neighborCombLength, xcorrLen, sizeof(float));
    // stores samples which are shifted out of the frame
    shiftBuffer = (float**)calloc2d(denseGridSize, maxShiftOverall, sizeof(float));
    outData = (float**)calloc2d(denseGridSize, blocksize, sizeof(float));

    input = new RingBuffer(64, bufferSize); // FIXME: matching channel num
    output = new RingBuffer(denseGridSize, bufferSize);

    currentTimeShift = (int*)malloc(idxNeighborsDenseLen * sizeof(int)); // TODO: correct size?
    currentBlock = ippsMalloc_32f(blocksize);
    
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
        input->popWithOverlap(sparseBuffer[0][ch], ch, blocksize, overlapSize); // TODO: maybe no double buffer needed here
        ippsMul_32f_I(hannWin, sparseBuffer[0][ch], blocksize);
    }

    // in each frame the cross-correlation required for the upsampling are determined
    uint8_t n1, n2;
    int maxSensors = denseGridSize;
    for (int n=0; n<neighborCombLength; n++) {
        n1 = neighborCombinations[n][0] - 1;
        n2 = neighborCombinations[n][1] - 1;
        // safety limit to max num channels. TODO: Assert?
        n1 = n1 >= maxSensors? maxSensors-1 : n1;
        n2 = n2 >= maxSensors? maxSensors-1 : n2;
        // cxcorr(processingBuffer[BufferNum][n1], processingBuffer[BufferNum][n2], xcorrBuffer[n], blocksize, blocksize); // cpu hog
        // vDSP_conv(processingBuffer[BufferNum][n1], 1, processingBuffer[BufferNum][n2], 1, xcorrBuffer[n], 1, (blocksize), (blocksize)); // cpu hog at higher block sizes
        IppEnum funCfgNormNo = (IppEnum)(ippAlgAuto | ippsNormNone);

        // ipp correlates the reverse way compared to Matlab
        ippsCrossCorrNorm_32f(sparseBuffer[0][n2], blocksize, sparseBuffer[0][n1], blocksize, xcorrBuffer[n], xcorrLen, -blocksize+1, funCfgNormNo, tmpXcorrBuffer); // performs best, switches to fft calc at higher block sizes
    }

    int neighborsIndexCounter=0; // Counter which entry in combination_ptr is to be assessed
    for (int dirIdx=0; dirIdx<denseGridSize; dirIdx++) {
        currentTimeShift[0] = 0;
        float timeShiftMean = 0;

        int numNeighbors = numNeighborsDense[dirIdx];
        for(int nodeIndex=1; nodeIndex<numNeighbors; nodeIndex++) {
            // correlation=correlationsFrame(:,combination_ptr(1,neighborsIndexCounter));
            int x = combinationsPtr[neighborsIndexCounter][0] - 1;
            ippsCopy_32f(xcorrBuffer[x], correlation, xcorrLen);

            if (combinationsPtr[neighborsIndexCounter][1] == -1) {
                ippsFlip_32f_I(correlation, xcorrLen);
            }
            neighborsIndexCounter++;

            // look for maximal value in the crosscorrelated IRs only in the relevant area
            // correlation = correlation(frame_length-maxShift(nodeIndex-1):frame_length+maxShift(nodeIndex-1));
            uint8_t maxShift = maxShiftDense[nodeIndex-1][dirIdx];
            int lowestLagIdx = blocksize-maxShift;
            int maxPos;
            int shiftLen = 2*maxShift+1; // TODO: correct?
            Ipp32f maxVal; // unused
            ippsMaxIndx_32f(&correlation[lowestLagIdx], shiftLen, &maxVal, &maxPos);
            currentTimeShift[nodeIndex] = (maxPos - (shiftLen + 1) / 2);
            timeShiftMean += currentTimeShift[nodeIndex] * weightsNeighborsDense[nodeIndex][dirIdx];
        }

        // zero dense buffer at the end
        ippsZero_32f(&denseBuffer[bufferNum][dirIdx][blocksize], maxShiftOverall);
        // copy last out-of-frame samples to denseBuffer
        ippsCopy_32f(shiftBuffer[dirIdx], denseBuffer[bufferNum][dirIdx], maxShiftOverall);
        // zero shift buffer
        ippsZero_32f(shiftBuffer[dirIdx], maxShiftOverall);

        // align every block according to the calculated time shift, weight and sum up
        for (int nodeIndex=0; nodeIndex<numNeighbors; nodeIndex++) {
            // currentBlock = neighborsIRs(nodeIndex, :) * weights(nodeIndex);
            int idx = idxNeighborsDense[nodeIndex][dirIdx]-1;
            float w = weightsNeighborsDense[nodeIndex][dirIdx];
            ippsMulC_32f(sparseBuffer[0][idx], w, currentBlock, blocksize);

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
    }
}
