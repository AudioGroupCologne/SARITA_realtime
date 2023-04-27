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

#ifdef SAF_USE_APPLE_ACCELERATE  // defined in projucer file
	#include <Accelerate/Accelerate.h>
	#define FLOATTYPE float
	#define BYTETYPE uint8_t
#else
	#include "ipp.h"
	#define FLOATTYPE Ipp32f
	#define BYTETYPE Ipp8u
#endif
//
#include "saf.h"           /* Main include header for SAF */
#include "array2sh.h"
#include "../src/array2sh/array2sh_internal.h"
#include <JuceHeader.h>

#define TEST_AUDIO_OUTPUT // Don't calc spherical harmonics, write SARITA-upsampled channels to output buffers
//#define VDSP_CONV     // prefer vDSP_conv() over fft-based correlation when SAF_USE_ACCELERATE is defined

static std::unique_ptr<juce::FileLogger> flogger;

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
    
    int numChannels() {
        return channels;
    }
    
    bool empty() {
        return bufferedBytes == 0; // writeIdx == readIdx;
    }
    
    bool full() {
        return bufferedBytes >= size;
    }
    
    void reset() {
        readIdx = writeIdx = 0;
        bufferedBytes = 0;
    }
    
    void skipPop(int len) {
        bufferedBytes -= len;
        readIdx += len;
        readIdx %= size;
    }
    
    void skipPush(int len) {
        bufferedBytes += len;
        writeIdx += len;
        writeIdx %= size;
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
        free(this->data);
    }
    
    void push(const float* input, int len, int ch)
    {
//         assert(len <= capacity());
         assert(!full());
         assert(len > 0);
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


class Sarita
{
    
public:
    Sarita();
   // Sarita::~Sarita() { deallocBuffers(); }
    void processFrame (int blocksize, int numInputChannels);
    void deallocBuffers();
    void allocBuffers(int blocksize, int numInputChannels);
    void setOverlap(float newOverlap);
    void updateOverlap(int blocksize);
    int setupSarita(const char* path, int blocksize, int numInputCount);
    void hannWindow(int len, int overlap);
    #ifdef SAF_USE_APPLE_ACCELERATE
    void setupFFT(int blocksize);
    void fftXcorr(float* buf1, float* buf2, float* xcorr, int blocksize);
    #endif
    int readConfigFile(const char* path);
    
    // copy config data to array2sh structs
    void updateArrayData(void* const hA2sh);
    
    int bufferSize = 0;
    int overlapSize;
    float overlapPercent=0.25;
    bool overlapChanged=false;
    
    /* config data from MATLAB export */
    // header
    uint32_t fs;
    uint32_t N;                 // order of source grid
    uint32_t NUpsampling;       // order of target grid
    uint32_t NRendering;        // rendering order
    float radius;               // radius of array
    uint32_t denseGridSize;
    uint32_t maxShiftOverall;
    uint32_t neighborCombLength;   // neighborCombinations size = neighborCombLength * 2
    uint32_t idxNeighborsDenseLen; // idxNeighborsDense array size = idxNeighborsDenseLen * dense grid size
    uint32_t combinationsPtrLen;   // length of combinations pointer, y is always 2

    // data
    uint8_t** neighborCombinations; // Array containing all combinations of nearest neighbors
    uint8_t* numNeighborsDense = NULL;     // Number of nearest neighbors for each sampling point
    uint8_t** idxNeighborsDense;    // Indices of neighbors of each sampling point
    float** weightsNeighborsDense;  // Weights of neighbors of each sampling point
    uint8_t** maxShiftDense;
    int8_t** combinationsPtr;       // Array describing which neighbors combination is required for each cross correlations
    float** denseGrid;              // Az, El and weight of each target sensor
    /* end of config data */

    RingBuffer *input;
    RingBuffer *output;
    float** sparseBuffer = NULL; // audio of source grid
    float*** denseBuffer = NULL; // audio of upsampled target grid
    float*** outputBuffer = NULL;
    float** outData = NULL;
    bool configError = true;
    bool wantsConfigUpdate = false;
    int bufferNum = 0; // double buffer 0/1

    float* xcorrBufferPadded = NULL;
    
//    File logFile;
//    juce::FileLogger logger;
    
private:
    
    // cross correlation buffersxc
    #ifdef SAF_USE_APPLE_ACCELERATE
    FFTSetup fftSetup = NULL;
    vDSP_Length log2n;
    DSPSplitComplex inputBuffer1;
    DSPSplitComplex inputBuffer2;
    DSPSplitComplex outputBuffer3;
    #endif
    
    int xcorrLen;
    int tmpXcorrBufferSize;
	BYTETYPE* tmpXcorrBuffer = NULL;
	FLOATTYPE* correlation;
    float** xcorrBuffer;

	FLOATTYPE* hannWin = NULL;
    float** shiftBuffer;
    int* currentTimeShift;
	FLOATTYPE* currentBlock;
	FLOATTYPE* tmpBuf;

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
};

#endif /* sarita_h */
