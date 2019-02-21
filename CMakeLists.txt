
cmake_minimum_required(VERSION 3.4.1)

project(InteractiveGraphCut)

# Enable C++11 for new language features
set(CMAKE_CXX_STANDARD 11)
# Release/Debug are available. 'Release' by default
set(CMAKE_BUILD_TYPE Debug)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Path to 3rdparty library
set(3RDPARTY_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib")

# Pre-define the extension name of library files on different platforms
if(WIN32)
	set(EXT_DYNAMIC_LIBRARY "dll")
elseif(APPLE)
	set(EXT_DYNAMIC_LIBRARY "dylib")
elseif(UNIX)
    set(EXT_DYNAMIC_LIBRARY "so")
endif()

if(WIN32)
	set(EXT_STATIC_LIBRARY "lib")
elseif(APPLE)
	set(EXT_STATIC_LIBRARY "a")
elseif(UNIX)
    set(EXT_STATIC_LIBRARY "a")
endif()

function(FILE_EXISTS FOUND FILES)
	set(${FOUND} TRUE PARENT_SCOPE)
	foreach(FILE ${FILES})
		if(NOT EXISTS ${FILE})
			set(${FOUND} FALSE PARENT_SCOPE)
			return()
		endif()
	endforeach()
endfunction()

# Eigen3
find_package(EIGEN3 QUIET)
if(NOT EIGEN3_FOUND)
	set(EIGEN3_INCLUDE_DIRS "${3RDPARTY_LIBRARY_DIR}/eigen3")
endif()
FILE_EXISTS(EIGEN3_FOUND ${EIGEN3_INCLUDE_DIRS})
if(NOT EIGEN3_FOUND)
	message(FATAL_ERROR "Failed to find Eigen3.")
endif()

# GLM
find_package(GLM QUIET)
if(NOT GLM_FOUND)
	set(GLM_INCLUDE_DIRS "${3RDPARTY_LIBRARY_DIR}/glm/include")
endif()
FILE_EXISTS(GLM_FOUND ${GLM_INCLUDE_DIRS})
if(NOT GLM_FOUND)
	message(FATAL_ERROR "Failed to find GLM.")
endif()
# Enable experimental modules provided by GLM
# Otherwise, compiler might report errors
add_definitions("-DGLM_ENABLE_EXPERIMENTAL")

# OpenCV
find_package(OpenCV QUIET)
if(NOT OpenCV_FOUND)
	set(OpenCV_INCLUDE_DIRS "${3RDPARTY_LIBRARY_DIR}/opencv/include")
	set(OpenCV_LIBRARY_DIR "${3RDPARTY_LIBRARY_DIR}/opencv/lib")
	file(GLOB OpenCV_LIBRARIES LIST_DIRECTORIES FALSE "${OpenCV_LIBRARY_DIR}/*opencv*.${EXT_STATIC_LIBRARY}")
	if(NOT OpenCV_LIBRARIES)
		file(GLOB OpenCV_LIBRARIES LIST_DIRECTORIES FALSE "${OpenCV_LIBRARY_DIR}/*opencv*.${EXT_DYNAMIC_LIBRARY}")
	endif()
	FILE_EXISTS(OpenCV_FOUND ${OpenCV_INCLUDE_DIRS} ${OpenCV_LIBRARIES})
endif()
if(NOT OpenCV_FOUND)
	message(FATAL_ERROR "Failed to find OpenCV.")
endif()

# Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL)
get_target_property(QtCore_LIBRARIES Qt5::Core LOCATION)
get_target_property(QtGui_LIBRARIES Qt5::Gui LOCATION)
get_target_property(QtWidgets_LIBRARIES Qt5::Widgets LOCATION)

set(3RDPARTY_INCLUDE_DIRS
	${EIGEN3_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}

	# Qt libraries
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS}
)

set(3RDPARTY_LIBRARIES
	# Qt libraries
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets

	${OpenCV_LIBRARIES}
)

# Specify the directories to headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${3RDPARTY_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(sample)
