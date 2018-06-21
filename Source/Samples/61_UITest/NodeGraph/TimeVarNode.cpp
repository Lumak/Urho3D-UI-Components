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

#include "TimeVarNode.h"
#include "TimeVarInput.h"
#include "InputNode.h"
#include "OutputNode.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void TimeVarNode::RegisterObject(Context* context)
{
    context->RegisterFactory<TimeVarNode>(UI_CATEGORY);
}

TimeVarNode::TimeVarNode(Context *context) 
    : GraphNode(context)
{
    timeVarInput_ = CreateChild<TimeVarInput>();
    outputNode_   = CreateChild<OutputNode>();
}

TimeVarNode::~TimeVarNode()
{
}

bool TimeVarNode::CreateTimeVarInput(const String &variableName, const IntVector2 &size)
{
    if ( !timeVarInput_->Create(variableName, size) )
    {
        return false;
    }

    if ( !outputNode_->Create(variableName, GraphNode::GetDefaultIONodeSize()) )
    {
        return false;
    }

    return true;
}

void TimeVarNode::SetScreenColor(const Color &color)
{
    timeVarInput_->SetColor(color);
}

bool TimeVarNode::InitDataCurvePoints(const PODVector<Vector2> &points)
{
    return timeVarInput_->InitDataCurvePoints(points);
}

void TimeVarNode::SetValueRange(float rmin, float rmax)
{
    timeVarInput_->SetValueRange(rmin, rmax);
}

void TimeVarNode::SetTimeRange(float mintime, float maxtime)
{
    timeVarInput_->SetTimeRange(mintime, maxtime);
}

bool TimeVarNode::ConnectToInput(InputNode *inputNode)
{
    return outputNode_->ConnectToInput(inputNode);
}

void TimeVarNode::SetEnableCtrlButton(bool enable)
{
    outputNode_->SetEnableCtrlButton(enable);
}


