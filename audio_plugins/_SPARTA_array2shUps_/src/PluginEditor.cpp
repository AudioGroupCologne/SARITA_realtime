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

//[Headers] You can add your own extra header files here...

//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor* ownerFilter)
    : AudioProcessorEditor(ownerFilter) , progressbar(progress)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    filterTypeCB.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (filterTypeCB.get());
    filterTypeCB->setEditableText (false);
    filterTypeCB->setJustificationType (juce::Justification::centredLeft);
    filterTypeCB->setTextWhenNothingSelected (juce::String());
    filterTypeCB->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    filterTypeCB->addListener (this);

    filterTypeCB->setBounds (640, 276, 128, 16);

    regAmountSlider.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (regAmountSlider.get());
    regAmountSlider->setRange (0, 80, 0.01);
    regAmountSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    regAmountSlider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 55, 20);
    regAmountSlider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff5c5d5e));
    regAmountSlider->setColour (juce::Slider::trackColourId, juce::Colour (0xff315b6d));
    regAmountSlider->addListener (this);

    regAmountSlider->setBounds (640, 308, 128, 16);

    CHOrderingCB.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (CHOrderingCB.get());
    CHOrderingCB->setEditableText (false);
    CHOrderingCB->setJustificationType (juce::Justification::centredLeft);
    CHOrderingCB->setTextWhenNothingSelected (TRANS("ACN"));
    CHOrderingCB->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CHOrderingCB->addSeparator();
    CHOrderingCB->addSeparator();
    CHOrderingCB->addListener (this);

    CHOrderingCB->setBounds (640, 377, 128, 16);

    normalisationCB.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (normalisationCB.get());
    normalisationCB->setEditableText (false);
    normalisationCB->setJustificationType (juce::Justification::centredLeft);
    normalisationCB->setTextWhenNothingSelected (TRANS("N3D"));
    normalisationCB->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    normalisationCB->addSeparator();
    normalisationCB->addSeparator();
    normalisationCB->addListener (this);

    normalisationCB->setBounds (640, 409, 128, 16);

    gainSlider.reset (new juce::Slider ("new slider"));
    addAndMakeVisible (gainSlider.get());
    gainSlider->setRange (-60, 60, 0.01);
    gainSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    gainSlider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 55, 20);
    gainSlider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff5c5d5e));
    gainSlider->setColour (juce::Slider::trackColourId, juce::Colour (0xff315b6d));
    gainSlider->addListener (this);

    gainSlider->setBounds (640, 341, 128, 16);

    textButton.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (textButton.get());
    textButton->setButtonText (TRANS("Analyse"));
    textButton->addListener (this);
    textButton->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff5c68a4));

    textButton->setBounds (228, 39, 60, 14);

    dispWindow.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (dispWindow.get());
    dispWindow->setEditableText (false);
    dispWindow->setJustificationType (juce::Justification::centredLeft);
    dispWindow->setTextWhenNothingSelected (TRANS("Filters"));
    dispWindow->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    dispWindow->addItem (TRANS("Filters"), 1);
    dispWindow->addItem (TRANS("Corr"), 2);
    dispWindow->addItem (TRANS("L Diff"), 3);
    dispWindow->addListener (this);

    dispWindow->setBounds (721, 39, 63, 14);

    tb_loadCfg.reset (new juce::TextButton ("new button"));
    addAndMakeVisible (tb_loadCfg.get());
    tb_loadCfg->setButtonText (TRANS("Import"));
    tb_loadCfg->addListener (this);
    tb_loadCfg->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff14889e));

    tb_loadCfg->setBounds (120, 64, 80, 16);

    CBencodingOrder.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (CBencodingOrder.get());
    CBencodingOrder->setEditableText (false);
    CBencodingOrder->setJustificationType (juce::Justification::centredLeft);
    CBencodingOrder->setTextWhenNothingSelected (TRANS("Default"));
    CBencodingOrder->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    CBencodingOrder->addListener (this);

    CBencodingOrder->setBounds (368, 274, 120, 20);

    overlapCB.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (overlapCB.get());
    overlapCB->setEditableText (false);
    overlapCB->setJustificationType (juce::Justification::centredLeft);
    overlapCB->setTextWhenNothingSelected (TRANS("25 %"));
    overlapCB->setTextWhenNoChoicesAvailable (TRANS("25 %"));
    overlapCB->addItem (TRANS("12.5 %"), 1);
    overlapCB->addItem (TRANS("25 %"), 2);
    overlapCB->addItem (TRANS("50 %"), 3);
    overlapCB->addListener (this);

    overlapCB->setBounds (368, 312, 120, 20);

    txtGrid.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (txtGrid.get());
    txtGrid->setMultiLine (true);
    txtGrid->setReturnKeyStartsNewLine (false);
    txtGrid->setReadOnly (true);
    txtGrid->setScrollbarsShown (false);
    txtGrid->setCaretVisible (false);
    txtGrid->setPopupMenuEnabled (false);
    txtGrid->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x4d2c3c6a));
    txtGrid->setText (TRANS("Import a config file first"));

    txtGrid->setBounds (24, 96, 176, 64);

    perform_SHT_btn.reset (new juce::ToggleButton ("AmbisonicsOutputBtn"));
    addAndMakeVisible (perform_SHT_btn.get());
    perform_SHT_btn->setTooltip (TRANS("If disabled, perform upsampling only, if enabled, the output are SH signlas."));
    perform_SHT_btn->setButtonText (TRANS("Ambisonics Output"));
    perform_SHT_btn->addListener (this);

    perform_SHT_btn->setBounds (240, 400, 150, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (800, 450);


    //[Constructor] You can add your own custom stuff here..

    /* handles */
	hVst = ownerFilter;
    hA2sh = hVst->getFXHandle();

    /* init OpenGL */
#ifndef PLUGIN_EDITOR_DISABLE_OPENGL
    openGLContext.setMultisamplingEnabled(true);
    openGLContext.attachTo(*this);
#endif

    /* Look and Feel */
    LAF.setDefaultColours();
    setLookAndFeel(&LAF);

    /* create EQ window and analysis windows */
    eqviewIncluded.reset (new eqview(556, 209, 30.0f, 20e3f, -30.0f, 60.0f, 48e3f )); /* TODO: switch to the more general "anaview"  */
    addAndMakeVisible (eqviewIncluded.get());
    eqviewIncluded->setAlwaysOnTop(true);
    eqviewIncluded->setTopLeftPosition(228, 56);
    eqviewIncluded->setVisible(true);
    cohviewIncluded.reset (new anaview(556, 209, 30.0f, 20e3f, 0.0f, 1.0f, TRANS("Spatial Corr. (T:I)"), 1, 48e3f ));
    addAndMakeVisible (cohviewIncluded.get());
    cohviewIncluded->setAlwaysOnTop(true);
    cohviewIncluded->setTopLeftPosition(228, 56);
    cohviewIncluded->setVisible(false);
    ldiffviewIncluded.reset (new anaview(556, 209, 30.0f, 20e3f, -30, 10, TRANS("Level Diff. (dB)"), 10.0f, 48e3f ));
    addAndMakeVisible (ldiffviewIncluded.get());
    ldiffviewIncluded->setAlwaysOnTop(true);
    ldiffviewIncluded->setTopLeftPosition(228, 56);
    ldiffviewIncluded->setVisible(false);
    dispID = SHOW_EQ;
    needScreenRefreshFLAG = true;

    /* add master decoding order options */
    CBencodingOrder->addItem (TRANS("1st order"), SH_ORDER_FIRST);
    CBencodingOrder->addItem (TRANS("2nd order"), SH_ORDER_SECOND);
    CBencodingOrder->addItem (TRANS("3rd order"), SH_ORDER_THIRD);
    CBencodingOrder->addItem (TRANS("4th order"), SH_ORDER_FOURTH);
    CBencodingOrder->addItem (TRANS("5th order"), SH_ORDER_FIFTH);
    CBencodingOrder->addItem (TRANS("6th order"), SH_ORDER_SIXTH);
    CBencodingOrder->addItem (TRANS("7th order"), SH_ORDER_SEVENTH);

    /* pass handles to data required for eq and analysis displays */
    int numFreqPoints, numCurves;
    float* freqVector = array2sh_getFreqVector(hA2sh, &numFreqPoints);
    float** solidCurves = array2sh_getbN_inv(hA2sh, &numCurves, &numFreqPoints);
    eqviewIncluded->setSolidCurves_Handle(freqVector, solidCurves, numFreqPoints, numCurves);
    float** faintCurves = array2sh_getbN_modal(hA2sh, &numCurves, &numFreqPoints);
    eqviewIncluded->setFaintCurves_Handle(freqVector, faintCurves, numFreqPoints, numCurves);
    float* dataHandle = array2sh_getSpatialCorrelation_Handle(hA2sh, &numCurves, &numFreqPoints);
    cohviewIncluded->setSolidCurves_Handle(freqVector, dataHandle, numFreqPoints, numCurves);
    dataHandle = array2sh_getLevelDifference_Handle(hA2sh, &numCurves, &numFreqPoints);
    ldiffviewIncluded->setSolidCurves_Handle(freqVector, dataHandle, numFreqPoints, numCurves);

    /* add filter options */
    filterTypeCB->addItem (TRANS("Soft-Limiting"), FILTER_SOFT_LIM);
    filterTypeCB->addItem (TRANS("Tikhonov"), FILTER_TIKHONOV);
    filterTypeCB->addItem (TRANS("Z-Style"), FILTER_Z_STYLE);
    filterTypeCB->addItem (TRANS("Z-Style (max_rE)"), FILTER_Z_STYLE_MAXRE);
    filterTypeCB->setSelectedId(array2sh_getFilterType(hA2sh), dontSendNotification);

    /* add channel format and norm scheme options */
    CHOrderingCB->addItem (TRANS("ACN"), CH_ACN);
    CHOrderingCB->addItem (TRANS("FuMa"), CH_FUMA);
    normalisationCB->addItem (TRANS("N3D"), NORM_N3D);
    normalisationCB->addItem (TRANS("SN3D"), NORM_SN3D);
    normalisationCB->addItem (TRANS("FuMa"), NORM_FUMA);

    /* Hide decoding orders that are unsuitable for number of sensors */
    for(int i=1; i<=7; i++)
        CBencodingOrder->setItemEnabled(i, (i+1)*(i+1) <= array2sh_getNumSensors(hA2sh) ? true : false);

    /* sensor coord table */
    //addAndMakeVisible (sensorCoordsVP = new Viewport ("new viewport"));
    sensorCoordsVP.reset(new Viewport ("new viewport"));
    addAndMakeVisible (sensorCoordsVP.get());
    sensorCoordsView_handle = new sensorCoordsView(ownerFilter, ARRAY2SH_MAX_NUM_SENSORS, array2sh_getNumSensors(hA2sh), showDegreesInstead);
    sensorCoordsVP->setViewedComponent (sensorCoordsView_handle);
    sensorCoordsVP->setScrollBarsShown (true, false);
    sensorCoordsVP->setAlwaysOnTop(true);
    sensorCoordsVP->setBounds(24, 168, 184, 265);
    sensorCoordsView_handle->setQ(array2sh_getNumSensors(hA2sh));

    /* ProgressBar */
    progress = 0.0;
    progressbar.setBounds(getLocalBounds().getCentreX()-175, getLocalBounds().getCentreY()-17, 350, 35);
    progressbar.ProgressBar::setAlwaysOnTop(true);
    progressbar.setColour(ProgressBar::backgroundColourId, Colours::gold);
    progressbar.setColour(ProgressBar::foregroundColourId, Colours::white);

    /* grab current parameter settings */
    CBencodingOrder->setSelectedId(array2sh_getEncodingOrder(hA2sh), dontSendNotification);

    switch (int(hVst->sarita.overlapPercent * 10.f)) {
        case 125: overlapCB->setSelectedId(1); break; // 12.5 %
        default:
        case 250: overlapCB->setSelectedId(2); break; // 25 %
        case 500: overlapCB->setSelectedId(3); break; // 50 %
    }

	perform_SHT_btn->setToggleState (hVst->_perform_sht, juce::dontSendNotification);

    regAmountSlider->setRange(ARRAY2SH_MAX_GAIN_MIN_VALUE, ARRAY2SH_MAX_GAIN_MAX_VALUE, 0.01f);
    regAmountSlider->setValue(array2sh_getRegPar(hA2sh), dontSendNotification);
    CHOrderingCB->setSelectedId(array2sh_getChOrder(hA2sh), dontSendNotification);
    normalisationCB->setSelectedId(array2sh_getNormType(hA2sh), dontSendNotification);
    gainSlider->setRange(ARRAY2SH_POST_GAIN_MIN_VALUE, ARRAY2SH_POST_GAIN_MAX_VALUE, 0.01f);
    gainSlider->setValue(array2sh_getGain(hA2sh), dontSendNotification);
    showDegreesInstead = true;
    CHOrderingCB->setItemEnabled(CH_FUMA, array2sh_getEncodingOrder(hA2sh)==SH_ORDER_FIRST ? true : false);
    normalisationCB->setItemEnabled(NORM_FUMA, array2sh_getEncodingOrder(hA2sh)==SH_ORDER_FIRST ? true : false);

    /* tooltips */
    CBencodingOrder->setTooltip("Encoding order. Note that the plug-in will require at least (order+1)^2 microphone array signals as input.");
    regAmountSlider->setTooltip("Maximum gain amplification permitted. Higher-values give a wider frequency range of usable spherical harmonic components, but at the cost of increased noise.");
    gainSlider->setTooltip("Post-gain factor (in dB).");
    filterTypeCB->setTooltip("Encoding filter design approach. Tikhonov is generally recommended as a starting point. However, Z-style may work better for Ambisonic reproduction purposes, and it can also have max_rE weights baked into the signals at the encoding stage.");
    CHOrderingCB->setTooltip("Ambisonic channel ordering convention (Note that AmbiX: ACN/SN3D).");
    normalisationCB->setTooltip("Ambisonic normalisation scheme (Note that AmbiX: ACN/SN3D).");
//    tb_loadJSON->setTooltip("Loads microphone array sensor directions from a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
//    tb_saveJSON->setTooltip("Saves the current microphone array sensor directions to a JSON file. The JSON file format follows the same convention as the one employed by the IEM plugin suite (https://plugins.iem.at/docs/configurationfiles/).");
    textButton->setTooltip("Anlyses the performance of the currently configured microphone array, based on established objective metrics. The plug-in first simulates the microphone array and applies the current encoding matrix to it, subsequently comparing the resulting patterns with ideal spherical harmonics.");
    dispWindow->setTooltip("Switches between the different display options. \n\nFilters: order-dependent equalisation curves, which are applied to eliminate the effect of the sphere. \n\nCorr: The spatial correlation is derived by comparing the patterns of the array responses with the patterns of ideal spherical harmonics, where '1' means they are perfect, and '0' completely uncorrelated; the spatial aliasing frequency can therefore be observed for each order, as the point where the spatial correlation tends towards 0. \n\nLdiff: The level difference is the mean level difference over all directions (diffuse level difference) between the ideal and simulated components. One can observe that higher permitted amplification limits [Max Gain (dB)] will result in noisier signals; however, this will also result in a wider frequency range of useful spherical harmonic components at each order.");

    /* Plugin description */
    pluginDescription.reset (new juce::ComboBox ("new combo box"));
    addAndMakeVisible (pluginDescription.get());
    pluginDescription->setBounds (0, 0, 200, 32);
    pluginDescription->setAlpha(0.0f);
    pluginDescription->setEnabled(false);
    pluginDescription->setTooltip(TRANS("This plug-in spatially encodes spherical/cylindrical array signals into spherical harmonic signals (aka: Ambisonic or B-Format signals). The plug-in utilises analytical descriptors, which ascertain the frequency and order-dependent influence that the physical properties of the array have on the plane-waves arriving at its surface. The plug-in then determines the order-dependent equalisation curves needed to be imposed onto the initial spherical harmonic signals estimate, in order to remove the influence of the array itself. However, especially for higher-orders, this generally results in a large amplification of the low frequencies (including the sensor noise at these frequencies that accompanies it); therefore, four different regularisation approaches have been integrated into the plug-in, which allow the user to make a compromise between noise amplification and transform accuracy. These target (faint lines) and regularised (solid lines) equalisation curves are depicted on the user interface.\n\n")+
                                  TRANS("Note that this ability to balance the noise amplification with the accuracy of the spatial encoding (to better suit a given application) is very important, for example: the perceived fidelity of Ambisonic decoded audio can be rather poor if the noise amplification is set too high: which is also sound-scene dependent: think orchestra vs pop-band, and which would be more permissive of increased noise. In general, a lower amplification regularisation limit is used for Ambisonics reproduction applications, when compared to sound-field visualisation algorithms, or beamformers that employ appropriate post-filtering etc."));
    addAndMakeVisible (publicationLink);
    publicationLink.setColour (HyperlinkButton::textColourId, Colours::lightblue);
    publicationLink.setBounds(getBounds().getWidth()-80, 4, 80, 12);
    publicationLink.setJustificationType(Justification::centredLeft);

	/* Specify screen refresh rate */
    startTimer(TIMER_GUI_RELATED, 40);

    /* warnings */
    currentWarning = k_warning_none;

    //[/Constructor]
}

PluginEditor::~PluginEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    filterTypeCB = nullptr;
    regAmountSlider = nullptr;
    CHOrderingCB = nullptr;
    normalisationCB = nullptr;
    gainSlider = nullptr;
    textButton = nullptr;
    dispWindow = nullptr;
    tb_loadCfg = nullptr;
    CBencodingOrder = nullptr;
    overlapCB = nullptr;
    txtGrid = nullptr;
    perform_SHT_btn = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    setLookAndFeel(nullptr);
    eqviewIncluded = nullptr;
    cohviewIncluded = nullptr;
    ldiffviewIncluded = nullptr;
    sensorCoordsVP = nullptr;
    sensorCoordsView_handle = nullptr;
    //[/Destructor]
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colours::white);

    {
        int x = 0, y = 240, width = 800, height = 210;
        juce::Colour fillColour1 = juce::Colour (0xff19313f), fillColour2 = juce::Colour (0xff041518);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                             8.0f - 0.0f + x,
                                             448.0f - 240.0f + y,
                                             fillColour2,
                                             8.0f - 0.0f + x,
                                             352.0f - 240.0f + y,
                                             false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 30, width = 800, height = 210;
        juce::Colour fillColour1 = juce::Colour (0xff19313f), fillColour2 = juce::Colour (0xff041518);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                             8.0f - 0.0f + x,
                                             32.0f - 30.0f + y,
                                             fillColour2,
                                             8.0f - 0.0f + x,
                                             128.0f - 30.0f + y,
                                             false));
        g.fillRect (x, y, width, height);
    }

    {
        float x = 1.0f, y = 2.0f, width = 798.0f, height = 31.0f;
        juce::Colour fillColour1 = juce::Colour (0xff041518), fillColour2 = juce::Colour (0xff19313f);
        juce::Colour strokeColour = juce::Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                             0.0f - 1.0f + x,
                                             32.0f - 2.0f + y,
                                             fillColour2,
                                             792.0f - 1.0f + x,
                                             24.0f - 2.0f + y,
                                             false));
        g.fillRoundedRectangle (x, y, width, height, 5.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 5.000f, 2.000f);
    }

    {
        int x = 228, y = 56, width = 556, height = 209;
        juce::Colour fillColour = juce::Colour (0x10f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 280, y = 70, width = 456, height = 158;
        juce::Colour fillColour = juce::Colour (0x13000000);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 506, y = 264, width = 278, height = 104;
        juce::Colour fillColour = juce::Colour (0x10f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 506, y = 264, width = 278, height = 104;
        juce::Colour fillColour = juce::Colour (0x08f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 506, y = 367, width = 278, height = 69;
        juce::Colour fillColour = juce::Colour (0x13f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 228, y = 264, width = 279, height = 172;
        juce::Colour fillColour = juce::Colour (0x10f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 228, y = 264, width = 279, height = 38;
        juce::Colour fillColour = juce::Colour (0x08f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 12, y = 56, width = 204, height = 384;
        juce::Colour fillColour = juce::Colour (0x10f4f4f4);
        juce::Colour strokeColour = juce::Colour (0x67a0a0a0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    {
        int x = 520, y = 268, width = 172, height = 30;
        juce::String text (TRANS("Filter Approach:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 520, y = 301, width = 172, height = 30;
        juce::String text (TRANS("Max Gain (dB):"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 520, y = 369, width = 172, height = 30;
        juce::String text (TRANS("Channel Order:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 520, y = 401, width = 172, height = 30;
        juce::String text (TRANS("Normalisation:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 520, y = 334, width = 172, height = 30;
        juce::String text (TRANS("Post Gain (dB):"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 24, y = 57, width = 88, height = 30;
        juce::String text (TRANS("Grid"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 440, y = 30, width = 149, height = 30;
        juce::String text (TRANS("Encoding Settings"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 240, y = 268, width = 172, height = 30;
        juce::String text (TRANS("Encoding Order:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 16, y = 1, width = 100, height = 32;
        juce::String text (TRANS("SPARTA|"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (18.80f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 92, y = 1, width = 244, height = 32;
        juce::String text (TRANS("Array2SH SARITA Upsampling"));
        juce::Colour fillColour = juce::Colour (0xffe9ff00);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (18.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 328, y = 65, width = 392, height = 31;
        juce::String text (TRANS("Press the \"Analyse\" button"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 291, y = 88, width = 477, height = 23;
        juce::String text (TRANS("Corr: The spatial correlation is derived by comparing the patterns of the array responses with"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 104, width = 477, height = 23;
        juce::String text (TRANS("the patterns of ideal spherical harmonics, where \'1\' means they are perfect, and \'0\' completely "));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 120, width = 477, height = 23;
        juce::String text (TRANS("uncorrelated; the spatial aliasing frequency can therefore be observed for each order, as the "));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 160, width = 477, height = 23;
        juce::String text (TRANS("Ldiff: The level difference is the mean level difference over all directions (diffuse level differe-"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 192, width = 477, height = 23;
        juce::String text (TRANS("amplification limits [Max Gain (dB)] will result in noisier signals; however, this will also result in "));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 176, width = 477, height = 23;
        juce::String text (TRANS("nce) between the ideal and simulated components. One can observe that higher permitted "));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 136, width = 477, height = 23;
        juce::String text (TRANS("point where the spatial correlation tends towards 0."));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 291, y = 208, width = 477, height = 23;
        juce::String text (TRANS("a wider frequency range of useful spherical harmonic components at each order."));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (11.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 673, y = 33, width = 119, height = 25;
        juce::String text (TRANS("Display:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 0, y = 0, width = 802, height = 2;
        juce::Colour strokeColour = juce::Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 0, y = 448, width = 802, height = 2;
        juce::Colour strokeColour = juce::Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 0, y = 0, width = 2, height = 450;
        juce::Colour strokeColour = juce::Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 798, y = 0, width = 2, height = 450;
        juce::Colour strokeColour = juce::Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 2);

    }

    {
        int x = 240, y = 305, width = 120, height = 30;
        juce::String text (TRANS("Window Overlap:"));
        juce::Colour fillColour = juce::Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    //[UserPaint] Add your own custom painting code here..

	g.setColour(Colours::white);
	g.setFont(Font(11.00f, Font::plain));
    g.drawText(TRANS("Ver ") + JucePlugin_VersionString + BUILD_VER_SUFFIX + TRANS(", Build Date ") + __DATE__ + TRANS(" "),
		385, 16, 530, 11,
		Justification::centredLeft, true);

    /* display warning message */
    g.setColour(Colours::red);
    g.setFont(Font(11.00f, Font::plain));
    switch (currentWarning){
        case k_warning_none:
            break;
        case k_warning_frameSize:
            g.drawText(TRANS("Set frame size to multiple of ") + String(array2sh_getFrameSize()),
                       getBounds().getWidth()-225, 16, 530, 11,
                       Justification::centredLeft, true);
            break;
        case k_warning_supported_fs:
            g.drawText(TRANS("Sample rate (") + String(array2sh_getSamplingRate(hA2sh)) + TRANS(") is unsupported"),
                       getBounds().getWidth()-225, 16, 530, 11,
                       Justification::centredLeft, true);
            break;
        case k_warning_NinputCH:
            g.drawText(TRANS("Insufficient number of input channels (") + String(hVst->getTotalNumInputChannels()) +
                       TRANS("/") + String(array2sh_getNumSensors(hA2sh)) + TRANS(")"),
                       getBounds().getWidth()-225, 16, 530, 11,
                       Justification::centredLeft, true);
            break;
        case k_warning_NoutputCH:
            g.drawText(TRANS("Insufficient number of output channels (") + String(hVst->getTotalNumOutputChannels()) +
                       TRANS("/") + String(array2sh_getNSHrequired(hA2sh)) + TRANS(")"),
                       getBounds().getWidth()-225, 16, 530, 11,
                       Justification::centredLeft, true);
            break;
    }

    //[/UserPaint]
}

void PluginEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..

    //[/UserResized]
}

void PluginEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]

    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == filterTypeCB.get())
    {
        //[UserComboBoxCode_filterTypeCB] -- add your combo box handling code here..
        array2sh_setFilterType(hA2sh, filterTypeCB->getSelectedId());
        needScreenRefreshFLAG = true;
        //[/UserComboBoxCode_filterTypeCB]
    }
    else if (comboBoxThatHasChanged == CHOrderingCB.get())
    {
        //[UserComboBoxCode_CHOrderingCB] -- add your combo box handling code here..
        array2sh_setChOrder(hA2sh, CHOrderingCB->getSelectedId());
        //[/UserComboBoxCode_CHOrderingCB]
    }
    else if (comboBoxThatHasChanged == normalisationCB.get())
    {
        //[UserComboBoxCode_normalisationCB] -- add your combo box handling code here..
        array2sh_setNormType(hA2sh, normalisationCB->getSelectedId());
        //[/UserComboBoxCode_normalisationCB]
    }
    else if (comboBoxThatHasChanged == dispWindow.get())
    {
        //[UserComboBoxCode_dispWindow] -- add your combo box handling code here..
        dispID = (DISP_WINDOW)dispWindow->getSelectedId();
        needScreenRefreshFLAG = 1;
        //[/UserComboBoxCode_dispWindow]
    }
    else if (comboBoxThatHasChanged == CBencodingOrder.get())
    {
        //[UserComboBoxCode_CBencodingOrder] -- add your combo box handling code here..
        int newOrder = CBencodingOrder->getSelectedId();
        array2sh_setEncodingOrder(hA2sh, newOrder);
        needScreenRefreshFLAG = true;
        //[/UserComboBoxCode_CBencodingOrder]
    }
    else if (comboBoxThatHasChanged == overlapCB.get())
    {
        //[UserComboBoxCode_overlapCB] -- add your combo box handling code here..
        String txt = overlapCB->getItemText(overlapCB->getSelectedItemIndex());
        String sub = txt.upToFirstOccurrenceOf(" ", false, true);
        float val = sub.getFloatValue();
        val = jlimit(0.f, 49.f, val);
        hVst->sarita.setOverlap(val);
        //[/UserComboBoxCode_overlapCB]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void PluginEditor::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == regAmountSlider.get())
    {
        //[UserSliderCode_regAmountSlider] -- add your slider handling code here..
        array2sh_setRegPar(hA2sh, (float)regAmountSlider->getValue());
        needScreenRefreshFLAG = true;
        //[/UserSliderCode_regAmountSlider]
    }
    else if (sliderThatWasMoved == gainSlider.get())
    {
        //[UserSliderCode_gainSlider] -- add your slider handling code here..
        array2sh_setGain(hA2sh, (float)gainSlider->getValue());
        //[/UserSliderCode_gainSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PluginEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton.get())
    {
        //[UserButtonCode_textButton] -- add your button handler code here..
        array2sh_setRequestEncoderEvalFLAG(hA2sh, 1);
        //[/UserButtonCode_textButton]
    }
    else if (buttonThatWasClicked == tb_loadCfg.get())
    {
        //[UserButtonCode_tb_loadCfg] -- add your button handler code here..
        chooser = std::make_unique<juce::FileChooser> ("Load configuration...",
                                                       hVst->getLastDir().exists() ? hVst->getLastDir() : File::getSpecialLocation (File::userHomeDirectory),
                                                       "*.cfg");
        auto chooserFlags = juce::FileBrowserComponent::openMode
                                  | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync (chooserFlags, [this] (const FileChooser& fc) {
            auto file = fc.getResult();
            if (file != File{}){
                hVst->setLastDir(file.getParentDirectory());
                hVst->newCfgFile = file;
                hVst->sarita.wantsConfigUpdate = true;
            }
        });
        //[/UserButtonCode_tb_loadCfg]
    }
    else if (buttonThatWasClicked == perform_SHT_btn.get())
    {
        //[UserButtonCode_perform_SHT_btn] -- add your button handler code here..
        hVst->setParameter(k_perform_sht, perform_SHT_btn.get()->getToggleState());
        //[/UserButtonCode_perform_SHT_btn]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PluginEditor::timerCallback(int timerID)
{
    switch (timerID) {
        case TIMER_PROCESSING_RELATED:
            /* handled in PluginProcessor */
            break;

        case TIMER_GUI_RELATED: {
            //            if (hVst->sarita.wantsConfigUpdate == true) {
            //                while(hVst->sarita.frameDone)
            //                    ;
            //                hVst->loadConfiguration (file);
            //            }

                        /* parameters whos values can change internally should be periodically refreshed */
            int curOrder = CBencodingOrder->getSelectedId();
            if (CBencodingOrder->getSelectedId() != array2sh_getEncodingOrder(hA2sh))
                CBencodingOrder->setSelectedId(array2sh_getEncodingOrder(hA2sh), dontSendNotification);

            //            // hack: force update overlap combo box => why doesn't that get updated automatically?
            //            String ovl = overlapCB->getItemText(overlapCB->getSelectedItemIndex());
            //            String sub = ovl.upToFirstOccurrenceOf(" ", false, true);
            //            float val = sub.getFloatValue();
            //            val = jlimit(0.f, 50.f, val);
            //            if (val != hVst->sarita.overlapPercent) {
            //                int id = 1;
            //                if (val < 25)
            //                    id = 0;
            //                else if (val > 25)
            //                    id = 2;
            //                overlapCB->setSelectedId(id);
            //            }
            if (!hVst->sarita.configError) {
                sensorCoordsView_handle->setQ(array2sh_getNumSensors(hA2sh));
                if (CHOrderingCB->getSelectedId() != array2sh_getChOrder(hA2sh))
                    CHOrderingCB->setSelectedId(array2sh_getChOrder(hA2sh), dontSendNotification);
                if (normalisationCB->getSelectedId() != array2sh_getNormType(hA2sh))
                    normalisationCB->setSelectedId(array2sh_getNormType(hA2sh), dontSendNotification);
                CHOrderingCB->setItemEnabled(CH_FUMA, array2sh_getEncodingOrder(hA2sh) == SH_ORDER_FIRST ? true : false);
                normalisationCB->setItemEnabled(NORM_FUMA, array2sh_getEncodingOrder(hA2sh) == SH_ORDER_FIRST ? true : false);
            }

            /* check if eval curves have recently been computed */
            if (array2sh_getEvalStatus(hA2sh) == EVAL_STATUS_RECENTLY_EVALUATED) {
                needScreenRefreshFLAG = true;
                array2sh_setEvalStatus(hA2sh, EVAL_STATUS_EVALUATED);
            }

            /* disable certain sliders if evaluation is ongoing */
            if (array2sh_getEvalStatus(hA2sh) == EVAL_STATUS_EVALUATING) {
                if (filterTypeCB->isEnabled())
                    filterTypeCB->setEnabled(false);
                if (regAmountSlider->isEnabled())
                    regAmountSlider->setEnabled(false);
                if (CBencodingOrder->isEnabled())
                    CBencodingOrder->setEnabled(false);
                if (sensorCoordsVP->isEnabled())
                    sensorCoordsVP->setEnabled(false);
            }
            else {
                if (!filterTypeCB->isEnabled())
                    filterTypeCB->setEnabled(true);
                if (!regAmountSlider->isEnabled())
                    regAmountSlider->setEnabled(true);
                if (!CBencodingOrder->isEnabled())
                    CBencodingOrder->setEnabled(true);
                if (!sensorCoordsVP->isEnabled())
                    sensorCoordsVP->setEnabled(true);
            }

            /* draw magnitude/spatial-correlation/level-difference curves */
            if (needScreenRefreshFLAG && !array2sh_getReinitSHTmatrixFLAG(hA2sh)) {
                switch (dispID) {
                default:
                case SHOW_EQ:
                    eqviewIncluded->setNumCurves(array2sh_getEncodingOrder(hA2sh) + 1);
                    eqviewIncluded->setVisible(true);
                    cohviewIncluded->setVisible(false);
                    ldiffviewIncluded->setVisible(false);
                    eqviewIncluded->repaint();
                    break;
                case SHOW_SPATIAL_COH:
                    eqviewIncluded->setVisible(false);
                    ldiffviewIncluded->setVisible(false);
                    if ((array2sh_getEvalStatus(hA2sh) == EVAL_STATUS_EVALUATED)) {
                        cohviewIncluded->setNumCurves(array2sh_getEncodingOrder(hA2sh) + 1);
                        cohviewIncluded->setVisible(true);
                        cohviewIncluded->repaint();
                    }
                    else
                        cohviewIncluded->setVisible(false);
                    break;
                case SHOW_LEVEL_DIFF:
                    eqviewIncluded->setVisible(false);
                    cohviewIncluded->setVisible(false);
                    if ((array2sh_getEvalStatus(hA2sh) == EVAL_STATUS_EVALUATED)) {
                        ldiffviewIncluded->setNumCurves(array2sh_getEncodingOrder(hA2sh) + 1);
                        ldiffviewIncluded->setVisible(true);
                        ldiffviewIncluded->repaint();
                    }
                    else
                        ldiffviewIncluded->setVisible(false);
                    break;
                }
                needScreenRefreshFLAG = false;
            }

            /* Progress bar */
            if (array2sh_getEvalStatus(hA2sh) == EVAL_STATUS_EVALUATING) {
                addAndMakeVisible(progressbar);
                progress = (double)array2sh_getProgressBar0_1(hA2sh);
                char text[PROGRESSBARTEXT_CHAR_LENGTH];
                array2sh_getProgressBarText(hA2sh, (char*)text);
                progressbar.setTextToDisplay(String(text));
            }
            else
                removeChildComponent(&progressbar);

            /* Hide decoding orders that are unsuitable for the current number of sensors */
            for (int i = 1; i <= 7; i++)
                CBencodingOrder->setItemEnabled(i, (i + 1) * (i + 1) <= array2sh_getNumSensors(hA2sh) ? true : false);

            /* display warning message, if needed */
            if ((hVst->getCurrentBlockSize() % array2sh_getFrameSize()) != 0) {
                currentWarning = k_warning_frameSize;
                repaint(0, 0, getWidth(), 32);
            }
            else if (!((array2sh_getSamplingRate(hA2sh) == 44.1e3) || (array2sh_getSamplingRate(hA2sh) == 48e3))) {
                currentWarning = k_warning_supported_fs;
                repaint(0, 0, getWidth(), 32);
            }
            else if (hVst->sarita.configError == false) { // needScreenRefreshFLAG &&
                auto txt = "Source Grid Order: " + String(hVst->sarita.N) + "\n";
                txt.append("Target Grid Order: " + String(hVst->sarita.NUpsampling) + "\n", 64);
                txt.append("Number of Sensors: " + String(hVst->sarita.denseGridSize) + "\n", 64);
                txtGrid->setText(txt);
                sensorCoordsView_handle->setUseDegreesInstead(true); // refreshCoords()
            }
            else if ((hVst->getCurrentNumInputs() < array2sh_getNumSensors(hA2sh))) {
                currentWarning = k_warning_NinputCH;
                repaint(0, 0, getWidth(), 32);
            }
            else if ((hVst->getCurrentNumOutputs() < array2sh_getNSHrequired(hA2sh))) {
                currentWarning = k_warning_NoutputCH;
                repaint(0, 0, getWidth(), 32);
            }
            else if (currentWarning) {
                currentWarning = k_warning_none;
                repaint(0, 0, getWidth(), 32);
            }
        } break; // case
    } // switch
}



//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginEditor" componentName=""
                 parentClasses="public AudioProcessorEditor, public MultiTimer"
                 constructorParams="PluginProcessor* ownerFilter" variableInitialisers="AudioProcessorEditor(ownerFilter) , progressbar(progress)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="800" initialHeight="450">
  <BACKGROUND backgroundColour="ffffffff">
    <RECT pos="0 240 800 210" fill="linear: 8 448, 8 352, 0=ff19313f, 1=ff041518"
          hasStroke="0"/>
    <RECT pos="0 30 800 210" fill="linear: 8 32, 8 128, 0=ff19313f, 1=ff041518"
          hasStroke="0"/>
    <ROUNDRECT pos="1 2 798 31" cornerSize="5.0" fill="linear: 0 32, 792 24, 0=ff041518, 1=ff19313f"
               hasStroke="1" stroke="2, mitered, butt" strokeColour="solid: ffb9b9b9"/>
    <RECT pos="228 56 556 209" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="280 70 456 158" fill="solid: 13000000" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="506 264 278 104" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="506 264 278 104" fill="solid: 8f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="506 367 278 69" fill="solid: 13f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="228 264 279 172" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="228 264 279 38" fill="solid: 8f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <RECT pos="12 56 204 384" fill="solid: 10f4f4f4" hasStroke="1" stroke="0.8, mitered, butt"
          strokeColour="solid: 67a0a0a0"/>
    <TEXT pos="520 268 172 30" fill="solid: ffffffff" hasStroke="0" text="Filter Approach:"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="520 301 172 30" fill="solid: ffffffff" hasStroke="0" text="Max Gain (dB):"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="520 369 172 30" fill="solid: ffffffff" hasStroke="0" text="Channel Order:"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="520 401 172 30" fill="solid: ffffffff" hasStroke="0" text="Normalisation:"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="520 334 172 30" fill="solid: ffffffff" hasStroke="0" text="Post Gain (dB):"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="24 57 88 30" fill="solid: ffffffff" hasStroke="0" text="Grid"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="440 30 149 30" fill="solid: ffffffff" hasStroke="0" text="Encoding Settings"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="240 268 172 30" fill="solid: ffffffff" hasStroke="0" text="Encoding Order:"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="16 1 100 32" fill="solid: ffffffff" hasStroke="0" text="SPARTA|"
          fontname="Default font" fontsize="18.8" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="92 1 244 32" fill="solid: ffe9ff00" hasStroke="0" text="Array2SH SARITA Upsampling"
          fontname="Default font" fontsize="18.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <TEXT pos="328 65 392 31" fill="solid: ffffffff" hasStroke="0" text="Press the &quot;Analyse&quot; button"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
          italic="0" justification="36"/>
    <TEXT pos="291 88 477 23" fill="solid: ffffffff" hasStroke="0" text="Corr: The spatial correlation is derived by comparing the patterns of the array responses with"
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 104 477 23" fill="solid: ffffffff" hasStroke="0" text="the patterns of ideal spherical harmonics, where '1' means they are perfect, and '0' completely "
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 120 477 23" fill="solid: ffffffff" hasStroke="0" text="uncorrelated; the spatial aliasing frequency can therefore be observed for each order, as the "
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 160 477 23" fill="solid: ffffffff" hasStroke="0" text="Ldiff: The level difference is the mean level difference over all directions (diffuse level differe-"
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 192 477 23" fill="solid: ffffffff" hasStroke="0" text="amplification limits [Max Gain (dB)] will result in noisier signals; however, this will also result in "
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 176 477 23" fill="solid: ffffffff" hasStroke="0" text="nce) between the ideal and simulated components. One can observe that higher permitted "
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 136 477 23" fill="solid: ffffffff" hasStroke="0" text="point where the spatial correlation tends towards 0."
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="291 208 477 23" fill="solid: ffffffff" hasStroke="0" text="a wider frequency range of useful spherical harmonic components at each order."
          fontname="Default font" fontsize="11.0" kerning="0.0" bold="0"
          italic="0" justification="33"/>
    <TEXT pos="673 33 119 25" fill="solid: ffffffff" hasStroke="0" text="Display:"
          fontname="Default font" fontsize="12.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
    <RECT pos="0 0 802 2" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="0 448 802 2" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="0 0 2 450" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <RECT pos="798 0 2 450" fill="solid: 61a52a" hasStroke="1" stroke="2, mitered, butt"
          strokeColour="solid: ffb9b9b9"/>
    <TEXT pos="240 305 120 30" fill="solid: ffffffff" hasStroke="0" text="Window Overlap:"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="1"
          italic="0" justification="33" typefaceStyle="Bold"/>
  </BACKGROUND>
  <COMBOBOX name="new combo box" id="d818d0d5310dc52a" memberName="filterTypeCB"
            virtualName="" explicitFocusOrder="0" pos="640 276 128 16" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <SLIDER name="new slider" id="9f4f4ac547d19161" memberName="regAmountSlider"
          virtualName="" explicitFocusOrder="0" pos="640 308 128 16" bkgcol="ff5c5d5e"
          trackcol="ff315b6d" min="0.0" max="80.0" int="0.01" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="55"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <COMBOBOX name="new combo box" id="44b90530e58253e" memberName="CHOrderingCB"
            virtualName="" explicitFocusOrder="0" pos="640 377 128 16" editable="0"
            layout="33" items="&#10;" textWhenNonSelected="ACN" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="caeee0fc74db72a4" memberName="normalisationCB"
            virtualName="" explicitFocusOrder="0" pos="640 409 128 16" editable="0"
            layout="33" items="&#10;" textWhenNonSelected="N3D" textWhenNoItems="(no choices)"/>
  <SLIDER name="new slider" id="ee4c42494881e7dc" memberName="gainSlider"
          virtualName="" explicitFocusOrder="0" pos="640 341 128 16" bkgcol="ff5c5d5e"
          trackcol="ff315b6d" min="-60.0" max="60.0" int="0.01" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="55"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TEXTBUTTON name="new button" id="dde3f82641b3717c" memberName="textButton"
              virtualName="" explicitFocusOrder="0" pos="228 39 60 14" bgColOff="ff5c68a4"
              buttonText="Analyse" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="new combo box" id="fb3d8d6828195921" memberName="dispWindow"
            virtualName="" explicitFocusOrder="0" pos="721 39 63 14" editable="0"
            layout="33" items="Filters&#10;Corr&#10;L Diff" textWhenNonSelected="Filters"
            textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="new button" id="527e24c6748d02d4" memberName="tb_loadCfg"
              virtualName="" explicitFocusOrder="0" pos="120 64 80 16" bgColOff="ff14889e"
              buttonText="Import" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="new combo box" id="a465903000494955" memberName="CBencodingOrder"
            virtualName="" explicitFocusOrder="0" pos="368 274 120 20" editable="0"
            layout="33" items="" textWhenNonSelected="Default" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="new combo box" id="846f2844ccf14242" memberName="overlapCB"
            virtualName="" explicitFocusOrder="0" pos="368 312 120 20" editable="0"
            layout="33" items="12.5 %&#10;25 %&#10;50 %" textWhenNonSelected="25 %"
            textWhenNoItems="25 %"/>
  <TEXTEDITOR name="new text editor" id="d784e0d737674a64" memberName="txtGrid"
              virtualName="" explicitFocusOrder="0" pos="24 96 176 64" bkgcol="4d2c3c6a"
              initialText="Import a config file first" multiline="1" retKeyStartsLine="0"
              readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <TOGGLEBUTTON name="AmbisonicsOutputBtn" id="ec36141ba8533172" memberName="perform_SHT_btn"
                virtualName="" explicitFocusOrder="0" pos="240 400 150 24" tooltip="If disabled, perform upsampling only, if enabled, the output are SH signlas."
                buttonText="Ambisonics Output" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

