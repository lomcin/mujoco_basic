###################################################################################
# MIT License                                                                     #
#                                                                                 #
# Copyright (c) 2024 Lucas Maggi                                                  #
#                                                                                 #
# Permission is hereby granted, free of charge, to any person obtaining a copy    #
# of this software and associated documentation files (the "Software"), to deal   #
# in the Software without restriction, including without limitation the rights    #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       #
# copies of the Software, and to permit persons to whom the Software is           #
# furnished to do so, subject to the following conditions:                        #
#                                                                                 #
# The above copyright notice and this permission notice shall be included in all  #
# copies or substantial portions of the Software.                                 #
#                                                                                 #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
# SOFTWARE.                                                                       #
###################################################################################



################################################
##             CMAKE REQUIREMENTS             ##
################################################
cmake_minimum_required(VERSION 3.17)



################################################
##               PROJECT INFO                 ##
################################################
project(
  mujoco_basic
  VERSION 0.0.0
  DESCRIPTION "Basic MuJoCo simulator and rendering example"
)



################################################
##               PROJECT FLAGS                ##
################################################
set(MUJOCO_BASIC_USE_HARDWARE_ENCODING OFF)



################################################
##             PROJECT FLAGS LOGIC            ##
################################################

# Here is the logic: Use CUDA and FFMPEG for video encoding
# If hardware encoding is set to ON
if (MUJOCO_BASIC_USE_HARDWARE_ENCODING)
  set(MUJOCO_BASIC_USE_CUDA ON)
  set(MUJOCO_BASIC_USE_FFMPEG ON)
  set(MUJOCO_BASIC_USE_OPENCV OFF)
  
  add_compile_definitions(USE_CUDA)
  add_compile_definitions(USE_FFMPEG)

# Here is the logic: Use OpenCV for video encoding
# If hardware encoding is set to OFF
else()
  set(MUJOCO_BASIC_USE_CUDA OFF)
  set(MUJOCO_BASIC_USE_FFMPEG OFF)
  set(MUJOCO_BASIC_USE_OPENCV ON)

  add_compile_definitions(USE_OPENCV)

endif()



################################################
##            INCLUDE FETCH CONTENT           ##
################################################
include(FetchContent)



################################################
##       SPECIAL CXX COMPILATION FLAGS        ##
################################################
## Compile as C++11 only in C++ compilation context
set(CMAKE_CXX_FLAGS
"${CMAKE_CXX_FLAGS} -std=c++11 -pthread -D__STDC_CONSTANT_MACROS"
)



################################################
##                  OpenGL                    ##
################################################
find_package(OpenGL REQUIRED)



################################################
##                  OpenCV                    ##
################################################
# NOTE: OpenCV is just being used for video encoding
# In the context of SOFTWARE video encoding.
if (MUJOCO_BASIC_USE_OPENCV)
  find_package( OpenCV REQUIRED )
endif()



################################################
##                  FFMPEG                    ##
################################################
# NOTE: FFMPEG is just being used for video encoding
# In the context of HARDWARE video encoding.
if (MUJOCO_BASIC_USE_FFMPEG)
  include(FindPkgConfig)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
      libavdevice
      libavfilter
      libavformat
      libavcodec
      libswresample
      libswscale
      libavutil
  )
endif()


################################################
##                   GLFW                     ##
################################################
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
  FetchContent_Populate(glfw)
  set(glfw_BUILD_SHARED_LIBS ON)
  add_subdirectory(
    ${glfw_SOURCE_DIR} ${glfw_BINARY_DIR}
  )
endif()



################################################
##                   IMGUI                    ##
################################################
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)
  include_directories(${imgui_SOURCE_DIR})
  set(imgui_SOURCES
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
  )
endif()

################################################
##                   MUJOCO                   ##
################################################
set(mujoco_BUILD_TESTING OFF)
FetchContent_Declare(
  mujoco
  GIT_REPOSITORY https://github.com/google-deepmind/mujoco.git
  GIT_TAG        3.1.1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  )
  # OVERRIDE_FIND_PACKAGE # 3.24 needed to work with find_package
  # find_package(mujoco) 3.24 needed to work with FetchContent
# set(FETCHCONTENT_QUIET OFF)
FetchContent_GetProperties(mujoco)
if(NOT mujoco_POPULATED)
  FetchContent_Populate(mujoco)
  add_subdirectory(
            ${mujoco_SOURCE_DIR} ${mujoco_BINARY_DIR}
            EXCLUDE_FROM_ALL
          )
endif()


################################################
##          MUJOCO BASIC INCLUDES             ##
################################################
include_directories(
  include
  ${mujoco_SOURCE_DIR}/include
  ${glfw_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)

add_executable(
  mujoco_basic
  src/basic.cpp
)

if (MUJOCO_BASIC_USE_HARDWARE_ENCODING)
  if (MUJOCO_BASIC_USE_FFMPEG)
    target_link_libraries(mujoco_basic PkgConfig::LIBAV)
  endif()
else()
  target_link_libraries(
    mujoco_basic
    PUBLIC
    mujoco
    OpenGL
    glfw
    ${OpenCV_LIBS}
  )
endif()