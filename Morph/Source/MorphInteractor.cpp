#include "MorphInteractor.h"
#include "MorphFFTProcessor.h"
#include "../../shared//utilities.h"

MorphInteractor::MorphInteractor(int numOverlaps, std::shared_ptr<MorphPluginParameters> params) :
    SpectralAudioProcessorInteractor(numOverlaps),
    m_morphParams(params),
    m_halfFftSize(0)
{
    params->setListener(this);
    m_morphPointsChanged = false;

    pReadArray = &pointsArray1;
    pWriteArray = &pointsArray2;
}

void MorphInteractor::onFftSizeChanged()
{
    m_halfFftSize = this->getFftSize() / 2;
}

void MorphInteractor::prepareProcess(StandardFFTProcessor* spectralProcessor)
{
    if(m_morphPointsChanged) {
        m_morphPointsChanged = false;
        std::swap(pReadArray, pWriteArray);
    }
    else if(pReadArray->size() != m_halfFftSize)
    {
        m_morphParams->triggerControlPointsChanged();
    }
}

std::unique_ptr<StandardFFTProcessor> MorphInteractor::createSpectralProcess(int index, int fftSize, int hopSize, int sampleRate, int numOverlaps, int chan, int numChans)
{
    m_halfFftSize = fftSize / 2;
    return std::make_unique<MorphFFTProcessor>(fftSize, hopSize, hopSize * (index%numOverlaps), (int)sampleRate, this->getPhaseBuffer(), &pReadArray);
}

void MorphInteractor::controlPointsChanged(const ControlPoints& controlPoints1, const ControlPoints& controlPoints2, float interpolation) {
    m_morphPointsChanged = false;
    
    auto audioValues1 = SplineHelper::getAudioSplineValues(controlPoints1);
    auto audioValues2 = SplineHelper::getAudioSplineValues(controlPoints2);

    if (audioValues1.isEmpty() || audioValues2.isEmpty()) {
        return;
    }

    auto controlPoints = SplineHelper::interpolate(audioValues1, audioValues2, interpolation);

    auto controlPointsSize = controlPoints.size();
    
    if(m_halfFftSize == 0 || controlPointsSize == 0) { return; }
    if(m_halfFftSize == controlPointsSize) {
        
        Array<int> fftRangedControlPoints;
        for(int i=0; i<controlPointsSize; ++i) {
            fftRangedControlPoints.add(controlPoints[i] * m_halfFftSize);
        }
        
        *pWriteArray = fftRangedControlPoints;
    }
    else if(m_halfFftSize < controlPointsSize) {
        auto skip = m_halfFftSize / controlPointsSize;
        
        Array<int> compactedOutput;
        for(int i=0; i<controlPointsSize; i += skip) {
            float value = controlPoints[i];
            compactedOutput.add(value * m_halfFftSize);
        }
        
        *pWriteArray = compactedOutput;
    }
    else /* fftSize > controlPointSize */ {
        
        Array<int> expandedOutput;
        auto indexScale = (float)controlPointsSize / (float)m_halfFftSize;
        
        for(int i=0; i<m_halfFftSize; ++i) {
            float controlPointIndex = (float)i * indexScale;
            int indA = (int)controlPointIndex;
            int indB = indA + 1;
            
            float aValue = controlPoints[indA];
            if(indB >= controlPointsSize) {
                expandedOutput.add(aValue * m_halfFftSize);
            } else {
                float bValue = controlPoints[indB];
                float value = utilities::interp_lin(aValue, bValue, controlPointIndex);
                expandedOutput.add(value * m_halfFftSize);
            }
        }
        
        *pWriteArray = expandedOutput;
    }
    
    m_morphPointsChanged = true;
};
