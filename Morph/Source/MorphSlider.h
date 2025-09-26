#pragma once

#include "JuceHeader.h"
#include "MorphPluginParameters.h"
#include "../../shared/SettingsPage.h"
#include "../../shared/ParameterContainerComponent.h"
#include "../../shared/components/ControlPointComponent.h"

class MorphSlider : public ParameterContainerComponent
{
public:
    MorphSlider(std::shared_ptr<MorphPluginParameters> valueTreeState, Colour textColour, int textBoxHeight);
    ~MorphSlider();

    void paint (Graphics&) override;
    void resized() override;

    std::shared_ptr<PluginParameters> getPluginParameters() override { return m_pluginParameters; }
    const int getComponentHeight() override { return 240; }
    void onFftSizeChanged() override {
        m_controlPointComponent1.notifyChanged();
        m_controlPointComponent2.notifyChanged();
    }

private:
    std::shared_ptr<MorphPluginParameters> m_pluginParameters;
    ControlPointComponent m_controlPointComponent1;
    ControlPointComponent m_controlPointComponent2;

    Slider m_interpolationSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m_interpolationAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MorphSlider)
};
