/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.6

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --

#include "PluginProcessor.h"
#include "eqview.h"
#include "anaview.h"
#include "sensorCoordsView.h"
#include "../../resources/SPARTALookAndFeel.h"
#include "JuceHeader.h"

typedef enum {
    SHOW_EQ = 1,
    SHOW_SPATIAL_COH,
    SHOW_LEVEL_DIFF
}DISP_WINDOW;

typedef enum _SPARTA_WARNINGS{
    k_warning_none,
    k_warning_frameSize,
    k_warning_supported_fs,
    k_warning_NinputCH,
    k_warning_NoutputCH
}SPARTA_WARNINGS;


//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginEditor  : public AudioProcessorEditor,
                      public MultiTimer,
                      public juce::ComboBox::Listener,
                      public juce::Slider::Listener,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    PluginEditor (PluginProcessor* ownerFilter);
    ~PluginEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.


    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PluginProcessor* hVst;
    void* hA2sh;

    void timerCallback(int timerID) override;
#ifndef PLUGIN_EDITOR_DISABLE_OPENGL
    std::unique_ptr<OpenGLGraphicsContextCustomShader> shader;
    OpenGLContext openGLContext;
#endif
    double progress = 0.0;
    ProgressBar progressbar;

    /* Look and Feel */
    SPARTALookAndFeel LAF;

    /* Custom components */
    std::unique_ptr<Viewport> sensorCoordsVP;
    sensorCoordsView* sensorCoordsView_handle;
    std::unique_ptr<eqview> eqviewIncluded;
    std::unique_ptr<anaview> cohviewIncluded;
    std::unique_ptr<anaview> ldiffviewIncluded;
    DISP_WINDOW dispID;

    bool needScreenRefreshFLAG;
    bool showDegreesInstead;

    /* json file loading/saving */
    std::unique_ptr<juce::FileChooser> chooser;

    /* warnings */
    SPARTA_WARNINGS currentWarning;

    /* tooltips */
    SharedResourcePointer<TooltipWindow> tipWindow;
    std::unique_ptr<juce::ComboBox> pluginDescription; /* Dummy combo box to provide plugin description tooltip */
    HyperlinkButton publicationLink { "(Related Publication)", {"https://ieeexplore.ieee.org/document/10057092" } };

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::ComboBox> filterTypeCB;
    std::unique_ptr<juce::Slider> regAmountSlider;
    std::unique_ptr<juce::ComboBox> CHOrderingCB;
    std::unique_ptr<juce::ComboBox> normalisationCB;
    std::unique_ptr<juce::Slider> gainSlider;
    std::unique_ptr<juce::TextButton> textButton;
    std::unique_ptr<juce::ComboBox> dispWindow;
    std::unique_ptr<juce::TextButton> tb_loadCfg;
    std::unique_ptr<juce::ComboBox> CBencodingOrder;
    std::unique_ptr<juce::ComboBox> overlapCB;
    std::unique_ptr<juce::TextEditor> txtGrid;
    std::unique_ptr<juce::ToggleButton> perform_SHT_btn;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

