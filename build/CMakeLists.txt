# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.

cmake_minimum_required(VERSION 3.1...3.14)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables

project(unfoldutils VERSION 1.0 LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS} $ENV{BOOSTDIR})

set(CMAKE_CXX_FLAGS "-Wformat=0" )

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
link_directories(${ROOT_LIBRARY_DIR})

MESSAGE("ROOT_LIBRARY_DIR:${ROOT_LIBRARY_DIR}")

# put some special flags here forgot what the DBUILD_SHARED_LIBS=OFF does..

add_definitions( -DFORM -DMNVROOT6 -DPLOTUTILS_STANDALONE "-DBUILD_SHARED_LIBS=OFF")

# do this or mac will make it dylib
set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")


# tell it about the source files
set(SRC_FILES
MinervaUnfold/MnvResponse.cxx
MinervaUnfold/MnvUnfold.cxx
MinervaUnfold/UnfoldCovmxAsErrorBand.cxx
RooUnfold/matrix_mult_mt.cxx
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
)

# tell it about the header files

set(H_FILES
MinervaUnfold/MnvResponse.h
MinervaUnfold/MnvUnfold.h
MinervaUnfold/UnfoldCovmxAsErrorBand.h
RooUnfold/matrix_mult_mt.h
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
)

# need to know about Boost on macs so check that you know where it is.

MESSAGE("boost:$ENV{BOOSTDIR}")

include_directories(MinervaUnfold RooUnfold $ENV{UNFOLDUTILSROOT} $ENV{PLOTUTILSROOT} $ENV{PLOTUTILSROOT}/PlotUtils $ENV{BOOSTDIR}/include $ENV{BOOSTDIR})

# now generate the dictionary

ROOT_GENERATE_DICTIONARY(G__${PROJECT_NAME} ${H_FILES} LINKDEF ${CMAKE_SOURCE_DIR}/RooUnfold_LinkDef.h)

# and then the shared library
# Output libname matches target name, with the usual extensions on your system

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} G__${PROJECT_NAME}.cxx )

MESSAGE("ROOT_LIBRARIES:${ROOT_LIBRARIES} ")

MESSAGE("PROJECT:${PROJECT_NAME}")

target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} Unfold MathCore -L$ENV{PLOTUTILSROOT} -lplotutils)

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
#add_executable(myExample simple_example.cpp)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
#target_link_libraries(MyExample PRIVATE MyLibExample)

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
add_executable(TransWarpExtraction macros/TransWarpExtraction.cxx)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(TransWarpExtraction PRIVATE ${PROJECT_NAME} )
