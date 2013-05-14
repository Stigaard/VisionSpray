project(VisionSpray)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMAKE_Modules/")
find_package(Qt4 REQUIRED QtGui QtCore QtOpenGL)
find_package(OpenCV REQUIRED)
find_package(RT REQUIRED)
find_package(OpenGL REQUIRED)
set(EMULATE_CAMERA 1)
set(EMULATE_ARMADILLO 0)
set(USE_GPS 1) #If compiling for using a GPS

set(USE_DATALOGGER 0) #If compiling with datalogging
set(CalibrateSprayTime 0) #If compiling for spray calibration
set(PROFILE 0) #If enabling profiling



#Performance tuning
if(PROFILE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -march=native")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif(PROFILE)

if(USE_GPS)
  add_definitions(-DUSE_GPS)
endif(USE_GPS)

if(EMULATE_CAMERA)
  add_definitions(-DEMULATE_CAMERA)
endif(EMULATE_CAMERA)

if(EMULATE_ARMADILLO)
  add_definitions(-DEMULATE_ARMADILLO)
endif(EMULATE_ARMADILLO)

if(CalibrateSprayTime)
  add_definitions(-DCalibrateSprayTime)
endif(CalibrateSprayTime)

ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(src)

include_directories(${QT_INCLUDES} ${OPENCV_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${ARAVIS_INCLUDE_DIRS})

qt4_automoc(${VisionSpray_SRCS} ${CAMERA_SRCS} ${GPS_SRCS})
add_definitions(${QT_DEFINITIONS})
add_executable(VisionSpray ${VisionSpray_SRCS} )
target_link_libraries(VisionSpray ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
		      ${OpenCV_LIBS} ${CAMERA_LIBS} ${RT_LIBRARIES} pthread ${QT_QTOPENGL_LIBRARY}
		      ${OPENGL_glu_LIBRARY} GL ${GPS_LIBS} qOpenGLCVWidget QtGigE qtgpscWidget 
		      BayerExG gpsReader qtRosBridge qtADAM)
