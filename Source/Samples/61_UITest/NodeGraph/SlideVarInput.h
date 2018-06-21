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
#include <Urho3D/Core/Variant.h>
#include "IOElement.h"

namespace Urho3D
{
class Text;
}

using namespace Urho3D;
//=============================================================================
//=============================================================================
URHO3D_EVENT(E_SLIDEBAR_VARCHANGED, SlideBarVarChanged)
{
    URHO3D_PARAM(P_ELEMENT, Element);  // UIElement pointer
    URHO3D_PARAM(P_VALUE, var);      // variable
}

//=============================================================================
//=============================================================================
typedef void (UIElement::*VarChangedCallback)(Variant &var);

class SlideVarInput : public IOElement
{
    URHO3D_OBJECT(SlideVarInput, IOElement);
public:
    static void RegisterObject(Context* context);

    SlideVarInput(Context *context);
    virtual ~SlideVarInput();

    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

    bool CreateBar(const String &variableName, const IntVector2 &size);

    void SetRange(const Variant &vmin, const Variant &vmax);
    void SetCurrentValue(const Variant &val);
    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }
    void SetVarChangedCallback(UIElement *process, VarChangedCallback callback);

    const Variant& GetCurrentValue() { return varCurrentValue_; }

    // related to slidevar input
    virtual const Variant& GetRangeMin(const String &varName);
    virtual const Variant& GetRangeMax(const String &varName);
    virtual const Variant& GetCurrentValue(const String &varName);

protected:
    bool InitInternal();
    void ValueUpdate(float val);

protected:
    UIElement           *processCaller;
    VarChangedCallback  pfnVarChangedCallback;

    WeakPtr<Text>          variableText_;
    Color                  internalColor_;
                          
    Variant                varMin_;
    Variant                varMax_;
    Variant                varCurrentValue_;

    float                  currentValue_;
    float                  sensitivity_;
};

