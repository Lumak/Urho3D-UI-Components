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

#include "PageManager.h"
#include "InputNodeManager.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
void InputNodeManager::RegisterObject(Context* context)
{
    context->RegisterSubsystem( new InputNodeManager(context) );
}

InputNodeManager::InputNodeManager(Context *context)
    : Object(context)
{
}

InputNodeManager::~InputNodeManager()
{
}

bool InputNodeManager::Add(InputBox *inputBox)
{
    for ( unsigned i = 0; i < inputBoxList_.Size(); ++i )
    {
        if ( inputBox == inputBoxList_[i] )
        {
            return false;
        }
    }

    inputBoxList_.Push(inputBox);
    return true;
}

bool InputNodeManager::Remove(InputBox *inputBox)
{
    bool found = false;

    for ( unsigned i = 0; i < inputBoxList_.Size(); ++i )
    {
        if ( inputBox == inputBoxList_[i] )
        {
            inputBoxList_.Erase(i);
            found = true;
            break;
        }
    }
    
    return found;
}

bool InputNodeManager::GetNodesInside(Vector<InputBox*>& result, const Vector2 &p0, const Vector2 &s0)
{
    result.Clear();

    // not concerned w/ accuracy: evaluate as circle collision instead of rectangle
    // touching  = (0.5*s1 + 0.5*s0)*0.707 = (s1+s0)*0.3535, base on the length of the sides, for corners change 0.707 to 1.0
    // collision < (s1+s0)*0.3535, use 0.34 for a slight overlap
    for ( unsigned i = 0; i < inputBoxList_.Size(); ++i )
    {
        Vector2 p1((float)inputBoxList_[i]->GetPosition().x_, (float)inputBoxList_[i]->GetPosition().y_);
        Vector2 s1((float)inputBoxList_[i]->GetSize().x_, (float)inputBoxList_[i]->GetSize().y_);

        float plen = (p1 - p0).Length();
        float slen = ((s1 + s0) * 0.34f).Length();

        if ( plen < slen )
        {
            result.Push(inputBoxList_[i]);
        }
    }

    return (result.Size() > 0);
}

