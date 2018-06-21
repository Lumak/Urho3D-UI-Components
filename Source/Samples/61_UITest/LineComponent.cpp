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
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "PageManager.h"
#include "LineComponent.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
#define MIN_X_DIST        0.3f
#define FRACTION_LEN      0.2f
#define MIN_BEND_LEN     20.0f
#define MAX_BEND_LEN    100.0f

//=============================================================================
//=============================================================================
void StaticLine::RegisterObject(Context* context)
{
    context->RegisterFactory<StaticLine>();
}

StaticLine::StaticLine(Context *context)
    : UIElement(context)
    , blendMode_(BLEND_REPLACE)
{
}

StaticLine::~StaticLine()
{
}

bool StaticLine::CreateLine(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize)
{
    if ( !CreateLineBatcher(linetype, color, pixelSize) )
        return false;

    for ( unsigned i = 0; i < points.Size(); ++i )
    {
        // points
        IntVector2 pt = points[i];
        pointList_.Push(pt);
    }

    lineBatcher_->DrawPoints(pointList_);

    return true;
}

bool StaticLine::CreateLineBatcher(LineType linetype, const Color& color, float pixelSize)
{
    if (lineBatcher_)
        return false;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = LineBatcher::GetBoxRect();

    pixelSize_ = pixelSize;

    lineBatcher_ = uiRoot_?uiRoot_->CreateChild<LineBatcher>():GetSubsystem<PageManager>()->GetRoot()->CreateChild<LineBatcher>();
    lineBatcher_->SetLineTexture(tex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(linetype);
    lineBatcher_->SetLinePixelSize(pixelSize_);
    lineBatcher_->SetColor(color);
    lineBatcher_->SetNumPointsPerSegment(linetype == STRAIGHT_LINE?0:NUM_PTS_PER_CURVE_SEGMENT);

    return true;
}

void StaticLine::SetBlendMode(BlendMode mode)
{
    lineBatcher_->SetBlendMode(mode);
}

void StaticLine::SetColor(const Color& color)
{
    lineBatcher_->SetColor(color);
}

void StaticLine::SetColor(Corner corner, const Color& color)
{
    lineBatcher_->SetColor(corner, color);
}

void StaticLine::Redraw()
{
    lineBatcher_->DrawPoints(pointList_);
}

//=============================================================================
//=============================================================================
void ControlLine::RegisterObject(Context* context)
{
    context->RegisterFactory<ControlLine>();
}

ControlLine::ControlLine(Context *context)
    : StaticLine(context)
    , buttnsEnable_(true)
{
}

ControlLine::~ControlLine()
{
}

bool ControlLine::CreateLine(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize)
{
    if ( !CreateLineBatcher(linetype, color, pixelSize) )
        return false;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = LineBatcher::GetBoxRect();

    for ( int i = 0; i < (int)points.Size(); ++i )
    {
        // point button
        IntVector2 pt = points[i];
        pointList_.Push(pt);

        Button *button = GetSubsystem<PageManager>()->GetRoot()->CreateChild<Button>();
        button->SetTexture(tex2d);
        button->SetImageRect(rect);
        button->SetPosition(points[i]);
        button->SetSize(LineBatcher::GetBoxSize());
        button->SetOpacity(0.4f);
        button->SetVisible(true);
        SubscribeToEvent(button, E_DRAGMOVE, URHO3D_HANDLER(ControlLine, HandleDragMove));

        buttonList_.Push(button);
    }

    lineBatcher_->DrawPoints(pointList_);

    return true;
}

void ControlLine::SetEnableButtons(bool enable)
{
    buttnsEnable_ = enable;

    for ( unsigned i = 0; i < buttonList_.Size(); ++i )
    {
        if (enable)
        {
            SubscribeToEvent(buttonList_[i], E_DRAGMOVE, URHO3D_HANDLER(ControlLine, HandleDragMove));
        }
        else
        {
            UnsubscribeFromEvent(buttonList_[i], E_DRAGMOVE);
        }
    }
}

void ControlLine::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
    using namespace DragMove;

    Button* button = (Button*)eventData[P_ELEMENT].GetVoidPtr();
    //int buttons = eventData[P_BUTTONS].GetInt();
    IntVector2 d = button->GetVar("DELTA").GetIntVector2();
    int X = eventData[P_X].GetInt() + d.x_;
    int Y = eventData[P_Y].GetInt() + d.y_;

    for ( unsigned i = 0; i < buttonList_.Size(); ++i )
    {
        if ( button == buttonList_[i] )
        {
            pointList_[i] = IntVector2(X, Y);
            button->SetPosition(X, Y);
        }
    }

    lineBatcher_->DrawPoints(pointList_);
}




