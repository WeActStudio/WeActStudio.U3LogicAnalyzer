# This file is part of the LogicAnalyzer project.
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

# u3logicanalyzer-mcp-server: an MCP server that drives sigrok-cli.
#
# No library dependencies beyond libc: the server talks to sigrok-cli as a
# child process, so it can be shipped on every platform we build without
# pulling libsigrok into it. No install rules either, build.sh copies the
# binary into dist/ (and into the app bundle on macOS).
cmake_minimum_required(VERSION 3.16)

# The version is the product's, read from the application so the two cannot
# drift: initialize replies with it, and the file properties carry it.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../pulseview/CMakeLists.txt" PV_VERSION_LINE
	REGEX "^set\\(PV_VERSION_STRING")
string(REGEX REPLACE "^set\\(PV_VERSION_STRING[ \t]+\"([^\"]+)\".*" "\\1" U3LA_VERSION "${PV_VERSION_LINE}")
if(NOT U3LA_VERSION)
	message(WARNING "PV_VERSION_STRING not found in pulseview/CMakeLists.txt, falling back to 0.0.0")
	set(U3LA_VERSION "0.0.0")
endif()

project(u3logicanalyzer_mcp_server VERSION ${U3LA_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Both the reply to initialize and the Windows resource take the version from
# the project above, so a release bumps it in one file only.
configure_file(src/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

add_executable(u3logicanalyzer_mcp_server
	src/main.c
	src/json.c
	src/proc.c
	src/tools.c
	src/util.c
)

target_compile_options(u3logicanalyzer_mcp_server PRIVATE -Wall -Wextra)
target_compile_definitions(u3logicanalyzer_mcp_server PRIVATE SERVER_VERSION="${PROJECT_VERSION}")

if(WIN32)
	# CreateProcessW, GetModuleFileNameW and the file attribute calls.
	target_link_libraries(u3logicanalyzer_mcp_server PRIVATE kernel32)
	# Version resource: without it the file properties are empty.
	target_sources(u3logicanalyzer_mcp_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()
