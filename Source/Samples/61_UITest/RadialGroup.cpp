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
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "RadialGroup.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
void RadialText::RegisterObject(Context* context)
{
    context->RegisterFactory<RadialText>();
}

void RadialText::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, 
                              int button, int buttons, int qualifiers, Cursor* cursor)
{
    using namespace Toggled;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    SendEvent(E_PRESSED, eventData);
}

void RadialGroup::RegisterObject(Context* context)
{
    context->RegisterFactory<RadialGroup>();
    RadialText::RegisterObject(context);
}

RadialGroup::RadialGroup(Context *context) 
    : BorderImage(context)
{
    SetLayoutMode(LM_VERTICAL);
    SetLayoutBorder(IntRect(4,4,4,4));

    // header
    headerElement_ = CreateChild<UIElement>();
    headerElement_->SetLayoutMode(LM_VERTICAL);
    headerElement_->SetLayoutBorder(IntRect(10,3,3,3));
    headerElement_->SetMaxHeight(25);

    headerText_ = headerElement_->CreateChild<Text>();
    headerText_->SetVerticalAlignment(VA_CENTER);

    // body
    bodyElement_ = CreateChild<UIElement>();
    bodyElement_->SetLayoutMode(LM_VERTICAL);
    bodyElement_->SetLayoutBorder(IntRect(10,3,3,3));
}

RadialGroup::~RadialGroup()
{
}

RadialElement* RadialGroup::CreateRadialButton()
{
    RadialElement radElem;

    UIElement *element = bodyElement_->CreateChild<UIElement>();
    radElem.bodyElement_ = element;
    element->SetLayoutBorder(IntRect(10,4,4,4));
    element->SetFocusMode(FM_FOCUSABLE_DEFOCUSABLE);
    element->SetLayoutMode(LM_HORIZONTAL);
    element->SetLayoutSpacing(10);
    element->SetMaxHeight(25);

    radElem.checkbox_ = element->CreateChild<CheckBox>();
    radElem.checkbox_->SetStyleAuto();
    radElem.checkbox_->SetImageRect(IntRect(208, 96, 224, 112));
    radElem.checkbox_->SetChecked(childList_.Size() == 0);

    radElem.textDesc_ = element->CreateChild<RadialText>();
    childList_.Push(radElem);

    return &childList_.Back();
}

RadialElement* RadialGroup::GetRadialButtonDesc(unsigned idx)
{
    RadialElement *desc = NULL;

    if (idx < childList_.Size())
    {
        desc = &childList_[idx];
    }

    return desc;
}

Text* RadialGroup::GetTitleTextElement() 
{
    return headerText_; 
}

bool RadialGroup::SetHeaderFont(const String& fontName, int size)
{
    return headerText_->SetFont(fontName, size);
}

bool RadialGroup::SetHeaderFont(Font* font, int size)
{
    return headerText_->SetFont(font, size);
}

bool RadialGroup::SetHeaderFontSize(int size)
{
    return headerText_->SetFontSize(size);
}

void RadialGroup::SetHeaderText(const String& text)
{
    headerText_->SetText(text);
}

void RadialGroup::SetEnabled(bool enabled)
{
    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        if (enabled)
        {
            SubscribeToEvent(childList_[i].checkbox_, E_TOGGLED, URHO3D_HANDLER(RadialGroup, HandleCheckbox));

            childList_[i].textDesc_->SetEnabled(true);
            SubscribeToEvent(childList_[i].textDesc_, E_PRESSED, URHO3D_HANDLER(RadialGroup, HandlePressed));
        }
        else
        {
            childList_[i].textDesc_->SetEnabled(false);
            UnsubscribeFromEvent(childList_[i].checkbox_, E_TOGGLED);
            UnsubscribeFromEvent(childList_[i].textDesc_, E_PRESSED);
        }
    }
}

void RadialGroup::HandlePressed(StringHash eventType, VariantMap& eventData)
{
    using namespace Pressed;
    UIElement *element = (UIElement*)eventData[P_ELEMENT].GetVoidPtr();
    int selectedIdx = -1;

    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        CheckBox *chkbox = childList_[i].checkbox_;

        if (childList_[i].textDesc_ == element && !chkbox->IsChecked())
            selectedIdx = i;

        chkbox->SetCheckedInternal(childList_[i].textDesc_ == element);
    }

    if (selectedIdx >= 0)
    {
        SendGroupToggleEvent(selectedIdx);
    }
}

void RadialGroup::HandleCheckbox(StringHash eventType, VariantMap& eventData)
{
    using namespace Toggled;

    CheckBox *element = (CheckBox*)eventData[P_ELEMENT].GetVoidPtr();
    bool checked = eventData[P_STATE].GetBool();
    int checkedIdx = -1;

    for ( int i = 0; i < (int)childList_.Size(); ++i )
    {
        CheckBox *chkbox = childList_[i].checkbox_;

        if (chkbox == element)
            checkedIdx = i;

        chkbox->SetCheckedInternal(chkbox == element);
    }

    if (checkedIdx >= 0)
    {
        SendGroupToggleEvent(checkedIdx);
    }
}

void RadialGroup::SendGroupToggleEvent(int idx)
{
    using namespace RadialGroupToggled;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    eventData[P_INDEX] = idx;
    SendEvent(E_RADIALGROUPTOGGLED, eventData);
}



