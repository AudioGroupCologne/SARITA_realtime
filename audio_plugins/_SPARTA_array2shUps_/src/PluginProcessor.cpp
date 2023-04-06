/*
 ==============================================================================
 
 This file is part of SPARTA; a suite of spatial audio plug-ins.
 Copyright (c) 2018 - Leo McCormack.
 
 SPARTA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 SPARTA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with SPARTA.  If not, see <http://www.gnu.org/licenses/>.
 
 ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor() :
	AudioProcessor(BusesProperties()
		.withInput("Input", AudioChannelSet::discreteChannels(64), true)
	    .withOutput("Output", AudioChannelSet::discreteChannels(64), true))
{
	array2sh_create(&hA2sh);
    startTimer(TIMER_PROCESSING_RELATED, 80);

}

PluginProcessor::~PluginProcessor()
{
	array2sh_destroy(&hA2sh);
}

void PluginProcessor::setParameter (int index, float newValue)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_overlap:     sarita.setOverlap(newValue);
            case k_outputOrder:   array2sh_setEncodingOrder(hA2sh, (SH_ORDERS)(int)(newValue*(float)(MAX_SH_ORDER-1) + 1.5f)); break;
            case k_channelOrder:  array2sh_setChOrder(hA2sh, (int)(newValue*(float)(NUM_CH_ORDERINGS-1) + 1.5f)); break;
            case k_normType:      array2sh_setNormType(hA2sh, (int)(newValue*(float)(NUM_NORM_TYPES-1) + 1.5f)); break;
            case k_filterType:    array2sh_setFilterType(hA2sh, (ARRAY2SH_FILTER_TYPES)(int)(newValue*(float)(ARRAY2SH_NUM_FILTER_TYPES-1) + 1.5f)); break;
            case k_maxGain:       array2sh_setRegPar(hA2sh, newValue*(ARRAY2SH_MAX_GAIN_MAX_VALUE-ARRAY2SH_MAX_GAIN_MIN_VALUE)+ARRAY2SH_MAX_GAIN_MIN_VALUE); break;
            case k_postGain:      array2sh_setGain(hA2sh, newValue*(ARRAY2SH_POST_GAIN_MAX_VALUE-ARRAY2SH_POST_GAIN_MIN_VALUE)+ARRAY2SH_POST_GAIN_MIN_VALUE); break;
        }
    }
    /* sensor direction parameters */
    else{
        index-=k_NumOfParameters;
        float newValueScaled;
        if (!(index % 2)){
            newValueScaled = (newValue - 0.5f)*360.0f;
            if (newValueScaled != array2sh_getSensorAzi_deg(hA2sh, index/2)){
                array2sh_setSensorAzi_deg(hA2sh, index/2, newValueScaled);
            }
        }
        else{
            newValueScaled = (newValue - 0.5f)*180.0f;
            if (newValueScaled != array2sh_getSensorElev_deg(hA2sh, index/2)){
                array2sh_setSensorElev_deg(hA2sh, index/2, newValueScaled);
            }
        }
    }
}

void PluginProcessor::setCurrentProgram (int /*index*/)
{
}

float PluginProcessor::getParameter (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_overlap: return (float) sarita.overlapPercent;
            case k_outputOrder:   return (float)(array2sh_getEncodingOrder(hA2sh)-1)/(float)(MAX_SH_ORDER-1);
            case k_channelOrder:  return (float)(array2sh_getChOrder(hA2sh)-1)/(float)(NUM_CH_ORDERINGS-1);
            case k_normType:      return (float)(array2sh_getNormType(hA2sh)-1)/(float)(NUM_NORM_TYPES-1);
            case k_filterType:    return (float)(array2sh_getFilterType(hA2sh)-1)/(float)(ARRAY2SH_NUM_FILTER_TYPES-1);
            case k_maxGain:       return (array2sh_getRegPar(hA2sh)-ARRAY2SH_MAX_GAIN_MIN_VALUE)/(ARRAY2SH_MAX_GAIN_MAX_VALUE-ARRAY2SH_MAX_GAIN_MIN_VALUE);
            case k_postGain:      return (array2sh_getGain(hA2sh)-ARRAY2SH_POST_GAIN_MIN_VALUE)/(ARRAY2SH_POST_GAIN_MAX_VALUE-ARRAY2SH_POST_GAIN_MIN_VALUE);
            default: return 0.0f;
        }
    }
    /* sensor direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return (array2sh_getSensorAzi_deg(hA2sh, index/2)/360.0f) + 0.5f;
        else
            return (array2sh_getSensorElev_deg(hA2sh, (index-1)/2)/180.0f) + 0.5f;
    }
}

int PluginProcessor::getNumParameters()
{
	return k_NumOfParameters + 2*ARRAY2SH_MAX_NUM_SENSORS;
}

const String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

const String PluginProcessor::getParameterName (int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_overlap:         return "overlap";
            case k_outputOrder:     return "order";
            case k_channelOrder:    return "channel_order";
            case k_normType:        return "norm_type";
            case k_filterType:      return "filter_type";
            case k_maxGain:         return "max_gain";
            case k_postGain:        return "post_gain";
            default: return "NULL";
        }
    }
    /* sensor direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return TRANS("Azim_") + String(index/2);
        else
            return TRANS("Elev_") + String((index-1)/2);
    }
}

const String PluginProcessor::getParameterText(int index)
{
    /* standard parameters */
    if(index < k_NumOfParameters){
        switch (index) {
            case k_overlap: return "FIXME";
            case k_outputOrder: return String(array2sh_getEncodingOrder(hA2sh));
            case k_channelOrder:
                switch(array2sh_getChOrder(hA2sh)){
                    case CH_ACN:  return "ACN";
                    case CH_FUMA: return "FuMa";
                    default: return "NULL";
                }
            case k_normType:
                switch(array2sh_getNormType(hA2sh)){
                    case NORM_N3D:  return "N3D";
                    case NORM_SN3D: return "SN3D";
                    case NORM_FUMA: return "FuMa";
                    default: return "NULL";
                }
                
            case k_filterType:
                switch(array2sh_getFilterType(hA2sh)){
                    case FILTER_SOFT_LIM:      return "Soft-Limiting";
                    case FILTER_TIKHONOV:      return "Tikhonov";
                    case FILTER_Z_STYLE:       return "Z-style";
                    case FILTER_Z_STYLE_MAXRE: return "Z-style (max_rE)";
                    default: return "NULL";
                }
            case k_maxGain:      return String(array2sh_getRegPar(hA2sh)) + " dB";
            case k_postGain:     return String(array2sh_getGain(hA2sh)) + " dB";
            default: return "NULL";
        }
    }
    /* sensor direction parameters */
    else{
        index-=k_NumOfParameters;
        if (!(index % 2))
            return String(array2sh_getSensorAzi_deg(hA2sh, index/2));
        else
            return String(array2sh_getSensorElev_deg(hA2sh, (index-1)/2));
    }
}

const String PluginProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 0;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

const String PluginProcessor::getProgramName (int /*index*/)
{
    return String();
}


bool PluginProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool PluginProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}


bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

void PluginProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bool sampleRateChanged = nSampleRate != sampleRate;
    bool blocksizeChanged = nHostBlockSize != samplesPerBlock;

    nHostBlockSize = samplesPerBlock;
    nNumInputs =  getTotalNumInputChannels();
    nNumOutputs =  getTotalNumOutputChannels();
    nSampleRate = (int)(sampleRate + 0.5);

    array2sh_init(hA2sh, nSampleRate);
    
    if (sarita.configError == true || sampleRateChanged || blocksizeChanged) {
        newCfgFile = lastCfgFile;
        sarita.wantsConfigUpdate = true;
    }
    
#ifdef TEST_AUDIO_OUTPUT
    AudioProcessor::setLatencySamples(nHostBlockSize + sarita.maxShiftOverall);
#else
    AudioProcessor::setLatencySamples(nHostBlockSize + sarita.maxShiftOverall + array2sh_getProcessingDelay());
#endif
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& /*midiMessages*/)
{
    int nCurrentBlockSize = buffer.getNumSamples();
    nNumInputs = jmin(getTotalNumInputChannels(), buffer.getNumChannels());
    nNumOutputs = jmin(getTotalNumOutputChannels(), buffer.getNumChannels());
    
    if (sarita.overlapChanged) {
        sarita.updateOverlap(nHostBlockSize);
    }
    else if(sarita.wantsConfigUpdate) {
        sarita.wantsConfigUpdate = false;
        DBG("load cfg");
        loadConfiguration(newCfgFile);
    }
    
    // if config file read is not successful
    if (sarita.configError || nCurrentBlockSize < nHostBlockSize)
        buffer.clear();
    else
    {
        // fill input ring buffer
        for (int ch = 0; ch < nNumInputs; ch++) {
            auto* channelData = buffer.getReadPointer(ch);
            sarita.input->push(channelData, nCurrentBlockSize, ch);
        }

        // process frame when frame size fulfilled
        while (sarita.input->bufferedBytes >= nHostBlockSize) { // TODO: independent buffer size
            // process frame for all channels
            sarita.processFrame(nCurrentBlockSize, nNumInputs);

            // add overlapping part of current frame to last frame end
            int overlapIdx = juce::jmax(nCurrentBlockSize-sarita.overlapSize, 0);
            for (uint32_t ch=0; ch<sarita.denseGridSize; ch++) {
                utility_svvadd(&sarita.denseBuffer[sarita.bufferNum][ch][0], &sarita.denseBuffer[!sarita.bufferNum][ch][overlapIdx], sarita.overlapSize, sarita.outputBuffer[sarita.bufferNum][ch]);
            }
            // copy last overlap to output ring buffer
            for (uint32_t ch=0; ch<sarita.denseGridSize ; ch++) {
                sarita.output->push(sarita.outputBuffer[sarita.bufferNum][ch], sarita.overlapSize, ch);
            }
            // copy non overlapping part to output ring buffer
            for (uint32_t ch=0; ch<sarita.denseGridSize; ch++) {
                sarita.output->push(&sarita.denseBuffer[sarita.bufferNum][ch][sarita.overlapSize], nHostBlockSize-2*sarita.overlapSize, ch);
            }
            sarita.bufferNum ^= 1; // swap buffer
        }
        
        // test output
#ifdef TEST_AUDIO_OUTPUT
        if (sarita.output->bufferedBytes >= nHostBlockSize) {
            uint32_t numCh = juce::jmin((int)buffer.getNumChannels(), (int)sarita.denseGridSize);
            for (uint32_t ch = 0; ch<numCh; ch++) {
                float* channelData = buffer.getWritePointer(ch);
                sarita.output->pop(channelData, ch, nHostBlockSize);
            }
            // increase read index cause not all ring buffer channels are used
            if ((uint32_t)buffer.getNumChannels() < (uint32_t)sarita.denseGridSize) {
                sarita.output->skipPop(nHostBlockSize);
            }
        }
#else
        /*
         * process array2sh with dense grid
         */
        float** bufferData = buffer.getArrayOfWritePointers();
        float* pFrameData[MAX_NUM_CHANNELS];
        int frameSize = array2sh_getFrameSize();
        
        if ((sarita.output->bufferedBytes >= nHostBlockSize) && (nHostBlockSize % frameSize == 0)) { /* buffer filled and blocksize divisible by frame size */
            for (int frame = 0; frame < nCurrentBlockSize/frameSize; frame++) {
                for (int ch = 0; ch < (int)sarita.output->numChannels(); ch++) {
                    // copy to array2Sh processing buffer
                    sarita.output->pop(sarita.outData[ch], ch, frameSize);
                    // normalize sh transform input
                    #ifdef SAF_USE_APPLE_ACCELERATE
                    float value = 1.f/sarita.denseGridSize;
                    vDSP_vsmul(sarita.outData[ch], 1, &value, sarita.outData[ch], 1, frameSize);
                    #else
                    ippsMulC_32f_I(1.f/sarita.denseGridSize, sarita.outData[ch], frameSize); // FIXME: find correct value
                    #endif
                }
                
                // gather channel pointers to output buffer
                for (int ch = 0; ch < nNumOutputs; ch++) {
                    pFrameData[ch] = &bufferData[ch][frame*frameSize];
                }
                
                /* perform processing and write to AudioSampleBuffer */
                array2sh_process(hA2sh, sarita.outData, pFrameData, sarita.denseGridSize, nNumOutputs, frameSize);
            }
        }
#endif
        else {
            buffer.clear();
        }
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; 
}

AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (this);
}

//==============================================================================
void PluginProcessor::getStateInformation (MemoryBlock& destData)
{
    XmlElement xml("ARRAY2SHPLUGINSETTINGS");
    
    xml.setAttribute("order", array2sh_getEncodingOrder(hA2sh));
    xml.setAttribute("overlap", sarita.overlapPercent);
//    xml.setAttribute("Q", array2sh_getNumSensors(hA2sh));
//    for(int i=0; i<MAX_NUM_CHANNELS; i++){
//        xml.setAttribute("AziRad" + String(i), array2sh_getSensorAzi_rad(hA2sh,i));
//        xml.setAttribute("ElevRad" + String(i), array2sh_getSensorElev_rad(hA2sh,i));
//    }
//    xml.setAttribute("r", array2sh_getr(hA2sh));
//    xml.setAttribute("R", array2sh_getR(hA2sh));
//    xml.setAttribute("arrayType", ARRAY_SPHERICAL); // array2sh_getArrayType(hA2sh));
    xml.setAttribute("weightType", array2sh_getWeightType(hA2sh));
    xml.setAttribute("filterType", array2sh_getFilterType(hA2sh));
    xml.setAttribute("regPar", array2sh_getRegPar(hA2sh));
    xml.setAttribute("chOrder", array2sh_getChOrder(hA2sh));
    xml.setAttribute("normType", array2sh_getNormType(hA2sh));
    xml.setAttribute("c", array2sh_getc(hA2sh));
    xml.setAttribute("gain", array2sh_getGain(hA2sh));
    //xml.setAttribute("maxFreq", array2sh_getMaxFreq(hA2sh));
    xml.setAttribute("enableDiffPastAliasing", 0); // array2sh_getDiffEQpastAliasing(hA2sh));
    
    String pathname = lastDir.getFullPathName();
    if (pathname.isNotEmpty())
        xml.setAttribute("CfgFilePath", pathname);
    String filename = lastCfgFile.getFullPathName();
    if (filename.isNotEmpty())
        xml.setAttribute("CfgFileName", filename);
    
    copyXmlToBinary(xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    int i;
    
    if (xmlState != nullptr) {
        if (xmlState->hasTagName("ARRAY2SHPLUGINSETTINGS")) {
//            for(i=0; i<array2sh_getMaxNumSensors(); i++){
//                if(xmlState->hasAttribute("AziRad" + String(i)))
//                    array2sh_setSensorAzi_rad(hA2sh, i, (float)xmlState->getDoubleAttribute("AziRad" + String(i), 0.0f));
//                if(xmlState->hasAttribute("ElevRad" + String(i)))
//                    array2sh_setSensorElev_rad(hA2sh, i, (float)xmlState->getDoubleAttribute("ElevRad" + String(i), 0.0f));
//            }
            if(xmlState->hasAttribute("order"))
                array2sh_setEncodingOrder(hA2sh, xmlState->getIntAttribute("order", 1));
            if(xmlState->hasAttribute("overlap"))
                sarita.setOverlap(xmlState->getDoubleAttribute("overlap", 25.0));
//            if(xmlState->hasAttribute("Q"))
//                array2sh_setNumSensors(hA2sh, xmlState->getIntAttribute("Q", 4));
//            if(xmlState->hasAttribute("r"))
//                array2sh_setr(hA2sh, (float)xmlState->getDoubleAttribute("r", 0.042));
//            if(xmlState->hasAttribute("R"))
//                array2sh_setR(hA2sh, (float)xmlState->getDoubleAttribute("R", 0.042));
//            if(xmlState->hasAttribute("arrayType"))
//                array2sh_setArrayType(hA2sh, xmlState->getIntAttribute("arrayType", 1));
//            if(xmlState->hasAttribute("weightType"))
//                array2sh_setWeightType(hA2sh, xmlState->getIntAttribute("weightType", 1));
            if(xmlState->hasAttribute("filterType"))
                array2sh_setFilterType(hA2sh, xmlState->getIntAttribute("filterType", 3));
            if(xmlState->hasAttribute("regPar"))
                array2sh_setRegPar(hA2sh, (float)xmlState->getDoubleAttribute("regPar", 15.0));
            if(xmlState->hasAttribute("chOrder"))
                array2sh_setChOrder(hA2sh, xmlState->getIntAttribute("chOrder", 1));
            if(xmlState->hasAttribute("normType"))
                array2sh_setNormType(hA2sh, xmlState->getIntAttribute("normType", 1));
//            if(xmlState->hasAttribute("c"))
//                array2sh_setc(hA2sh, (float)xmlState->getDoubleAttribute("c", 343.0));
            if(xmlState->hasAttribute("gain"))
                array2sh_setGain(hA2sh, (float)xmlState->getDoubleAttribute("gain", 0.0));
            //if(xmlState->hasAttribute("maxFreq"))
            //    array2sh_setMaxFreq(hA2sh, (float)xmlState->getDoubleAttribute("maxFreq", 20000.0));
//            if(xmlState->hasAttribute("enableDiffPastAliasing"))
//                array2sh_setDiffEQpastAliasing(hA2sh, 0); // xmlState->getIntAttribute("enableDiffPastAliasing", 0));
            
            if(xmlState->hasAttribute("CfgFilePath"))
                lastDir = xmlState->getStringAttribute("CfgFilePath", "");
            
            if(xmlState->hasAttribute("CfgFileName")) {
                lastCfgFile = xmlState->getStringAttribute("CfgFileName", "");
                if(sarita.readConfigFile(lastCfgFile.getFullPathName().getCharPointer()) < 0);
                    return;
                sarita.updateArrayData(hA2sh);
            }
            array2sh_refreshSettings(hA2sh);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

/* Adapted from the AllRADecoder by Daniel Rudrich, (c) 2017 (GPLv3 license) */
void PluginProcessor::saveConfigurationToFile (File destination)
{
    sensors.removeAllChildren(nullptr);
    for (int i=0; i<array2sh_getNumSensors(hA2sh);i++)
    {
        sensors.appendChild (ConfigurationHelper::
                             createElement(array2sh_getSensorAzi_deg(hA2sh, i),
                                           array2sh_getSensorElev_deg(hA2sh, i),
                                           1.0f, i+1, false, 1.0f), nullptr);
    }
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("Name", var("SPARTA Array2SH sensor directions."));
    char versionString[10];
    strcpy(versionString, "v");
    strcat(versionString, JucePlugin_VersionString);
    jsonObj->setProperty("Description", var("This configuration file was created with the SPARTA Array2SH " + String(versionString) + " plug-in. " + Time::getCurrentTime().toString(true, true)));
    jsonObj->setProperty ("GenericLayout", ConfigurationHelper::convertElementsToVar (sensors, "Sensor Directions"));
    Result result = ConfigurationHelper::writeConfigurationToFile (destination, var (jsonObj));
}

/*   */
void PluginProcessor::loadConfiguration (const File& configFile)
{
    if (!configFile.existsAsFile())
        return;
    
    lastCfgFile = configFile;
    const char *p = configFile.getFullPathName().getCharPointer();
    if(sarita.setupSarita(p, nHostBlockSize, nNumInputs) == -1)
        return; // config error
    
    sarita.updateArrayData(hA2sh);
}

