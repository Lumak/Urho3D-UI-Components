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

#include "GraphNode.h"

//=============================================================================
//=============================================================================
enum IOType
{
    IOTYPE_UNDEF,
    IOTYPE_INPUT,
    IOTYPE_OUTPUT
};

//=============================================================================
//=============================================================================
class IOElement : public BorderImage
{
    URHO3D_OBJECT(IOElement, BorderImage);

public:
    static void RegisterObject(Context* context);

    IOElement(Context *context);
    virtual ~IOElement(){}

    IOType GetIOType() const { return ioType_; }
    GraphNode* GetNodeBasePtr() { return nodebaseParent_; }

    // related to I/O
    void SetVariableName(const String& varName);
    const String& GetVariableName()         { return variableName_; }

    // related to timevar input
    virtual float GetValueRangeMin(const String &varName){ return 0.0f; }
    virtual float GetValueRangeMax(const String &varName){ return 0.0f; }
    virtual float GetStartTime(const String &varName){ return 0.0f; }
    virtual float GetEndTime(const String &varName){ return 0.0f; }
    virtual float GetValueAtTime(const String &varName, float time){ return 0.0f; }

    // related to slidevar input
    virtual const Variant& GetRangeMin(const String &varName){ return Variant::EMPTY; }
    virtual const Variant& GetRangeMax(const String &varName){ return Variant::EMPTY; }
    virtual const Variant& GetCurrentValue(const String &varName){ return Variant::EMPTY; }


protected:
    void SetIOType(IOType iotype) { ioType_ = iotype; }

    bool InitBaseNodeParent();
    bool SetBasePtrs(UIElement *nodebaseParent);
    bool FindBaseNodePtr();
    UIElement* RecursiveFindNodebase(UIElement*);

    void CreateTextLabel();
    void SetTextLabel(const String& label);

    UIElement* GetHeaderElement()    { return nodebaseParent_->GetHeaderElement();     }
    UIElement* GetBodyElement()      { return nodebaseParent_->GetBodyElement();       }
    UIElement* GetInputBodyElement() { return nodebaseParent_->GetInputBodyElement();  }
    UIElement* GetOutputBodyElement(){ return nodebaseParent_->GetOutputBodyElement(); }

protected:
    WeakPtr<GraphNode> nodebaseParent_;
    WeakPtr<Text>      labelText_;
    String             variableName_;

private:
    IOType ioType_;
};


