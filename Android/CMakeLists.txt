cmake_minimum_required (VERSION 2.8.7)
project(MCServer)
include(ExternalProject)

externalproject_add(lua_native
	SOURCE_DIR ../lib/lua
	CMAKE_GENERATOR "Visual Studio 14 2015"
	CMAKE_ARGS -D ANDROID_CROSSCOMPILE:BOOL=TRUE
	BUILD_COMMAND
		${CMAKE_COMMAND} --build <BINARY_DIR> --config Release
	INSTALL_COMMAND ""
)
externalproject_add(tolua_native
	SOURCE_DIR ../lib/tolua++
	CMAKE_GENERATOR "Visual Studio 14 2015"
	BUILD_COMMAND
		${CMAKE_COMMAND} --build <BINARY_DIR> --config Release
	INSTALL_COMMAND ""
)

# Tell libevent that Android has pthread (not auto detected due to NDK implementation problems)
# Tell Lua to not use unimplemented Android locale features
add_definitions(-Dgetlocaledecpoint\(\)="'.'")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # Add c++14 support here since we're not calling set_flags()

add_library(AndroidNative jni/app-android.cpp)
add_dependencies(tolua_native lua_native)
add_dependencies(AndroidNative tolua_native)
target_link_libraries(AndroidNative log dl)

include_directories(
	../lib/
	../src/
	../lib/jsoncpp/include/
	../lib/polarssl/include/
	../lib/sqlitecpp/include/
	../lib/sqlitecpp/sqlite3/
	../lib/libevent/include/
)

# Probably not the recommended method, but other ways don't seem to generate libraries in libs/armeabi
set(CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/MCServer)
