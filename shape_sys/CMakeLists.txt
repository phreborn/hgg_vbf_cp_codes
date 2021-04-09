cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(GetMag CXX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

message("Project dir is ${PROJECT_SOURCE_DIR}")
if(NOT DEFINED ENV{AtlasProject}) # not needed when inside ATLAS environment
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
  set(CMAKE_INSTALL_BINDIR ${PROJECT_SOURCE_DIR}/bin)
  set(CMAKE_INSTALL_LIBDIR ${PROJECT_SOURCE_DIR}/lib)
endif()

# CERN ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core Gpad Hist HistFactory MathCore Matrix Minuit Net Physics RIO Rint RooFit RooFitCore RooStats TMVA Thread Tree TreePlayer Graf)

# BOOST
find_package(Boost 1.60 COMPONENTS REQUIRED program_options log log_setup system filesystem date_time thread chrono)

#set(PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT DEFINED ENV{AtlasProject}) # not defined when inside ATLAS environment
  include(${ROOT_USE_FILE})
endif()
message("root use file " ${ROOT_USE_FILE})

#include_directories(${PROJECT_INCLUDE_DIRS})
include_directories(src/ include/)

# Generate dictionary
#ROOT_GENERATE_DICTIONARY(G__GetMag HggTwoSidedCBPdf.h LINKDEF include/LinkDef.h)
ROOT_GENERATE_DICTIONARY(G__GetMag RooTwoSidedCBShape.h LINKDEF include/LinkDef.h)

set(PROJECT_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES})
#add_library(lGetMag SHARED src/HggTwoSidedCBPdf.cxx G__GetMag.cxx)
add_library(lGetMag SHARED src/RooTwoSidedCBShape.cxx G__GetMag.cxx)
target_link_libraries(lGetMag ${PROJECT_LIBRARIES})

# Create a main program using the library
add_executable(getMag.exe getMag.cxx)
target_link_libraries(getMag.exe lGetMag ${PROJECT_LIBRARIES})
