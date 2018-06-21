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
#include <Urho3D/Core/Spline.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <SDL/SDL_log.h>

#include "SlideVarInput.h"
#include "PageManager.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
#define MIN_VAR_HEIGHT  22

//=============================================================================
//=============================================================================
void SlideVarInput::RegisterObject(Context* context)
{
    context->RegisterFactory<SlideVarInput>();
}

SlideVarInput::SlideVarInput(Context *context)
    : IOElement(context)
    , processCaller(NULL)
    , pfnVarChangedCallback(NULL)
    , currentValue_(0.0f)
    , sensitivity_(0.1f)
{
    SetEnabled(true);
    SetIOType(IOTYPE_INPUT);
}

SlideVarInput::~SlideVarInput()
{
}

bool SlideVarInput::InitInternal()
{

    return true;
}

bool SlideVarInput::CreateBar(const String &variableName, const IntVector2 &size)
{
    InitBaseNodeParent();

    if ( variableText_ == NULL )
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        SetLayoutMode(LM_HORIZONTAL);
        SetLayoutBorder(IntRect(5,0,0,0));

        variableText_ = CreateChild<Text>();
        variableText_->SetVerticalAlignment(VA_CENTER);
        variableText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    }

    SetVariableName(variableName);

    // set size
    SetFixedSize(size);

    return true;
}

void SlideVarInput::SetRange(const Variant &vmin, const Variant &vmax)
{
    if (!(vmin.GetType() == VAR_INT || vmin.GetType() == VAR_FLOAT) ||
        !(vmax.GetType() == VAR_INT || vmax.GetType() == VAR_FLOAT) ||
         vmin.GetType() != vmax.GetType())
    {
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }

    varMin_ = vmin;
    varMax_ = vmax;
}

void SlideVarInput::SetCurrentValue(const Variant &val)
{
    varCurrentValue_ = val;

    switch (val.GetType())
    {
    case VAR_INT:
        currentValue_ = (float)varCurrentValue_.GetInt();
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
        break;

    case VAR_FLOAT:
        currentValue_ = varCurrentValue_.GetFloat();
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
        break;

    default:
        assert(false && "only INT and FLOAT are implemented, implement what you need");
    }
}

void SlideVarInput::SetVarChangedCallback(UIElement *process, VarChangedCallback callback)
{
    processCaller = process;
    pfnVarChangedCallback = callback;
}

void SlideVarInput::ValueUpdate(float delta)
{
    if (varMax_.GetType() == VAR_INT)
    {
        currentValue_ += delta * sensitivity_;
        currentValue_ = Clamp(currentValue_, (float)varMin_.GetInt(), (float)varMax_.GetInt());
        varCurrentValue_ = (int)currentValue_;
        variableText_->SetText( String(varCurrentValue_.GetInt()) );
    }
    else //(varMax_.GetType() == VAR_FLOAT)
    {
        currentValue_ += delta * sensitivity_;
        currentValue_ = Clamp(currentValue_, varMin_.GetFloat(), varMax_.GetFloat());
        varCurrentValue_ = currentValue_;
        variableText_->SetText( String(varCurrentValue_.GetFloat()) );
    }

    // listener callback
    if (processCaller && pfnVarChangedCallback)
    {
        (processCaller->*pfnVarChangedCallback)(varCurrentValue_);
    }
    else
    {
        using namespace SlideBarVarChanged;
        VariantMap& eventData = GetEventDataMap();
        eventData[P_ELEMENT] = this;
        eventData[P_VALUE] = varCurrentValue_;
        SendEvent(E_SLIDEBAR_VARCHANGED, eventData);
    }
}

void SlideVarInput::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                          const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    ValueUpdate((float)deltaPos.x_ );
}

//===============================================
// related to slidevar input
const Variant& SlideVarInput::GetRangeMin(const String &varName)
{
    if ( varName == variableName_ )
    {
        return varMin_;
    }
    return Variant::EMPTY;
}

const Variant& SlideVarInput::GetRangeMax(const String &varName)
{
    if ( varName == variableName_ )
    {
        return varMax_;
    }
    return Variant::EMPTY;
}

const Variant& SlideVarInput::GetCurrentValue(const String &varName)
{
    if ( varName == variableName_ )
    {
        return varCurrentValue_;
    }
    return Variant::EMPTY;
}

