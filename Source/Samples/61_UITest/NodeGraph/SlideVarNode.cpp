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
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>

#include "SlideVarNode.h"
#include "OutputNode.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void SlideVarNode::RegisterObject(Context* context)
{
    context->RegisterFactory<SlideVarNode>();
}

SlideVarNode::SlideVarNode(Context *context)
    : GraphNode(context)
{
    slideVarInput_ = CreateChild<SlideVarInput>();
    outputNode_    = CreateChild<OutputNode>();
}

SlideVarNode::~SlideVarNode()
{
}

bool SlideVarNode::CreateBar(const String &variableName, const IntVector2 &size, bool showOutput)
{
    slideVarInput_->CreateBar(variableName, size);

    if ( showOutput )
    {
        outputNode_->Create(variableName, GraphNode::GetDefaultIONodeSize());
    }

    return true;
}

void SlideVarNode::SetBarColor(const Color &color)
{
    slideVarInput_->SetColor(color);
}

void SlideVarNode::SetRange(const Variant &vmin, const Variant &vmax)
{
    slideVarInput_->SetRange(vmin, vmax);
}

void SlideVarNode::SetCurrentValue(const Variant &val)
{
    slideVarInput_->SetCurrentValue(val);
}

void SlideVarNode::SetSensitivity(float sensitivity)
{
    slideVarInput_->SetSensitivity(sensitivity);
}

void SlideVarNode::SetVarChangedCallback(UIElement *process, VarChangedCallback callback)
{
    slideVarInput_->SetVarChangedCallback(process, callback);
}



