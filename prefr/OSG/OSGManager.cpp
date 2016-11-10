/*
 * Copyright (c) 2014-2016 GMRV/URJC.
 *
 * Authors: Sergio Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of PReFr <https://gmrv.gitlab.com/nsviz/prefr>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "OSGManager.h"

#include "OSGRenderer.h"

#ifdef PREFR_USE_OPENSCENEGRAPH

namespace prefr
{
  void OSGNodeCallBack::operator( )( osg::Node* node,
                                     osg::NodeVisitor* /* nv */ )
  {
    osg::ref_ptr< OSGManager > osgMng =
        static_cast< OSGManager* >( node->asGeode( )->getDrawable( 0 ) );

    if( osgMng )
    {
      osgMng->particleSystem( )->update( 0.1f );
      osgMng->updateUniformVariables( );
      osgMng->particleSystem( )->updateRender( );
    }
  }

  OSGManager::OSGManager( void )
  : osg::Drawable( )
  , _particleSystem( nullptr )
  , _renderConfig( nullptr )
  , _cameraManipulator( nullptr )

  {
    _geode = new osg::Geode;
    _geode->addDrawable( this );

    _geode->setCullCallback( new OSGNodeCallBack );

    setUseDisplayList( false );
    setUseVertexBufferObjects( true );
  }

  OSGManager::OSGManager( ParticleSystem& ps )
  : osg::Drawable( )
  , _particleSystem( &ps )
  , _renderConfig( nullptr )
  , _cameraManipulator( nullptr )
  {
    assert( _particleSystem->renderer( ));
    _renderConfig = dynamic_cast< OSGRenderConfig* >(
        _particleSystem->renderer( )->renderConfig( ));

    assert( _renderConfig );

    _geode = new osg::Geode;
    _geode->addDrawable( this );

    _geode->setCullCallback( new OSGNodeCallBack );

    setUseDisplayList( false );
    setUseVertexBufferObjects( true );

  }

  OSGManager::OSGManager( const OSGManager& other,
                          const osg::CopyOp& copyop )
  : osg::Drawable( other, copyop )
  , _particleSystem( other._particleSystem )
  , _renderConfig( other._renderConfig )
  , _cameraManipulator( nullptr )
  , _geode( other._geode )
  {
    setUseDisplayList( false );
    setUseVertexBufferObjects( true );

    assert( _particleSystem->renderer( ));
    _renderConfig = dynamic_cast< OSGRenderConfig* >(
        _particleSystem->renderer( )->renderConfig( ));

    assert( _renderConfig );

    _geode = new osg::Geode;
    _geode->addDrawable( this );

    _geode->setCullCallback( new OSGNodeCallBack );
  }

  OSGManager::~OSGManager( void )
  { }

  ParticleSystem* OSGManager::particleSystem( void ) const
  {
    return _particleSystem;
  }

  osg::Geode* OSGManager::node( void ) const
  {
    return _geode;
  }

  void OSGManager::setCameraManipulator( osgViewer::ViewerBase* _viewer,
                                         unsigned int contextNumber,
                                         unsigned int viewNumber )
  {
    osgViewer::ViewerBase::Contexts contexts;
    osgViewer::View* view;
    osgViewer::Viewer* viewer = dynamic_cast< osgViewer::Viewer* >( _viewer );

    view = dynamic_cast< osgViewer::View* >( viewer );

    if ( !view )
      view = dynamic_cast< osgViewer::CompositeViewer* >
        ( _viewer )->getView( viewNumber );

    PREFR_DEBUG_CHECK( view, "View is nullptr");

    _cameraManipulator = dynamic_cast< osgGA::StandardManipulator* >
      ( view->getCameraManipulator( ));

    PREFR_DEBUG_CHECK( _cameraManipulator, "camera manipulator is nullptr" );

    _viewer->getContexts( contexts, true );

    _acquireGraphicsContext( contexts[ contextNumber ]);
  }

  void OSGManager::_acquireGraphicsContext( osg::GraphicsContext* context )
  {
    context->realize( );
    context->makeCurrent( );

    // init glew
    glewInit( );

    context->releaseContext( );
  }

  void OSGManager::compileGLObjects( osg::RenderInfo& /* renderInfo */ ) const
  {
    OSGRenderConfig* osgrc = static_cast< OSGRenderConfig* >( _renderConfig );

    glGenVertexArrays( 1, &osgrc->_vao );

    GLuint buffersGL[ 4 ];
    glGenBuffers( 4, buffersGL );

    osgrc->_vboBillboardVertex = buffersGL[ 0 ];
    osgrc->_vboParticlesPositions = buffersGL[ 1 ];
    osgrc->_vboParticlesColors = buffersGL[ 2 ];
    osgrc->_vboDrawElements = buffersGL[ 3 ];

    osgrc->_init = true;

    // Assign billboard vertices
    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboBillboardVertex );

    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( GLfloat ) * osgrc->_billboardVertices->size( ),
                  &osgrc->_billboardVertices->front( ), GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboParticlesPositions );

    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( GLfloat ) * osgrc->_particlePositions->size( ),
                  nullptr, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboParticlesColors );

    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( GLfloat ) * osgrc->_particleColors->size( ),
                  nullptr, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, osgrc->_vboDrawElements );

    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  osgrc->_billboardIndices->getTotalDataSize( ),
                  osgrc->_billboardIndices->getDataPointer( ), GL_STATIC_DRAW );

    glBindVertexArray( osgrc->_vao );

    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboBillboardVertex );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( void* ) 0 );

    glEnableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboParticlesPositions );
    glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, ( void* ) 0 );

    glEnableVertexAttribArray( 2 );
    glBindBuffer( GL_ARRAY_BUFFER, osgrc->_vboParticlesColors );
    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_TRUE, 0, ( void* ) 0 );

    glVertexAttribDivisor( 1, 1 );
    glVertexAttribDivisor( 2, 1 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, osgrc->_vboDrawElements );

    glBindVertexArray( 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  }

  osg::BoundingBox OSGManager::computeBound( ) const
  {
    return dynamic_cast< OSGRenderConfig* >( _renderConfig )->_boundingBox;
  }

  void OSGManager::drawImplementation( osg::RenderInfo& renderInfo ) const
  {
    osg::State* state = renderInfo.getState( );
    state->setUseVertexAttributeAliasing( true );
    state->setUseModelViewAndProjectionUniforms( true );

    _particleSystem->render( );
  }

  void OSGManager::releaseGLObjects( osg::State* /* state */ ) const
  {
    OSGRenderConfig* osgrc = static_cast< OSGRenderConfig* >( _renderConfig );

    glDeleteBuffers( 1, &osgrc->_vboBillboardVertex );
    glDeleteBuffers( 1, &osgrc->_vboDrawElements );
    glDeleteBuffers( 1, &osgrc->_vboParticlesPositions );
    glDeleteBuffers( 1, &osgrc->_vboParticlesColors );
    glDeleteVertexArrays( 1, &osgrc->_vao );
  }

  void OSGManager::accept( osg::PrimitiveFunctor& functor ) const
  {
    OSGRenderConfig* osgrc = static_cast< OSGRenderConfig* >( _renderConfig );

    if ( !osgrc->_vertexArray || !osgrc->_billboardIndices )
      return;

    // add drawable to the stats
    functor.setVertexArray( osgrc->_vertexArray->size( ),
                            static_cast< const osg::Vec3* >(
                                osgrc->_vertexArray->getDataPointer( )));
    osgrc->_billboardIndices->accept( functor );
  }


  void OSGManager::configureProgram( const std::string& shaderPathVert,
                                     const std::string& shaderPathFrag )
  {
    OSGRenderConfig* osgrc = static_cast< OSGRenderConfig* >(
        _particleSystem->renderer( )->renderConfig( ));

    osg::StateSet* psState = getOrCreateStateSet( );

    osg::Program* program = new osg::Program;
    osg::Shader* vertexShader = new osg::Shader( osg::Shader::VERTEX );
    osg::Shader* fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );

    std::string fullPath;

    std::cout << "Loading vertex shader:" << shaderPathVert << std::endl;

    // Load vertex shader
    fullPath = osgDB::findDataFile( shaderPathVert );

    if ( fullPath.empty( ))
      PREFR_THROW( "Vertex file not found at: " + shaderPathVert )

    assert(vertexShader->loadShaderSourceFromFile( fullPath ));

    std::cout << "Loading vertex shader:" << shaderPathFrag << std::endl;

    // Load fragment shader
    fullPath = osgDB::findDataFile( shaderPathFrag );

    if ( fullPath.empty( ))
      PREFR_THROW( "Fragment shader file not found at: " + shaderPathVert )

    assert(fragmentShader->loadShaderSourceFromFile( fullPath ));

    program->addShader( vertexShader );
    program->addShader( fragmentShader );

    osgrc->_uCameraUp = new osg::Uniform( "cameraUp", osg::Vec3f( ));
    osgrc->_uCameraRight = new osg::Uniform( "cameraRight", osg::Vec3f( ));

    psState->addUniform( osgrc->_uCameraUp );
    psState->addUniform( osgrc->_uCameraRight );

    program->addBindAttribLocation( "vertexPosition", 0 );
    program->addBindAttribLocation( "particlePosition", 1 );
    program->addBindAttribLocation( "particleColor", 2 );

    psState->setRenderingHint(
      osg::StateSet::/*RenderingHint::*/TRANSPARENT_BIN );

    psState->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    psState->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::BlendFunc* blendFunc = new osg::BlendFunc( );

    blendFunc->setSource( osg::BlendFunc::SRC_ALPHA );
    blendFunc->setDestination( osg::BlendFunc::ONE_MINUS_SRC_ALPHA );

    psState->setAttributeAndModes( blendFunc, osg::StateAttribute::ON );

    psState->setAttributeAndModes( program, osg::StateAttribute::ON );

  }

  void OSGManager::updateUniformVariables( void )
  {
    assert(_cameraManipulator != nullptr);

    OSGRenderConfig* osgrc =
        static_cast< OSGRenderConfig* >(
            _particleSystem->renderer( )->renderConfig( ));

    // Get camera position to calculate distances
    osg::Vec3d eye, center, up;
    _cameraManipulator->getTransformation( eye, center, up );

    osgrc->_eye = osg::Vec3f( eye );
    osgrc->_center = osg::Vec3f( center );
    osgrc->_up = osg::Vec3f( up );

    _particleSystem->updateCameraDistances(
        glm::vec3( osgrc->_eye.x( ), osgrc->_eye.y( ), osgrc->_eye.z( )));

    osg::Vec3f forward = ( osgrc->_center - osgrc->_eye );
    forward.normalize( );

    osgrc->_right = osgrc->_up ^ forward;

    osgrc->_uCameraUp->set( osgrc->_up );

    osgrc->_uCameraRight->set( osgrc->_right );
  }

}

#endif