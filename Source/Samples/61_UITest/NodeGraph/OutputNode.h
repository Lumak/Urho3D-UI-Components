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
#include "InputNode.h"

class OutputBox;
//=============================================================================
//=============================================================================
class OutputNode : public IOElement
{
    URHO3D_OBJECT(IOElement, GraphNode);
public:
    static void RegisterObject(Context* context);

    OutputNode(Context *context);
    virtual ~OutputNode();

    virtual bool Create(const String &variableName, const IntVector2 &size);
    void ShowOutputLine(bool show) { showOutputLine_ = show; }

    bool ConnectToInput(InputNode *inputNode);
    void SetEnableCtrlButton(bool enable) { ctrlButton_->SetEnabled(enable); }

    // related to timevar input
    virtual float GetValueRangeMin(const String &varName);
    virtual float GetValueRangeMax(const String &varName);
    virtual float GetStartTime(const String &varName);
    virtual float GetEndTime(const String &varName);
    virtual float GetValueAtTime(const String &varName, float time);

    // related to slidevar input
    virtual const Variant& GetRangeMin(const String &varName);
    virtual const Variant& GetRangeMax(const String &varName);
    virtual const Variant& GetCurrentValue(const String &varName);

protected:
    bool InitInternal();
    bool CreateLineBatcher(LineType linetype, const Color& color, float pixelSize);

    void HandleBaseDragMove(StringHash eventType, VariantMap& eventData);

    // button
    void HandleButtonDragBegin(StringHash eventType, VariantMap& eventData);
    void HandleButtonDragMove(StringHash eventType, VariantMap& eventData);
    void HandleButtonDragEnd(StringHash eventType, VariantMap& eventData);
    void HandleReceiverMoved(StringHash eventType, VariantMap& eventData);
    void HandleLayoutUpdated(StringHash eventType, VariantMap& eventData);
    bool AttemptConnect(InputNode *inputNode);
    void CreateLinePoints(const IntVector2 &pos0, const IntVector2 &pos4);
    void SnapToInputNode();
    void CalculateInnerPoints();

protected:
    WeakPtr<OutputBox>    outputBox_;
    WeakPtr<Button>       ctrlButton_;
    WeakPtr<InputNode>    connectedInputNode_;

    WeakPtr<LineBatcher>  lineBatcher_;
    PODVector<IntVector2> absolutePositionList_;

    IntVector2            controlBoxSize_;
    LineType              linetype_;
    float                 pixelSize_;
    bool                  showOutputLine_;

protected:
    enum PointSizeType{ MAX_POINTS = 5 };
};

//=============================================================================
//=============================================================================
class OutputBox : public BorderImage
{
    URHO3D_OBJECT(OutputBox, BorderImage);
public:
    static void RegisterObject(Context* context)
    {
        context->RegisterFactory<OutputBox>(UI_CATEGORY);
    }

    OutputBox(Context *context) : BorderImage(context){}
    virtual ~OutputBox(){}

    void SetOutputParent(OutputNode *parent)  { outputParent_ = parent; }
    OutputNode* GetOutputParent()            { return outputParent_;   }

protected:
    WeakPtr<OutputNode> outputParent_;
};




