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
#ifndef PARTICLEUPDATER_H_
#define PARTICLEUPDATER_H_

#include <type_traits>

#include <prefr/api.h>

#include "Particles.h"

#include "Cluster.h"
#include "Model.h"
#include "Source.h"
#include "UpdateConfig.h"

namespace prefr
{

  class Cluster;

  /*! \class Updater
   *  \brief This class emits particles and updates their state over
   *  the execution.
   *
   *  Updater class is in charge of both, emit and update the particles
   *  along the system execution. For this, both Emit and Update methods
   *  can be re-implemented to fit the desired behavior. Both Updater#Emit and
   *  Updater#Update will be called from ParticleSystem#Update method each
   *  frame. Assign an Updater object to a Cluster to use it as the updater
   *  for cluster's particles.
   *
   *  @see ParticleSystem
   *  @see Particles
   *  @see Source
   *  @see Model
   *
   *  */
  class Updater
  {
    friend class ParticleSystem;

  public:

    /*! \brief Default constructor.
     *
     * Default constructor.
     *  */
    PREFR_API Updater( void );

    /*! \brief Default destructor.
     *
     * Default destructor.
     */
    PREFR_API virtual ~Updater( );

    /*! \brief Emit method.
     *
     *  This method is used to initialize dead particles and their
     *  properties. This method will set particles' life, color, position and so
     *  on taking into account the Model established properties.
     *
     *  Re-implement for different behaviors or when using non-default Model
     *  and Source classes when adding new properties.
     *
     *  Note: By default, Updater#Emit is always called before Updater#Update
     *  method.
     *
     * @param cluster Cluster owner of tparticle_ptr as second parameter.
     * @param current tparticle_ptr referencing current particle to be emitted.
     *
     * @see ParticleSytem#Update
     *
     */

// TODO merge both comments.

    /*! \brief Update method.
     *
     *  This method updates the state of particles each frame. This is,
     *  updating their life values (and setting them as dead if life is lower
     *  than 0) as well as other properties such as position, color, velocity,
     *  size and so on.
     *
     *  Re-implement for adding different behaviors as well as updating
     *  other non-default properties added or re-implemented Model objects.
     *
     *  Note: By default, Updater#Update is always called after
     *  Updater#Emit method.
     *
     * @param cluster Cluster owner of the particle to be updated.
     * @param current tparticle_ptr referencing the particle to be updated.
     * @param deltaTime Current delta time to compute attributes variations
     * according to elapsed time since last frame.
     */
    PREFR_API virtual void updateParticle( tparticle current,
                                           float deltaTime );

  protected:

    UpdateConfig* _updateConfig;
  };

  typedef std::vector< Updater* > UpdatersArray;
}

#endif /* PARTICLEUPDATER_H_ */
