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
#include "GraphNode.h"
#include "SlideVarInput.h"

class OutputNode;

//=============================================================================
//=============================================================================
class SlideVarNode : public GraphNode
{
    URHO3D_OBJECT(SlideVarNode, GraphNode);
public:
    static void RegisterObject(Context* context);

    SlideVarNode(Context *context);
    virtual ~SlideVarNode();

    bool CreateBar(const String &variableName, const IntVector2 &size, bool showOutput);
    void SetBarColor(const Color &color);

    SlideVarInput* GetSlideVarElement() { return slideVarInput_; }
    OutputNode* GetOutputNodeElement()  { return outputNode_;    }

    void SetRange(const Variant &vmin, const Variant &vmax);
    void SetCurrentValue(const Variant &val);
    void SetSensitivity(float sensitivity);
    void SetVarChangedCallback(UIElement *process, VarChangedCallback callback);

    const Variant& GetCurrentValue() { return slideVarInput_->GetCurrentValue(); }

protected:
    WeakPtr<SlideVarInput> slideVarInput_;
    WeakPtr<OutputNode>    outputNode_;
};


