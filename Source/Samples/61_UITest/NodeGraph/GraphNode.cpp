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

#include "GraphNode.h"
#include "PageManager.h"
#include "InputNodeManager.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "SlideVarInput.h"
#include "TimeVarInput.h"
#include "TimeVarNode.h"
#include "SlideVarNode.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
Color GraphNode::colorHeader_(0.2f, 0.2f, 0.2f);
Color GraphNode::colorBody_(0.3f, 0.3f, 0.3f);
IntVector2 GraphNode::ioNodeSize_(25, 25);

//=============================================================================
//=============================================================================
void GraphNode::RegisterObject(Context* context)
{
    context->RegisterFactory<GraphNode>(UI_CATEGORY);

    // register all node graph components
    NodeHeader::RegisterObject(context);
    InputNodeManager::RegisterObject(context);

    // io elements
    IOElement::RegisterObject(context);
    InputNode::RegisterObject(context);
    OutputNode::RegisterObject(context);

    // specific io elements
    SlideVarInput::RegisterObject(context);
    TimeVarInput::RegisterObject(context);

    TimeVarNode::RegisterObject(context);
    SlideVarNode::RegisterObject(context);
}

GraphNode::GraphNode(Context *context) 
    : BorderImage(context)
    , footerToggle_(true)
{
    UIElement::SetEnabled(false);

    InitInternal();
}

GraphNode::~GraphNode()
{
}

bool GraphNode::InitInternal()
{
    SetLayoutBorder(IntRect(2,2,2,2));
    SetLayoutMode(LM_VERTICAL);
    UIElement::SetColor(colorHeader_);

    // header
    headerElement_ = CreateChild<NodeHeader>();
    headerElement_->SetLayoutMode(LM_HORIZONTAL);
    headerElement_->SetLayoutBorder(IntRect(5,0,5,0));
    headerElement_->SetMinHeight(25);
    headerElement_->SetColor(colorHeader_);

    headerText_ = headerElement_->CreateChild<Text>();
    headerText_->SetAlignment(HA_LEFT, VA_CENTER);

    // body
    bodyElement_= CreateChild<BorderImage>();
    bodyElement_->SetLayoutMode(LM_HORIZONTAL);
    bodyElement_->SetLayoutBorder(IntRect(0,0,0,0));
    bodyElement_->SetClipChildren(true);
    bodyElement_->SetColor(colorBody_);

        // inner bodies
        inputBodyElement_= bodyElement_->CreateChild<BorderImage>();
        inputBodyElement_->SetLayoutMode(LM_VERTICAL);
        inputBodyElement_->SetLayoutBorder(IntRect(0,0,0,0));
        inputBodyElement_->SetClipChildren(true);
        inputBodyElement_->SetColor(colorBody_);

        outputBodyElement_= bodyElement_->CreateChild<BorderImage>();
        outputBodyElement_->SetLayoutMode(LM_VERTICAL);
        outputBodyElement_->SetLayoutBorder(IntRect(0,0,0,0));
        outputBodyElement_->SetClipChildren(true);
        outputBodyElement_->SetColor(colorBody_);

    // footer - not visible by default
    footerElement_ = CreateChild<BorderImage>();
    footerElement_->SetLayoutMode(LM_HORIZONTAL);
    footerElement_->SetLayoutBorder(IntRect(5,0,0,0));
    footerElement_->SetVisible(false);
    footerElement_->SetColor(colorHeader_);

    footerText_ = footerElement_->CreateChild<Text>();

    return true;
}

void GraphNode::AddChild(UIElement* element)
{
    IOElement *ioelem = dynamic_cast<IOElement*>(element);

    if ( ioelem )
    {
        if (ioelem->GetIOType() == IOTYPE_INPUT)
        {
            inputBodyElement_->AddChild(element);
        }
        else if (ioelem->GetIOType() == IOTYPE_OUTPUT)
        {
            outputBodyElement_->AddChild(element);
        }
        else
        {
            assert( false && "all derived IOElement must specify the IOType, call SetIOType() in the constructor");
        }
    }
    else
    {
        assert( false && "can't add any other element types in GraphNode class");
    }
}

void GraphNode::SetEnabled(bool enable)
{
    /// disabled - use the header instead
}

void GraphNode::SetColor(const Color& color)
{
    headerElement_->SetColor(color);
    footerElement_->SetColor(color);

    UIElement::SetColor(color);
}

void GraphNode::SetBodyColor(const Color& color)
{
    bodyElement_->SetColor(color);
    inputBodyElement_->SetColor(color);
    outputBodyElement_->SetColor(color);
}

bool GraphNode::SetHeaderFont(const String& fontName, int size)
{
    return headerText_->SetFont(fontName, size);
}

bool GraphNode::SetHeaderFont(Font* font, int size)
{
    return headerText_->SetFont(font, size);
}

void GraphNode::SetHeaderText(const String& text)
{
    headerText_->SetText(text);
}

void GraphNode::SetFooterVisible(bool vis)
{
    footerElement_->SetVisible(vis);
}

bool GraphNode::SetFooterFont(const String& fontName, int size)
{
    return footerText_->SetFont(fontName, size);
}

bool GraphNode::SetFooterFont(Font* font, int size)
{
    return footerText_->SetFont(font, size);
}

void GraphNode::SetFooterText(const String& text)
{
    footerText_->SetText(text);
}

void GraphNode::OnDoubleClick(const IntVector2& position, const IntVector2& screenPosition, 
                              int button, int buttons, int qualifiers, Cursor* cursor)
{
    footerToggle_ = !footerToggle_;

    if ( !footerToggle_ )
    {
        IntVector2 tsize = GetSize();
        tsize.x_ -= footerElement_->GetSize().x_;
        tsize.y_ -= footerElement_->GetSize().y_;
        footerElement_->SetVisible(footerToggle_);
        SetSize( tsize );
    }
    else
    {
        footerElement_->SetVisible(footerToggle_);
    }
}

void GraphNode::OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                           int buttons, int qualifiers, Cursor* cursor)
{
    dragBeginCursor_ = screenPosition;
    dragBeginPosition_ = GetPosition();

    // send event
    using namespace BaseDragBegin;
    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    SendEvent(E_BASE_DRAGBEIN, eventData);
}

void GraphNode::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                          const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    IntVector2 delta = screenPosition - dragBeginCursor_;
    SetPosition(dragBeginPosition_ + delta);

    // send event
    using namespace BaseDragMove;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_ELEMENT] = this;
    SendEvent(E_BASE_DRAGMOVE, eventData);
}

void GraphNode::OnDragEnd(const IntVector2& position, const IntVector2& screenPosition, 
                         int dragButtons, int releaseButton, Cursor* cursor)
{
}

//=============================================================================
//=============================================================================
void NodeHeader::RegisterObject(Context* context)
{
    context->RegisterFactory<NodeHeader>(UI_CATEGORY);
}

NodeHeader::NodeHeader(Context *context)
    : BorderImage(context)
{
    SetEnabled(true);
}

NodeHeader::~NodeHeader()
{
}

void NodeHeader::OnDoubleClick(const IntVector2& position, const IntVector2& screenPosition, 
                               int button, int buttons, int qualifiers, Cursor* cursor)
{
    ((GraphNode*)GetParent())->OnDoubleClick(position, screenPosition, button, buttons, qualifiers, cursor);
}

void NodeHeader::OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                             int buttons, int qualifiers, Cursor* cursor)
{
    ((GraphNode*)GetParent())->OnDragBegin(position, screenPosition, buttons, qualifiers, cursor);
}

void NodeHeader::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    ((GraphNode*)GetParent())->OnDragMove(position, screenPosition, deltaPos, buttons, qualifiers, cursor);
}

//===============================================
// related to timevar input
float GraphNode::GetValueRangeMin(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetValueRangeMin(varName);
    }
    return 0.0f;
}

float GraphNode::GetValueRangeMax(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetValueRangeMax(varName);
    }
    return 0.0f;
}

float GraphNode::GetStartTime(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetStartTime(varName);
    }
    return 0.0f;
}

float GraphNode::GetEndTime(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetEndTime(varName);
    }
    return 0.0f;
}

float GraphNode::GetValueAtTime(const String &varName, float time)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetValueAtTime(varName, time);
    }
    return 0.0f;
}

// related to slidevar input
const Variant& GraphNode::GetRangeMin(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetRangeMin(varName);
    }

    return Variant::EMPTY;
}

const Variant& GraphNode::GetRangeMax(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetRangeMax(varName);
    }

    return Variant::EMPTY;
}

const Variant& GraphNode::GetCurrentValue(const String &varName)
{
    IOElement *elem = FindInuptVarName(varName);

    if ( elem )
    {
        return elem->GetCurrentValue(varName);
    }

    return Variant::EMPTY;
}

IOElement* GraphNode::FindInuptVarName(const String &varName)
{
    HashMap<String, IOElement*>::Iterator itr = cacheVarNameToIOElement_.Find( varName );
    IOElement *elem = NULL;

    if ( itr != cacheVarNameToIOElement_.End() )
    {
        return itr->second_;
    }

    // else query
    for ( unsigned i = 0; i < inputBodyElement_->GetNumChildren(); ++i )
    {
        // we've already insured that every child in inputBodyElement_
        // must be IOElement, no need to dynamically cast to be sure
        elem = (IOElement*)inputBodyElement_->GetChild(i);

        if ( elem->GetVariableName() == varName )
        {
            cacheVarNameToIOElement_.Insert( Pair<String, IOElement*>(varName, elem) );
            return elem;
        }
    }

    return NULL;
}



