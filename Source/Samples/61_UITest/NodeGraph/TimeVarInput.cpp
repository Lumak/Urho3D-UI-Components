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
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>

#include <stdio.h>

#include "TimeVarInput.h"
#include "PageManager.h"
#include "LineComponent.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
#define CTRL_BUTTON_SIZE     10
#define BUTTON_SPACING       CTRL_BUTTON_SIZE*2
#define DEFAULT_LINE_SIZE    2.0f
//=============================================================================
//=============================================================================
void TimeVarInput::RegisterObject(Context* context)
{
    context->RegisterFactory<TimeVarInput>(UI_CATEGORY);
}

TimeVarInput::TimeVarInput(Context *context) 
    : IOElement(context)
    , minValue_(0.0f)
    , maxValue_(1.0f)
    , valueRange_(1.0f)
    , timeStart_(0.0f)
    , timeEnd_(1.0f)
    , timeRange_(1.0f)
{
    SetIOType(IOTYPE_INPUT);
    spline_.SetInterpolationMode(CATMULL_ROM_FULL_CURVE);
}

TimeVarInput::~TimeVarInput()
{
}

bool TimeVarInput::InitInternal()
{
    return true;
}

bool TimeVarInput::Create(const String &variableName, const IntVector2 &size)
{
    InitBaseNodeParent();

    if ( !InitScreen(size) )
        return false;

    if ( !CreateLineBatcher(CURVE_LINE, Color::RED, DEFAULT_LINE_SIZE) )
        return false;

    if ( !CreateButtons() )
        return false;

    SetVariableName(variableName);

    return true;
}

bool TimeVarInput::InitScreen(const IntVector2 &size)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetTexture(cache->GetResource<Texture2D>("Textures/SignalPanel.png"));

    // fix size
    SetFixedSize(size);

    IntVector2 absPos = GetScreenPosition();
    IntVector2 scrnSize = size;
    int horizSpacing = scrnSize.x_/4;
    controlBoxSize_ = IntVector2(CTRL_BUTTON_SIZE, CTRL_BUTTON_SIZE);

    IntVector2 points[MAX_POINTS] =
    {
        { controlBoxSize_.x_/2, scrnSize.y_/2 },
        { horizSpacing * 1, scrnSize.y_/2 },
        { horizSpacing * 2, scrnSize.y_/2 },
        { horizSpacing * 3, scrnSize.y_/2 },
        { horizSpacing * 4 - controlBoxSize_.x_/2, scrnSize.y_/2 },
    };

    pointList_.Resize(MAX_POINTS);
    absolutePositionList_.Resize(MAX_POINTS);
    spline_.Clear();

    for ( int i = 0; i < MAX_POINTS; ++i )
    {
        pointList_[i] = points[i];
        absolutePositionList_[i] = points[i] + absPos;
        Variant var = Vector2((float)pointList_[i].x_, (float)pointList_[i].y_);
        spline_.AddKnot(var);
    }

    // text
    int textsize = (size.x_ > 400)?8:6;
    textTitle_     = CreateText(textsize, IntVector2(size.x_/2, 5) );
    textMaxValue_  = CreateText(textsize, IntVector2(size.x_ - 30, 10) );
    textMinValue_  = CreateText(textsize, IntVector2(size.x_ - 35, size.y_ - 40) );
    textTimeStart_ = CreateText(textsize, IntVector2(20, size.y_ - 15) );
    textTimeEnd_   = CreateText(textsize, IntVector2(size.x_ - 40, size.y_ - 15) );

    // sub
    SubscribeToEvent(GetNodeBasePtr(), E_BASE_DRAGMOVE, URHO3D_HANDLER(TimeVarInput, HandleBaseDragMove));
    SubscribeToEvent(GetNodeBasePtr(), E_LAYOUTUPDATED, URHO3D_HANDLER(TimeVarInput, HandleLayoutUpdated));

    return true;
}
Text* TimeVarInput::CreateText(int size, const IntVector2 &pos)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Text* text = CreateChild<Text>();
    text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), size);
    text->SetPosition(pos);
    return text;
}

bool TimeVarInput::CreateLineBatcher(LineType linetype, const Color& color, float pixelSize)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    IntRect rect = LineBatcher::GetBoxRect();

    pixelSize_ = pixelSize;

    lineBatcher_ = CreateChild<LineBatcher>();
    lineBatcher_->SetLineTexture(uiTex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(linetype);
    lineBatcher_->SetLinePixelSize(pixelSize_);
    lineBatcher_->SetColor(color);
    lineBatcher_->SetNumPointsPerSegment(NUM_PTS_PER_CURVE_SEGMENT);
    lineBatcher_->SetPriority(-1);
    lineBatcher_->SetBringToBack(true);

    lineBatcher_->DrawPoints(absolutePositionList_);

    return true;
}

bool TimeVarInput::CreateButtons()
{
    // init line prior to calling this fn
    if (lineBatcher_ == NULL)
    {
        return false;
    }

    ResourceCache *cache = GetSubsystem<ResourceCache>();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    IntRect rect = LineBatcher::GetBoxRect();
    IntVector2 boxsize = LineBatcher::GetBoxSize();

    for ( int i = 0; i < (int)pointList_.Size(); ++i )
    {
        Button *button = CreateChild<Button>();
        button->SetTexture(uiTex2d);
        button->SetImageRect(rect);
        button->SetPosition(pointList_[i] - controlBoxSize_/2);
        button->SetSize(controlBoxSize_);
        button->SetVisible(true);
        button->SetColor(Color(1,1,0));

        buttonList_.Push(button);

        SubscribeToEvent(button, E_DRAGMOVE, URHO3D_HANDLER(TimeVarInput, HandleButtonDragMove));
    }

    return true;
}

bool TimeVarInput::InitDataCurvePoints(const PODVector<Vector2> &points)
{
    if ( points.Size() != MAX_POINTS )
    {
        return false;
    }

    // eval curve points and normalize (scaled to range)
    IntVector2 scrnSize = GetSize() - controlBoxSize_;
    PODVector<IntVector2> scrnPoints;

    EvaluateCurvePoints(points, scrnPoints);

    for ( int i = 0; i < MAX_POINTS; ++i )
    {
        IntVector2 btnPos = scrnPoints[ i ];
        //btnPos.y_ = scrnSize.y_ - btnPos.y_;

        btnPos.x_ = ( btnPos.x_ < 0)?0:(btnPos.x_ > scrnSize.x_)?scrnSize.x_:btnPos.x_;
        btnPos.y_ = ( btnPos.y_ < 0)?0:(btnPos.y_ > scrnSize.y_)?scrnSize.y_:btnPos.y_;

        if (i == 0)
        {
            btnPos.x_ = 0;
        }
        else if (i == buttonList_.Size() - 1)
        {
            btnPos.x_ = scrnSize.x_;
        }
        else
        {
            if (btnPos.x_ < buttonList_[i-1]->GetPosition().x_ + BUTTON_SPACING )
            {
                btnPos.x_ = buttonList_[i-1]->GetPosition().x_ + BUTTON_SPACING;
            }

            if (btnPos.x_ > buttonList_[i+1]->GetPosition().x_ - BUTTON_SPACING)
            {
                btnPos.x_ = buttonList_[i+1]->GetPosition().x_ - BUTTON_SPACING;
            }
        }

        buttonList_[i]->SetPosition(btnPos);
        pointList_[i] = btnPos;
    }

    // updat draw line
    UpdateDrawLine();

    return true;
}

void TimeVarInput::EvaluateCurvePoints(const PODVector<Vector2> &points, PODVector<IntVector2> &outPoints)
{
    float rmin=1e19f;
    float rmax=-1e19f;
    float tmin=1e19f;
    float tmax=-1e19f;

    for ( unsigned i = 0; i < points.Size(); ++i )
    {
        assert(points[i].x_ >= 0.0f && "time value must be positive");

        if (points[i].x_ < tmin) tmin = points[i].x_;
        if (points[i].x_ > tmax) tmax = points[i].x_;
        if (points[i].y_ < rmin) rmin = points[i].y_;
        if (points[i].y_ > rmax) rmax = points[i].y_;
    }

    SetValueRange(rmin, rmax);
    SetTimeRange(tmin, tmax);
    CurvePointsNormalized(points, outPoints);
}

void TimeVarInput::CurvePointsNormalized(const PODVector<Vector2> &points, PODVector<IntVector2> &scrnPoints)
{
    scrnPoints.Resize(MAX_POINTS);
    IntVector2 scrnSize = GetSize() - controlBoxSize_;
    Vector2 fscrnSize((float)scrnSize.x_, (float)scrnSize.y_);

    for ( int i = 0; i < MAX_POINTS; ++i )
    {
        Vector2 p = points[ i ];
        p.x_ =  fscrnSize.x_ * ((float)p.x_/timeRange_); 
        p.y_ =  fscrnSize.y_ - fscrnSize.y_ * (p.y_ - minValue_)/valueRange_;

        scrnPoints[ i ] = IntVector2((int)p.x_, (int)p.y_);
    }
}

void TimeVarInput::SetValueRange(float rmin, float rmax)
{
    assert(rmax > rmin);

    minValue_ = rmin;
    maxValue_ = rmax;

    valueRange_ = maxValue_ - minValue_;

    char buff[20];
    sprintf(buff, "%.1f", minValue_);
    textMinValue_->SetText( String(buff) );
    sprintf(buff, "%.1f", maxValue_);
    textMaxValue_->SetText( String(buff) );
}

void TimeVarInput::SetTimeRange(float mintime, float maxtime)
{
    assert(mintime >= 0.0f);
    assert(maxtime >= 0.1f);

    timeStart_ = mintime;
    timeEnd_ = maxtime;
    timeRange_ = maxtime - mintime;

    char buff[20];
    sprintf(buff, "%.2f", mintime);
    textTimeStart_->SetText( String(buff) );
    sprintf(buff, "%.2f", maxtime);
    textTimeEnd_->SetText( String(buff) );
}

void TimeVarInput::HandleBaseDragMove(StringHash eventType, VariantMap& eventData)
{
    UpdateDrawLine();
}

void TimeVarInput::HandleButtonDragMove(StringHash eventType, VariantMap& eventData)
{
    using namespace DragMove;

    Button* button = (Button*)eventData[P_ELEMENT].GetVoidPtr();
    IntVector2 d = button->GetVar("DELTA").GetIntVector2();
    int X = eventData[P_X].GetInt() + d.x_;
    int Y = eventData[P_Y].GetInt() + d.y_;

    IntVector2 absPos = GetScreenPosition();
    IntVector2 btnScrnPos(X, Y);
    IntVector2 btnPos = btnScrnPos - absPos - controlBoxSize_/2;
    IntVector2 scrnSize = GetSize() - controlBoxSize_;

    // constrain within the object's space
    btnPos.x_ = ( btnPos.x_ < 0)?0:(btnPos.x_ > scrnSize.x_)?scrnSize.x_:btnPos.x_;
    btnPos.y_ = ( btnPos.y_ < 0)?0:(btnPos.y_ > scrnSize.y_)?scrnSize.y_:btnPos.y_;

    for ( unsigned i = 0; i < buttonList_.Size(); ++i )
    {
        if ( button == buttonList_[i] )
        {
            if (i == 0)
            {
                btnPos.x_ = 0;
            }
            else if (i == buttonList_.Size() - 1)
            {
                btnPos.x_ = scrnSize.x_;
            }
            else
            {
                if (btnPos.x_ < buttonList_[i-1]->GetPosition().x_ + BUTTON_SPACING )
                {
                    btnPos.x_ = buttonList_[i-1]->GetPosition().x_ + BUTTON_SPACING;
                }

                if (btnPos.x_ > buttonList_[i+1]->GetPosition().x_ - BUTTON_SPACING)
                {
                    btnPos.x_ = buttonList_[i+1]->GetPosition().x_ - BUTTON_SPACING;
                }
            }
            button->SetPosition(btnPos);
        }
    }

    // updat draw line
    UpdateDrawLine();
}

void TimeVarInput::HandleLayoutUpdated(StringHash eventType, VariantMap& eventData)
{
    UpdateDrawLine();
}

void TimeVarInput::UpdateDrawLine()
{
    IntVector2 absPos = GetScreenPosition();
    IntVector2 scrnSize = GetSize() - controlBoxSize_;

    spline_.Clear();

    for ( unsigned i = 0; i < buttonList_.Size(); ++i )
    {
        IntVector2 btnPos = buttonList_[i]->GetPosition();
        absolutePositionList_[i] = btnPos + absPos + controlBoxSize_/2;

        Vector2 v0((float)btnPos.x_, (float)(scrnSize.y_ - btnPos.y_));
        spline_.AddKnot( v0 );
    }

    lineBatcher_->DrawPoints(absolutePositionList_);
}

float TimeVarInput::GetValueRangeMin(const String &varName)
{
    if ( varName == variableName_ )
    {
        return minValue_;
    }
    return 0.0f;
}

float TimeVarInput::GetValueRangeMax(const String &varName)
{
    if ( varName == variableName_ )
    {
        return maxValue_;
    }
    return 0.0f;
}

float TimeVarInput::GetStartTime(const String &varName)
{
    if ( varName == variableName_ )
    {
        return timeStart_;
    }
    return 0.0f;
}

float TimeVarInput::GetEndTime(const String &varName)
{
    if ( varName == variableName_ )
    {
        return timeEnd_;
    }
    return 0.0f;
}

float TimeVarInput::GetValueAtTime(float time)
{
    float atTime = time<timeStart_?timeStart_:time>timeEnd_?timeEnd_:time;
    atTime /= timeRange_;

    IntVector2 scrnSize = GetSize() - controlBoxSize_;
    Vector2 scrn((float)scrnSize.x_, (float)scrnSize.y_);
    Vector2 v1 = spline_.GetPoint( atTime ).GetVector2();
    float pctScale = ((float)v1.y_) / (float)scrn.y_;
    float val = valueRange_ * pctScale + minValue_;

    return val;
}

float TimeVarInput::GetValueAtTime(const String &varName, float time)
{
    if ( varName == variableName_ )
    {
        return GetValueAtTime(time);
    }
    return 0.0f;
}

