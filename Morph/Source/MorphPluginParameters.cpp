#include "MorphPluginParameters.h"
#include "MorphInteractor.h"
#include "../../shared/components/SplineHelper.h"

const Identifier MorphPluginParameters::pointTree1Id("pointTree1");
const Identifier MorphPluginParameters::pointTree2Id("pointTree2");

MorphPluginParameters::MorphPluginParameters(AudioProcessor * processor) :
PluginParameters(processor),
m_didSetInitialAudioState(false),
m_controlPointComponent1(nullptr),
m_controlPointComponent2(nullptr),
m_listener(nullptr)
{
    m_interpolation = m_vts.getRawParameterValue(INTERPOLATION_ID);

    m_vts.addParameterListener(INTERPOLATION_ID, this);
}

AudioProcessorValueTreeState::ParameterLayout MorphPluginParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(INTERPOLATION_ID, INTERPOLATION_NAME, 0.0f, 1.0f, 0.0f));
    
    return { params.begin(), params.end() };
}


void MorphPluginParameters::parameterChanged(const String &parameterID, float newValue)
{
    triggerControlPointsChanged();
}

void MorphPluginParameters::controlPointsChanged(Array<float> controlPoints, ControlPointComponent* component) {
    if (component == m_controlPointComponent1) {
        m_controlPoints1.setPointsAndScale(component->getSourcePoints(), component->getBounds().getBottom());
    } else if (component == m_controlPointComponent2) {
        m_controlPoints2.setPointsAndScale(component->getSourcePoints(), component->getBounds().getBottom());
    }
    
    auto newState = copyState();
    PluginParameters::replaceState(newState);
    
    if(m_listener != nullptr) {
        m_listener->controlPointsChanged(m_controlPoints1, m_controlPoints2, m_interpolation->load());
    }
}

void MorphPluginParameters::replaceState(const ValueTree& newState) {
    PluginParameters::replaceState(newState);
    
    bool paramsEmptyBeforePopulate = m_controlPoints1.isEmpty() && m_controlPoints2.isEmpty();
    
    setPointsFromState(newState, m_controlPoints1, pointTree1Id);
    if(!m_controlPoints1.points.isEmpty() && m_controlPointComponent1 != nullptr) {
        m_controlPointComponent1->setSourcePoints(m_controlPoints1.points);
    }
    
    setPointsFromState(newState, m_controlPoints2, pointTree2Id);
    if(!m_controlPoints2.points.isEmpty() && m_controlPointComponent2 != nullptr) {
        m_controlPointComponent2->setSourcePoints(m_controlPoints2.points);
    }

    bool paramsDidChange = paramsEmptyBeforePopulate && (!m_controlPoints1.isEmpty() || !m_controlPoints2.isEmpty());
    if(!m_didSetInitialAudioState || paramsDidChange) {
        m_didSetInitialAudioState = true;
        this->triggerControlPointsChanged();
    }
}

void MorphPluginParameters::triggerControlPointsChanged() {
    if(m_listener != nullptr && !m_controlPoints1.points.isEmpty() && !m_controlPoints2.points.isEmpty()) {
        m_listener->controlPointsChanged(m_controlPoints1, m_controlPoints2, m_interpolation->load());
    };
}

ValueTree MorphPluginParameters::copyState() {
    ValueTree tree = PluginParameters::copyState();
    
    if (tree.getChildWithName(pointTree1Id).isValid()) {
        tree.removeChild(tree.getChildWithName(pointTree1Id), nullptr);
    }
    
    if (tree.getChildWithName(pointTree2Id).isValid()) {
        tree.removeChild(tree.getChildWithName(pointTree2Id), nullptr);
    }
    
    auto pointTree1 = SplineHelper::pointsToValueTree(m_controlPoints1, pointTree1Id);
    if(pointTree1.isValid()) {
        tree.addChild(pointTree1, -1, nullptr);
    }
    
    auto pointTree2 = SplineHelper::pointsToValueTree(m_controlPoints2, pointTree2Id);
    if(pointTree2.isValid()) {
        tree.addChild(pointTree2, -1, nullptr);
    }
    
    return tree;
}

void MorphPluginParameters::setControlPointComponents(ControlPointComponent* component1, ControlPointComponent* component2) {
    component1->setListener(this);
    m_controlPointComponent1 = component1;
    setPointsFromState(getState(), m_controlPoints1, pointTree1Id);

    if(!m_controlPoints1.points.isEmpty()) {
        component1->setSourcePoints(m_controlPoints1.points);
    }

    component2->setListener(this);
    m_controlPointComponent2 = component2;
    setPointsFromState(getState(), m_controlPoints2, pointTree2Id);
    
    if(!m_controlPoints2.points.isEmpty()) {
        component2->setSourcePoints(m_controlPoints2.points);
    }
}

void MorphPluginParameters::setPointsFromState(const ValueTree& state, ControlPoints &controlPoints, const Identifier& pointTreeId) {
    auto pointTree = state.getChildWithName(pointTreeId);
    if(!pointTree.isValid()) {
        controlPoints.clear();
        return;
    }
    
    int yLimit;
    if (pointTree.hasProperty("ylimit"))
    {
        yLimit = pointTree.getPropertyAsValue("ylimit", nullptr).getValue();
    }
    else if(m_controlPointComponent1 != nullptr) {
        yLimit = m_controlPointComponent1->getLocalBounds().getBottom();
    }
    else {
        controlPoints.clear();
        return;
    }
    
    Array<juce::Point<int>> points;
    for(int i=0; i<pointTree.getNumChildren(); ++i) {
        auto childPoint = pointTree.getChild(i);
        auto xValue = childPoint.getPropertyAsValue("x", nullptr);
        auto yValue = childPoint.getPropertyAsValue("y", nullptr);
        juce::Point<int> point;
        
        point.setX(xValue.getValue());
        point.setY(yValue.getValue());
        
        points.add(point);
    }
    
    controlPoints.setPointsAndScale(points, yLimit);
}
