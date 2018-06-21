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

#include "OutputNode.h"
#include "PageManager.h"
#include "InputNodeManager.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
#define MIN_X_DIST        0.3f
#define FRACTION_LEN      0.2f
#define MIN_BEND_LEN     20.0f
#define MAX_BEND_LEN    100.0f

const Color LINEColor(0.0f, 0.8f, 0.8f);
const Color CONNECTEDColor(0.3f, 0.8f, 0.3f);
const Color DISCONNECTEDColor(0.8f, 0.3f, 0.3f);

//=============================================================================
//=============================================================================
void OutputNode::RegisterObject(Context* context)
{
    context->RegisterFactory<OutputNode>(UI_CATEGORY);
    OutputBox::RegisterObject(context);
}

OutputNode::OutputNode(Context *context)
    : IOElement(context)
    , showOutputLine_(true)
{
    SetIOType(IOTYPE_OUTPUT);
    SetColor(GraphNode::GetDefaultBodyColor());
}

OutputNode::~OutputNode()
{
}

bool OutputNode::InitInternal()
{
    InitBaseNodeParent();

    CreateTextLabel();
    SetHorizontalAlignment(HA_RIGHT);

    return true;
}

bool OutputNode::Create(const String &variableName, const IntVector2 &size)
{
    InitBaseNodeParent();

    CreateTextLabel();
    SetVariableName(variableName);
    SetHorizontalAlignment(HA_RIGHT);

    SetFixedSize(size);

    if ( !CreateLineBatcher(CURVE_LINE, LINEColor, 2.0f) )
        return false;

    return true;
}

bool OutputNode::CreateLineBatcher(LineType linetype, const Color& color, float pixelSize)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = LineBatcher::GetBoxRect();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    controlBoxSize_ = LineBatcher::GetBoxSize();

    IntVector2 absPos = GetScreenPosition();
    IntVector2 tPos = GetSize();
    absPos.x_ += tPos.x_ + 2;

    // outputBox_
    outputBox_ = root->CreateChild<OutputBox>();
    outputBox_->SetTexture(tex2d);
    outputBox_->SetImageRect(rect);
    outputBox_->SetPosition(absPos);
    outputBox_->SetColor(Color(Color::YELLOW));
    outputBox_->SetSize(controlBoxSize_);
    outputBox_->SetOutputParent(this);

    // ctrlButton_ 
    ctrlButton_ = root->CreateChild<Button>();
    ctrlButton_->SetTexture(tex2d);
    ctrlButton_->SetImageRect(rect);
    ctrlButton_->SetSize(controlBoxSize_);
    ctrlButton_->SetColor(DISCONNECTEDColor);
    ctrlButton_->SetPosition(absPos);
    ctrlButton_->SetPriority(2);

    SubscribeToEvent(ctrlButton_, E_DRAGBEGIN, URHO3D_HANDLER(OutputNode, HandleButtonDragBegin));
    SubscribeToEvent(ctrlButton_, E_DRAGMOVE, URHO3D_HANDLER(OutputNode, HandleButtonDragMove));
    SubscribeToEvent(ctrlButton_, E_DRAGEND, URHO3D_HANDLER(OutputNode, HandleButtonDragEnd));

    // line
    pixelSize_ = pixelSize;

    lineBatcher_ = root->CreateChild<LineBatcher>();
    lineBatcher_->SetLineTexture(tex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(linetype);
    lineBatcher_->SetLinePixelSize(pixelSize_);
    lineBatcher_->SetColor(color);
    lineBatcher_->SetNumPointsPerSegment(NUM_PTS_PER_CURVE_SEGMENT);
    lineBatcher_->SetPriority(-100);
    lineBatcher_->SetBringToBack(true);

    SubscribeToEvent(GetNodeBasePtr(), E_BASE_DRAGMOVE, URHO3D_HANDLER(OutputNode, HandleBaseDragMove));
    SubscribeToEvent(GetNodeBasePtr(), E_LAYOUTUPDATED, URHO3D_HANDLER(OutputNode, HandleLayoutUpdated));

    return true;
}

bool OutputNode::ConnectToInput(InputNode *inputNode)
{
    return AttemptConnect(inputNode);
}

void OutputNode::HandleBaseDragMove(StringHash eventType, VariantMap& eventData)
{
    IntVector2 halfBtnSize = LineBatcher::GetBoxSize()/2;
    IntVector2 absPos = GetScreenPosition();
    IntVector2 tPos = GetSize();

    absPos.x_ += tPos.x_ + 2;
    outputBox_->SetPosition(absPos);

    if (absolutePositionList_.Size() == 0)
    {
        ctrlButton_->SetPosition(absPos);
    }
    else
    {
        absolutePositionList_[0] = absPos + halfBtnSize;

        // calculate inner pts
        CalculateInnerPoints();

        // draw call
        lineBatcher_->DrawPoints(absolutePositionList_);
    }
}

void OutputNode::HandleButtonDragBegin(StringHash eventType, VariantMap& eventData)
{
    // clear connection
    if ( connectedInputNode_ )
    {
        connectedInputNode_->SetConnectedOutputNode(NULL);
        ctrlButton_->SetColor(DISCONNECTEDColor);
        UnsubscribeFromEvent(connectedInputNode_, E_RECEIVER_MOVED);
        connectedInputNode_ = NULL;
    }
}

void OutputNode::HandleButtonDragMove(StringHash eventType, VariantMap& eventData)
{
    using namespace DragMove;

    Button* button = (Button*)eventData[P_ELEMENT].GetVoidPtr();
    IntVector2 d = button->GetVar("DELTA").GetIntVector2();
    int X = eventData[P_X].GetInt() + d.x_;
    int Y = eventData[P_Y].GetInt() + d.y_;

    IntVector2 scrnPos(X, Y);
    IntVector2 firstPos = outputBox_->GetPosition() + controlBoxSize_/2;
    IntVector2 btnPos = scrnPos + controlBoxSize_/2;

    ctrlButton_->SetPosition(scrnPos);

    if ( showOutputLine_ )
    {
        CreateLinePoints( firstPos, btnPos );

        // draw call
        lineBatcher_->DrawPoints(absolutePositionList_);
    }
}

void OutputNode::HandleButtonDragEnd(StringHash eventType, VariantMap& eventData)
{
    using namespace DragEnd;

    Vector2 p0((float)ctrlButton_->GetPosition().x_, (float)ctrlButton_->GetPosition().y_);
    Vector2 s0((float)ctrlButton_->GetSize().x_, (float)ctrlButton_->GetSize().y_);
    Vector<InputBox*> result;

    if ( GetSubsystem<InputNodeManager>()->GetNodesInside(result, p0, s0) )
    {
        for ( unsigned i = 0; i < result.Size(); ++i )
        {
            // skip if already connected
            if ( result[i]->GetInputParent()->GetConnectedOutputNode() )
                continue;

            if ( AttemptConnect( result[i]->GetInputParent() ) )
                break;
        }
    }
}

bool OutputNode::AttemptConnect(InputNode *inputNode)
{
    if ( inputNode )
    {
        connectedInputNode_ = inputNode;
        connectedInputNode_->SetConnectedOutputNode(this);

        if ( connectedInputNode_->GetNodeBasePtr() != GetNodeBasePtr() )
        {
            ctrlButton_->SetColor(CONNECTEDColor);

            if (absolutePositionList_.Size() == 0)
            {
                IntVector2 p0 = outputBox_->GetPosition() + controlBoxSize_/2;
                IntVector2 p4 = connectedInputNode_->GetInputBox()->GetPosition() + controlBoxSize_/2;

                CreateLinePoints( p0, p4 );
            }

            SnapToInputNode();

            SubscribeToEvent(connectedInputNode_, E_RECEIVER_MOVED, URHO3D_HANDLER(OutputNode, HandleReceiverMoved));
        }
        else
        {
            connectedInputNode_->SetConnectedOutputNode(NULL);
            connectedInputNode_ = NULL;
        }
    }

    return (connectedInputNode_ != NULL);
}

void OutputNode::HandleLayoutUpdated(StringHash eventType, VariantMap& eventData)
{
    IntVector2 halfBtnSize = LineBatcher::GetBoxSize()/2;
    IntVector2 absPos = GetScreenPosition();
    IntVector2 tPos = GetSize();
    absPos.x_ += tPos.x_ + 2;
    outputBox_->SetPosition(absPos);

    if (absolutePositionList_.Size() == 0)
    {
        ctrlButton_->SetPosition(absPos);
    }
    else
    {
        absolutePositionList_[0] = absPos + halfBtnSize;

        // calculate inner pts
        CalculateInnerPoints();

        // draw call
        lineBatcher_->DrawPoints(absolutePositionList_);
    }
}

void OutputNode::CreateLinePoints(const IntVector2 &pos0, const IntVector2 &pos4)
{
    if (absolutePositionList_.Size() == 0)
    {
        absolutePositionList_.Resize(MAX_POINTS);
    }

    absolutePositionList_[0] = pos0;
    absolutePositionList_[4] = pos4;

    CalculateInnerPoints();
}

void OutputNode::SnapToInputNode()
{
    if ( connectedInputNode_ )
    {
        ctrlButton_->SetPosition(connectedInputNode_->GetInputBox()->GetPosition() );
        absolutePositionList_[4] = ctrlButton_->GetPosition() + controlBoxSize_/2;

        CalculateInnerPoints();
        lineBatcher_->DrawPoints(absolutePositionList_);
    }
}

void OutputNode::HandleReceiverMoved(StringHash eventType, VariantMap& eventData)
{
    SnapToInputNode();
}

void OutputNode::CalculateInnerPoints()
{
    // steps:
    // 1 - set ends: 0 & 4 (done prior to entering here)
    // 2 - calculate pts next to the ends: 1 & 3
    // 3 - pt2 = avg( 1 & 3 )
    Vector2 p0 = Vector2((float)absolutePositionList_[0].x_, (float)absolutePositionList_[0].y_);
    Vector2 p4 = Vector2((float)absolutePositionList_[4].x_, (float)absolutePositionList_[4].y_);
    Vector2 dir(p4 - p0);
    Vector2 dirN = dir.Normalized();
    if (dirN.x_ < MIN_X_DIST) dirN.x_ = MIN_X_DIST;
    float dLen = Clamp(dir.Length()*FRACTION_LEN, MIN_BEND_LEN, MAX_BEND_LEN);

    absolutePositionList_[1] = IntVector2((int)(p0.x_ + dirN.x_*dLen), (int)(p0.y_ + dirN.y_*dLen*0.4f));
    absolutePositionList_[3] = IntVector2((int)(p4.x_ - dirN.x_*dLen), (int)(p4.y_ - dirN.y_*dLen*0.4f));
    absolutePositionList_[2] = (absolutePositionList_[1] + absolutePositionList_[3])/2;
}

// related to timevar input
float OutputNode::GetValueRangeMin(const String &varName)
{
    return GetNodeBasePtr()->GetValueRangeMin(varName);
}

float OutputNode::GetValueRangeMax(const String &varName)
{
    return GetNodeBasePtr()->GetValueRangeMax(varName);
}

float OutputNode::GetStartTime(const String &varName)
{
    return GetNodeBasePtr()->GetStartTime(varName);
}

float OutputNode::GetEndTime(const String &varName)
{
    return GetNodeBasePtr()->GetEndTime(varName);
}

float OutputNode::GetValueAtTime(const String &varName, float time)
{
    return GetNodeBasePtr()->GetValueAtTime(varName, time);
}

// related to slidevar input
const Variant& OutputNode::GetRangeMin(const String &varName)
{
    return GetNodeBasePtr()->GetRangeMin(varName);
}

const Variant& OutputNode::GetRangeMax(const String &varName)
{
    return GetNodeBasePtr()->GetRangeMax(varName);
}

const Variant& OutputNode::GetCurrentValue(const String &varName)
{
    return GetNodeBasePtr()->GetCurrentValue(varName);
}


