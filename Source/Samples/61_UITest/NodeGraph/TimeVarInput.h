//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once
#include <Urho3D/Core/Spline.h>

#include "IOElement.h"
#include "LineBatcher.h"

//=============================================================================
//=============================================================================
enum MaxPoint
{
    MAX_POINTS = 5
};

//=============================================================================
//=============================================================================
class TimeVarInput : public IOElement
{
    URHO3D_OBJECT(TimeVarInput, GraphNode);
public:
    static void RegisterObject(Context* context);

    TimeVarInput(Context *context);
    virtual ~TimeVarInput();

    virtual bool Create(const String &variableName, const IntVector2 &size);

    virtual float GetValueRangeMin(const String &varName);
    virtual float GetValueRangeMax(const String &varName);
    virtual float GetStartTime(const String &varName);
    virtual float GetEndTime(const String &varName);
    virtual float GetValueAtTime(const String &varName, float time);

    bool InitDataCurvePoints(const PODVector<Vector2> &points);
    void SetValueRange(float rmin, float rmax);
    void SetTimeRange(float mintime, float maxtime);

protected:
    bool InitInternal();
    bool InitScreen(const IntVector2 &size);
    Text* CreateText(int size, const IntVector2 &pos);
    bool CreateButtons();
    bool CreateLineBatcher(LineType linetype, const Color& color, float pixelSize);

    void EvaluateCurvePoints(const PODVector<Vector2> &points, PODVector<IntVector2> &outPoints);
    void CurvePointsNormalized(const PODVector<Vector2> &points, PODVector<IntVector2> &scrnPoints);

    void HandleBaseDragMove(StringHash eventType, VariantMap& eventData);
    void HandleButtonDragMove(StringHash eventType, VariantMap& eventData);
    void HandleLayoutUpdated(StringHash eventType, VariantMap& eventData);

    void  UpdateDrawLine();
    float GetValueAtTime(float time);

protected:
    WeakPtr<Text>         textTitle_;
    WeakPtr<Text>         textMinValue_;
    WeakPtr<Text>         textMaxValue_;
    WeakPtr<Text>         textTimeStart_;
    WeakPtr<Text>         textTimeEnd_;

    WeakPtr<LineBatcher>  lineBatcher_;
    Vector<Button*>       buttonList_;
    LineType              linetype_;
    float                 pixelSize_;

    PODVector<IntVector2> absolutePositionList_;
    PODVector<IntVector2> pointList_;
    IntVector2            controlBoxSize_;

    Spline                spline_;

    float                 minValue_;
    float                 maxValue_;
    float                 valueRange_;

    float                 timeStart_;
    float                 timeEnd_;
    float                 timeRange_;
};


