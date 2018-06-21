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
#include <Urho3D/UI/Text.h>

namespace Urho3D
{
class Text;
}
using namespace Urho3D;
//=============================================================================
//=============================================================================
/// group checkbox toggled.
URHO3D_EVENT(E_RADIALGROUPTOGGLED, RadialGroupToggled)
{
    URHO3D_PARAM(P_ELEMENT, Element);              // UIElement pointer
    URHO3D_PARAM(P_INDEX, Index);                  // int
}

//=============================================================================
//=============================================================================
class RadialText : public Text
{
    URHO3D_OBJECT(RadialText, Text);
public:
    static void RegisterObject(Context* context);

    RadialText(Context *context) : Text(context){}
    virtual ~RadialText(){}

    virtual void OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, 
                              int button, int buttons, int qualifiers, Cursor* cursor);

};

struct RadialElement
{
    WeakPtr<CheckBox>   checkbox_;
    WeakPtr<RadialText> textDesc_;
    WeakPtr<UIElement>  bodyElement_;
};


class RadialGroup : public BorderImage
{
    URHO3D_OBJECT(RadialGroup, BorderImage);
public:
    static void RegisterObject(Context* context);

    RadialGroup(Context *context);
    virtual ~RadialGroup();

    RadialElement* CreateRadialButton();
    RadialElement* GetRadialButtonDesc(unsigned idx);

    UIElement* GetHeaderElement() { return headerElement_; }
    UIElement* GetBodyElement()   { return bodyElement_;   }
    Text* GetTitleTextElement();

    void SetEnabled(bool enabled);

    bool SetHeaderFont(const String& fontName, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFont(Font* font, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFontSize(int size);
    void SetHeaderText(const String& text);

protected:
    void HandleCheckbox(StringHash eventType, VariantMap& eventData);
    void HandlePressed(StringHash eventType, VariantMap& eventData);
    void SendGroupToggleEvent(int idx);

protected:
    WeakPtr<UIElement> headerElement_;
    WeakPtr<Text>      headerText_;
    WeakPtr<UIElement> bodyElement_;

    Vector<RadialElement> childList_;
    IntVector2         internalSize_; 
};


