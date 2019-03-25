cmake_minimum_required(VERSION 3.4)
project(peerboot)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
if(DEFINED GIT_COMMIT)
	set(CPACK_PACKAGE_VERSION_PATCH "GIT-${GIT_COMMIT}")
endif()
set(CPACK_PACKAGE_VENDOR "Catenocrypt")

# Create all libraries and executables in the root binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
	add_definitions(-D_WIN32_WINNT=0x0600
					-D_CRT_SECURE_NO_WARNINGS
					-DWINVER=0x0600
					-DWIN32_LEAN_AND_MEAN)
else()
	add_compile_options(-Werror=switch)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	add_definitions(/bigobj)
endif()

if(APPLE)
	set(PLATFORM_LINK_FLAGS "-framework Foundation -framework OpenCL")
elseif(WIN32)
	set(PLATFORM_LINK_FLAGS "/STACK:8000000") #provides 8MB default stack size for linux on windows
else()
	set(PLATFORM_LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINK_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(src/peerboot)
add_subdirectory(src/test/testlib)
add_subdirectory(src/test/testapp1)
add_subdirectory(src/test/testapp2)
add_subdirectory(src/sample/sampleclientapp)

#set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)

#include(CPack NO_POLICY_SCOPE)