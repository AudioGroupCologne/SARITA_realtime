//
//  sarita.h
//  sparta_array2sh
//
//  Created by Gary Grutzek on 11.05.22.
//  Copyright © 2022 TH Köln. All rights reserved.
/* Pseudocode for the SARITA algorithm. L total number of frames
 
 // loop over dense sampling points
 for q = 1:Q􏰉 do
    Calculate next neighbors ΩN and weights wN
    Calculate maximum time shift ∆tgeom based on grid geometry
 end for
 
 for f = 1:L do // loop over all frames
    for q = 1:Q􏰉 do  // loop over dense sampling points
        for qN = 1:ΩN do // loop over next neighbors
            determine time shift ∆tN by cross-correlation with restriction: ∆tN < ∆tgeom
            calculate weighted mean time shift ∆tN = ∆tN wN
        end for
        
        for qN = 0:ΩN do // loop over next neighbors
            align Bn with ∆tN
            sum Bn weighted with wN
        end for
        apply time shift with ∆tN
    end for
 end for
*/

#ifndef sarita_h
#define sarita_h

#include "ipp.h"
#include "saf.h"           /* Main include header for SAF */

//#define SARITA_FRAMESIZE 4096
#define SARITA_OVERLAP 0.25 // 50% = 2048, 25% = 1024 @ 4096 Framesize

//#define TESTDATA
int testcount = 0;
std::unique_ptr<FileLogger> flogger;

/*
* a multi channel vector ring buffer
*/
class RingBuffer
{
private:
    float** data;
    int readIdx, writeIdx;
    int size;
    int channels;
    
public:
    int getReadIdx() { return readIdx; }
    int getWriteIdx() { return writeIdx; }
    int bufferedBytes;
    
    int capacity() {
        return size - bufferedBytes;
    }
    
    bool empty() {
        return bufferedBytes == 0; // writeIdx == readIdx;
    }
    
    bool full() {
        return bufferedBytes >= size;
    }
    
    
    RingBuffer(int channels, int bufferSize)
    {
        size = bufferSize;
        this->channels = channels;
        readIdx = writeIdx = 0;
        bufferedBytes = 0;
        data = (float**)malloc2d(channels, bufferSize, sizeof(float));
    }
    
    ~RingBuffer()
    {
        free(data);
    }
    
    void push(const float* input, int len, int ch)
    {
         assert(len <= capacity());
         assert(!full());
            if (size - writeIdx >= len) { // copy buffer to ringbuffer without wrap around
                utility_svvcopy(input, len, &data[ch][writeIdx]);
            }
            else { // copy needs wrap around
                auto numSamplesToEnd = size - writeIdx;
                auto numSamplesAtStart = len - numSamplesToEnd;
                utility_svvcopy(&input[0], numSamplesToEnd, &data[ch][writeIdx]);
                utility_svvcopy(&input[numSamplesToEnd], numSamplesAtStart, &data[ch][0]);
            }
            if (ch == (channels-1)) { // won't work when not pushing for all channels
                bufferedBytes += len;
                writeIdx += len;
                writeIdx %= size;
            }
    }
    
    void pop(float* output, int ch, int len)
    {
        assert(len <= bufferedBytes);
        assert(!empty());
        
        if (size - readIdx >= len) // no wrap around
            utility_svvcopy(&data[ch][readIdx], len, output);
        else { // copy needs wrap around
            auto numSamplesFromTail = size - readIdx;
            auto numSamplesFromStart = len - numSamplesFromTail;
            utility_svvcopy(&data[ch][readIdx], numSamplesFromTail, output);
            utility_svvcopy(&data[ch][0], numSamplesFromStart, &output[numSamplesFromTail]);
        }
        if (ch == (channels-1)) { // won't work when not reading all channels
            bufferedBytes -= len;
            readIdx += len;
            readIdx %= size;
        }
    }
    
    void popWithOverlap(float* output, int ch, int len, int overlap)
    {
        assert(len <= bufferedBytes);
        assert(!empty());
        if (size - readIdx >= len) // no wrap around
            utility_svvcopy(&data[ch][readIdx], len, output);
        else { // copy needs wrap around
            auto numSamplesFromTail = size - readIdx;
            auto numSamplesFromStart = len - numSamplesFromTail;
            utility_svvcopy(&data[ch][readIdx], numSamplesFromTail, output);
            utility_svvcopy(&data[ch][0], numSamplesFromStart, &output[numSamplesFromTail]);
        }
        if (ch == (channels-1)) { // won't work when not reading all channels
            bufferedBytes -= (len-overlap);
            readIdx += (len-overlap);
            readIdx %= size;
        }
    }
};


RingBuffer *input;
RingBuffer *output;
float*** sparseBuffer; // audio of source grid
float*** denseBuffer; // audio of upsampled target grid
float*** outputBuffer;

int saritaBufferSize = 0;
int saritaOverlapSize;
int BufferNum = 0; // double buffer 0/1
int outputGridLen = 64;

// cross corretlation buffers
int xcorrLen;
int tmpXcorrBufferSize;
Ipp8u* tmpXcorrBuffer;
Ipp32f* correlation;
float** xcorrBuffer;
Ipp32f *hannWin;

int* currentTimeShift;
Ipp32f* currentBlock;

bool configError = true;

// config data
struct SaritaConfig {
    // header
//    int N;                 // order of source grid
//    int NUpsampling;       // order of target grid
//    int NRendering;        // rendering order
    int fs;
    int overlapInv;         // 1/overlap in %   e.g. 4096/128 => 32
    int denseGridSize;
    int maxShiftOverall;
    int neighborCombLength;  // neighborCombinations size = neighborCombLength * 2
    int idxNeighborsDenseLen; // idxNeighborsDense array size = idxNeighborsDenseLen * dense grid size
    int combinationsPtrLen;  // length of combinations pointer, y is always 2

    // data
    uint8_t** neighborCombinations; // Array containing all combinations of nearest neighbors
    uint8_t* numNeighborsDense;     // Number of nearest neighbors for each sampling point
    uint8_t** idxNeighborsDense;    // Indices of neighbors of each sampling point
    float** weightsNeighborsDense;  // Weights of neighbors of each sampling point
    uint8_t** maxShiftDense;
    int8_t** combinationsPtr;       // Array describing which neighbors combination is required for each cross correlations
} cfg;

/*
 *  custom window: window ramp up and down only in overlapping parts
 */
inline void saritaWin(int len, int overlap)
{
    hannWin = (float*) malloc(len * sizeof(float));
    float *tmpWin = (float*) malloc(overlap*2 * sizeof(float));

    // ippsSet_32f & ippsWinHann_32f_I was added to custom saf ipp list!
    ippsSet_32f(1, tmpWin, overlap*2);
    ippsWinHann_32f_I(tmpWin, overlap*2);
    
    ippsSet_32f(1, hannWin, len);
    ippsCopy_32f(tmpWin, hannWin, overlap); // ramp up
    ippsCopy_32f(&tmpWin[overlap], &hannWin[len-overlap], overlap); // ramp down
}

/*
* binary file read helpers
*/
int readArrayUint8(FILE *fid, uint8_t** dst, int sizeX, int sizeY)
{
    uint8_t* tmp = (uint8_t*) malloc(sizeX * sizeY * sizeof(uint8_t));
    int result = fread(tmp, sizeof(uint8_t), sizeX * sizeY, fid);
    // copy with stride
    for (int x=0; x<sizeX; x++) {
        for (int y=0; y<sizeY; y++) {
            dst[x][y] = tmp[x*sizeY + y];
//            int z = dst[x][y];
//            DBG(String(x) + " " + String(y) + " = " + String(z));
        }
    }
    free(tmp);
    return result;
}


int readArrayFloat(FILE *fid, float** dst, int sizeX, int sizeY)
{
    float* tmp = (float*) malloc(sizeX * sizeY * sizeof(float));
    int result = fread(tmp, sizeof(float), sizeX * sizeY, fid);
    // copy with stride
    for (int x=0; x<sizeX; x++) {
        for (int y=0; y<sizeY; y++) {
            dst[x][y] = tmp[x*sizeY + y];
//            float z = dst[x][y];
//            DBG(String(x) + " " + String(y) + " = " + String(z));
        }
    }
    free(tmp);
    return result;
}


/*
* read config data from matlab workspace dump
*/
inline int saritaReadConfigFile(const char* path)
{
    FILE* configFile = fopen(path, "rb");

    /* read config file header data
     * fs, overlap, dense gridSize, maxShiftOverall,
     * neighborCombLength, idxNeighborsDenseLen, combinationPtrLen
     */
    size_t result = fread(&cfg, sizeof(int), 7, configFile);

    /* read config file data */
    // neighbor combinations
    cfg.neighborCombinations = (uint8_t**)calloc2d(cfg.neighborCombLength, 2, sizeof(uint8_t));
    result = readArrayUint8(configFile, cfg.neighborCombinations, cfg.neighborCombLength, 2);

    // num of neighbors dense grid
    cfg.numNeighborsDense = (uint8_t*)malloc(cfg.denseGridSize * sizeof(uint8_t));
    result = fread(cfg.numNeighborsDense, sizeof(uint8_t), cfg.denseGridSize, configFile);

    // dense grid neighbors index
    cfg.idxNeighborsDense = (uint8_t**)calloc2d(cfg.denseGridSize, cfg.idxNeighborsDenseLen, sizeof(uint8_t));
    result = readArrayUint8(configFile, cfg.idxNeighborsDense, cfg.idxNeighborsDenseLen, cfg.denseGridSize);

    // dense grid neighbors weights
    cfg.weightsNeighborsDense = (float**)calloc2d(cfg.denseGridSize, cfg.idxNeighborsDenseLen, sizeof(float));
    result = readArrayFloat(configFile, cfg.weightsNeighborsDense, cfg.idxNeighborsDenseLen, cfg.denseGridSize);

    // max shift dense
    cfg.maxShiftDense = (uint8_t**)calloc2d(cfg.denseGridSize, cfg.idxNeighborsDenseLen-1, sizeof(uint8_t));
    result = readArrayUint8(configFile, cfg.maxShiftDense, cfg.idxNeighborsDenseLen-1, cfg.denseGridSize);
    
    // combination Pointer
    cfg.combinationsPtr = (int8_t**)calloc2d(cfg.combinationsPtrLen, 2, sizeof(int8_t));
    result = readArrayUint8(configFile, (uint8_t**)cfg.combinationsPtr, cfg.combinationsPtrLen, 2);

    return result;
}


int setupSarita(int blocksize, int numInputCount, int numOutputCount)
{
    configError = true;
    
    // FIXME: move to better location. currently "~/Documents/GitHub/config.dat" };
    juce::File f = juce::File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("GitHub").getChildFile("SaritaConfig.dat");
    if (!f.existsAsFile())
        return -1;
    
    const char *p = f.getFullPathName().getCharPointer();
    if(saritaReadConfigFile(p) < 0)
        return -1;
    
    saritaBufferSize = 2*blocksize; //  (2 - SARITA_OVERLAP)*blockSize;
    saritaOverlapSize = SARITA_OVERLAP*blocksize;
    saritaWin(blocksize, saritaOverlapSize);
    
    // allocate buffers
    xcorrLen = 2*blocksize-1;
    IppStatus stat;
    stat = ippsCrossCorrNormGetBufferSize(blocksize, blocksize, xcorrLen, 0, ipp32f, ippAlgAuto, &tmpXcorrBufferSize);
    tmpXcorrBuffer = ippsMalloc_8u(tmpXcorrBufferSize);
    correlation = ippsMalloc_32f(blocksize * 2);
    
    sparseBuffer = (float***)calloc3d(2, 64, blocksize, sizeof(float));
    denseBuffer = (float***)calloc3d(2, cfg.denseGridSize, blocksize, sizeof(float));
    outputBuffer = (float***)calloc3d(2, cfg.denseGridSize, blocksize, sizeof(float));
    xcorrBuffer = (float**)calloc2d(cfg.neighborCombLength, xcorrLen, sizeof(float));
    
    input = new RingBuffer(numInputCount, saritaBufferSize);
    output = new RingBuffer(numOutputCount, saritaBufferSize);

    currentTimeShift = (int*)malloc(cfg.idxNeighborsDenseLen * sizeof(int)); // TODO: correct size?
    currentBlock = ippsMalloc_32f(blocksize);
    
    configError = false;
    return 0;
}

/*
 * process all channels of a frame
 * read from input ringbuffer and write to denseBuffer
 */
void PluginProcessor::processFrame (int blocksize, int numInputChannels)
{
    // apply hann window
    for (int ch=0; ch<numInputChannels; ch++) {
        input->popWithOverlap(sparseBuffer[BufferNum][ch], ch, blocksize, saritaOverlapSize); // TODO: maybe no double buffer needed here
        ippsMul_32f_I(hannWin, sparseBuffer[BufferNum][ch], blocksize);
    }
    
    // in each frame the cross-correlation required for the upsampling are determined
    uint8_t n1, n2;
    for (int n=0; n<cfg.neighborCombLength; n++) {
        n1 = 0; //cfg.neighborCombinations[n][0];
        n2 = 1; //cfg.neighborCombinations[n][1];
        // cxcorr(processingBuffer[BufferNum][n1], processingBuffer[BufferNum][n2], xcorrBuffer[n], blocksize, blocksize); // cpu hog
        // vDSP_conv(processingBuffer[BufferNum][n1], 1, processingBuffer[BufferNum][n2], 1, xcorrBuffer[n], 1, (blocksize), (blocksize)); // cpu hog at higher block sizes
        ippsCrossCorrNorm_32f(sparseBuffer[BufferNum][n1], blocksize, sparseBuffer[BufferNum][n2], blocksize, xcorrBuffer[n], xcorrLen, 0, ippAlgAuto, tmpXcorrBuffer); // performs best, switches to fft calc at higher block sizes
        // TODO: norm ok? max lag? len of xcorr = 2*blocksize-1?
    }

    int neighborsIndexCounter=0; // Counter which entry in combination_ptr is to be assessed
    for (int dirIdx=0; dirIdx<cfg.denseGridSize; dirIdx++) {
        memset(currentTimeShift, 0, cfg.idxNeighborsDenseLen);
        float timeShiftMean = 0;
        // Get nearst neighbors, weights and maxShift for actual direction, can
        // later be directly addressed in following lines if desired
        // neighborsIndex = idx_neighbors_dense_grid(dirIndex,1:num_neighbors_dense_grid(dirIndex));
        // uint8_t *neighborsIdx = cfg.idxNeighborsDense[dirIdx]; // pointer to e.g. 6 neighbor indices
        // weights = weights_neighbors_dense_grid(dirIndex,1:num_neighbors_dense_grid(dirIndex));
        // float *weights = cfg.weightsNeighborsDense[dirIdx]; // pointer to weights
        // maxShift = maxShift_dense(dirIndex,1:num_neighbors_dense_grid(dirIndex)-1);
        uint8_t *maxShift = cfg.maxShiftDense[dirIdx]; // pointer to shifts
        
        for(int nodeIndex=1; nodeIndex<cfg.idxNeighborsDenseLen; nodeIndex++) {
            neighborsIndexCounter++;
            // correlation=correlationsFrame(:,combination_ptr(1,neighborsIndexCounter));
            ippsCopy_32f(xcorrBuffer[cfg.combinationsPtr[0][neighborsIndexCounter]], correlation, xcorrLen);
            
            if (cfg.combinationsPtr[1][neighborsIndexCounter] == -1) {
                ippsFlip_32f_I(correlation, xcorrLen); // TODO: length ok?  why reverse vector?
            }
            // look for maximal value in the crosscorrelated IRs only in the relevant area
            // correlation = correlation(frame_length-maxShift(nodeIndex-1):frame_length+maxShift(nodeIndex-1));
            int lagIdx = blocksize-maxShift[nodeIndex-1]; // TODO: why nodeindex-1?
            int maxPos;
            int corrLen = 2*maxShift[nodeIndex-1]+1; // TODO: correct?
            Ipp32f maxVal; // unused
            ippsMaxIndx_32f(&correlation[lagIdx], corrLen, &maxVal, &maxPos);
            currentTimeShift[nodeIndex] = (maxPos - (corrLen + 1) / 2);
            timeShiftMean += currentTimeShift[nodeIndex] * cfg.weightsNeighborsDense[dirIdx][nodeIndex];
        }
        
        // TODO: neighborsIRs = irsFrame(neighborsIndex,:); % get all next neighbor irs of one frame and perform windowing
        // TODO: for each neighborIR
        // align every block according to the calculated time shift, weight and sum up
        memset(denseBuffer[BufferNum][dirIdx], 0, blocksize);
        for (int nodeIndex=0; nodeIndex<cfg.numNeighborsDense[dirIdx]; nodeIndex++) {
            // currentBlock = neighborsIRs(nodeIndex, :) * weights(nodeIndex);
            int idx = cfg.idxNeighborsDense[dirIdx][nodeIndex];
            ippsMulC_32f(sparseBuffer[BufferNum][idx], cfg.weightsNeighborsDense[dirIdx][nodeIndex], currentBlock, blocksize);
            int timeShiftFinal = round(-timeShiftMean + currentTimeShift[nodeIndex] + cfg.maxShiftOverall); // As maxShiftOverall is added, timeShiftFinal will always be positive
            timeShiftFinal = (timeShiftFinal < 0) ? 0: timeShiftFinal; // Added 22.12.2021 to assure that timeShiftFinal does not become negative
            
            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) = ...
            //drirs_upsampled(dirIndex, startTab + timeShiftFinal:endTab + timeShiftFinal) + currentBlock;
            ippsAdd_32f_I(currentBlock,  &denseBuffer[BufferNum][dirIdx][timeShiftFinal], blocksize-timeShiftFinal);
        }
    }
    
}


#endif /* sarita_h */
