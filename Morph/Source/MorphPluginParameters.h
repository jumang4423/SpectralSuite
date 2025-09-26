#pragma once

#include "../../shared/PluginParameters.h"
#include "../../shared/components/ControlPointComponent.h"
#include "ControlPoints.h"

#include <atomic>

#define INTERPOLATION_ID "interpolation"
#define INTERPOLATION_NAME "Interpolation"

class MorphInteractor;

class MorphPluginParameters : public PluginParameters, public ControlPointComponent::Listener {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void controlPointsChanged(const ControlPoints& controlPoints1, const ControlPoints& controlPoints2, float interpolation) = 0;
    };

    MorphPluginParameters(AudioProcessor* processor);

    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void replaceState(const ValueTree& newState) override;
    ValueTree copyState() override;

    void setListener(Listener* listener){this->listener = listener;}
    void setControlPointComponents(ControlPointComponent* component1, ControlPointComponent* component2);
    void triggerControlPointsChanged();
    
    void controlPointsChanged (Array<float> controlPoints, ControlPointComponent* component) override;
    
    const ControlPoints& getControlPoints1() const { return m_controlPoints1; }
    const ControlPoints& getControlPoints2() const { return m_controlPoints2; }

private:
    void setPointsFromState(const ValueTree& state, ControlPoints &controlPoints, const Identifier& pointTreeId);
    
    ControlPoints m_controlPoints1;
    ControlPoints m_controlPoints2;
    
    Listener* m_listener;
    bool m_didSetInitialAudioState;
    
    ControlPointComponent* m_controlPointComponent1;
    ControlPointComponent* m_controlPointComponent2;
    
    std::atomic<float>* m_interpolation;
};
