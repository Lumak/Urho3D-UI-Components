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
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <SDL/SDL_log.h>

#include "GraphNode.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "InputNodeManager.h"
#include "PageManager.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void InputNode::RegisterObject(Context* context)
{
    context->RegisterFactory<InputNode>(UI_CATEGORY);
    InputBox::RegisterObject(context);
}

InputNode::InputNode(Context *context)
    : IOElement(context)
    , showInputBox_(true)
{
    SetIOType(IOTYPE_INPUT);
    SetColor(GraphNode::GetDefaultBodyColor());
}

InputNode::~InputNode()
{
    GetSubsystem<InputNodeManager>()->Remove(inputBox_);
}

bool InputNode::InitInternal()
{
    return true;
}

bool InputNode::Create(const String &variableName, const IntVector2 &size)
{
    InitBaseNodeParent();

    CreateTextLabel();

    SetVariableName(variableName);

    if ( !CreateInputbox() )

        return false;

    SetFixedSize(size);

    return true;
}

bool InputNode::CreateInputbox()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = LineBatcher::GetBoxRect();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    controlBoxSize_ = LineBatcher::GetBoxSize();

    IntVector2 absPos = GetScreenPosition();
    absPos.x_ -= controlBoxSize_.x_ + 2;

    // inputBox_
    inputBox_ = root->CreateChild<InputBox>();
    inputBox_->SetTexture(tex2d);
    inputBox_->SetImageRect(rect);
    inputBox_->SetPosition(absPos);
    inputBox_->SetColor(Color(1,1,0));
    inputBox_->SetSize(controlBoxSize_);
    inputBox_->SetEnabled(true);
    inputBox_->SetInputParent(this);
    inputBox_->SetName("InputRx");

    // store it
    GetSubsystem<InputNodeManager>()->Add(inputBox_);

    SubscribeToEvent(GetNodeBasePtr(), E_BASE_DRAGMOVE, URHO3D_HANDLER(InputNode, HandleBaseDragMove));
    SubscribeToEvent(GetNodeBasePtr(), E_LAYOUTUPDATED, URHO3D_HANDLER(InputNode, HandleLayoutUpdated));

    return true;
}

void InputNode::HandleBaseDragMove(StringHash eventType, VariantMap& eventData)
{
    IntVector2 absPos = GetScreenPosition();
    absPos.x_ -= controlBoxSize_.x_ + 2;
    inputBox_->SetPosition(absPos);

    SendReceiverMovedEvent();
}

void InputNode::HandleLayoutUpdated(StringHash eventType, VariantMap& eventData)
{
    IntVector2 absPos = GetScreenPosition();
    absPos.x_ -= controlBoxSize_.x_ + 2;
    inputBox_->SetPosition(absPos);

    SendReceiverMovedEvent();
}

void InputNode::SendReceiverMovedEvent()
{
    using namespace ReceiverMoved;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = inputBox_;
    SendEvent(E_RECEIVER_MOVED, eventData);
}

void InputNode::SetConnectedOutputNode(OutputNode *outputNode)
{
    connectedOutputNode_ = outputNode;
    connectedOutputVarName_.Clear();

    if (connectedOutputNode_)
    {
        connectedOutputVarName_ = connectedOutputNode_->GetVariableName();
    }
}

//=========================================================
// related to timevar input
float InputNode::GetValueRangeMin(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetValueRangeMin(connectedOutputVarName_);
    }

    return 0.0f;
}

float InputNode::GetValueRangeMax(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetValueRangeMax(connectedOutputVarName_);
    }

    return 0.0f;
}

float InputNode::GetStartTime(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetStartTime(connectedOutputVarName_);
    }

    return 0.0f;
}

float InputNode::GetEndTime(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetEndTime(connectedOutputVarName_);
    }

    return 0.0f;
}

float InputNode::GetValueAtTime(const String &varName, float time)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetValueAtTime(connectedOutputVarName_, time);
    }

    return 0.0f;
}


// related to slidevar input
const Variant& InputNode::GetRangeMin(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetRangeMin(connectedOutputVarName_);
    }

    return Variant::EMPTY;
}

const Variant& InputNode::GetRangeMax(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetRangeMax(connectedOutputVarName_);
    }

    return Variant::EMPTY;
}

const Variant& InputNode::GetCurrentValue(const String &varName)
{
    if ( varName == variableName_  && connectedOutputNode_)
    {
        return connectedOutputNode_->GetCurrentValue(connectedOutputVarName_);
    }

    return Variant::EMPTY;
}

