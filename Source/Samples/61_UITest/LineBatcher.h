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
#include <Urho3D/UI/UIElement.h>

namespace Urho3D
{
extern const char* blendModeNames[];
class Button;
}

using namespace Urho3D;
//=============================================================================
//=============================================================================
#define NUM_PTS_PER_CURVE_SEGMENT   5

enum LineType
{
    STRAIGHT_LINE,
    CURVE_LINE,
};

struct RectVectors
{
    RectVectors(){}
    RectVectors(Vector2 &_a, Vector2 &_b, Vector2 &_c, Vector2 &_d) 
        : a(_a), b(_b), c(_c), d(_d) {}

    Vector2 a, b, c, d;
};

//=============================================================================
//=============================================================================
class LineBatcher : public UIElement
{
    URHO3D_OBJECT(LineBatcher, UIElement);
public:
    static void RegisterObject(Context* context);

    LineBatcher(Context *context);
    virtual ~LineBatcher();

    static IntRect& GetBoxRect()    { return boxRect_; }
    static IntVector2& GetBoxSize() { return boxSize_; }

    void SetConstrainingParent(UIElement *contrainingParent) { constrainParentElement_ = contrainingParent; }

    void SetColor(const Color& color);
    void SetColor(Corner corner, const Color& color);

    void SetLineTexture(Texture* texture);
    void SetLineRect(const IntRect& rect)   { lineImageRect_ = rect; }
    void SetLineData(Texture* texture, const IntRect& rect);
    void SetLinePixelSize(float pixelSize)  { linePixelSize_ = pixelSize; }
    void SetLineOpacity(float opacity);
    void SetLineType(LineType lineType);
    LineType GetLineType() const { return lineType_; }
    void SetBlendMode(BlendMode mode);
    BlendMode GetBlendMode() const { return blendMode_; }

    void SetNumPointsPerSegment(int numPtsPerSegment) { numPtsPerSegment_ = numPtsPerSegment; }
    void AddPoint(const IntVector2& pt);
    void AddPoints(const PODVector<IntVector2> &points);
    void DrawPoints(const PODVector<IntVector2> &points);
    void ClearPointList();
    void ClearBatchList();
    int GetBatchCount() const { return (int)batches_.Size(); }

    // virtual override
    virtual void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);

protected:
    void DrawInternalPoints();

    void CreateLineSegments();
    void CreateCurveSegments();
    void StitchQuadPoints();
    void LinePointsToQuadPoints(const Vector2 &v0, const Vector2 &v1, Vector2 &a, Vector2 &b, Vector2 &c, Vector2 &d);
    bool ValidateTextures() const;
    void AddQuad(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d);
    void AddCrossQuad(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d);

protected:
    static IntRect          boxRect_;
    static IntVector2       boxSize_;

    WeakPtr<UIElement>      constrainParentElement_;

    SharedPtr<Texture>      lineTexture_;
    IntRect                 lineImageRect_;
    float                   linePixelSize_;
    float                   lineOpacity_;
    BlendMode               blendMode_;

    Vector<IntVector2>      pointList_;
    LineType                lineType_;
    int                     numPtsPerSegment_;

    float                   invLineTextureWidth_;
    float                   invLineTextureHeight_;

    PODVector<RectVectors>  rectVectorList_;
    PODVector<float>        vertexData_;
    PODVector<UIBatch>      batches_;
};

