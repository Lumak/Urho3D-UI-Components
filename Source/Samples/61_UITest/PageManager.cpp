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
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "PageManager.h"
#include "LineComponent.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
const Color ENABLEDColor(0.0f, 0.8f, 0.9f, 0.9f);
const Color DISABLEDColor(0.0f, 0.5f, 0.6f, 0.9f);

//=============================================================================
//=============================================================================
void PageManager::RegisterObject(Context* context)
{
    context->RegisterSubsystem( new PageManager(context) );
}

PageManager::PageManager(Context *context) : Object(context)
{
}

PageManager::~PageManager()
{
}

bool PageManager::CreatePages(int numPages)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    UIElement* root = ui->GetRoot();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/rightleftarrows.png");

    rootSize_ = root->GetSize();
    currentPageIdx_ = 0;

    // create control page
    controlPage_ = root->CreateChild<UIElement>();
    controlPage_->SetSize(rootSize_);

    // static line
    PODVector<IntVector2> points;
    points.Push(IntVector2(0, 40));
    points.Push(IntVector2(rootSize_.x_, 40));
    StaticLine *line = controlPage_->CreateChild<StaticLine>();
    line->SetUIRoot(controlPage_);
    line->CreateLine(points, STRAIGHT_LINE, Color(0.0f, 0.0f, 0.5f), 50.0f);
    line->SetColor(C_TOPLEFT, Color(1.0f, 0.0f, 0.0f, 0.0f));
    line->SetColor(C_BOTTOMLEFT, Color(1.0f, 0.0f, 0.0f, 0.0f));
    line->SetBlendMode(BLEND_ALPHA);

    // page buttons
    IntRect leftRect(300, 0, 399, 71);
    IntRect rightRect(100, 0, 199, 71);
    IntVector2 bpos = rootSize_/2;
    bpos.y_ = 10;

    buttonPrev_ = CreateButton(bpos - IntVector2(120, 0), leftRect, DISABLEDColor);
    buttonPrev_->SetEnabled(false);

    buttonNext_ = CreateButton(bpos + IntVector2(120, 0), rightRect, ENABLEDColor);

    // text
    Text *text = controlPage_->CreateChild<Text>();
    text->SetFont(cache->GetResource<Font>("Fonts/DejaVu/DejaVuSerifCondensed-Bold.ttf"), 40);
    text->SetText("Urho3D UI");
    text->SetColor(Color::CYAN);
    text->SetPosition(rootSize_.x_ - 300, 10);
    text->SetTextEffect(TE_SHADOW);

    // create pages
    for ( int i = 0; i < numPages; ++i )
    {
        UIElement *page = root->CreateChild<UIElement>();
        page->SetSize(rootSize_);
        page->SetVisible(i==0);
        pageList_.Push(page);
    }

    return true;
}

Button* PageManager::CreateButton(const IntVector2 &pos, const IntRect &rect, const Color &color)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *tex2d = cache->GetResource<Texture2D>("Textures/rightleftarrows.png");

    Button* button = controlPage_->CreateChild<Button>();
    button->SetTexture(tex2d);
    button->SetImageRect(rect);
    button->SetPosition(pos);
    button->SetSize(70, 70);
    button->SetColor(color);
    button->SetHoverOffset(IntVector2(0, 71));
    button->SetPressedOffset(IntVector2(-99, 0));

    SubscribeToEvent(button, E_RELEASED, URHO3D_HANDLER(PageManager, HandleButtonReleased));

    return button;
}

UIElement* PageManager::GetRoot()
{
    return GetPageRoot(currentPageIdx_);
}

UIElement* PageManager::GetPageRoot(unsigned idx)
{
    UIElement *page = NULL;

    if (idx < pageList_.Size())
    {
        page = pageList_[idx];
    }

    return page;
}

void PageManager::SetPageIndex(int idx)
{
    if (idx < (int)pageList_.Size())
    {
        GetPageRoot(currentPageIdx_)->SetVisible(false);
        currentPageIdx_ = idx;
        GetPageRoot(currentPageIdx_)->SetVisible(true);
    }

    UpdateButtonState(currentPageIdx_);
}

int PageManager::GetPageIndex()
{
    return currentPageIdx_;
}

void PageManager::HandleButtonReleased(StringHash eventType, VariantMap& eventData)
{
    using namespace Released;

    Button *button = (Button*)eventData[P_ELEMENT].GetVoidPtr();
    unsigned idx = currentPageIdx_;

    if (button == buttonPrev_ && idx > 0)
    {
        idx--;
        SetPageIndex(idx);
    }

    if (button == buttonNext_ && idx < pageList_.Size() - 1)
    {
        idx++;
        SetPageIndex(idx);
    }
}

void PageManager::UpdateButtonState(int idx)
{
    if (idx == 0)
    {
        buttonPrev_->SetEnabled(false);
        buttonPrev_->SetColor(DISABLEDColor);
    }
    else
    {
        buttonPrev_->SetEnabled(true);
        buttonPrev_->SetColor(ENABLEDColor);
    }

    if (idx == pageList_.Size() - 1)
    {
        buttonNext_->SetEnabled(false);
        buttonNext_->SetColor(DISABLEDColor);
    }
    else
    {
        buttonNext_->SetEnabled(true);
        buttonNext_->SetColor(ENABLEDColor);
    }
}




