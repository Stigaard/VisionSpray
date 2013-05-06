if(USE_DATALOGGER) #If logging data
  add_definitions(-DUSE_DATALOGGER)
endif(USE_DATALOGGER)

set(VisionSpray_SRCS 
      ${DATALOGGER_SRCS} 
      ${CMAKE_CURRENT_SOURCE_DIR}/VisionSpray.cpp 
      ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
      ${CMAKE_CURRENT_SOURCE_DIR}/spraytimekeeper.cpp 
      ${CMAKE_CURRENT_SOURCE_DIR}/gpsreader.cpp 
      ${CMAKE_CURRENT_SOURCE_DIR}/demosaic_cv.cpp
    PARENT_SCOPE)
