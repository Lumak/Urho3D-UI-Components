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
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <SDL/SDL_log.h>

#include "IOElement.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void IOElement::RegisterObject(Context* context)
{
    context->RegisterFactory<IOElement>(UI_CATEGORY);
}

IOElement::IOElement(Context *context) 
    : BorderImage(context)
    , ioType_(IOTYPE_UNDEF) 
{
}

bool IOElement::InitBaseNodeParent()
{
    if ( FindBaseNodePtr() )
    {
        // place the object into the proper container
        if ( GetIOType() == IOTYPE_INPUT )
        {
            GetInputBodyElement()->AddChild(this);
        }
        else if ( GetIOType() == IOTYPE_OUTPUT )
        {
            GetOutputBodyElement()->AddChild(this);
        }
        else
        {
            assert( false && "all derived IOElement must specify the IOType, call SetIOType() in the constructor");
        }
    }

    return true;
}

void IOElement::CreateTextLabel()
{
    if ( labelText_ == NULL )
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        SetLayoutBorder(IntRect(5,0,0,0));

        labelText_ = CreateChild<Text>();
        labelText_->SetAlignment(HA_CENTER, VA_TOP);
        labelText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 8);
        labelText_->SetText("out");
    }
}

void IOElement::SetTextLabel(const String& label)
{
    if ( labelText_ )
    {
        labelText_->SetText(label);
    }
}

void IOElement::SetVariableName(const String& varName)
{
    variableName_ = varName;
    SetTextLabel(varName);
}

bool IOElement::FindBaseNodePtr()
{
    nodebaseParent_ = (GraphNode*)RecursiveFindNodebase( GetParent() );

    return (nodebaseParent_ != NULL);
}

UIElement* IOElement::RecursiveFindNodebase(UIElement *searchParent)
{
    if (searchParent == NULL)
    {
        return NULL;
    }

    if ( searchParent->IsInstanceOf( StringHash("GraphNode") ) )
    {
        return searchParent;
    }

    return RecursiveFindNodebase( searchParent->GetParent() );
}


