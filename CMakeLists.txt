cmake_minimum_required(VERSION 3.31)
project(IOTDPatch VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (!WIN32 OR !MSVC)
	message(SEND_ERROR "Only supported platform is Windows")
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCHITECTURE "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ARCHITECTURE "x86")
else ()
	message(SEND_ERROR "Only supported architectures are 'x64' and 'x86'")
endif ()

add_subdirectory(deps/minhook)

set(DEPS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(SOURCE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCE_DIRS}/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${DEPS_ROOT}/ASTD/include)
target_link_libraries(${PROJECT_NAME} PRIVATE minhook dinput8)
target_link_options(${PROJECT_NAME} PRIVATE /LTCG)
target_compile_definitions(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Debug>:BUILD_DEBUG=1>
	$<$<CONFIG:Release>:BUILD_RELEASE=1>
	ASTD_USE_UNICODE=0
)
