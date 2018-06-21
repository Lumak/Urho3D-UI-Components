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
#include <Urho3D/UI/CheckBox.h>
#include "LineComponent.h"

namespace Urho3D
{
extern const char* UI_CATEGORY;
}
using namespace Urho3D;
//=============================================================================
//=============================================================================
class IOElement;

//=============================================================================
//=============================================================================
URHO3D_EVENT(E_BASE_DRAGBEIN, BaseDragBegin)
{
    URHO3D_PARAM(P_ELEMENT, Element);  // UIElement pointer
}

URHO3D_EVENT(E_BASE_DRAGMOVE, BaseDragMove)
{
    URHO3D_PARAM(P_ELEMENT, Element);  // UIElement pointer
}


//=============================================================================
//=============================================================================
class GraphNode : public BorderImage
{
    URHO3D_OBJECT(GraphNode, BorderImage);
public:
    static void RegisterObject(Context* context);

    GraphNode(Context *context);
    virtual ~GraphNode();

    static const Color& GetDefaultHeaderColor()     { return colorHeader_; }
    static const Color& GetDefaultBodyColor()       { return colorBody_;   }
    static const IntVector2& GetDefaultIONodeSize() { return ioNodeSize_;  }

    void AddChild(UIElement* element);

    UIElement* GetHeaderElement()     { return headerElement_;      }
    UIElement* GetBodyElement()       { return bodyElement_;        }
    UIElement* GetInputBodyElement()  { return inputBodyElement_;   }
    UIElement* GetOutputBodyElement() { return outputBodyElement_;  }
    UIElement* GetFooterElement()     { return footerElement_;      }
    Text* GetFooterTextElement()      { return footerText_;         }

    void SetColor(const Color& color);
    void SetBodyColor(const Color& color);
    bool SetHeaderFont(const String& fontName, int size = DEFAULT_FONT_SIZE);
    bool SetHeaderFont(Font* font, int size = DEFAULT_FONT_SIZE);
    void SetHeaderText(const String& text);

    void SetFooterVisible(bool vis);
    bool SetFooterFont(const String& fontName, int size = DEFAULT_FONT_SIZE);
    bool SetFooterFont(Font* font, int size = DEFAULT_FONT_SIZE);
    void SetFooterText(const String& text);

    void SetEnabled(bool enable);

    // related to timevar input
    virtual float GetValueRangeMin(const String &varName);
    virtual float GetValueRangeMax(const String &varName);
    virtual float GetStartTime(const String &varName);
    virtual float GetEndTime(const String &varName);
    virtual float GetValueAtTime(const String &varName, float time);

    // related to slidevar input
    virtual const Variant& GetRangeMin(const String &varName);
    virtual const Variant& GetRangeMax(const String &varName);
    virtual const Variant& GetCurrentValue(const String &varName);


    // mouse events
    virtual void OnDoubleClick(const IntVector2& position, const IntVector2& screenPosition, 
                               int button, int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                           int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                          const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition, 
                         int dragButtons, int releaseButton, Cursor* cursor);

protected:
    IOElement* FindInuptVarName(const String &varName);

private:
    bool InitInternal();

protected:
    // static
    static Color colorHeader_;
    static Color colorBody_;
    static IntVector2 ioNodeSize_;

    // header
    WeakPtr<BorderImage>  headerElement_;
    WeakPtr<Text>         headerText_;

    // body
    WeakPtr<BorderImage>  bodyElement_;
    WeakPtr<BorderImage>  inputBodyElement_;
    WeakPtr<BorderImage>  outputBodyElement_;

    // footer
    WeakPtr<BorderImage>  footerElement_;
    WeakPtr<Text>         footerText_;

    bool                  footerToggle_;

    IntVector2            dragBeginPosition_;
    IntVector2            dragBeginCursor_;

    // input function
    HashMap<String, IOElement*>   cacheVarNameToIOElement_;
};

//=============================================================================
//=============================================================================
class NodeHeader : public BorderImage
{
    URHO3D_OBJECT(NodeHeader, BorderImage);
public:
    static void RegisterObject(Context* context);

    NodeHeader(Context *context);
    virtual ~NodeHeader();

    virtual void OnDoubleClick(const IntVector2& position, const IntVector2& screenPosition, 
                               int button, int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                             int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

protected:
    WeakPtr<UIElement> headerParen_;
};


