cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED QtCore)

include_directories(${QT_INCLUDES} ${OPENCV_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QT_DEFINITIONS})

set(velocityFilter_SRCS velocityfilter.cpp)

qt4_automoc(${velocityFilter_SRCS})

ADD_LIBRARY(velocityFilter STATIC ${velocityFilter_SRCS})
	   
TARGET_LINK_LIBRARIES(velocityFilter ${QT_QTCORE_LIBRARY})
