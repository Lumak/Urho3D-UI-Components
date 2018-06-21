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
#include <Urho3D/Core/Object.h>
//#include <Urho3D/UI/CheckBox.h>

namespace Urho3D
{
extern const char* UI_CATEGORY;
class Button;
}
using namespace Urho3D;
//=============================================================================
//=============================================================================
class PageManager : public Object
{
    URHO3D_OBJECT(PageManager, Object);
public:
    static void RegisterObject(Context* context);

    PageManager(Context *context);
    virtual ~PageManager();

    bool CreatePages(int numPages);

    UIElement* GetControlPage() { return controlPage_; }

    UIElement* GetRoot();
    UIElement* GetPageRoot(unsigned idx);
    void SetPageIndex(int idx);
    int GetPageIndex();

protected:
    Button* CreateButton(const IntVector2 &pos, const IntRect &rect, const Color &color);
    void UpdateButtonState(int idx);
    void HandleButtonReleased(StringHash eventType, VariantMap& eventData);

protected:
    WeakPtr<UIElement> controlPage_;
    Vector<UIElement*> pageList_;
    IntVector2         rootSize_;
    int                currentPageIdx_;

    // buttons
    WeakPtr<Button>    buttonPrev_;
    WeakPtr<Button>    buttonNext_;
};


