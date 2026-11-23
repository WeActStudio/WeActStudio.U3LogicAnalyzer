# This file is part of u3logicanalyzer-cli, a fork of the sigrok-cli project.
#
# Copyright (C) 2026 WeAct Studio
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# u3logicanalyzer-cli, built against the libsigrok of this tree.
#
# It is shipped next to the application so scripts and agents can drive the
# analyzer without a graphical interface. No install rules: build.sh copies
# the binary into dist/ (and into the app bundle on macOS).
cmake_minimum_required(VERSION 3.16)

# The version is the product's, not the 0.8.0 of the upstream sigrok-cli this
# tool descends from: a user who reports a problem quotes what --version
# prints, and it has to match the release the binary shipped in.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../pulseview/CMakeLists.txt" PV_VERSION_LINE
	REGEX "^set\\(PV_VERSION_STRING")
string(REGEX REPLACE "^set\\(PV_VERSION_STRING[ \t]+\"([^\"]+)\".*" "\\1" U3LA_VERSION "${PV_VERSION_LINE}")
if(NOT U3LA_VERSION)
	message(WARNING "PV_VERSION_STRING not found in pulseview/CMakeLists.txt, falling back to 0.0.0")
	set(U3LA_VERSION "0.0.0")
endif()

project(u3logicanalyzer-cli VERSION ${U3LA_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# The binary is copied out of the build tree and shipped, so the build tree
# has to carry the install rpath. Without this CMake puts the absolute library
# directories of the build machine into it. Must be set before the target is
# created: the target property is initialised from this variable.
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0>=2.32.0)
pkg_check_modules(LIBSIGROK REQUIRED libsigrok>=0.5.0)
pkg_check_modules(LIBSIGROKDECODE REQUIRED libsigrokdecode>=0.5.0)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Version resource for Windows, generated so the numbers cannot drift from the
# project version above.
configure_file(version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

# The name is deliberately not "sigrok-cli": dist packages of that name land in
# usr/bin as well, and the AppImage puts this binary in the same directory.
add_executable(u3logicanalyzer-cli
	main.c
	show.c
	device.c
	session.c
	input.c
	output.c
	decode.c
	parsers.c
	anykey.c
	options.c
)

target_compile_definitions(u3logicanalyzer-cli PRIVATE HAVE_CONFIG_H)
target_compile_options(u3logicanalyzer-cli PRIVATE -Wall -Wextra)

target_include_directories(u3logicanalyzer-cli PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${GLIB_INCLUDE_DIRS}
	${LIBSIGROK_INCLUDE_DIRS}
	${LIBSIGROKDECODE_INCLUDE_DIRS}
)

target_link_directories(u3logicanalyzer-cli PRIVATE
	${GLIB_LIBRARY_DIRS}
	${LIBSIGROK_LIBRARY_DIRS}
	${LIBSIGROKDECODE_LIBRARY_DIRS}
)

target_link_libraries(u3logicanalyzer-cli PRIVATE
	${GLIB_LIBRARIES}
	${LIBSIGROK_LIBRARIES}
	${LIBSIGROKDECODE_LIBRARIES}
)

if(WIN32)
	target_link_libraries(u3logicanalyzer-cli PRIVATE ws2_32 setupapi ole32)
	# Version resource: without it the file properties are empty.
	target_sources(u3logicanalyzer-cli PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
else()
	target_link_libraries(u3logicanalyzer-cli PRIVATE m pthread dl)
endif()

# The binary travels next to the libsigrok of this tree instead of being
# installed, so the search path has to be relative: $ORIGIN covers the flat
# package and the AppImage (usr/bin against usr/lib), the bundle directory
# covers macOS, where build.sh rewrites every reference to Contents/Frameworks.
if(APPLE)
	set_target_properties(u3logicanalyzer-cli PROPERTIES
		INSTALL_RPATH "@executable_path/../Frameworks")
elseif(UNIX)
	set_target_properties(u3logicanalyzer-cli PROPERTIES
		INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")
endif()
