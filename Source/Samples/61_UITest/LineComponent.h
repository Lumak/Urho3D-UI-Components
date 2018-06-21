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
#include <Urho3D/Core/Variant.h>

#include "LineBatcher.h"

namespace Urho3D
{
class Button;
class CheckBox;
class BorderImage;
class Text;
}

using namespace Urho3D;

class LineBatcher;

//=============================================================================
//=============================================================================
class StaticLine : public UIElement
{
    URHO3D_OBJECT(StaticLine, UIElement);
public:
    static void RegisterObject(Context* context);

    StaticLine(Context *context);
    virtual ~StaticLine();

    virtual bool CreateLine(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize);
    bool SetPoints(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize);
    void SetUIRoot(UIElement *root) { uiRoot_ = root; }

    void SetBlendMode(BlendMode mode);
    BlendMode GetBlendMode() const { return blendMode_; }
    void SetColor(const Color& color);
    void SetColor(Corner corner, const Color& color);
    void Redraw();

protected:
    bool CreateLineBatcher(LineType linetype, const Color& color, float pixelSize);

protected:
    WeakPtr<UIElement>    uiRoot_;
    WeakPtr<LineBatcher>  lineBatcher_;
    PODVector<IntVector2> pointList_;
    LineType              linetype_;
    float                 pixelSize_;
    BlendMode             blendMode_;
};

//=============================================================================
//=============================================================================
class ControlLine : public StaticLine
{
    URHO3D_OBJECT(ControlLine, StaticLine);
public:
    static void RegisterObject(Context* context);

    ControlLine(Context *context);
    virtual ~ControlLine();

    virtual bool CreateLine(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize);
    void SetEnableButtons(bool enable);
    Vector<Button*>& GetButtonList() { return buttonList_; }

protected:
    void HandleDragMove(StringHash eventType, VariantMap& eventData);

protected:
    Vector<Button*>       buttonList_;
    bool                  buttnsEnable_;
};
