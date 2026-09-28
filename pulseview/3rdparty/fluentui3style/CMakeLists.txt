##
## FluentUI3Style, vendored so that the "FluentUI3" entry of the widget
## style setting is the same on every Qt version.
##
## Upstream builds this as a style plugin, and a plugin cannot be loaded
## at run time here: the application is linked against a static Qt, which
## has no plugin loader, so every plugin has to be linked in. The style is
## therefore built as a static library and handed to QApplication::
## setStyle() directly, which is the integration upstream describes for
## platforms without plugin loading.
##
## Only the style itself is taken; ExWidgets, the frameless helper (and
## with it QWindowKit) and the examples are not part of this copy. The
## style needs nothing but Qt Core, Gui, Widgets and Svg.
##

cmake_minimum_required(VERSION 3.16)

# The style is written for C++17 ("if" with initializer). The rest of the
# application is built with the standard its Qt needs, which is C++14 for
# Qt 5, so this directory has to set its own.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC TRUE)

file(GLOB fluentui3style_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc"
)

add_library(fluentui3style STATIC ${fluentui3style_SOURCES})

# Static build: the export macros have to expand to nothing, or both this
# library and the application would declare the symbols as imported.
target_compile_definitions(fluentui3style PUBLIC FLUENTUI3STYLE_STATIC)

target_include_directories(fluentui3style PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/FluentUI3Colors>
)

target_link_libraries(fluentui3style PUBLIC
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Svg
)

if(WIN32)
	target_link_libraries(fluentui3style PRIVATE dwmapi)
endif()

# With a static Qt, Qt adds a source file that imports a plugin to every
# target that links one of its modules, so that the plugins are registered
# in the binary. The application does that for its own target already and
# would end up registering every plugin twice, so none of them is taken
# here.
set(_no_plugins "")
foreach(_module Core Gui Widgets Svg)
	if(Qt${QT_VERSION_MAJOR}${_module}_PLUGINS)
		list(APPEND _no_plugins ${Qt${QT_VERSION_MAJOR}${_module}_PLUGINS})
	endif()
endforeach()
if(_no_plugins)
	set_property(TARGET fluentui3style PROPERTY QT_NO_PLUGINS ${_no_plugins})
endif()
