#An alternative build system for UnfoldUtils.
#Inspired by Heidi Schellman's CMakeLists.txt file in build.
#Debugged and maintained by Andrew Olivier aolivier@ur.rochester.edu
cmake_minimum_required(VERSION 2.8.12)
project(UnfoldUtils CXX C)

# Include module with function 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

#Set up different build modes with custom compiler flags.
#I could do this, but a c++17-dependent ROOT build overrides it anyway.
#set( CMAKE_CXX_STANDARD 11 )
#set( CMAKE_CXX_STANDARD_REQUIRED ON )

#I think Heidi disabled warnings about the form() function with -Wformat=0
set( GCC_Flags_For_CXX "-Wall -fPIC -pthread -Wformat=0 --std=c++0x" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_Flags_For_CXX}" )
set( CMAKE_CXX_FLAGS_DEBUG "-ggdb" )

#Required packages
find_package(ROOT REQUIRED COMPONENTS RIO Net OPTIONAL_COMPONENTS Reflex Cintex Unfold) #TODO: Only ROOT 6 seems to have the Unfold package

if(${ROOT_VERSION} VERSION_LESS 6 AND NOT ${ROOT_Reflex_FOUND})
  MESSAGE(FATAL_ERROR "Reflex is optional except when it's not.  ROOT 6 has Reflex "
                      "support built in, so it doesn't have a separate component "
                      "for Reflex.  Reflex was an experimental feature in ROOT 5, "
                      "so I have to require it as a component there.  You appear to "
                      "be using ${ROOT_VERSION}, so I can't let you get away with "
                      "skipping out on Reflex support!  I need Reflex to build the "
                      "ROOT dictionary for UnfoldUtils.")
endif()

if(${ROOT_VERSION} VERSION_LESS 6 AND NOT ${ROOT_Cintex_FOUND})
  MESSAGE(FATAL_ERROR "Cintex is optional except when it's not.  ROOT 6 has Reflex "
                      "support built in, so it doesn't have a separate component "
                      "for Cintex.  Cintex was an experimental feature in ROOT 5, "
                      "so I have to require it as a component there.  You appear to "
                      "be using ${ROOT_VERSION}, so I can't let you get away with "
                      "skipping out on Cintex support!  I need Cintex to load metadata "
                      "about MnvH1D and MnvH2D at runtime.")
endif()

if(${ROOT_VERSION} VERSION_LESS 6)
  MESSAGE("I need Cintex on this platform, so making sure to compile against it...")
else()
  MESSAGE("I don't need Cintex on this platform, so skipping it...")
  add_definitions(-DMNVROOT6)
endif()

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} $ENV{BOOSTDIR})
#Hack to dodge a bug I'm encountering with ROOT 5.34 on the MINERvA GPVMs but
#not develop of ROOT (6) from mid 2019.  ROOT is failing to set up a variable
#to find genreflex for some weird reason.  It's on PATH when I have the MINERvA
#framework setup, so I'll just go find it myself.
if(NOT ROOT_genreflex_cmd)
  MESSAGE("Hmm, ROOT failed to set up genreflex for some weird reason.  I'll "
          "go off and try to find it myself...")
  find_program(ROOT_genreflex_cmd genreflex)
  if(NOT ROOT_genreflex_cmd)
    MESSAGE(FATAL_ERROR "Failed to find genreflex on PATH!  ROOT is supposed to "
                        "set it up but doesn't seem to on the MINERvA GPVMs with "
                        "ROOT 6.  I need genreflex to build the ROOT dictionary "
                        "for UnfoldUtils.")
  endif()
endif()

#All components in this project can refer to other components by
#relative path from the root source code directory.
#This must happen before any other packages are set up in case
#UnfoldUtils is installed to the same prefix.
include_directories( "${PROJECT_SOURCE_DIR}" )

#From Heidi: ---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

find_package(MAT REQUIRED)
include_directories(${MAT_INCLUDE_DIR})

find_package(MAT-MINERvA REQUIRED)
include_directories(${MAT-MINERvA_INCLUDE_DIR})

#From Heidi: put some special flags here forgot what the DBUILD_SHARED_LIBS=OFF does..
add_definitions(-DFORM "-DBUILD_SHARED_LIBS=OFF")

#From Heidi: do this or mac will make it dylib
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

#Build code from 2 subdirectories directly here
#From Heidi
# tell it about the source files
set(SOURCE_FILES
MinervaUnfold/MnvResponse.cxx
MinervaUnfold/MnvUnfold.cxx
MinervaUnfold/UnfoldCovmxAsErrorBand.cxx
RooUnfold/RooUnfold.cxx
RooUnfold/RooUnfoldBayes.cxx
RooUnfold/RooUnfoldBinByBin.cxx
RooUnfold/RooUnfoldErrors.cxx
RooUnfold/RooUnfoldInvert.cxx
RooUnfold/RooUnfoldParms.cxx
RooUnfold/RooUnfoldResponse.cxx
RooUnfold/RooUnfoldSvd.cxx
RooUnfold/RooUnfoldTUnfold.cxx
RooUnfold/TSVDUnfold.cxx
RooUnfold/matrix_mult_mt.cxx
)

# tell it about the header files

set(MNV_HEADER_FILES
MinervaUnfold/MnvResponse.h
MinervaUnfold/MnvUnfold.h
MinervaUnfold/UnfoldCovmxAsErrorBand.h
)

set(ROO_HEADER_FILES
RooUnfold/RooUnfold.h
RooUnfold/RooUnfoldBayes.h
RooUnfold/RooUnfoldBinByBin.h
RooUnfold/RooUnfoldErrors.h
RooUnfold/RooUnfoldInvert.h
RooUnfold/RooUnfoldParms.h
RooUnfold/RooUnfoldResponse.h
RooUnfold/RooUnfoldSvd.h
RooUnfold/RooUnfoldTUnfold.h
#RooUnfold/RooUnfold_LinkDef.h
RooUnfold/TSVDUnfold_local.h
RooUnfold/matrix_mult_mt.h
)

#Include build system for the rest of this project.  Add new top-level directories here.
#First, build a ROOT dictionary
#I modified this dictionary generation code from Heidi's CMakeLists.txt
REFLEX_GENERATE_DICTIONARY(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/dict/${PROJECT_NAME}Dict.h SELECTION ${CMAKE_CURRENT_SOURCE_DIR}/dict/${PROJECT_NAME}Dict.xml OPTIONS -D__GCC_XML__ --noIncludePaths)

#install the dictionary files ROOT generates where LD_LIBRARY_PATH can find them
#(after sourcing setup.sh of course)
#Taken from Clark McGrew's edepsim package
if(${ROOT_VERSION} VERSION_GREATER 6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Dict.rootmap
    DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_rdict.pcm
    DESTINATION lib)

  #In the transition to ROOT 6, dictionary files generated by reflex
  #through ROOT adopted a different naming convention.  The commit
  #that changed this was https://github.com/root-project/root/commit/d62b4fed98805d45079f9863
  set(DICTIONARY_NAME ${PROJECT_NAME}.cxx)
else(${ROOT_VERSION} VERSION_GREATER 6)
  set(DICTIONARY_NAME ${PROJECT_NAME}_dict.cpp)
endif(${ROOT_VERSION} VERSION_GREATER 6)

#Now, build libUnfoldUtils.  Note that this builds the ROOT dictionary for PlotUtils into libplotutils itself.
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${DICTIONARY_NAME})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} MAT MAT-MINERvA) 
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets DESTINATION lib)
install(CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E create_symlink lib${PROJECT_NAME}.so ${CMAKE_INSTALL_PREFIX}/lib/lib${PROJECT_NAME}Dict.so )" ) #Make a symbolic link to the "magic" file name that older ROOT 6 versions look for.
install(FILES ${MNV_HEADER_FILES} DESTINATION include/MinervaUnfold)
install(FILES ${ROO_HEADER_FILES} DESTINATION include/RooUnfold)
install(EXPORT ${PROJECT_NAME}Targets DESTINATION lib/cmake/${PROJECT_NAME})

add_executable(TransWarpExtraction macros/TransWarpExtraction.cxx)
target_link_libraries(TransWarpExtraction ${ROOT_LIBRARIES} MAT MAT-MINERvA ${PROJECT_NAME})
install(TARGETS TransWarpExtraction DESTINATION bin)

export(TARGETS ${PROJECT_NAME} FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

#add_subdirectory(python) #TODO: Copy this configuration over from PlotUtils if I ever need it

#Install setup script.  This adds the install location of PlotUtils to
#LD_LIBRARY_PATH and sets other useful environment variables.
configure_file(setup.sh.in setup_${PROJECT_NAME}.sh @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup_${PROJECT_NAME}.sh DESTINATION bin)

#Make the results of this build into a distributable package.  Designed to be distributed as a .tar.gz
#Learned to do this from http://agateau.com/2009/cmake-and-make-dist/
set( CPACK_PACKAGE_VERSION_MAJOR "0" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "0" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
set( CPACK_SOURCE_GENERATOR "TGZ" )

include( CPack )
add_custom_target( dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source )

#Learned to set up CMake package from Clark McGrew's edep-sim package
# Write the 'UnfoldUtilsConfigVersion.cmake' file which can be used to
# check if a version meets the requested properties.
write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  VERSION 1.0)

# Write the 'EDepSimConfig.cmake' file so that a user package can
# access this with find_package.
configure_package_config_file(
  PackageConfig.cmake.in
  ${PROJECT_NAME}Config.cmake
  PATH_VARS CMAKE_INSTALL_PREFIX
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})

# Install the config files.
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME})
