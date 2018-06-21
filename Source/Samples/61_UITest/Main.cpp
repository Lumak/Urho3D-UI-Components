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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/IO/FileSystem.h>

#include "Main.h"

#include "PageManager.h"
#include "RadialGroup.h"
#include "TabGroup.h"
#include "SpriteAnimBox.h"
#include "LineBatcher.h"
#include "LineComponent.h"
#include "DrawTool.h"

#include "GraphNode.h"
#include "SlideVarInput.h"
#include "SlideVarNode.h"
#include "TimeVarInput.h"
#include "TimeVarNode.h"
#include "InputNode.h"
#include "OutputNode.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
URHO3D_DEFINE_APPLICATION_MAIN(Main)

//=============================================================================
//=============================================================================
Main::Main(Context* context) :
    Sample(context)
{
    PageManager::RegisterObject(context);

    RadialGroup::RegisterObject(context);
    TabGroup::RegisterObject(context);
    SpriteAnimBox::RegisterObject(context);
    LineBatcher::RegisterObject(context);
    StaticLine::RegisterObject(context);
    ControlLine::RegisterObject(context);
    DrawTool::RegisterObject(context);

    // nodegraph
    GraphNode::RegisterObject(context);
}

void Main::Setup()
{
    engineParameters_["WindowTitle"]  = GetTypeName();
    engineParameters_["LogName"]      = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"]   = false;
    engineParameters_["Headless"]     = false;
    engineParameters_["WindowWidth"]  = 1280; 
    engineParameters_["WindowHeight"] = 720;
}

void Main::Start()
{
    // Execute base class startup
    Sample::Start();

    // Set mouse visible
    String platform = GetPlatform();
    if (platform != "Android" && platform != "iOS")
        GetSubsystem<Input>()->SetMouseVisible(true);

    // change background color
    colorBackground_ = Color(0.1f, 0.3f, 0.9f);
    GetSubsystem<Renderer>()->GetDefaultZone()->SetFogColor(colorBackground_);

    // Create the UI content

    CreatePageManager();
    CreateGUI();
    //CreateInstructions();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}

void Main::CreatePageManager()
{
    PageManager *pageManager = GetSubsystem<PageManager>();
    pageManager->CreatePages(2);
}

void Main::CreateGUI()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    CreateRadialGroup();
    CreateTabGroup();
    CreateSpriteAnimBox();
    CreateLineComponents();
    CreateDrawTool();

    // set page
    PageManager *pageManager = GetSubsystem<PageManager>();
    pageManager->SetPageIndex(1);

    CreateSliderBarInput();
    CreateNodeGraph();

    // set page
    pageManager->SetPageIndex(0);
}

void Main::CreateRadialGroup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    // create group
    RadialGroup *radialGroup = root->CreateChild<RadialGroup>();
    radialGroup->SetPosition(20, 120);
    radialGroup->SetSize(300, 100);
    radialGroup->SetColor(Color(1.0f, 0.66f, 0.0f));
    radialGroup->GetTitleTextElement()->SetColor(Color(0.0f, 0.4f, 0.7f));
    radialGroup->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    radialGroup->SetHeaderText("Radial Group");

    for (int i = 0; i < 4; ++i)
    {
        String desc = String("radial btn ") + String(i+1) + String(" - (click text)"); 
        RadialElement *radElem = radialGroup->CreateRadialButton();

        radElem->textDesc_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        radElem->textDesc_->SetText(desc);
        radElem->textDesc_->SetColor(Color(0.0f, 0.4f, 0.7f));
        radElem->checkbox_->SetColor(Color(0.0f, 0.8f, 0.8f));
    }

    radialGroup->SetEnabled(true);
    SubscribeToEvent(E_RADIALGROUPTOGGLED, URHO3D_HANDLER(Main, HandleRadialGroupToggled));
}

void Main::CreateTabGroup()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    Text* gtext1 = ui->GetRoot()->CreateChild<Text>();
    gtext1->SetPosition(470, 100);
    gtext1->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 14);
    gtext1->SetText("Tab group");
    gtext1->SetColor(Color::YELLOW);

    // create tab group
    TabGroup *tabgroup = root->CreateChild<TabGroup>();
    tabgroup->SetPosition(450, 120);
    tabgroup->SetSize(300, 180);
    tabgroup->SetLayoutBorder(IntRect(10,10,10,10));
    tabgroup->SetColor(Color(0,0,0,0));

    // then create tabs
    const IntVector2 tabSize(60, 25);
    const IntVector2 tabBodySize(280, 150);

    for (int i = 0; i < 4; ++i)
    {
        String tabLabel = String("tab ") + String(i+1);

        TabElement *tabElement = tabgroup->CreateTab(tabSize, tabBodySize);
        tabElement->tabText_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        tabElement->tabText_->SetText(tabLabel);
        tabElement->tabText_->SetColor(Color(0.9f, 0.9f, 0.0f));

        tabElement->tabButton_->SetColor(Color(0.3f,0.7f,0.3f));
        tabElement->tabBody_->SetColor(Color(0.3f,0.7f,0.3f));

        Text *bodyText = tabElement->tabBody_->CreateChild<Text>();
        bodyText->SetAlignment(HA_CENTER, VA_CENTER);
        bodyText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 16);
        String btext = "Tab body " + String(i+1);
        bodyText->SetText(btext);
        bodyText->SetColor(Color(0.9f, 0.9f, 0.0f));
    }

    tabgroup->SetEnabled(true);
    SubscribeToEvent(E_TABSELECTED, URHO3D_HANDLER(Main, HandleTabSelected));
}

void Main::CreateSpriteAnimBox()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();

    SpriteAnimBox* animbox = root->CreateChild<SpriteAnimBox>();

    animbox->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    animbox->SetHeaderText("Sprite Anim Box");

    IntVector2 size(300, 180);
    animbox->Create(size, true, true);
    animbox->SetColor(Color(0,0,0,0));
    animbox->SetPosition(800, 120);

    animbox->AddSprite("Urho2D/GoldIcon/1.png");
    animbox->AddSprite("Urho2D/GoldIcon/2.png");
    animbox->AddSprite("Urho2D/GoldIcon/3.png");
    animbox->AddSprite("Urho2D/GoldIcon/4.png");
    animbox->AddSprite("Urho2D/GoldIcon/5.png");

    animbox->SetFPS(20.0f);
    animbox->SetEnabled(true);
}

void Main::CreateLineComponents()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();

    Text *text = root->CreateChild<Text>();
    text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
    text->SetText("Control Lines - drag control boxes to adjust");
    text->SetPosition(120, 350);

    IntVector2 points[5] =
    {
        {  70, 380 },
        { 150, 430 },
        { 300, 400 },
        { 420, 450 },
        { 560, 400 },
    };
    PODVector<IntVector2> pointList;
    pointList.Resize(5);
    pointList[0] = points[0];
    pointList[1] = points[1];
    pointList[2] = points[2];
    pointList[3] = points[3];
    pointList[4] = points[4];

    ControlLine *lineTest1 = root->CreateChild<ControlLine>();
    lineTest1->CreateLine(pointList, STRAIGHT_LINE, Color::BLUE, 8.0f);

    pointList[0] += IntVector2(0, 100);
    pointList[1] += IntVector2(0, 100);
    pointList[2] += IntVector2(0, 100);
    pointList[3] += IntVector2(0, 100);
    pointList[4] += IntVector2(0, 100);

    ControlLine *lineTest2 = root->CreateChild<ControlLine>();
    lineTest2->CreateLine(pointList, CURVE_LINE, Color::RED, 4.0f);
}

void Main::CreateDrawTool()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect(84,87,85,88);

#ifdef SHOW_TEXT_DRAWTOOL
    // texture draw tool
    DrawTool *drawtoolTexture = root->CreateChild<DrawTool>();

    if ( drawtoolTexture->Create( IntVector2(500, 330), uiTex2d, rect, false) )
    {
        drawtoolTexture->SetPosition(700, 350);
        drawtoolTexture->SetColor(Color(0.2f,0.2f,0.2f));
        drawtoolTexture->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        drawtoolTexture->SetHeaderText("Drawtool Texture (right mouse click to draw)");
    }
#endif

    // linebatcher draw tool
    DrawTool *drawtoolLineBatcher = root->CreateChild<DrawTool>();

    if ( drawtoolLineBatcher->Create( IntVector2(500, 330), uiTex2d, rect, true) )
    {
        drawtoolLineBatcher->SetScreenColor(Color(0.8f, 0.95f, 0.2f));
        drawtoolLineBatcher->SetPosition(700, 365);
        //drawtoolLineBatcher->SetPosition(700, 5);
        drawtoolLineBatcher->SetColor(Color(0.2f,0.2f,0.2f));
        drawtoolLineBatcher->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
        drawtoolLineBatcher->SetHeaderText("Drawtool LineBatcher (RMB to draw)");
    }
}

void Main::CreateSliderBarInput()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();

    // create and init nodebase
    IntVector2 pos00(20,100);
    SlideVarNode *slideVarNode = root->CreateChild<SlideVarNode>();
    slideVarNode->SetPosition(pos00);
    slideVarNode->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    slideVarNode->SetHeaderText("Screen color");

    // footer info
    slideVarNode->SetFooterVisible(true);
    slideVarNode->SetFooterFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    slideVarNode->SetFooterText("-drag on the red bar to\nchange value\n-drag the header to move\n"
                                "-double click the header\nto toggle the footer");

    IntVector2 size2(200, 30);
    slideVarNode->CreateBar("-", size2, false);
    slideVarNode->SetBarColor(Color(0.8f, 0.3f, 0.3f) );
    slideVarNode->SetRange((Variant)0.0f, (Variant)1.0f);
    slideVarNode->SetCurrentValue((Variant)colorBackground_.r_);
    slideVarNode->SetSensitivity(0.005f);

    // ui callback helper - a direct and alternative method to get events
    // you can use a typical event handler instead, e.g. HandleMessage(StringHash eventType, VariantMap& eventData);
    // register for E_SLIDEBAR_VARCHANGED
    class UICallbackHelper : public UIElement
    {
        URHO3D_OBJECT(UICallbackHelper, UIElement);
    public:
        UICallbackHelper(Context *context) : UIElement(context){}
        virtual ~UICallbackHelper(){}
        void SetBackgroundColor(const Color &color) { colorBackground_ = color; }
        void RedColorHandler(Variant &var)
        {
            colorBackground_.r_ = var.GetFloat();
            GetSubsystem<Renderer>()->GetDefaultZone()->SetFogColor(colorBackground_);
        }

    protected:
        Color colorBackground_;
    };

    // setup the callback helper
    UICallbackHelper *colorChangedHelper = new UICallbackHelper(context_);
    root->AddChild(colorChangedHelper);
    colorChangedHelper->SetBackgroundColor(colorBackground_);
    slideVarNode->SetVarChangedCallback(colorChangedHelper, (VarChangedCallback)&UICallbackHelper::RedColorHandler);
}

void Main::CreateNodeGraph()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    PageManager* ui = GetSubsystem<PageManager>();
    UIElement* root = ui->GetRoot();

    //===========================================
    // input collector
    //===========================================
    GraphNode *nodeBase5 = root->CreateChild<GraphNode>();

    IntVector2 pos01(350, 350);
    nodeBase5->SetPosition( pos01 );
    nodeBase5->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    nodeBase5->SetHeaderText("Input sum");

    // footer info
    nodeBase5->SetFooterVisible(true);
    nodeBase5->SetFooterFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    nodeBase5->SetFooterText("-inputs Xi and Yi are locked\n"
                             "-the 'out' node is also locked\n"
                             "but still can pull data from it\n");

    IntVector2 size25(GraphNode::GetDefaultIONodeSize());
    InputNode *inputNode0 = nodeBase5->CreateChild<InputNode>();
    inputNode0->Create("Ni", size25);

    InputNode *inputNodeX = nodeBase5->CreateChild<InputNode>();
    inputNodeX->Create("Xi", size25);

    InputNode *inputNodeY = nodeBase5->CreateChild<InputNode>();
    inputNodeY->Create("Yi", size25);

    OutputNode *outNode2 = nodeBase5->CreateChild<OutputNode>();
    outNode2->Create("out", size25);
    outNode2->SetEnableCtrlButton(false); // lock

    //===========================================
    // ball spwaning node
    //===========================================
    IntVector2 pos22(20,240);
    SlideVarNode *slideVarNode = root->CreateChild<SlideVarNode>();
    slideVarNode->SetPosition( pos22 );
    slideVarNode->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    slideVarNode->SetHeaderText("Balls to spawn");

    // footer
    slideVarNode->SetFooterVisible(true);
    slideVarNode->SetFooterFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    slideVarNode->SetFooterText("-drag the red box next to the\nN and connect to the yellow Ni");

    IntVector2 size222(200, 30);
    slideVarNode->CreateBar("N", size222, true);
    slideVarNode->SetBarColor(Color(0.8f, 0.3f, 0.3f) );
    slideVarNode->SetRange((Variant)1, (Variant)30);
    slideVarNode->SetCurrentValue((Variant)1);
    slideVarNode->SetSensitivity(0.1f);

    //===========================================
    // X variance
    //===========================================
    IntVector2 pos00(20,340);
    TimeVarNode *timeVarNodeX = root->CreateChild<TimeVarNode>();
    timeVarNodeX->SetPosition(pos00);
    timeVarNodeX->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    timeVarNodeX->SetHeaderText("Vary X input");

    // footer info (off by default)
    timeVarNodeX->SetFooterVisible(true);
    timeVarNodeX->SetFooterFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    timeVarNodeX->SetFooterText("-drag the small yellow\nsquares to adjust the curve");

    // create timed input
    IntVector2 size(200, 100);
    timeVarNodeX->CreateTimeVarInput("X", size);
    timeVarNodeX->SetScreenColor(Color(0.7f, 0.4f, 0.4f) );

    // curve points
    // specifies: time range, values per time and min/max
    // requires the star time = 0
    PODVector<Vector2> curvePoints(5);
    curvePoints[0] = Vector2(0.0f, -200.0f);
    curvePoints[1] = Vector2(1.0f,  180.0f);
    curvePoints[2] = Vector2(2.0f,  190.0f);
    curvePoints[3] = Vector2(3.0f, -110.0f);
    curvePoints[4] = Vector2(4.0f, -200.0f);

    timeVarNodeX->InitDataCurvePoints(curvePoints);
    timeVarNodeX->ConnectToInput(inputNodeX); // connect input
    timeVarNodeX->SetEnableCtrlButton(false); // lock

    //===========================================
    // Y variance
    //===========================================
    IntVector2 posy(20,510);
    TimeVarNode *timeVarNodeY = root->CreateChild<TimeVarNode>();
    timeVarNodeY->SetPosition(posy);
    timeVarNodeY->SetHeaderFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    timeVarNodeY->SetHeaderText("Vary Y input");

    // footer info (off by default)
    timeVarNodeY->SetFooterVisible(true);
    timeVarNodeY->SetFooterFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);
    timeVarNodeY->SetFooterText("-note: moving in +y on the\nmain screen means moving\ntowards the bottom");

    // create timed input
    IntVector2 sizey0(200, 100);
    timeVarNodeY->CreateTimeVarInput("Y", sizey0);
    timeVarNodeY->SetScreenColor(Color(0.7f, 0.4f, 0.4f) );
    curvePoints[0].y_ =  150.0f;
    curvePoints[1].y_ =  150.0f;
    curvePoints[2].y_ = -150.0f;
    curvePoints[3].y_ =  180.0f;
    curvePoints[4].y_ =  150.0f;

    timeVarNodeY->InitDataCurvePoints(curvePoints);
    timeVarNodeY->ConnectToInput(inputNodeY); // connect input
    timeVarNodeY->SetEnableCtrlButton(false); // lock

    //===========================================
    // InputProcessor
    //===========================================
    class InputProcessor : public UIElement
    {
        URHO3D_OBJECT(InputProcessor, UIElement);

        struct BallData
        {
            BallData() : sprite(NULL), pos(0,0), time(0.0f) {}

            Sprite  *sprite;
            Vector2 pos;
            float   time;
        };
    public:
        InputProcessor(Context *context) : UIElement(context) , 
            ballCount_(0), numBallsShown_(0), dataSet_(false), 
            minTime_(0.0f), maxTime_(0.0f), 
            elapsedTimeAccum_(0.0f), limitFrameRate_(true)
        {
        }

        virtual ~InputProcessor()
        {
            ballList_.Clear();
        }

        void SetOutputConnection(OutputNode *outputNode)
        {
            outputNode_ = outputNode;
            minTime_ = outputNode_->GetStartTime("Xi");
            maxTime_ = outputNode_->GetEndTime("Xi");
        }

        void Start()
        {
            elapsedTimeAccum_ = 0.0f;

            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputProcessor, HandleUpdate));
        }

        void HandleUpdate(StringHash eventType, VariantMap& eventData)
        {
            using namespace Update;
            float timeStep = eventData[P_TIMESTEP].GetFloat();

            if ( !IsVisible() || !GetParent()->IsVisible() )
                return;

            Variant var = outputNode_->GetCurrentValue("Ni");
            ballCount_ = 0;

            if (var != Variant::EMPTY )
            {
                ballCount_ = var.GetInt();
            }

            elapsedTimeAccum_ += timeStep;

            if ( !limitFrameRate_ || timerFrame_.GetMSec(false) > FrameRate_MSec)
            {
                if (ballCount_ > 0)
                {
                    UpdateBallVis();

                    UpdateBallPosition(elapsedTimeAccum_);
                }

                elapsedTimeAccum_ = 0.0f;
                timerFrame_.Reset();
            }
        }

        void UpdateBallPosition(float timeStep)
        {
            for ( unsigned i = 0; i < numBallsShown_ && i < ballList_.Size(); ++i )
            {
                ballList_[i].time += timeStep;

                if ( ballList_[i].time > maxTime_) ballList_[i].time = 0.0f;

                float x = outputNode_->GetValueAtTime("Xi", ballList_[i].time);
                float y = outputNode_->GetValueAtTime("Yi", ballList_[i].time);

                ballList_[i].sprite->SetPosition( ballList_[i].pos + Vector2(x, y) );
            }
        }

        void UpdateBallVis()
        {
            if ( ballCount_ != ballList_.Size() )
            {
                while (ballCount_ > ballList_.Size())
                {
                    CreateBall();
                }

            }
            if ( ballCount_ > numBallsShown_ )
            {
                for ( unsigned i = 0; i < ballCount_; ++i )
                {
                    ballList_[i].sprite->SetVisible(true);
                }
                numBallsShown_ = ballCount_;
            }

            if ( ballCount_ < numBallsShown_ )
            {
                for ( int i = (int)ballList_.Size()-1; i >= 0 && i >= (int)ballCount_; --i )
                {
                    ballList_[i].sprite->SetVisible(false);
                }
                numBallsShown_ = ballCount_;
            }
        }

        void CreateBall()
        {
            PageManager* ui = GetSubsystem<PageManager>();
            UIElement* root = ui->GetRoot();

            ResourceCache* cache = GetSubsystem<ResourceCache>();
            Texture2D *balltex2d = cache->GetResource<Texture2D>("Urho2D/Ball.png");
            SharedPtr<Sprite> sprite(new Sprite(context_));
            sprite->SetTexture(balltex2d);

            Vector2 pos( Vector2( 700 + Random() * 200, 350 + Random() * 200) );

            BallData bdata;
            bdata.sprite = sprite;
            bdata.pos = pos;

            sprite->SetPosition(pos);
            sprite->SetSize(IntVector2(32, 32));
            sprite->SetBlendMode(BLEND_REPLACE);

            root->AddChild(sprite);

            ballList_.Push(bdata);
            numBallsShown_++;
        }

        void LimitFrameRate(bool limit) { limitFrameRate_ = limit; }

    protected:
        WeakPtr<OutputNode> outputNode_;
        Vector<BallData>    ballList_;
        unsigned            ballCount_;
        unsigned            numBallsShown_;

        bool                dataSet_;
        float               minTime_;
        float               maxTime_;

        float               elapsedTimeAccum_;
        bool                limitFrameRate_;
        Timer               timerFrame_;

        enum FrameRateType { FrameRate_MSec = 32 };
    };

    // create the processor
    InputProcessor *inputProcessor = new InputProcessor(context_);
    root->AddChild(inputProcessor);

    inputProcessor->SetOutputConnection(outNode2);
    inputProcessor->Start();
}

void Main::CreateInstructions()
{
}

void Main::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Main, HandleUpdate));
}

void Main::HandleRadialGroupToggled(StringHash eventType, VariantMap& eventData)
{
    using namespace RadialGroupToggled;
}

void Main::HandleTabSelected(StringHash eventType, VariantMap& eventData)
{
    using namespace TabSelected;
}

void Main::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
}
