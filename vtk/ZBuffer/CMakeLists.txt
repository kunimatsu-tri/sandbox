
cmake_minimum_required(VERSION 2.8)

PROJECT(ZBuffer)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(ZBuffer MACOSX_BUNDLE ZBuffer)

if(VTK_LIBRARIES)
  target_link_libraries(ZBuffer ${VTK_LIBRARIES})
else()
  target_link_libraries(ZBuffer vtkHybrid vtkWidgets)
endif()
