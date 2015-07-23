cmake_minimum_required(VERSION 2.4)

project(fmjpeg2k)

set(VERSION "1.0.1")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

find_package(dcmtk REQUIRED CONFIG HINTS ${DCMTK_DIR})

#include openjpeg
include(${OPENJPEG}/lib/openjpeg-2.1/OpenJPEGConfig.cmake)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${DCMTK_INCLUDE_DIRS} ${OPENJPEG_INCLUDE_DIRS})

add_definitions(-DOPJ_STATIC)
add_definitions(-Dfmjpeg2k_EXPORTS)

set(fmjpeg2k_HEADERS
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

set(fmjpeg2k_SRCS
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


#VC++ stuff
add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)

ADD_EXECUTABLE(dcmcjp2k dcmcjp2k.cc)
TARGET_LINK_LIBRARIES(dcmcjp2k 			
	${DCMTK_LIBRARIES}		
	openjp2
	fmjpeg2k
	)

add_library(fmjpeg2k ${fmjpeg2k_SRCS})

TARGET_LINK_LIBRARIES(fmjpeg2k 			
	${DCMTK_LIBRARIES}	
	openjp2
	)
	
	
install(TARGETS fmjpeg2k            	  
      LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
      ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	  
	  )
	 
install(FILES ${fmjpeg2k_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/fmjpeg2k)