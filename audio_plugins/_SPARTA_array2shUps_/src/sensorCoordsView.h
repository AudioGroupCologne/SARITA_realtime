/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.4

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --

#include "PluginProcessor.h"
#include "JuceHeader.h"

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class sensorCoordsView  : public juce::Component,
                          public Slider::Listener
{
public:
    //==============================================================================
    sensorCoordsView (PluginProcessor* ownerFilter, int _maxQ, int _currentQ, bool _useDegreesInstead);
    ~sensorCoordsView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void setUseDegreesInstead(bool newState);
    void setQ(int newQ){
//		newQ = newQ > 64 ? 64 : newQ;
        refreshCoords();
		if (newQ != currentQ) {
			currentQ = newQ;
			resized();
		}
    }
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PluginProcessor* hVst;
    void* hA2sh;
    void refreshCoords();

    std::unique_ptr<Slider>* aziSliders;
    std::unique_ptr<Slider>* elevSliders;

    int maxQ, currentQ;
    bool useDegreesInstead;

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Slider> dummySlider;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (sensorCoordsView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

