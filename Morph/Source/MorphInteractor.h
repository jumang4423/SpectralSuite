#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../shared/StandardFFTProcessor.h"
#include "../../shared/SpectralAudioProcessorInteractor.h"
#include "MorphPluginParameters.h"
#include "../../shared/components/SplineHelper.h"

class MorphInteractor : public SpectralAudioProcessorInteractor, MorphPluginParameters::Listener {
public:
    MorphInteractor(int numOverlaps, std::shared_ptr<MorphPluginParameters> params);

    void prepareProcess(StandardFFTProcessor* spectralProcessor) override;
    std::unique_ptr<StandardFFTProcessor> createSpectralProcess(int index, int fftSize, int hopSize, int sampleRate, int numOverlaps, int chan, int numChans) override;
    void controlPointsChanged(const ControlPoints& controlPoints1, const ControlPoints& controlPoints2, float interpolation) override;
    void onFftSizeChanged() override;

private:
    juce::Array<int> pointsArray1;
    juce::Array<int> pointsArray2;
    
    juce::Array<int>* pWriteArray;
    juce::Array<int>* pReadArray;
    bool m_morphPointsChanged;
    
    std::shared_ptr<MorphPluginParameters> m_morphParams;

    int m_halfFftSize;
};
