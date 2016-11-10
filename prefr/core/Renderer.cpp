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
#include "Renderer.h"

namespace prefr
{

  Renderer::Renderer( )
  : _distances( nullptr )
  , _renderConfig( nullptr )
  , _parallel( false )
  { }

  Renderer::~Renderer()
  {
    if (_renderConfig)
      delete( _renderConfig );
  }

  void Renderer::particles( const ParticleRange& particles_ )
  {
    _particles = particles_;
  }

  RenderConfig* Renderer::renderConfig( void ) const
  {
    return _renderConfig;
  }

  void Renderer::distanceArray( DistanceArray* distArray )
  {
    _distances = distArray;
  }

}