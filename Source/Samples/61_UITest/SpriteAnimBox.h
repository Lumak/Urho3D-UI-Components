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
#include <Urho3D/UI/Window.h>

namespace Urho3D
{
class Text;
class BorderImage;
class CheckBox;
}

using namespace Urho3D;
//=============================================================================
//=============================================================================
class SpriteAnimBox : public BorderImage
{
    URHO3D_OBJECT(SpriteAnimBox, UIElement);
public:
    static void RegisterObject(Context* context);

    SpriteAnimBox(Context *context);
    virtual ~SpriteAnimBox();

    UIElement* GetHeaderElement()  { return headerElement_;  }
    UIElement* GetControlElement() { return controlElement_; }
    Text* GetHeaderTextElement()   { return headerText_;     }

    void ShowHeader(bool show);
    void ShowControl(bool show);

    void Create(IntVector2 &size, bool showHeader=false, bool showControl=false);
    void AddSprite(const String& spriteFile);
    void SetFPS(float fps) { assert(fps >= 1.0f); frameMsec_ = (int)(1000.0f/fps); }
    void SetEnabled(bool enable);
    void Play();
    void Pause();
    void Quit();

    bool SetHeaderFont(const String& fontName, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFont(Font* font, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFontSize(int size);
    void SetHeaderText(const String& text);

protected:
    void SetDefaultPlayButton();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleCheckbox(StringHash eventType, VariantMap& eventData);

protected:
    WeakPtr<UIElement>   headerElement_;
    WeakPtr<UIElement>   controlElement_;
    WeakPtr<BorderImage> bodyElement_;
    WeakPtr<Text>        headerText_;
    WeakPtr<CheckBox>    playButton_;

    Vector<Texture2D*>   spriteList_;
    int                  spriteIndex_;
    int                  elapsedTime_;
    int                  frameMsec_;
    bool                 paused_;
};

