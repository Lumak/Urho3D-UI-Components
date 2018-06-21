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
#include "IOElement.h"

//=============================================================================
//=============================================================================
URHO3D_EVENT(E_RECEIVER_MOVED, ReceiverMoved)
{
    URHO3D_PARAM(P_ELEMENT, Element);  // UIElement pointer
}
class OutputNode;
class InputBox;

//=============================================================================
//=============================================================================
class InputNode : public IOElement
{
    URHO3D_OBJECT(InputNode, IOElement);
public:
    static void RegisterObject(Context* context);

    InputNode(Context *context);
    virtual ~InputNode();

    bool Create(const String &variableName, const IntVector2 &size);
    void ShowInputBox(bool show) { showInputBox_ = show; }
    InputBox* GetInputBox() { return inputBox_; }

    void SetConnectedOutputNode(OutputNode *outputNode);
    OutputNode* GetConnectedOutputNode() { return connectedOutputNode_; }

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

    bool Init(const IntVector2 &pos, const IntVector2 &size);
    bool CreateInputbox();

    void HandleBaseDragMove(StringHash eventType, VariantMap& eventData);
    void HandleLayoutUpdated(StringHash eventType, VariantMap& eventData);
    void SendReceiverMovedEvent();

protected:
    WeakPtr<OutputNode> connectedOutputNode_;
    String              connectedOutputVarName_;

    WeakPtr<InputBox>   inputBox_;
    IntVector2          controlBoxSize_;
    bool                showInputBox_;
};

//=============================================================================
//=============================================================================
class InputBox : public BorderImage
{
    URHO3D_OBJECT(InputBox, BorderImage);
public:
    static void RegisterObject(Context* context)
    {
        context->RegisterFactory<InputBox>(UI_CATEGORY);
    }

    InputBox(Context *context) : BorderImage(context){}
    virtual ~InputBox(){}

    void SetInputParent(InputNode *parent)  { inputParent_ = parent; }
    InputNode* GetInputParent()             { return inputParent_;   }

protected:
    WeakPtr<InputNode> inputParent_;
};

