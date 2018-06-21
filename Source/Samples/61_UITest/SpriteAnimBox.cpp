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
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLElement.h>

#include "SpriteAnimBox.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void SpriteAnimBox::RegisterObject(Context* context)
{
    context->RegisterFactory<SpriteAnimBox>();
}

SpriteAnimBox::SpriteAnimBox(Context *context)
    : BorderImage(context)
    , spriteIndex_(0)
    , elapsedTime_(0)
    , frameMsec_(30)
    , paused_(true)
{
    SetLayoutMode(LM_VERTICAL);

    headerElement_ = CreateChild<UIElement>();
    headerElement_->SetMaxHeight(20);

    headerText_ = headerElement_->CreateChild<Text>();
    headerText_->SetHorizontalAlignment(HA_CENTER);

    controlElement_ = CreateChild<UIElement>();
    controlElement_->SetLayoutMode(LM_VERTICAL); //doesn't seem to matter if it's set as LM_HORIZONTAL
    controlElement_->SetMaxHeight(50);

    playButton_ = controlElement_->CreateChild<CheckBox>();
    playButton_->SetHorizontalAlignment(HA_CENTER);

    bodyElement_ = CreateChild<BorderImage>();
    bodyElement_->SetHorizontalAlignment(HA_CENTER);
}

SpriteAnimBox::~SpriteAnimBox()
{
}

void SpriteAnimBox::Create(IntVector2 &size, bool showHeader, bool showControl)
{
    headerElement_->SetVisible(showHeader);
    headerElement_->SetWidth(size.x_);

    controlElement_->SetWidth(size.x_);
    controlElement_->SetVisible(showControl);

    SetDefaultPlayButton();

    SetSize(size);
}

void SpriteAnimBox::ShowHeader(bool show)
{
    headerElement_->SetVisible(show);
}

void SpriteAnimBox::ShowControl(bool show)
{
    controlElement_->SetVisible(show);
}

void SpriteAnimBox::SetDefaultPlayButton()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    playButton_->SetMaxSize(30,30);
    playButton_->SetDefaultStyle(cache->GetResource<XMLFile>("UI/EditorIcons.xml"));
    playButton_->SetStyle("RunUpdatePlay");
    playButton_->SetCheckedOffset(IntVector2(32,0));
}

bool SpriteAnimBox::SetHeaderFont(const String& fontName, int size)
{
    return headerText_->SetFont(fontName, size);
}

bool SpriteAnimBox::SetHeaderFont(Font* font, int size)
{
    return headerText_->SetFont(font, size);
}

bool SpriteAnimBox::SetHeaderFontSize(int size)
{
    return headerText_->SetFontSize(size);
}

void SpriteAnimBox::SetHeaderText(const String& text)
{
    headerText_->SetText(text);
}

void SpriteAnimBox::AddSprite(const String& spriteFile)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>(spriteFile);
    spriteList_.Push(tex2d);

    if (spriteList_.Size() == 1)
    {
        bodyElement_->SetMaxSize(tex2d->GetWidth(), tex2d->GetHeight());
		bodyElement_->SetTexture(spriteList_[0]);
    }
}

void SpriteAnimBox::SetEnabled(bool enable)
{
    elapsedTime_ = 0;
    spriteIndex_ = 0;

    if ( enable )
    {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SpriteAnimBox, HandleUpdate));
        SubscribeToEvent(playButton_, E_TOGGLED, URHO3D_HANDLER(SpriteAnimBox, HandleCheckbox));
    }
    else
    {
        UnsubscribeFromEvent(E_UPDATE);
        UnsubscribeFromEvent(playButton_, E_TOGGLED);
    }
}

void SpriteAnimBox::Play()
{
    paused_ = false;
}

void SpriteAnimBox::Pause()
{
    paused_ = true;
}

void SpriteAnimBox::Quit()
{
    UnsubscribeFromEvent(E_UPDATE);
    UnsubscribeFromEvent(playButton_, E_TOGGLED);
}

void SpriteAnimBox::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    elapsedTime_ += (int)(eventData[P_TIMESTEP].GetFloat() * 1000.0f);

    if ( !paused_ && elapsedTime_ >= frameMsec_ )
    {
        elapsedTime_ = 0;
		spriteIndex_ = ++spriteIndex_ % spriteList_.Size();
		bodyElement_->SetTexture(spriteList_[spriteIndex_]);
    }
}

void SpriteAnimBox::HandleCheckbox(StringHash eventType, VariantMap& eventData)
{
    using namespace Toggled;

    bool checked = eventData[P_STATE].GetBool();

    if (checked)
        Play();
    else
        Pause();
}
