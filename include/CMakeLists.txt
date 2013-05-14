if(USE_RS232)
#ADD_SUBDIRECTORY(qextserialport)
endif(USE_RS232)
#add_subdirectory(modicovi-rt)

#qextserialport
#Set twice once for each scope
set(qextserialport_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/qextserialport/libqextserialport.a PARENT_SCOPE)
set(qextserialport_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/qextserialport/libqextserialport.a)
# create a custom target called build_qextserialport that is part of ALL
# and will run each time you type make 
# add_custom_command(TARGET qmake_qextserialport
# 		   PRE_BUILD
# 		   COMMAND qmake
# 		   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qextserialport
# 		   COMMENT "Qmake qextserialport"
# 		   )
add_custom_target(build_qextserialport ALL 
                   COMMAND ${CMAKE_MAKE_PROGRAM}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qextserialport
                   COMMENT "Original qextserialport makefile target")
# now create an imported static target
add_library(qextserialport STATIC IMPORTED PARENT_SCOPE)
# Import target "qextserialport" for configuration ""
set_property(TARGET qextserialport APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(qextserialport PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${qextserialport_LIBRARY}")
# now you can use qextserialport as if it were a regular cmake built target in your project
add_dependencies(qextserialport build_qextserialport)

add_subdirectory(qOpenGLCVWidget)
add_subdirectory(QtGigE)
add_subdirectory(qtgpscWidget)
add_subdirectory(BayerExG)
add_subdirectory(gpsReader)
add_subdirectory(qtRosBridge)
add_subdirectory(qtADAM)