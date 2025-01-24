
cmake_minimum_required(VERSION 3.2)

project(chiaki)

# Like option(), but the value can also be AUTO
macro(tri_option name desc default)
    set("${name}" "${default}" CACHE STRING "${desc}")
    set_property(CACHE "${name}" PROPERTY STRINGS AUTO ON OFF)
endmacro()

option(CHIAKI_ENABLE_TESTS "Enable tests for Chiaki" OFF)
tri_option(CHIAKI_ENABLE_SETSU "Enable libsetsu for touchpad input from controller" AUTO)
#added below
option(CHIAKI_LIB_ENABLE_OPUS "Use Opus as part of Chiaki Lib" ON)

option(CHIAKI_LIB_ENABLE_MBEDTLS "Use mbedtls instead of OpenSSL as part of Chiaki Lib" OFF)
option(CHIAKI_LIB_OPENSSL_EXTERNAL_PROJECT "Use OpenSSL as CMake external project" OFF)
option(CHIAKI_GUI_ENABLE_SDL_GAMECONTROLLER "Use SDL Gamecontroller for Input" ON)
option(CHIAKI_CLI_ARGP_STANDALONE "Search for standalone argp lib for CLI" OFF)
tri_option(CHIAKI_USE_SYSTEM_JERASURE "Use system-provided jerasure instead of submodule" AUTO)
tri_option(CHIAKI_USE_SYSTEM_NANOPB "Use system-provided nanopb instead of submodule" AUTO)

set(CHIAKI_VERSION_MAJOR 2)
set(CHIAKI_VERSION_MINOR 1)
set(CHIAKI_VERSION_PATCH 1)
set(CHIAKI_VERSION ${CHIAKI_VERSION_MAJOR}.${CHIAKI_VERSION_MINOR}.${CHIAKI_VERSION_PATCH})

set(CPACK_PACKAGE_NAME "chiaki-rpi")
set(CPACK_PACKAGE_DESCRIPTION "Open Source PS4|PS5 remote play client")
set(CPACK_PACKAGE_VERSION_MAJOR ${CHIAKI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CHIAKI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CHIAKI_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
include(CPack)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/setsu/cmake")

set(CMAKE_CXX_STANDARD 11)

if(CHIAKI_USE_SYSTEM_JERASURE)
	if(CHIAKI_USE_SYSTEM_JERASURE STREQUAL AUTO)
		find_package(Jerasure QUIET)
		set(CHIAKI_USE_SYSTEM_JERASURE ${Jerasure_FOUND})
	else()
		find_package(Jerasure REQUIRED)
		set(CHIAKI_USE_SYSTEM_JERASURE ON)
	endif()
endif()

find_package(PythonInterp 3 REQUIRED) # Make sure nanopb doesn't find Python 2.7 because Python 2 should just die.

if(CHIAKI_USE_SYSTEM_NANOPB)
	if(CHIAKI_USE_SYSTEM_NANOPB STREQUAL AUTO)
		find_package(Nanopb QUIET)
		set(CHIAKI_USE_SYSTEM_NANOPB ${Nanopb_FOUND})
	else()
		find_package(Nanopb REQUIRED)
		set(CHIAKI_USE_SYSTEM_NANOPB ON)
	endif()
endif()

add_subdirectory(third-party)

add_definitions(-DCHIAKI_VERSION_MAJOR=${CHIAKI_VERSION_MAJOR} -DCHIAKI_VERSION_MINOR=${CHIAKI_VERSION_MINOR} -DCHIAKI_VERSION_PATCH=${CHIAKI_VERSION_PATCH} -DCHIAKI_VERSION=\"${CHIAKI_VERSION}\")

if(CHIAKI_LIB_OPENSSL_EXTERNAL_PROJECT)
	include(OpenSSLExternalProject)
endif()

if(CHIAKI_LIB_ENABLE_MBEDTLS)
	add_definitions(-DCHIAKI_LIB_ENABLE_MBEDTLS)
endif()



if(0)
	set(CMAKE_PREFIX_PATH "/home/pi/dev/rpi-ffmpeg/out/armv7-buster-shared-rel/install")
	find_package(PkgConfig REQUIRED)	
	pkg_check_modules(PC_FFMPEG avcodec avutil avformat)

	find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h HINTS ${PC_FFMPEG_LIBAVFORMAT_INCLUDEDIR} ${PC_FFMPEG_INCLUDE_DIRS})
	find_library(AVFORMAT_LIBRARY NAMES avformat HINTS ${PC_FFMPEG_LIBAVFORMAT_LIBDIR} ${PC_FFMPEG_LIBRARY_DIRS})

	find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h HINTS ${PC_FFMPEG_LIBAVCODEC_INCLUDEDIR} ${PC_FFMPEG_INCLUDE_DIRS})
	find_library(AVCODEC_LIBRARY NAMES avcodec HINTS ${PC_FFMPEG_LIBAVCODEC_LIBDIR} ${PC_FFMPEG_LIBRARY_DIRS})

	find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h HINTS ${PC_FFMPEG_LIBAVUTIL_INCLUDEDIR} ${PC_FFMPEG_INCLUDE_DIRS})
	find_library(AVUTIL_LIBRARY NAMES avutil HINTS ${PC_FFMPEG_LIBAVUTIL_LIBDIR} ${PC_FFMPEG_LIBRARY_DIRS})

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(AVFormat DEFAULT_MSG AVFORMAT_LIBRARY AVFORMAT_INCLUDE_DIR)
	find_package_handle_standard_args(AVCodec DEFAULT_MSG AVCODEC_LIBRARY AVCODEC_INCLUDE_DIR)
	find_package_handle_standard_args(AVUtil DEFAULT_MSG AVUTIL_LIBRARY AVUTIL_INCLUDE_DIR)

	mark_as_advanced(AVFORMAT_INCLUDE_DIR AVFORMAT_LIBRARY)
	mark_as_advanced(AVCODEC_INCLUDE_DIR AVCODEC_LIBRARY)
	mark_as_advanced(AVUTIL_INCLUDE_DIR AVUTIL_LIBRARY)

	set(FFMPEG_INCLUDE_DIRS ${AVFORMAT_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
	set(FFMPEG_LIBRARIES ${AVUTIL_LIBRARY} ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY})

	if(${AVFORMAT_FOUND} AND ${AVCODEC_FOUND} AND ${AVUTIL_FOUND})
		set(CHIAKI_ENABLE_FFMPEG_DECODER ON)
		message("--- FFMPEG FOUND ---")
		#message("${FFMPEG_INCLUDE_DIRS}")
		message("${FFMPEG_LIBRARIES}")
	else()
		set(CHIAKI_ENABLE_FFMPEG_DECODER OFF)
		message("--- FFMPEG NOT FOUND ---")
	endif()


	#~ find_package(FFMPEG COMPONENTS avcodec avutil)
	#~ if(FFMPEG_FOUND)
		#~ set(CHIAKI_ENABLE_FFMPEG_DECODER ON)
	#~ else()
		#~ if(NOT CHIAKI_ENABLE_FFMPEG_DECODER STREQUAL AUTO)
			#~ message(FATAL_ERROR "CHIAKI_ENABLE_FFMPEG_DECODER is set to ON, but ffmpeg could not be found.")
		#~ endif()
		#~ set(CHIAKI_ENABLE_FFMPEG_DECODER OFF)
	#~ endif()
endif()

find_package(Nanopb QUIET)

#~ if(CHIAKI_ENABLE_FFMPEG_DECODER)
	#~ message(STATUS "FFMPEG Decoder enabled")
#~ else()
	#~ message(STATUS "FFMPEG Decoder disabled")
#~ endif()


# FRED
set(CHIAKI_ENABLE_PI_DECODER OFF)

add_subdirectory(lib)

#~ if(CHIAKI_ENABLE_CLI)
	#~ add_subdirectory(cli)
#~ endif()

if(CHIAKI_ENABLE_SETSU)
	if(CHIAKI_ENABLE_SETSU STREQUAL AUTO AND SDL2_FOUND AND (SDL2_VERSION_MINOR GREATER 0 OR SDL2_VERSION_PATCH GREATER_EQUAL 14))
		message(STATUS "SDL version ${SDL2_VERSION} is >= 2.0.14, disabling Setsu")
		set(CHIAKI_ENABLE_SETSU OFF)
	else()
		find_package(Udev QUIET)
		find_package(Evdev QUIET)
		if(Udev_FOUND AND Evdev_FOUND)
			set(CHIAKI_ENABLE_SETSU ON)
		else()
			if(NOT CHIAKI_ENABLE_SETSU STREQUAL AUTO)
				message(FATAL_ERROR "
	CHIAKI_ENABLE_SETSU is set to ON, but its dependencies (udev and evdev) could not be resolved.
	Keep in mind that setsu is only supported on Linux!")
			endif()
			set(CHIAKI_ENABLE_SETSU OFF)
		endif()
		if(CHIAKI_ENABLE_SETSU)
			add_subdirectory(setsu)
		endif()
	endif()
endif()

if(CHIAKI_ENABLE_SETSU)
	message(STATUS "Setsu enabled")
else()
	message(STATUS "Setsu disabled")
endif()

if(CHIAKI_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

#fred rpi
add_subdirectory(rpi)


