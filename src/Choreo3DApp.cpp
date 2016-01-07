#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Choreo3DApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void setupEnviron( int xSize, int zSize, int spacing );     //METHOD FOR SETTING UP THE 3D ENVIRONMENT
    void renderScene();     //METHOD FOR RENDERING THE GRID
    
    //CREATE A VERTEX BATCH FOR THE FLOOR MESH
    gl::VertBatchRef	mGridMesh;
    
    //SETUP THE CAMERA
    CameraPersp			mCamera;
    CameraUi			mCamUi;
};

void Choreo3DApp::setup()
{
    
    //SET THE GLOBAL FRAMERATE
    setFrameRate(60);
    
    //SETUP THE 3D ENVIRONMENT
    setupEnviron( 5000, 5000, 100 );
    
    //SETUP GL
    gl::enableDepthWrite();
    gl::enableDepthRead();
    gl::enableAlphaBlending();
    
    //INITIALISE THE CAMERA
    mCamUi = CameraUi( &mCamera, getWindow() );
}

void Choreo3DApp::mouseDrag( MouseEvent event )
{
    //MOUSE DRAG EVEMT FOR ROTATING CAMERA
    mCamUi.mouseDrag( event );
}

void Choreo3DApp::update()
{
}

void Choreo3DApp::draw()
{
    
    gl::clear(Color(0.05f,0.05f,0.05f) );
    gl::setMatrices( mCamera );
    renderScene();
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
    const ColorA defaultColor( 0.9f, 0.9f, 0.9f,0.1f);
    const ColorA black( 0, 0, 0, 1 );
    
    mGridMesh = gl::VertBatch::create( GL_LINES );
    
    // Add x lines.
    for( int xVal = -xSize; xVal < xMax; xVal += spacing ) {
        mGridMesh->color( defaultColor );
        mGridMesh->vertex( (float)xVal, 0, (float)-zSize );
        mGridMesh->vertex( (float)xVal, 0, (float)zSize );
    }// end for each x dir line
    
    // Add z lines.
    for( int zVal = -zSize; zVal < zMax; zVal += spacing ) {
        mGridMesh->color( defaultColor );
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

CINDER_APP( Choreo3DApp, RendererGl(RendererGl::Options().msaa( 16 ) ), [&]( App::Settings *settings ) {
    settings->setWindowSize( 1280, 720 );
    //settings->setFullScreen();
    } )
