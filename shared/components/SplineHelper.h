#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Morph/Source/ControlPoints.h"

namespace SplineHelper {
    Array<float> getAudioSplineValues(const ControlPoints& controlPoints);
    ValueTree pointsToValueTree(const ControlPoints& controlPoints, const Identifier& pointTreeId);
    Array<float> interpolate(const Array<float>& array1, const Array<float>& array2, float mix);
}
