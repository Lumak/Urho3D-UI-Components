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
#include <Urho3D/UI/BorderImage.h>
#include "LineBatcher.h"

namespace Urho3D
{
class Button;
class Text;
class Font;
class Texture2D;
}

using namespace Urho3D;
//=============================================================================
// WARNING: avoid inheriting from Window because if the Window is moved
// then all lineBatcher drawn objects must also be moved
//=============================================================================
class DrawAreaBatcher : public BorderImage
{
    URHO3D_OBJECT(DrawAreaBatcher, BorderImage);
public:
    static void RegisterObject(Context* context);

    DrawAreaBatcher(Context *context);
    virtual ~DrawAreaBatcher();

    bool Create(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect);

    virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                             int buttons, int qualifiers, Cursor* cursor);

    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

    void SetBatchCountText(Text *text) { batchCountText_ = text;}

protected:
    bool CreateLineBatcher(Texture2D *tex2d, const IntRect &rect);
    bool InsideParent(const IntVector2 &position);

protected:
    WeakPtr<LineBatcher>  lineBatcher_;

    PODVector<IntVector2> drawPointsList_;
    int                   drawPointsIndex_;
    bool                  flushDrawBuffers_;
    float                 minLineLength_;
    IntVector2            lastPos_;
    unsigned              pointListLimit_;

    WeakPtr<Text>            batchCountText_;

};

class ColorMap : public Image
{
    URHO3D_OBJECT(ColorMap, Image);

public:
    ColorMap(Context *_pContext) : Image( _pContext ) {}
    virtual ~ColorMap(){}

    void SetSource(Texture2D *texture)
    {
        textureSrc_ = texture; 
        SetSize( texture->GetWidth(), texture->GetHeight(), 1, texture->GetComponents() );
        ApplyColor();
    }

    void ApplyColor()
    {
        textureSrc_->SetData( 0, 0, 0, GetWidth(), GetHeight(), GetData() );
    }

protected:
    WeakPtr<Texture2D> textureSrc_;
};

class DrawAreaTexure : public BorderImage
{
    URHO3D_OBJECT(DrawAreaTexure, BorderImage);
public:
    static void RegisterObject(Context* context);

    DrawAreaTexure(Context *context);
    virtual ~DrawAreaTexure();

    bool Create(const IntVector2 &size);
    virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                             int buttons, int qualifiers, Cursor* cursor);

    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

protected:
    void ClearBuffer();
    void Bresenham(int x1, int y1, int x2, int y2);
    bool InsideParent(const IntVector2 &position);

protected:
    SharedPtr<Texture2D> drawTexture_;
    SharedPtr<ColorMap>  colorMap_;

    IntVector2           textureSize_;
    Vector2              textureScale_;
    IntVector2           lastPos_;
    unsigned             pointListLimit_;
};

//=============================================================================
//=============================================================================
class DrawTool : public BorderImage
{
    URHO3D_OBJECT(DrawTool, BorderImage);
public:
    static void RegisterObject(Context* context);

    DrawTool(Context *context);
    virtual ~DrawTool();

    bool Create(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect, bool useLineBatcher);
    void SetColor(const Color& color);
    bool SetHeaderFont(const String& fontName, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFont(Font* font, int size = DEFAULT_FONT_SIZE);
    void SetHeaderText(const String& text);
    void SetScreenColor(const Color &color);

protected:
    bool InitInternal(const IntVector2 &size);
    bool CreateDrawArea(const IntVector2 &size, Texture2D *tex2d, const IntRect &rect);

protected:
    WeakPtr<BorderImage>     headerElement_;
    WeakPtr<Text>            headerText_;
    WeakPtr<Text>            batchCountText_;

    WeakPtr<DrawAreaBatcher> drawArea_;
    WeakPtr<DrawAreaTexure>  drawAreaTexture_;

    IntRect                  borderRect_;
    bool                     useLineBatcher_;
    Color                    colorScreen_;

};

