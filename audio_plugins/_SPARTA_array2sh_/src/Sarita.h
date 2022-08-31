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
#include "/Applications/MATLAB_R2015a.app/extern/include/mat.h"            /* read matlab files */

//#define SARITA_FRAMESIZE 4096
#define SARITA_OVERLAP 0.25 // 50% = 2048, 25% = 1024 @ 4096 Framesize
#define SARITA_DENSESAMPLINGPOINTS 64
#define NUM_NEXTneighbor 4

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


/**
 * TODO: Main structure for SARITA. Contains variables for audio buffers, ...
 */
//typedef struct _sarita_data
//{
//    /* audio buffers */
//    float** inputBuffer;           /**< Input sensor signals in the time-domain; #MAX_NUM_SENSORS x #ARRAY2SH_FRAME_SIZE */
//    float** processingBuffer;              /**< Output SH signals in the time-domain; #MAX_NUM_SH_SIGNALS x #ARRAY2SH_FRAME_SIZE */
//    float** outputBuffer;
////    float_complex*** inputframeTF;  /**< Input sensor signals in the time-domain; #HYBRID_BANDS x #MAX_NUM_SENSORS x #TIME_SLOTS */
////    void* arraySpecs;               /**< array configuration */
////    int fs;                         /**< sampling rate, hz */
//} sarita_data;

RingBuffer *input;
RingBuffer *output;
float*** processingBuffer;
float*** tmpBuffer;
float*** outputBuffer;

float** xcorrBuffer;

int saritaBufferSize = 0;
int saritaFrame = 0; // double buffer 0/1
int outputGridLen = 64;

Ipp32f *hannWin;

// config data
struct SaritaConfig {
    // header
    int denseGridSize;
    int maxShiftOverall;
    int neighborCombLength;    // uint8_t neighborSizeY; always 2?
    int idxNeighborsDenseLen;

    // data
    uint8_t** neighborCombinations;
    // uint8_t** maxShiftDense;
    uint8_t** idxNeighborsDense;
} cfg;


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

inline void sarita_neigbours()
{
    ;
}

inline AudioSampleBuffer* sarita_upsampling(AudioSampleBuffer& buffer)
{
    ;
}


int setupSarita(int blockSize)
{
    saritaBufferSize = 2*blockSize; //  (2 - SARITA_OVERLAP)*blockSize;
    saritaWin(blockSize, blockSize*SARITA_OVERLAP);
    
    char path[256] = { "/Users/gary/Documents/GitHub/SARITA_realtime/config.dat" };
    FILE* configFile = fopen(path, "rb");
    // read config file header data
    // dense gridSize, maxShiftOverall, neighborCombLength, idxNeighborsDenseLen
    size_t result = fread(&cfg, sizeof(int), 4, configFile);
    
    // neighbor combinations
    uint8_t* tmp = (uint8_t*) malloc(cfg.neighborCombLength * 2 * sizeof(uint8_t));
    cfg.neighborCombinations = (uint8_t**)calloc2d(cfg.neighborCombLength, 2, sizeof(uint8_t));
    result = fread(tmp, sizeof(uint8_t), cfg.neighborCombLength * 2, configFile);
    // copy with stride 2
    for (int y=0; y<2; y++) {
        for (int x=0; x<cfg.neighborCombLength; x++) {
            cfg.neighborCombinations[x][y] = tmp[x*2 + y];
        }
    }
    free(tmp);
        
    // dense grid neighbors index
    tmp = (uint8_t*) malloc(cfg.idxNeighborsDenseLen * cfg.denseGridSize * sizeof(uint8_t));
    cfg.idxNeighborsDense = (uint8_t**)calloc2d(cfg.denseGridSize, cfg.idxNeighborsDenseLen, sizeof(uint8_t));
    result = fread(tmp, sizeof(uint8_t), cfg.denseGridSize * cfg.idxNeighborsDenseLen, configFile);
    // copy with stride 2
    for (int x=0; x<cfg.idxNeighborsDenseLen; x++) {
        for (int y=0; y<cfg.denseGridSize; y++) {
            cfg.idxNeighborsDense[x][y] = tmp[x*cfg.denseGridSize + y];
//            int z = cfg.idxNeighborsDense[x][y];
//            DBG(String(x) + " " + String(y) + " = " + String(z));
        }
    }
    free(tmp);
    
    // allocate buffers   // TODO: dense channels
    processingBuffer = (float***)calloc3d(2, 64, blockSize, sizeof(float));
    tmpBuffer = (float***)calloc3d(2, 64, blockSize*SARITA_OVERLAP, sizeof(float));
    outputBuffer = (float***)calloc3d(2, 64, blockSize, sizeof(float));
    xcorrBuffer = (float**)calloc2d(cfg.neighborCombLength, blockSize + cfg.maxShiftOverall, sizeof(float));
    
    input = new RingBuffer(2, saritaBufferSize);
    output = new RingBuffer(2, saritaBufferSize);
    
    return result;
}
#endif /* sarita_h */
