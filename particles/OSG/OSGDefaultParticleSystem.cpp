/*
 * OSGDefaultParticleSystem.cpp
 *
 *  Created on: 28/10/2014
 *      Author: sgalindo
 */

#include "OSGDefaultParticleSystem.h"

#include <particles/config.h>

#include "../log.h"

namespace particles
{

  void OSGPSNodeCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    osg::ref_ptr<OSGDefaultParticleSystem> osgps =
        static_cast<OSGDefaultParticleSystem*>( node->asGeode()->getDrawable(0) );

    if (osgps)
    {
      osgps->UpdateUnified(0.1f);
    }
  }

  OSGDefaultParticleSystem::OSGDefaultParticleSystem()
  : ParticleSystem(0, 0, false)
  , osg::Drawable()
  , cameraManipulator( nullptr )
  , rootNode( nullptr )
  {

//        PREFR_DEBUG_CHECK( glDrawElementsInstanced == nullptr,
//                           "glDrawElementsInstanced is nullptr");


    setUseDisplayList(false);
    setUseVertexBufferObjects(true);
  }

  OSGDefaultParticleSystem::OSGDefaultParticleSystem(
const OSGDefaultParticleSystem& other,
const osg::CopyOp& copyOp)
  : ParticleSystem(0, 0, false)
  , osg::Drawable(other, copyOp)
  , cameraManipulator( nullptr )
  , rootNode( nullptr )
  {
    setUseDisplayList(false);
    setUseVertexBufferObjects(true);
  }

  OSGDefaultParticleSystem::OSGDefaultParticleSystem(
  unsigned int initialParticlesNumber, unsigned int _maxParticles, bool _loop)
  : ParticleSystem(initialParticlesNumber,
        _maxParticles,
        loop)
  , osg::Drawable()
  , cameraManipulator( nullptr )
  , rootNode( nullptr )
  {

    rootNode = new osg::Geode;
    rootNode->addDrawable(this);

    rootNode->setCullCallback(new OSGPSNodeCallBack);

     setUseDisplayList(false);
     setUseVertexBufferObjects(true);

  }

  OSGDefaultParticleSystem::~OSGDefaultParticleSystem()
  {}

  void OSGDefaultParticleSystem::ConfigureProgram(const std::string& shaderPathVert, const std::string& shaderPathFrag)
  {

    PREFR_DEBUG_CHECK( renderer, "Configure executed before adding a renderer to the particle system.");
    OSGRenderConfig* osgrc = static_cast<OSGRenderConfig*>(renderer->renderConfig);


    osg::StateSet* psState = rootNode->getOrCreateStateSet();

    osg::Program* program = new osg::Program;
    osg::Shader* vertexShader =
      new osg::Shader( osg::Shader::VERTEX );
    osg::Shader* fragmentShader =
      new osg::Shader( osg::Shader::FRAGMENT );

    std::string fullPath;

    std::cout << "Loading vertex shader:" << shaderPathVert << std::endl;

    // Load vertex shader
    fullPath = osgDB::findDataFile( shaderPathVert );

    if ( fullPath.empty() )
      PREFR_THROW("Vertex file not found at: " + shaderPathVert)

    assert(vertexShader->loadShaderSourceFromFile( fullPath ));

    std::cout << "Loading vertex shader:" << shaderPathFrag << std::endl;

    // Load fragment shader
    fullPath = osgDB::findDataFile( shaderPathFrag );

    if ( fullPath.empty() )
      PREFR_THROW("Fragment shader file not found at: " + shaderPathVert)

    assert(fragmentShader->loadShaderSourceFromFile( fullPath ));

    program->addShader( vertexShader );
    program->addShader( fragmentShader );

    osgrc->uCameraUp = new osg::Uniform("cameraUp", osg::Vec3f());
    osgrc->uCameraRight = new osg::Uniform("cameraRight", osg::Vec3f());

    psState->addUniform(osgrc->uCameraUp);
    psState->addUniform(osgrc->uCameraRight);

    program->addBindAttribLocation( "vertexPosition", 0 );
    program->addBindAttribLocation( "particlePosition", 1 );
    program->addBindAttribLocation( "particleColor", 2 );

    psState->setRenderingHint(osg::StateSet::RenderingHint::TRANSPARENT_BIN);

    psState->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
//
    psState->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::BlendFunc* blendFunc = new osg::BlendFunc();
    blendFunc->setSource(osg::BlendFunc::SRC_ALPHA);
    blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
//        blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_ALPHA);

    psState->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);

    psState->setAttributeAndModes(program, osg::StateAttribute::ON);

  }

  void OSGDefaultParticleSystem::SetCameraManipulator(osgGA::StandardManipulator* cam)
  {
    cameraManipulator = cam;
  }

  void OSGDefaultParticleSystem::UpdateUniformVariables(float deltaTime)
  {
    assert(cameraManipulator != nullptr);

    OSGRenderConfig* osgrc = static_cast<OSGRenderConfig*>(renderer->renderConfig);

    // Get camera position to calculate distances
    osg::Vec3d eye, center, up;
    cameraManipulator->getTransformation(eye, center, up);

    osgrc->eye = osg::Vec3f(eye);
    osgrc->center = osg::Vec3f(center);
    osgrc->up = osg::Vec3f(up);

    UpdateCameraDistances(glm::vec3(osgrc->eye.x(), osgrc->eye.y(), osgrc->eye.z()));

    osg::Vec3f forward = (osgrc->center - osgrc->eye);
    forward.normalize();
    osgrc->right = osgrc->up ^ forward;

    osgrc->uCameraUp->set( osgrc->up );

    osgrc->uCameraRight->set( osgrc->right );
  }

  void OSGDefaultParticleSystem::Update(float deltaTime)
  {
    ParticleSystem::Update(deltaTime);

    UpdateUniformVariables(deltaTime);

    UpdateRender();
  }

  void OSGDefaultParticleSystem::UpdateUnified(float deltaTime)
  {
    ParticleSystem::UpdateUnified(deltaTime);

    UpdateUniformVariables(deltaTime);

    UpdateRender();
  }


  void OSGDefaultParticleSystem::UpdateCameraDistances(const glm::vec3& cameraPosition)
  {

    unsigned int i = 0;
    for (tparticleContainer::iterator it = particles->start; it != particles->end; it++)
    {
      PREFR_DEBUG_CHECK( *it , "null pointer access" );

     i = ((tparticle_ptr) *it)->id;

     sorter->UpdateCameraDistance(i, cameraPosition);
    }

  }

  void OSGDefaultParticleSystem::UpdateRender()
  {
    this->sorter->Sort();

    PREFR_DEBUG_CHECK( static_cast<OSGDefaultParticleRenderer*>(this->renderer), "casting failed" );

    static_cast<OSGDefaultParticleRenderer*>(this->renderer)->SetupRender(this->aliveParticles);

    dirtyBound();
  }

  void OSGDefaultParticleSystem::Render() const
  {
    PREFR_DEBUG_CHECK( static_cast<OSGDefaultParticleRenderer*>(this->renderer),  "casting failed" );

    static_cast<OSGDefaultParticleRenderer*>(this->renderer)->Paint(aliveParticles);
  }

  osg::BoundingBox OSGDefaultParticleSystem::computeBound() const
  {
    PREFR_DEBUG_CHECK( renderer->renderConfig, "renderConfig is nullptr" );

    return static_cast<OSGRenderConfig*>(renderer->renderConfig)->boundingBox;

  }


  void OSGDefaultParticleSystem::compileGLObjects( osg::RenderInfo& renderInfo ) const
  {
    PREFR_DEBUG_CHECK( static_cast<OSGDefaultParticleRenderer*>(this->renderer) , "casting failed" );

    static_cast<OSGDefaultParticleRenderer*>(this->renderer)->osgCompileGLObjects(renderInfo);
  }

  void OSGDefaultParticleSystem::drawImplementation(osg::RenderInfo& renderInfo) const
  {
    osg::State* state = renderInfo.getState();
    state->setUseVertexAttributeAliasing(true);

    Render();
  }

  void OSGDefaultParticleSystem::releaseGLObjects(osg::State* state) const
  {
    OSGRenderConfig* osgrc = static_cast<OSGRenderConfig*>(renderer->renderConfig);

    glDeleteBuffers(1, &osgrc->vboBillboardVertex);
    glDeleteBuffers(1, &osgrc->vboDrawElements);
    glDeleteBuffers(1, &osgrc->vboParticlesPositions);
    glDeleteBuffers(1, &osgrc->vboParticlesColors);
    glDeleteVertexArrays(1, &osgrc->vao);
  }

}

