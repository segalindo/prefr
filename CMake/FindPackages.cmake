include(System)
list(APPEND FIND_PACKAGES_DEFINES ${SYSTEM})
find_package(PkgConfig)


#########################################################
# FIND NVIDIA OPENGL
#########################################################

# This tries to fix problems when linking in Linux with nVidia OpenGL
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(nvidiaOpenGL)
  if ( NVIDIA_OPENGL_gl_LIBRARY )
    set( CMAKE_EXE_LINKER_FLAGS "-L${NVIDIA_OPENGL_gl_LIBRARY_PATH}" )
    message(STATUS "nVidia library used ["
      ${NVIDIA_OPENGL_gl_LIBRARY_PATH}
      "]")
  endif()
endif()


#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
if(OPENGL_FOUND)
  set(OPENGL_name LIBOPENGL)
  set(OPENGL_FOUND TRUE)
endif()
if(OPENGL_name)
  list(APPEND PREFR_DEPENDENT_LIBRARIES OPENGL)
  list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_OPENGL)
  list(APPEND PREFR_LINK_LIBRARIES ${OPENGL_LIBRARIES})
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OPENGL")
  if(NOT "${${libOPENGL_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${libOPENGL_name}_INCLUDE_DIRS})
    add_definitions(${OPENGL_DEFINITIONS})
  endif()
endif()


#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
if(GLEW_FOUND)
  set(GLEW_name LIBGLEW)
  set(GLEW_FOUND TRUE)
endif()
if(GLEW_name)
  list(APPEND PREFR_DEPENDENT_LIBRARIES GLEW)
  list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_GLEW)
  list(APPEND PREFR_LINK_LIBRARIES ${GLEW_LIBRARIES})
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLEW")
  if(NOT "${${libGLEW_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${libGLEW_name}_INCLUDE_DIRS})
    add_definitions(${GLEW_DEFINITIONS})
  endif()
endif()


#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
if(GLUT_FOUND)
  set(GLUT_name LIBGLUT)
  set(GLUT_FOUND TRUE)
endif()
if(GLUT_name)
  list(APPEND PREFR_DEPENDENT_LIBRARIES GLUT)
  list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_GLUT)
  list(APPEND PREFR_LINK_LIBRARIES ${GLUT_LIBRARIES})
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} GLUT")
  if(NOT "${${libGLUT_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${libGLUT_name}_INCLUDE_DIRS})
    add_definitions(${GLUT_DEFINITIONS})
  endif()
endif()


#########################################################
# FIND GLM
#########################################################
include_directories(${GLM_INCLUDE_DIRS})

#########################################################
# FIND CUDA
#########################################################

if (WITH_CUDA)
  find_package(CUDA 6.5 REQUIRED)
  include_directories(${CUDA_INCLUDE_DIRS})
  link_directories(${CUDA_LIBRARY_DIRS})
  add_definitions(${CUDA_DEFINITIONS})
  if(NOT CUDA_FOUND)
    message(ERROR " CUDA not found!")
  endif(NOT CUDA_FOUND)

  set( CUDA_PROPAGATE_HOST_FLAGS OFF )
  set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};
    -O3 -gencode arch=compute_20,code=sm_20 -Xcompiler "-std=c++11" ) #-std=c++11)

endif()

#########################################################
# FIND Thrust
#########################################################
if (WITH_CUDA)
  find_package(CUDAThrust REQUIRED)
  if(CUDATHRUST_FOUND)
    set(CUDAThrust_name LIBCUDATHRUST)
    set(CUDAThrust_FOUND TRUE)
  elseif(CUDAThrust_FOUND)
    set(CUDAThrust_name LIBCUDATHRUST)
    set(CUDAThrust_FOUND TRUE)
  endif()
  if(CUDAThrust_name)
    list(APPEND PREFR_DEPENDENT_LIBRARIES CUDATHRUST)
    list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_CUDATHRUST)
    list(APPEND PREFR_LINK_LIBRARIES ${CUDATHRUST_LIBRARIES})
    set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} CUDAThrust")
    if(NOT "${${libCUDAThrust_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
      include_directories(BEFORE SYSTEM
        ${${libCUDAThrust_name}_INCLUDE_DIRS})
      add_definitions(${CUDAThrust_DEFINITIONS})
    endif()
  endif()



  # include_directories(${CUDATHRUST_INCLUDE_DIRS})
  # link_directories(${CUDATHRUST_LIBRARY_DIRS})
  # add_definitions(${CUDATHRUST_DEFINITIONS})
  # if(NOT CUDATHRUST_FOUND)
  #   message(ERROR " CUDATHRUST not found!")
  # endif(NOT CUDATHRUST_FOUND)

  list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_CUDA)

endif()

#########################################################
# FIND OpenSceneGraph
#########################################################
if (WITH_OPENSCENEGRAPH)
  FIND_PACKAGE(OpenSceneGraph
    COMPONENTS osgViewer osgGA osgDB osgUtil)
  include(Findosg_functions)
  if(OPENSCENEGRAPH_FOUND)
    set(OpenSceneGraph_name LIBOPENSCENEGRAPH)
    set(OpenSceneGraph_FOUND TRUE)
  elseif(OpenSceneGraph_FOUND)
    set(OpenSceneGraph_name LIBOPENSCENEGRAPH)
    set(OpenSceneGraph_FOUND TRUE)
  endif()
  if(OpenSceneGraph_name)
    list(APPEND PREFR_DEPENDENT_LIBRARIES OPENSCENEGRAPH)
    list(APPEND FIND_PACKAGES_DEFINES PREFR_WITH_OPENSCENEGRAPH)
    list(APPEND PREFR_LINK_LIBRARIES ${OPENSCENEGRAPH_LIBRARIES})
    set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} OpenSceneGraph")
    if(NOT "${${libOpenSceneGraph_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
      include_directories(BEFORE SYSTEM
        ${${libOpenSceneGraph_name}_INCLUDE_DIRS})
      add_definitions(${OpenSceneGraph_DEFINITIONS})
    endif()
  endif()
endif()



##########################################################
##########################################################
##########################################################

# Write defines.h and options.cmake
if(NOT PROJECT_INCLUDE_NAME)
  message(WARNING
    "PROJECT_INCLUDE_NAME not set, old or missing Common.cmake?")
  set(PROJECT_INCLUDE_NAME ${PROJECT_NAME})
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_CURRENT_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_INCLUDE_NAME}/defines${SYSTEM}.h")
list(APPEND COMMON_INCLUDES ${DEFINES_FILE})
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by CMake/FindPackages.cmake, do not edit.\n\n"
  "#ifndef ${PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_DEFINES})
  add_definitions(-D${DEF}=1)
  file(APPEND ${DEFINES_FILE_IN}
  "#ifndef ${DEF}\n"
  "#  define ${DEF} 1\n"
  "#endif\n")
if(NOT DEF STREQUAL SYSTEM)
  file(APPEND ${OPTIONS_CMAKE} "set(${DEF} ON)\n")
endif()
endforeach()
if(CMAKE_MODULE_INSTALL_PATH)
  install(FILES ${OPTIONS_CMAKE} DESTINATION ${CMAKE_MODULE_INSTALL_PATH}
    COMPONENT dev)
else()
  message(WARNING
    "CMAKE_MODULE_INSTALL_PATH not set, old or missing Common.cmake?")
endif()
file(APPEND ${DEFINES_FILE_IN}
  "\n#endif\n")

include(UpdateFile)
configure_file(${DEFINES_FILE_IN} ${DEFINES_FILE} COPYONLY)

if(FIND_PACKAGES_FOUND)
  if(MSVC)
    message(STATUS
      "Configured ${PROJECT_NAME} with ${FIND_PACKAGES_FOUND}")
  else()
    message(STATUS
      "Configured ${PROJECT_NAME} with ${CMAKE_BUILD_TYPE}${FIND_PACKAGES_FOUND}")
  endif()
endif()
