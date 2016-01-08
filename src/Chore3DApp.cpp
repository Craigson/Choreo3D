/*
 *      Choreo3D began as a product of the Choreographic Coding Lab, a project of The Motion Bank,
 *      at the New York workshop in August 2014.  The software uses raw data captured from a Vicon
 *      Motion capture system in JSON format.  The data is parsed and the visualisation can be
 *      controlled using the GUI.  The end goal for the project is to allow the user to manipulate
 *      the data during playback and allow them to create new and dynamic performances and pieces
 *      of choreography.
 *
 *      Coded by Craig Pickard, Gene Han and Stream Gao.
 */




#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//UTILITY INCLUDES
#include "cinder/CameraUi.h"
#include "cinder/Json.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "cinder/Utilities.h"
#include "cinder/ImageIO.h"

//MY INCLUDES
#include "CCL_MocapData.h"
#include "Skeleton.h"
#include "RibbonFunctions.h"
#include "cinder/Easing.h"
#include "Trail.h"

#include "CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Choreo3DApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
    void keyDown(KeyEvent event) override;      //FOR SAVING SCREENSHOTS
	void update() override;
	void draw() override;
    
    void setupEnviron( int xSize, int zSize, int spacing );     //METHOD FOR SETTING UP THE 3D ENVIRONMENT
    void renderScene();     //METHOD FOR RENDERING THE GRID
    void setupShader();     //METHOD FOR INITIALIZING THE STANDARD SHADER
    
    void drawRibbons();
    void updateRibbons();
    
    void initData();    //METHOD FOR IMPORTING AND INTIALIZING ALL MOCAP DATA
    
    void initGui();     //CONVENIENCE METHOD FOR INITIALIZING THE GUI PARAMS
    void updateGui();   //UPDATE THE GUI EVERY FRAME
    
    void reset();
    
    //CREATE A VERTEX BATCH FOR THE FLOOR MESH
    gl::VertBatchRef	mGridMesh;
    
    //SETUP THE CAMERA
    CameraPersp			mCamera;
    CameraUi			mCamUi;
    
    //GLOBAL DEFUALT SHADER
    gl::GlslProgRef		mGlsl;
    gl::GlslProgRef     solidShader;
    
    //TEST SPHERES
    gl::BatchRef        mSphereBatch;
    gl::VboRef			mInstanceDataVbo;
    
    //NOTE: NEED TO TURN THIS INTO A CLASS
    struct Ribbon
    {
        vec3              _target;
        std::vector<vec3> _spine;
        std::vector<vec3> _triangles;
        size_t            _joint_index = 0;
    };
    
    int FRAME_COUNT;
    int TOTAL_FRAMES;
    
    int CURRENT_DATA_SET = 0;
    int LOADED_DATA_SET = 0;
    
    //GLOBAL CONTAINER TO HOLD THE JOINT POSITIONS
    std::vector<CCL_MocapJoint> jointList;
    
    //CREATE A CONTAINER TO STORE THE POSITION OF EACH JOINT FOR A SINGLE FRAME
    //NOTE: SHOULD BE STORING THIS INTO SOME SORT OF BUFFER
    std::vector<glm::vec3> framePositions;
    
    Skeleton skeleton;
    std::vector<Ribbon> ribbons;
    Trail handTrail;
    
    int mCurrentFrame = 0;
    
    //SETUP GLOBAL UI VARIABLES
    ImGui::Options opts;
    
    bool paused;
    bool camActive;
    bool uiActive;
    bool ribbonsActive;
    bool trailsActive;
    bool skeletonActive;
    bool markersActive;
    bool showGrid;
    int sphereRadius;
    int frameRate;
    
    ColorA gridColor;
    
    float background;
    
    float dancerColor[3] =  { 122.f, 219.f, 56.f };
    
};

void Choreo3DApp::setup()
{
    
    opts.window( getWindow() );
    
    frameRate = 12;
    //SET THE GLOBAL FRAMERATE
    setFrameRate(frameRate);
    
    //SETUP THE 3D ENVIRONMENT
    setupEnviron( 5000, 5000, 100 );
    
    //INITIALIZE THE SHADERS
    setupShader();
    
    //SETUP GL
    gl::enableDepthWrite();
    gl::enableDepthRead();
    gl::enableAlphaBlending();
    
    //INITIALISE THE CAMERA (UI ONLY WORKS IF CAMERA IS DISABLED)
    mCamUi = CameraUi( &mCamera, getWindow() );
    
    initData(); //IMPORT THE JSON DATA AND SORT IT INTO A LIST
    
    //NOTE: THIS NEEDS TO BE CLEANED UP
    FRAME_COUNT = 0;
    TOTAL_FRAMES = jointList[0].jointPositions.size(); //SHOULD PROBABLY PUT A TRY/CATCH HERE
    
    std::cout << "total frames: " << TOTAL_FRAMES << std::endl;
    
    sphereRadius = 1;
    
    gl::VboMeshRef body = gl::VboMesh::create( geom::Sphere().subdivisions( 16 ).radius(sphereRadius) );
    
    
    // CREATE THE SPHERES AT THE INITIAL JOINT LOCATIONS
    for ( int i = 0; i < jointList.size(); ++i ) {
        float instanceX = jointList[i].jointPositions[0].x;
        float instanceY = jointList[i].jointPositions[0].y;
        float instanceZ = jointList[i].jointPositions[0].z;
        // float instanceZ = 0;
        
        framePositions.push_back( vec3( instanceX, instanceY, instanceZ));
    }
    
    skeleton = Skeleton(framePositions);
    
    handTrail = Trail(framePositions[17]);
    
    
    // create the VBO which will contain per-instance (rather than per-vertex) data
    mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, framePositions.size() * sizeof(vec3), framePositions.data(), GL_DYNAMIC_DRAW );
    
    // we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
    geom::BufferLayout instanceDataLayout;
    
    instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );
    
    //NOW ADD IT TO THE VBO MESH THAT WE INITIAL CREATED FOR THE BODY / SKELETON
    body->appendVbo( instanceDataLayout, mInstanceDataVbo );
    
    //FINALLY, BUILD THE BATCH, AND MAP THE CUSTOM_0 ATTRIBUTE TO THE "vInstancePosition" GLSL VERTEX ATTRIBUTE
    mSphereBatch = gl::Batch::create( body, mGlsl, { { geom::Attrib::CUSTOM_0, "vInstancePosition" } } );
    
//    gl::enableDepthWrite();
//    gl::enableDepthRead();
    
    //PRINT OUT JOINT INDEX AND NAME OF JOINT
    
    for (int i = 0; i < jointList.size(); i++)
    {
        std::cout << "index: " << i << ", Joint name: " << jointList[i].jointName << std::endl;
    }
    
    //SETUP RIBBONS
    for (auto i = 0; i < jointList.size(); i += 1)
    {
        auto r = Ribbon();
        auto pos = framePositions[i];
        r._spine.assign(10, pos);
        r._joint_index = i;
        r._target = pos;
        ribbons.push_back(r);
    }
    
    //SETUP THE GUI
    initGui();
    
    //mCamUi.disable();
    

}

void Choreo3DApp::mouseDrag( MouseEvent event )
{
    //MOUSE DRAG EVEMT FOR ROTATING CAMERA
   // mCamUi.mouseDrag( event );
}

void Choreo3DApp::update()
{
    setFrameRate(frameRate);
    
    //DISABLE CAMERA INTERACTION IF MOUSE IS OVER UI REGION
    if (getMousePos().x > 3 * getWindowWidth()/4. && camActive)
    {
        camActive = false;
        mCamUi.disconnect();
        mCamUi.disable();
        cout << "disabling camera UI" << endl;
    } else {
        if (!camActive)
        {
            mCamUi.connect(getWindow());
            mCamUi.enable();
        }
        camActive = true;
        cout << "enabling camera UI" << endl;
    }
    

    
    if (!paused)
    {
        
        
        //UPDATE POSITIONS
        //MAP INSTANCE DATA TO VBO
        //WRITE NEW POSITIONS
        //UNMAP
        
        glm::vec3 *newPositions = (glm::vec3*)mInstanceDataVbo->mapReplace();
        
        for( int i = 0; i < jointList.size(); ++i )
        {
            
            float instanceX = jointList[i].jointPositions[FRAME_COUNT].x;
            float instanceY = jointList[i].jointPositions[FRAME_COUNT].y;
            float instanceZ = jointList[i].jointPositions[FRAME_COUNT].z;
            
            vec3 newPos(vec3(instanceX,instanceY, instanceZ)); //CREATE A NEW VEC3 FOR UPDATING THE VBO
            
            framePositions[i] = newPos;
            
        }
        
        //REPLACE VEC3s IN VBO BY INCREMENTING THE POINTER
        for (int i = 0; i < framePositions.size(); i++){
            *newPositions++ = framePositions[i];
        }
        
        handTrail.update(framePositions[17]);
        
        //    std::cout << framePositions[17] << std::endl;
        
        
        skeleton.update(framePositions);

        
        mInstanceDataVbo->unmap();
        // std::cout << "position: " << positions[0] << std::endl;
        
        if (ribbonsActive)updateRibbons();
        
        //MANUALLY INCREMENT THE FRAME, IF THE FRAME_COUNT EXCEEDS TOTAL FRAMES, RESET THE COUNTER
        if (FRAME_COUNT < TOTAL_FRAMES)
        {
            FRAME_COUNT += 1;
        } else {
            FRAME_COUNT = 0;
        }
        
        //std::cout << getAverageFps() << std:: endl;
        // std::cout << "frame rate: " << getAverageFps() << ", frame count: " << FRAME_COUNT << std::endl;
        
        //define changed color
        Color temp = Color(dancerColor[0],dancerColor[1],dancerColor[2]);
        mGlsl->uniform("uColor", temp );
        
        mCurrentFrame++; //MANUALLY ADVANCE THE CURRENT FRAME - WITH RESPECT TO THE DANCERS
    
    }
    
    updateGui();
}

void Choreo3DApp::draw()
{
  
    gl::clear( ColorA::gray( background ) );
    
    //THIS MAY NEED TO BE CLEANED UP
    vector<std::string> dataVector = {"CCL_JOINT_CCL3_00_skip10.json"};
    
    if( CURRENT_DATA_SET != LOADED_DATA_SET){
       // paused = true;
        
        
        //   jointList = {};
        jointList = ccl::loadMotionCaptureFromJson(getAssetPath(dataVector[CURRENT_DATA_SET]));
        
        FRAME_COUNT = 0;
        TOTAL_FRAMES = jointList[0].jointPositions.size(); //SHOULD PROBABLY PUT A TRY/CATCH HERE
        
        std::cout << "total frames: " << TOTAL_FRAMES << ", total joints:"<< jointList.size() << std::endl;
        
        gl::VboMeshRef body = gl::VboMesh::create( geom::Sphere().subdivisions( 16 ).radius(4) );
        
        //CREATE A CONTAINER TO STORE THE INITIAL POSITIONS FOR INITIALISING THE JOINTS
        std::vector<glm::vec3> positions;
        
        // CREATE THE SPHERES AT THE INITIAL JOINT LOCATIONS
        for ( int i = 0; i < jointList.size(); ++i ) {
            glm::vec3 jointAt = jointList[i].jointPositions[i];
            float instanceX = jointAt.x;
            float instanceY = jointAt.y;
            float instanceZ = jointAt.z;
            // float instanceZ = 0;
            
            positions.push_back( vec3( instanceX, instanceY, instanceZ));
        }
        //std::cout << "positions: " << positions[0] << std::endl;
        
        // create the VBO which will contain per-instance (rather than per-vertex) data
        mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_DYNAMIC_DRAW );
        
        // we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
        geom::BufferLayout instanceDataLayout;
        
        instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );
        
        //NOW ADD IT TO THE VBO MESH THAT WE INITIAL CREATED FOR THE BODY / SKELETON
        body->appendVbo( instanceDataLayout, mInstanceDataVbo );
        
        //FINALLY, BUILD THE BATCH, AND MAP THE CUSTOM_0 ATTRIBUTE TO THE "vInstancePosition" GLSL VERTEX ATTRIBUTE
        mSphereBatch = gl::Batch::create( body, mGlsl, { { geom::Attrib::CUSTOM_0, "vInstancePosition" } } );
        
        LOADED_DATA_SET = CURRENT_DATA_SET;
        
    }
    
    
    
    gl::setMatrices( mCamera );
    
    if (showGrid)renderScene();
    
    Color( dancerColor[0], dancerColor[1], dancerColor[2] );
    //gl::ScopedModelMatrix modelScope;
    
    if(markersActive)mSphereBatch->drawInstanced( jointList.size() );
    
    if(skeletonActive)skeleton.renderSkeleton();
    
    if(ribbonsActive)drawRibbons();
    
    if(trailsActive)handTrail.render();
    
    
}

//--------------------  KEY DOWN -----------------------------

void Choreo3DApp::keyDown (KeyEvent event) {
    //skeleton.pushone(vec3(200,200,0));
    //writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "CCL_images" / "saveImage_" /( toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    std::cout << "Saving image" << std::endl;
}


//-------------------- IMPORT DATA -------------------------

void Choreo3DApp::initData()
{
    /********** DATA ____ GUI ***********************/
    
    //CCL_MocapData("CCL_JOINT_CCL3_00_skip10.json", jointList);
    
    /********** DATA ____ GUI ***********************/
    jointList = ccl::loadMotionCaptureFromJson(getAssetPath("CCL_JOINT_CCL3_00_skip10.json"));
    
    //CREATE AND INITIALISE A CCL_MOCAPDATA OBJECT, PASSING IN THE GLOBAL "jointList" AS A REFERENCE
    //-->   jointList = ccl::loadMotionCaptureFromJson(getAssetPath("CCL_JOINT.json"));
    // jointList = ccl::loadMotionCaptureFromJson(getAssetPath("CCL_JOINT_CCL4_00.json"));
    //  jointList = ccl::loadMotionCaptureFromSite(Url(ccl::URL_STREAM_JSON), 1);
    //    CCL_MocapData(1, jointList); //UNCOMMENT THIS LINE TO CAPTURE NEW JSON DATA
    std::cout << jointList.size()<< endl;
    std::cout << endl;
    std::cout << endl;
    std::cout << endl;
}



//------------------- SETUP THE ENVIRONMENT / GRID -----------------------

void Choreo3DApp::setupEnviron( int xSize, int zSize, int spacing )
{
    CI_ASSERT( ( spacing <= xSize ) && ( spacing <= zSize ) );
    
    // Cut in half and adjust for spacing.
    xSize = ( ( xSize / 2 ) / spacing ) * spacing;
    zSize = ( ( zSize / 2 ) / spacing ) * spacing;
    
    const int xMax = xSize + spacing;
    const int zMax = zSize + spacing;
//    const ColorA defaultColor( 0.9f, 0.9f, 0.9f,0.1f);
    gridColor = ColorA( 0.9f, 0.9f, 0.9f,0.1f);
    const ColorA black( 0, 0, 0, 1 );
    
    mGridMesh = gl::VertBatch::create( GL_LINES );
    
    // Add x lines.
    for( int xVal = -xSize; xVal < xMax; xVal += spacing ) {
        mGridMesh->color( gridColor );
        mGridMesh->vertex( (float)xVal, 0, (float)-zSize );
        mGridMesh->vertex( (float)xVal, 0, (float)zSize );
    }// end for each x dir line
    
    // Add z lines.
    for( int zVal = -zSize; zVal < zMax; zVal += spacing ) {
        mGridMesh->color( gridColor );
        mGridMesh->vertex( (float)xSize, 0, (float)zVal );
        mGridMesh->vertex( (float)-xSize, 0, (float)zVal );
    }// end for each z dir line
    
    //SETUP THE CAMERA
    mCamera.lookAt( vec3( 500, 500, 0 ), vec3( 0 ) );
    mCamera.setEyePoint(vec3(500,1000,0));
    mCamera.setFarClip(20000);
}

/*------------------ RENDER THE SCENE ------------------------*/

void Choreo3DApp::renderScene()
{
    
    // gl::pushMatrices();
    mGridMesh->draw();
    // gl::popMatrices();
}

/*--------------------- SETUP SHADERS -----------------------*/

void Choreo3DApp::setupShader(){
    
    //CHOOSE BETWEEN solidShader AND mGlsl AS SHADERS FOR THE SPHERES
    //    gl::ScopedColor color( Color( 0, 1, 0 ) );
    //    solidShader = gl::getStockShader( color );
    
    
    mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
    mSphereBatch = gl::Batch::create( geom::Sphere(), mGlsl );
    
}

//-------------------------UPDATE RIBBONS--------------------------------

void Choreo3DApp::updateRibbons()
{
    auto easing = 1.0f;
    int i = 0;
    for (auto &r:ribbons)
    {
        auto target = framePositions[i];
        const auto no_data_value = -123456;
        if (glm::all(glm::greaterThan(target, vec3(no_data_value))))
        {
            r._target = target;
        }
        
        auto &points = r._spine;
        for (auto i = points.size() - 1; i > 0; i -= 1)
        {
            auto &p1 = points.at(i);
            auto &p2 = points.at(i - 1);
            p1 += (p2 - p1) * easing;
        }
        auto &point = points.at(0);
        point += (r._target - point) * easing;
        i++;
    }
    
    for (auto &r: ribbons)
    {
        r._triangles = sansumbrella::createRibbon(20.0f, ci::EaseInOutQuad(), mCamera.getEyePoint(), r._spine);
    }
    
    //  _camera.lookAt(currentJointPosition(0));
}


//--------------------- DRAW RIBBONS ---------------------------------

void Choreo3DApp::drawRibbons(){
    for (auto &ribbon: ribbons)
    {
        gl::enableAlphaBlending();
        ColorA moop(1.0,1.0,1.,0.4);
        gl::begin(GL_TRIANGLE_STRIP);
        for (auto &p : ribbon._triangles)
        {
            gl::vertex(p);
        }
        gl::end();
    }
}

//------------------------ S E T U P  G U I ------------------------------
void Choreo3DApp::initGui()
{
    ui::initialize(opts);
    
    camActive = true;   //CAMERA MOVEMENT ENABLED BY DEFAULT
    
    ribbonsActive = false;
    trailsActive = false;
    skeletonActive = false;
    markersActive = true;
    showGrid = true;
    
    paused = true;  //PAUSED ENABLED BY DEFAULT
    
    background = 0.3f;

    
   

    
}

//------------------------ D I S P L A Y  G U I -------------------------

void Choreo3DApp::updateGui()
{
    
    //CREATE A WINDOW
    ui::ScopedWindow window( "Choreo3D" , ImGuiWindowFlags_NoMove);
    ui::SetWindowPos(ImVec2(930.0, 10));
    
    //CREATE A SLIDING BAR TO SET THE BACKGROUND COLOR
    ImGui::SliderInt("SET FRAMERATE", &frameRate, 1, 60);
    
    //BACKGROUND COLOUR
    ui::ColorEdit3( "Color", dancerColor );
    
 
    //CREATE A SLIDING BAR TO SET THE BACKGROUND COLOR
    ui::SliderFloat( "BACKGROUND", &background, 0.0f, 1.0f, "%.3f", 1.0f);
    
    //PLAY / PAUSE DANCER
    ui::Checkbox("PAUSED", &paused);
    
    //RIBBONS
    ui::Checkbox("SHOW TRAILS", &ribbonsActive);
    
    //SKELETON
    ui::Checkbox("SHOW SKELETON", &skeletonActive);
    
    //SHOW MOCAP MARKERS
    ui::Checkbox("SHOW MOCAP MARKERS", &markersActive);
    
    //CREATE A SLIDING BAR TO SET THE BACKGROUND COLOR
    ImGui::SliderInt("MARKER SIZE", &sphereRadius, 1, 10);
    
    //SHOW PATH
    ui::Checkbox("SHOW PATH", &trailsActive);
    
    //SHOW GRID
    ui::Checkbox("SHOW FLOOR", &showGrid);
    
    if( ui::Button( "RESET" ) ) reset();

}

void Choreo3DApp::reset()
{
    gl::clear( ColorA::gray( background ) );
    FRAME_COUNT = 0;
    mCurrentFrame = 0;
}


CINDER_APP( Choreo3DApp, RendererGl(RendererGl::Options().msaa( 16 ) ), [&]( App::Settings *settings ) {
    settings->setWindowSize( 1280, 720 );
    //settings->setFullScreen();
    } )
