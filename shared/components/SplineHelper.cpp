#include "SplineHelper.h"
#include "Spline.h"

namespace SplineHelper {
Array<float> getAudioSplineValues(const ControlPoints& controlPoints) {
    Array<float> outputValues;
    
    if(controlPoints.points.size() < 2) { return outputValues; }
    
    const int resolution = 128;
    auto pStart = controlPoints.points.getFirst();
    auto pEnd = controlPoints.points.getLast();
    
    int xRange = pEnd.x - pStart.x;
    float increment = (float)xRange / (float)resolution;
    
    Array<juce::Point<double>> dpoints;
    if (controlPoints.points.size() >= 3) {
        for (auto p : controlPoints.points)
            dpoints.add ({ double (p.getX()), double (p.getY())});
    }
    else {
        dpoints.add({double (pStart.getX()), double (pStart.getY())});
        
        // add a mid point so that we have over 3 points to satisfy the Spline interpolator
        int yRange = pEnd.y - pStart.y;
        double midX = (double)pEnd.getX() - ((double)xRange / 2.0);
        double midY = (double)pEnd.getY() - ((double)yRange / 2.0);
        
        dpoints.add({midX, midY});
        dpoints.add({double (pEnd.getX()), double (pEnd.getY())});
    }
    
    outputValues.clearQuick();
    Spline spline (dpoints);
    double bottom = (double)controlPoints.yScale;
    if(bottom == 0) { return outputValues; }
    
    for (float x = (float)controlPoints.points.getFirst().getX(); x < (float)controlPoints.points.getLast().getX(); x += increment)
    {
        if(outputValues.size() >= resolution) {
            break;
        }
        
        double y = spline.interpolate (x);
        if(y < 0.0) {
            y = 0.0;
        }
        else if(y > bottom) {
            y = bottom;
        }
        
        outputValues.add((bottom - float(y)) / bottom);
    }
    
    return outputValues;
}

ValueTree pointsToValueTree(const ControlPoints& controlPoints, const Identifier& pointTreeId)
{
    if(controlPoints.points.isEmpty()) {
        return ValueTree();
    }

    ValueTree pointTree(pointTreeId);
    pointTree.setProperty("ylimit", controlPoints.yScale, nullptr);

    for(auto& point : controlPoints.points) {
        ValueTree valuePoint ("point");
        valuePoint.setProperty("x", point.x, nullptr);
        valuePoint.setProperty("y", point.y, nullptr);
        pointTree.appendChild(valuePoint, nullptr);
    }

    return pointTree;
}

Array<float> interpolate(const Array<float>& array1, const Array<float>& array2, float mix)
{
    Array<float> result;

    jassert(array1.size() == array2.size());

    for (int i = 0; i < array1.size(); ++i) {
        result.add(jmap(mix, array1[i], array2[i]));
    }

    return result;
}

}
