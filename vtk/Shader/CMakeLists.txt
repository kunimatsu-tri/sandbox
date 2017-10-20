cmake_minimum_required(VERSION 3.5)

find_package(VTK 8.0.0 REQUIRED
  PATHS /home/kunimatsu/work/vtk/VTK/_build/
)

include(${VTK_USE_FILE})
include_directories(${VTK_INCLUDE_DIRS})

add_executable(shader shader.cxx)

target_link_libraries(shader
  # For >= v7.1.0.
  vtkRenderingOpenGL2
  vtkRenderingGL2PSOpenGL2

  # Common for v6.1 and v7.1.
  vtkCommonCore
  vtkInteractionStyle
  vtkIOGeometry
  vtkIOImage
  vtkRenderingFreeType
)
