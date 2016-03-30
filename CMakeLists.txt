cmake_minimum_required(VERSION 2.4)

project(fmjp2k)

set (fmjp2k_VERSION_MAJOR 1)
set (fmjp2k_VERSION_MINOR 0)
set (fmjp2k_VERSION_BUILD 2)
set(fmjp2k_VERSION "${fmjp2k_VERSION_MAJOR}.${fmjp2k_VERSION_MINOR}.${fmjp2k_VERSION_BUILD}")

option(BUILD_SHARED_LIBS "Build fmjp2k shared library and link executables against it." ON)

find_package(dcmtk REQUIRED CONFIG HINTS ${DCMTK_DIR})
find_package(openjpeg REQUIRED CONFIG HINTS ${OPENJPEG})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${DCMTK_INCLUDE_DIRS} ${OPENJPEG_INCLUDE_DIRS})

add_definitions(-DOPJ_STATIC)
add_definitions(-Dfmjpeg2k_EXPORTS)

set(fmjp2k_HEADERS
	djcodecd.h
    djcodece.h
	djcparam.h
	djdecode.h
	djencode.h
	djerror.h
	djlsutil.h
	djrparam.h
	dldefine.h
	memory_file.h
)

set(fmjp2k_SRCS
    ${fmjpeg2k_HEADERS}    
	djcodecd.cc
	djcodece.cc
	djcparam.cc
	djdecode.cc
	djencode.cc
	djrparam.cc
	djutils.cc
	memory_file.cpp
)

if(WIN32)
	add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
endif()

ADD_EXECUTABLE(dcmcjp2k dcmcjp2k.cc)
TARGET_LINK_LIBRARIES(dcmcjp2k 			
	${DCMTK_LIBRARIES}		
	openjp2
	fmjpeg2k
	)
	
ADD_EXECUTABLE(dcmdjp2k dcmdjp2k.cc)
TARGET_LINK_LIBRARIES(dcmdjp2k 			
	${DCMTK_LIBRARIES}		
	openjp2
	fmjpeg2k
	)


add_library(fmjpeg2k ${fmjpeg2k_SRCS})
set(fmjpeg2k_LIBRARY_NAME fmjpeg2k)
TARGET_LINK_LIBRARIES(fmjpeg2k
	${DCMTK_LIBRARIES}	
	openjp2
	)
	
include(GenerateExportHeader)	
generate_export_header(fmjpeg2k)
set_property(TARGET fmjpeg2k PROPERTY VERSION ${fmjpeg2k_VERSION})
set_property(TARGET fmjpeg2k PROPERTY SOVERSION 1)
set_property(TARGET fmjpeg2k PROPERTY
  INTERFACE_fmjpeg2k_MAJOR_VERSION 1)
set_property(TARGET fmjpeg2k APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING fmjpeg2k_MAJOR_VERSION
)

install(TARGETS fmjpeg2k EXPORT fmjpeg2kTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  FILES
    ${fmjpeg2k_HEADERS}
  DESTINATION
    include/fmjpeg2k
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/fmjp2k/fmjp2kConfigVersion.cmake"
  VERSION ${fmjp2k_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT fmjp2kTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/fmjp2k/fmjp2kTargets.cmake"  
)
configure_file( ${CMAKE_SOURCE_DIR}/cmake/fmjp2kConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/fmjp2kConfig.cmake
  @ONLY
)
#configure_file(cmake/fmjpeg2kConfig.cmake
#  "${CMAKE_CURRENT_BINARY_DIR}/fmjpeg2k/fmjpeg2kConfig.cmake"
#  COPYONLY
#)

set(ConfigPackageLocation lib/cmake/fmjpeg2k)
install(EXPORT fmjp2kTargets
  FILE
    fmjp2kTargets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)


install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/fmjp2kConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/fmjp2k/fmjp2kConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
