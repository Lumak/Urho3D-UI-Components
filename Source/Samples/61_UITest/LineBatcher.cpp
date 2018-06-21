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
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <SDL/SDL_log.h>

#include "LineBatcher.h"

#include <Urho3D/DebugNew.h>

//=============================================================================
//=============================================================================
#define DEFAULT_UI_RECT     16

IntRect LineBatcher::boxRect_(84,87,85,88);
IntVector2 LineBatcher::boxSize_(16, 16);

//=============================================================================
//=============================================================================
void LineBatcher::RegisterObject(Context* context)
{
    context->RegisterFactory<LineBatcher>();

    URHO3D_COPY_BASE_ATTRIBUTES(UIElement);
    //URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Texture", GetTextureAttr, SetTextureAttr, ResourceRef, ResourceRef(Texture2D::GetTypeStatic()),
    //    AM_FILE);
    //URHO3D_ACCESSOR_ATTRIBUTE("Image Rect", GetImageRect, SetImageRect, IntRect, IntRect::ZERO, AM_FILE);
    //URHO3D_ACCESSOR_ATTRIBUTE("Border", GetBorder, SetBorder, IntRect, IntRect::ZERO, AM_FILE);
    //URHO3D_ACCESSOR_ATTRIBUTE("Image Border", GetImageBorder, SetImageBorder, IntRect, IntRect::ZERO, AM_FILE);
    //URHO3D_ACCESSOR_ATTRIBUTE("Hover Image Offset", GetHoverOffset, SetHoverOffset, IntVector2, IntVector2::ZERO, AM_FILE);
    //URHO3D_ACCESSOR_ATTRIBUTE("Tiled", IsTiled, SetTiled, bool, false, AM_FILE);
    URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Blend Mode", GetBlendMode, SetBlendMode, BlendMode, blendModeNames, 0, AM_FILE);
}

LineBatcher::LineBatcher(Context *context) 
    : UIElement(context)
    , lineImageRect_(IntRect::ZERO)
    , lineOpacity_(1.0f)
    , linePixelSize_(1.0f)
    , blendMode_(BLEND_REPLACE)
    , numPtsPerSegment_(0)
    , invLineTextureWidth_(1)
    , invLineTextureHeight_(1)
{
    SetSize(1, 1);
}

LineBatcher::~LineBatcher()
{
}

void LineBatcher::SetBlendMode(BlendMode mode)
{
    blendMode_ = mode;

    // redraw if we have a batch
    if (batches_.Size() > 0 && pointList_.Size() > 0)
    {
        DrawInternalPoints();
    }
}

void LineBatcher::SetColor(const Color& color)
{
    UIElement::SetColor(color);

    // redraw if we have a batch
    if (batches_.Size() > 0 && pointList_.Size() > 0)
    {
        DrawInternalPoints();
    }
}

void LineBatcher::SetColor(Corner corner, const Color& color)
{
    UIElement::SetColor(corner, color);

    // redraw if we have a batch
    if (batches_.Size() > 0 && pointList_.Size() > 0)
    {
        DrawInternalPoints();
    }
}

void LineBatcher::SetLineType(LineType lineType)
{
    lineType_ = lineType;
}

void LineBatcher::SetLineData(Texture* texture, const IntRect& rect)
{
    lineImageRect_ = rect;

    SetLineTexture(texture);
}

void LineBatcher::SetLineTexture(Texture* texture)
{
    lineTexture_ = texture; 

    invLineTextureWidth_  = 1.0f/(float)lineTexture_->GetWidth();
    invLineTextureHeight_ = 1.0f/(float)lineTexture_->GetHeight();   
}

void LineBatcher::AddPoint(const IntVector2& pt)
{
    pointList_.Push(pt);
}

void LineBatcher::AddPoints(const PODVector<IntVector2> &points)
{
    for ( unsigned i = 0; i < points.Size(); ++i )
    {
        pointList_.Push(points[i]);
    }
}

void LineBatcher::DrawPoints(const PODVector<IntVector2> &points)
{
    assert(points.Size() > 1 && "try adding more draw points");

    // clear
    ClearPointList();
    ClearBatchList();

    // add
    AddPoints(points);

    // process
    if ( lineType_ == STRAIGHT_LINE )
        CreateLineSegments();
    else
        CreateCurveSegments();
}

void LineBatcher::DrawInternalPoints()
{
    // clear
    ClearBatchList();

    // process
    if ( lineType_ == STRAIGHT_LINE )
        CreateLineSegments();
    else
        CreateCurveSegments();
}

void LineBatcher::ClearPointList()
{
    pointList_.Clear();
}

void LineBatcher::ClearBatchList()
{
    vertexData_.Clear();
    batches_.Clear();
}

void LineBatcher::CreateLineSegments()
{
    Vector2 v0, v1;
    Vector2 a, b, c, d;

    rectVectorList_.Clear();
    v0 = Vector2((float)pointList_[0].x_, (float)pointList_[0].y_);

    for ( int i = 1; i < (int)pointList_.Size(); ++i )
    {
        v1 = Vector2((float)pointList_[i].x_, (float)pointList_[i].y_);

        LinePointsToQuadPoints(v0, v1, a, b, c, d);
        rectVectorList_.Push(RectVectors(a, b, c, d));
        v0 = v1;
    }

    StitchQuadPoints();
}

void LineBatcher::CreateCurveSegments()
{
    // create spline
    Spline spl(CATMULL_ROM_FULL_CURVE);

    for ( int i = 0; i < (int)pointList_.Size(); ++i )
    {
        Variant var = Vector2((float)pointList_[i].x_, (float)pointList_[i].y_);
        spl.AddKnot(var);
    }

    // line segment
    Vector2 v0, v1;
    Vector2 a, b, c, d;
    int numSegs = numPtsPerSegment_ * (int)pointList_.Size();
    float invSegs = 1.0f/(float)numSegs;

    rectVectorList_.Clear();
    v0 = Vector2((float)pointList_[0].x_, (float)pointList_[0].y_);

    for ( int i = 1; i < numSegs + 1; ++i )
    {
        v1 = spl.GetPoint( (float)i * invSegs ).GetVector2();

        LinePointsToQuadPoints(v0, v1, a, b, c, d);
        rectVectorList_.Push(RectVectors(a, b, c, d));
        v0 = v1;
    }

    StitchQuadPoints();
}

void LineBatcher::LinePointsToQuadPoints(const Vector2 &v0, const Vector2 &v1, Vector2 &a, Vector2 &b, Vector2 &c, Vector2 &d)
{
    Vector3 p0(v0.x_, v0.y_, 0);
    Vector3 p1(v1.x_, v1.y_, 0);
    Vector3 zn(0,0,1);
    Vector3 line = (p1 - p0).Normalized();
    zn = zn.CrossProduct(line).Normalized();
    zn = zn * linePixelSize_;
    Vector2 n(zn.x_, zn.y_);
    a = v0 - n; 
    b = v1 - n; 
    c = v0 + n; 
    d = v1 + n; 
}

void LineBatcher::StitchQuadPoints()
{
    unsigned numRects = rectVectorList_.Size();

    for ( unsigned i = 1; i < numRects; ++i )
    {
        Vector2 L0 = (rectVectorList_[i-1].b - rectVectorList_[i-1].a).Normalized();
        Vector2 L1 = (rectVectorList_[i].b - rectVectorList_[i].a).Normalized();

        // stitch near parallel quads
        if ( L0.DotProduct(L1) > 0.9f )
        {
            Vector2 avg0 = (rectVectorList_[i-1].b + rectVectorList_[i].a) * 0.5f;
            Vector2 avg1 = (rectVectorList_[i-1].d + rectVectorList_[i].c) * 0.5f;

            rectVectorList_[i-1].b = avg0;
            rectVectorList_[i  ].a = avg0;
            rectVectorList_[i-1].d = avg1;
            rectVectorList_[i  ].c = avg1;

            AddQuad(rectVectorList_[i-1].a, rectVectorList_[i-1].b, rectVectorList_[i-1].c, rectVectorList_[i-1].d );
        }
        else
        {
            AddQuad(rectVectorList_[i-1].a, rectVectorList_[i-1].b, rectVectorList_[i-1].c, rectVectorList_[i-1].d );
            AddCrossQuad(rectVectorList_[i].a, rectVectorList_[i-1].b, rectVectorList_[i].c, rectVectorList_[i-1].d );
        }
    }

    // add the last quad
    numRects--;
    AddQuad(rectVectorList_[numRects].a, rectVectorList_[numRects].b, rectVectorList_[numRects].c, rectVectorList_[numRects].d );
}

bool LineBatcher::ValidateTextures() const
{
    if ( lineTexture_ == NULL || lineImageRect_ == IntRect::ZERO )
    {
        return false;
    }

    return true;
}

void LineBatcher::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
    for ( unsigned i = 0; i < batches_.Size(); ++i )
    {
        UIBatch &batch     = batches_[ i ];
        unsigned beg       = batch.vertexStart_;
        unsigned end       = batch.vertexEnd_;
        batch.vertexStart_ = vertexData.Size();
        batch.vertexEnd_   = vertexData.Size() + (end - beg);

        vertexData.Resize( batch.vertexEnd_ );
        memcpy( &vertexData[ batch.vertexStart_ ], &vertexData_[ beg ], (end - beg) * sizeof(float) );

        UIBatch::AddOrMerge( batch, batches );

        // restore
        batch.vertexStart_ = beg;
        batch.vertexEnd_   = end;
    }
}

void LineBatcher::AddQuad(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d)
{
    struct VertexData
    {
        float x;
        float y;
        float u;
        float v;
        Color col;
    };
    VertexData ver[6];

    ver[0].x = a.x_;
    ver[0].y = a.y_;
    ver[0].u = (float)lineImageRect_.left_ * invLineTextureWidth_;
    ver[0].v = (float)lineImageRect_.top_ * invLineTextureHeight_;
    ver[0].col = color_[C_TOPLEFT];
    ver[1].x = b.x_;
    ver[1].y = b.y_;
    ver[1].u = (float)lineImageRect_.right_ * invLineTextureWidth_;
    ver[1].v = (float)lineImageRect_.top_ * invLineTextureHeight_;
    ver[1].col = color_[C_TOPRIGHT];
    ver[2].x = d.x_;
    ver[2].y = d.y_;
    ver[2].u = (float)lineImageRect_.right_ * invLineTextureWidth_;
    ver[2].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[2].col = color_[C_BOTTOMRIGHT];

    ver[3].x = a.x_;
    ver[3].y = a.y_;
    ver[3].u = (float)lineImageRect_.left_ * invLineTextureWidth_;
    ver[3].v = (float)lineImageRect_.top_ * invLineTextureHeight_; 
    ver[3].col = color_[C_TOPLEFT];
    ver[4].x = d.x_;
    ver[4].y = d.y_;
    ver[4].u = (float)lineImageRect_.right_ * invLineTextureWidth_;  
    ver[4].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[4].col = color_[C_BOTTOMRIGHT];
    ver[5].x = c.x_;
    ver[5].y = c.y_;
    ver[5].u = (float)lineImageRect_.left_ * invLineTextureWidth_;  
    ver[5].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[5].col = color_[C_BOTTOMLEFT];

    // scissor min/max
    int minx =  M_MAX_INT, miny =  M_MAX_INT;
    int maxx = -M_MAX_INT, maxy = -M_MAX_INT;

    for ( int i = 0; i < 6; ++i )
    {
        if ((int)(ver[i].x + 0.5f) < minx ) minx = (int)(ver[i].x + 0.5f);
        if ((int)(ver[i].x + 0.5f) > maxx ) maxx = (int)(ver[i].x + 0.5f);
        if ((int)(ver[i].y + 0.5f) < miny ) miny = (int)(ver[i].y + 0.5f);
        if ((int)(ver[i].y + 0.5f) > maxy ) maxy = (int)(ver[i].y + 0.5f);
    }

    IntRect scissor(minx, miny, maxx, maxy);
    UIBatch batch( this, blendMode_, scissor, lineTexture_, &vertexData_ );

    unsigned begin = batch.vertexData_->Size();
    batch.vertexData_->Resize(begin + 6*UI_VERTEX_SIZE);
    float* dest = &(batch.vertexData_->At(begin));

    // set start/end
    batch.vertexStart_ = begin;
    batch.vertexEnd_   = batch.vertexData_->Size();

    for ( int i = 0; i < 6; ++i )
    {
        dest[0+i*UI_VERTEX_SIZE]              = ver[i].x; 
        dest[1+i*UI_VERTEX_SIZE]              = ver[i].y; 
        dest[2+i*UI_VERTEX_SIZE]              = 0.0f;
        ((unsigned&)dest[3+i*UI_VERTEX_SIZE]) = ver[i].col.ToUInt();
        dest[4+i*UI_VERTEX_SIZE]              = ver[i].u; 
        dest[5+i*UI_VERTEX_SIZE]              = ver[i].v;
    }

    UIBatch::AddOrMerge( batch, batches_ );
}

void LineBatcher::AddCrossQuad(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d)
{
    struct VertexData
    {
        float x;
        float y;
        float u;
        float v;
        Color col;
    };
    VertexData ver[6];

    ver[0].x = b.x_;
    ver[0].y = b.y_;
    ver[0].u = (float)lineImageRect_.right_ * invLineTextureWidth_;
    ver[0].v = (float)lineImageRect_.top_ * invLineTextureHeight_;
    ver[0].col = color_[C_TOPRIGHT];

    ver[1].x = a.x_;
    ver[1].y = a.y_;
    ver[1].u = (float)lineImageRect_.left_ * invLineTextureWidth_;
    ver[1].v = (float)lineImageRect_.top_ * invLineTextureHeight_;
    ver[1].col = color_[C_TOPLEFT];

    ver[2].x = d.x_;
    ver[2].y = d.y_;
    ver[2].u = (float)lineImageRect_.right_ * invLineTextureWidth_;
    ver[2].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[2].col = color_[C_BOTTOMRIGHT];

    // 2nd tri
    ver[3].x = b.x_;
    ver[3].y = b.y_;
    ver[3].u = (float)lineImageRect_.right_ * invLineTextureWidth_;
    ver[3].v = (float)lineImageRect_.top_ * invLineTextureHeight_;
    ver[3].col = color_[C_TOPRIGHT];

    ver[4].x = c.x_;
    ver[4].y = c.y_;
    ver[4].u = (float)lineImageRect_.left_ * invLineTextureWidth_;  
    ver[4].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[4].col = color_[C_BOTTOMLEFT];

    ver[5].x = d.x_;
    ver[5].y = d.y_;
    ver[5].u = (float)lineImageRect_.right_ * invLineTextureWidth_;  
    ver[5].v = (float)lineImageRect_.bottom_ * invLineTextureHeight_;
    ver[5].col = color_[C_BOTTOMRIGHT];

    // scissor min/max
    int minx =  M_MAX_INT, miny =  M_MAX_INT;
    int maxx = -M_MAX_INT, maxy = -M_MAX_INT;

    for ( int i = 0; i < 6; ++i )
    {
        #if 0
        if ((int)(ver[i].x + 0.5f) < minx ) minx = (int)(ver[i].x + 0.5f);
        if ((int)(ver[i].x + 0.5f) > maxx ) maxx = (int)(ver[i].x + 0.5f);
        if ((int)(ver[i].y + 0.5f) < miny ) miny = (int)(ver[i].y + 0.5f);
        if ((int)(ver[i].y + 0.5f) > maxy ) maxy = (int)(ver[i].y + 0.5f);
        #else
        if ((int)(ver[i].x) < minx ) minx = (int)(ver[i].x);
        if ((int)(ver[i].x) > maxx ) maxx = (int)(ver[i].x);
        if ((int)(ver[i].y) < miny ) miny = (int)(ver[i].y);
        if ((int)(ver[i].y) > maxy ) maxy = (int)(ver[i].y);
        #endif
    }

    IntRect scissor(minx, miny, maxx, maxy);
    UIBatch batch( this, blendMode_, scissor, lineTexture_, &vertexData_ );

    unsigned begin = batch.vertexData_->Size();
    batch.vertexData_->Resize(begin + 6*UI_VERTEX_SIZE);
    float* dest = &(batch.vertexData_->At(begin));

    // set start/end
    batch.vertexStart_ = begin;
    batch.vertexEnd_   = batch.vertexData_->Size();

    for ( int i = 0; i < 6; ++i )
    {
        dest[0+i*UI_VERTEX_SIZE]              = ver[i].x; 
        dest[1+i*UI_VERTEX_SIZE]              = ver[i].y; 
        dest[2+i*UI_VERTEX_SIZE]              = 0.0f;
        ((unsigned&)dest[3+i*UI_VERTEX_SIZE]) = ver[i].col.ToUInt();
        dest[4+i*UI_VERTEX_SIZE]              = ver[i].u; 
        dest[5+i*UI_VERTEX_SIZE]              = ver[i].v;
    }

    UIBatch::AddOrMerge( batch, batches_ );
}


