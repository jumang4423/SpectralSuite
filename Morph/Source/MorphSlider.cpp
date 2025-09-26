#include "../JuceLibraryCode/JuceHeader.h"
#include "MorphSlider.h"

MorphSlider::MorphSlider(std::shared_ptr<MorphPluginParameters> valueTreeState, Colour textColour, int textBoxHeight)
{
    m_pluginParameters = valueTreeState;

    addAndMakeVisible(m_controlPointComponent1);
    addAndMakeVisible(m_controlPointComponent2);

    m_pluginParameters->setControlPointComponents(&m_controlPointComponent1, &m_controlPointComponent2);

    addAndMakeVisible(m_interpolationSlider);
    m_interpolationSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    m_interpolationAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(m_pluginParameters->getVts(), INTERPOLATION_ID, m_interpolationSlider);
}

MorphSlider::~MorphSlider()
{
}

void MorphSlider::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void MorphSlider::resized()
{
    InitialPoint p1(0.0, 1.0);
    p1.lockX = true;
    p1.lockY = true;
    
    InitialPoint p2(1.0, 0.0);
    p2.lockX = true;
    p2.lockY = true;
    
    Array<InitialPoint> initialPoints(p1, p2);

    m_controlPointComponent1.setInitialPoints(initialPoints);
    auto lastPoints1 = m_pluginParameters->getControlPoints1();
    if(!lastPoints1.points.isEmpty()) {
        m_controlPointComponent1.setSourcePoints(lastPoints1.points);
    }
    
    m_controlPointComponent2.setInitialPoints(initialPoints);
    auto lastPoints2 = m_pluginParameters->getControlPoints2();
    if(!lastPoints2.points.isEmpty()) {
        m_controlPointComponent2.setSourcePoints(lastPoints2.points);
    }

    auto bounds = getLocalBounds();
    auto graphWidth = bounds.getWidth() * 0.8;

    m_controlPointComponent1.setBounds(0, 0, graphWidth, bounds.getHeight() / 2);
    m_controlPointComponent2.setBounds(0, bounds.getHeight() / 2, graphWidth, bounds.getHeight() / 2);

    m_interpolationSlider.setBounds(graphWidth, 0, 30, bounds.getHeight());
}
