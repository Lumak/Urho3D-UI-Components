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
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <SDL/SDL_log.h>

#include "DrawTool.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
#define MIN_BAR_HEIGHT  30

//=============================================================================
//=============================================================================
void DrawAreaBatcher::RegisterObject(Context* context)
{
    context->RegisterFactory<DrawAreaBatcher>();
}

DrawAreaBatcher::DrawAreaBatcher(Context *context)
    : BorderImage(context)
    , flushDrawBuffers_(false)
    , minLineLength_(8.0f)
    , pointListLimit_(100)
{
}

DrawAreaBatcher::~DrawAreaBatcher()
{
}

bool DrawAreaBatcher::Create(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect)
{
    if ( !CreateLineBatcher(tex2d, rect) )
        return false;

    SetEnabled(true);
    SetSize(size);

    return true;
}

bool DrawAreaBatcher::CreateLineBatcher(Texture2D *tex2d, const IntRect &rect)
{
    if ( lineBatcher_ )
        return false;

    lineBatcher_ = CreateChild<LineBatcher>();
    lineBatcher_->SetConstrainingParent(this);

    lineBatcher_->SetLineTexture(tex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(STRAIGHT_LINE);
    lineBatcher_->SetLinePixelSize(2.0f);
    lineBatcher_->SetColor(Color::RED);
    lineBatcher_->SetNumPointsPerSegment(0);

    return true;
}

void DrawAreaBatcher::OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                           int buttons, int qualifiers, Cursor* cursor)
{
    drawPointsList_.Clear();
}

void DrawAreaBatcher::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                          const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    if (buttons != MOUSEB_RIGHT || !InsideParent(position) || lineBatcher_ == NULL)
        return;

    Vector2 vec((float)(lastPos_.x_ - screenPosition.x_), (float)(lastPos_.y_ - screenPosition.y_));

    // limit the minimum line length to push onto the linebatcher
    if ( vec.Length() < minLineLength_)
        return;

    lastPos_ = screenPosition;

    drawPointsList_.Push( screenPosition );

    if ( drawPointsList_.Size() > 1 )
    {
        lineBatcher_->DrawPoints(drawPointsList_);

        if (batchCountText_)
        {
            String str = String("batch count = ") + String(lineBatcher_->GetBatchCount());
            batchCountText_->SetText( str );
        }
    }
}

bool DrawAreaBatcher::InsideParent(const IntVector2 &p)
{
    IntVector2 size = GetSize();
    return (p.x_ >= 0 && p.x_ <= size.x_ && p.y_ >= 0 && p.y_ <= size.y_);
}

//=============================================================================
//=============================================================================
void DrawAreaTexure::RegisterObject(Context* context)
{
    context->RegisterFactory<DrawAreaTexure>();
}

DrawAreaTexure::DrawAreaTexure(Context *context)
    : BorderImage(context)
{
}

DrawAreaTexure::~DrawAreaTexure()
{
}

bool DrawAreaTexure::Create(const IntVector2 &size)
{
    // set texture format
    drawTexture_ = new Texture2D(context_);
    textureSize_ = IntVector2( 256, 256 );

    textureScale_ = Vector2( (float)textureSize_.x_/ (float)size.x_, (float)textureSize_.y_/ (float)size.y_ );
    drawTexture_->SetMipsToSkip(QUALITY_LOW, 0);
    drawTexture_->SetNumLevels(1);
    drawTexture_->SetSize(textureSize_.x_, textureSize_.y_, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC);

    colorMap_ = new ColorMap(context_);
    colorMap_->SetSource(drawTexture_);
    for (int y = 0; y < textureSize_.y_; ++y )
    {
        for (int x = 0; x < textureSize_.x_; ++x )
        {
            colorMap_->SetPixel(x, y, Color::WHITE);
        }
    }
    colorMap_->ApplyColor();

    SetTexture(drawTexture_);

    SetEnabled(true);
    SetSize(size);

    return true;
}

void DrawAreaTexure::ClearBuffer()
{
}

void DrawAreaTexure::OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                                 int buttons, int qualifiers, Cursor* cursor)
{
    if (buttons != MOUSEB_RIGHT || !InsideParent(position) )
        return;

    lastPos_ = position;
}

void DrawAreaTexure::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                                const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    if (buttons != MOUSEB_RIGHT || !InsideParent(position) )
        return;

    IntVector2 p0( (int)(textureScale_.x_ * (float)lastPos_.x_), (int)(textureScale_.y_ * (float)lastPos_.y_) );
    IntVector2 p1( (int)(textureScale_.x_ * (float)position.x_), (int)(textureScale_.y_ * (float)position.y_) );

    lastPos_ = position;

    Bresenham(p0.x_, p0.y_, p1.x_, p1.y_);

    // update texture
    colorMap_->ApplyColor();
}

// from:
// http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm
void DrawAreaTexure::Bresenham(int x1, int y1, int const x2, int const y2)
{
    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;
 
    //plot(x1, y1);
    colorMap_->SetPixel(x1, y1, Color::RED);
 
    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));
 
        while (x1 != x2)
        {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing
 
            error += delta_y;
            x1 += ix;
 
            //plot(x1, y1);
            colorMap_->SetPixel(x1, y1, Color::RED);
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));
 
        while (y1 != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing
 
            error += delta_x;
            y1 += iy;
 
            //plot(x1, y1);
            colorMap_->SetPixel(x1, y1, Color::RED);
        }
    }
}

bool DrawAreaTexure::InsideParent(const IntVector2 &p)
{
    IntVector2 size = GetSize();
    return (p.x_ >= 0 && p.x_ <= size.x_ && p.y_ >= 0 && p.y_ <= size.y_);
}

//=============================================================================
//=============================================================================
void DrawTool::RegisterObject(Context* context)
{
    context->RegisterFactory<DrawTool>();
    DrawAreaBatcher::RegisterObject(context);
    DrawAreaTexure::RegisterObject(context);
}

DrawTool::DrawTool(Context *context) 
    : BorderImage(context)
{
    SetLayoutMode(LM_VERTICAL);
    borderRect_ = IntRect(3,3,3,3);
    SetLayoutBorder(borderRect_);
    SetClipBorder(borderRect_);

    // header
    headerElement_ = CreateChild<BorderImage>();
    headerElement_->SetLayoutMode(LM_HORIZONTAL);
    headerElement_->SetMaxHeight(MIN_BAR_HEIGHT);
    headerElement_->SetLayoutBorder(IntRect(3,3,3,3));
    headerElement_->SetClipBorder(IntRect(3,3,3,3));

    headerText_ = headerElement_->CreateChild<Text>();
    headerText_->SetAlignment(HA_LEFT, VA_CENTER);

    batchCountText_ = headerElement_->CreateChild<Text>();
    batchCountText_->SetAlignment(HA_RIGHT, VA_CENTER);
}

DrawTool::~DrawTool()
{
}

bool DrawTool::InitInternal(const IntVector2 &size)
{
    return true;
}

bool DrawTool::Create(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect, bool useLineBatcher)
{
    useLineBatcher_ = useLineBatcher;

    // min size requirement
    if ( size.x_ < 200 || size.y_ < 200 )
        return false;

    headerElement_->SetSize(size.x_, MIN_BAR_HEIGHT);

    if ( !CreateDrawArea(size, tex2d, rect) )
        return false;

    SetSize(size);

    return true;
}

bool DrawTool::CreateDrawArea(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect)
{
    IntVector2 drawAreaSize;
    drawAreaSize.x_ = size.x_ - borderRect_.left_ - borderRect_.right_;
    drawAreaSize.y_ = size.y_ - borderRect_.top_ - borderRect_.bottom_ - MIN_BAR_HEIGHT;

    if ( useLineBatcher_ )
    {
        drawArea_ = CreateChild<DrawAreaBatcher>();
        drawArea_->SetBatchCountText(batchCountText_);
        return drawArea_->Create(drawAreaSize, tex2d, rect);
    }
    else
    {
        drawAreaTexture_ = CreateChild<DrawAreaTexure>();
        drawAreaTexture_->Create(drawAreaSize);
    }

    return true;
}

void DrawTool::SetScreenColor(const Color &color)
{
    colorScreen_ = color;

    if ( useLineBatcher_ )
    {
        drawArea_->SetColor(colorScreen_);
    }
}

void DrawTool::SetColor(const Color& color)
{
    headerElement_->SetColor(color);

    UIElement::SetColor(color);
}

bool DrawTool::SetHeaderFont(const String& fontName, int size)
{
    batchCountText_->SetFont(fontName, size);
    return headerText_->SetFont(fontName, size);
}

bool DrawTool::SetHeaderFont(Font* font, int size)
{
    batchCountText_->SetFont(font, 10);
    return headerText_->SetFont(font, size);
}

void DrawTool::SetHeaderText(const String& text)
{
    headerText_->SetText(text);
}


