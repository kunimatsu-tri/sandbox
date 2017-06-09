cmake_minimum_required(VERSION 2.8)

find_package(VTK 7.1.0 REQUIRED
  PATHS /home/kunimatsu/work/vtk/VTK/_build/v7.1
)

include(${VTK_USE_FILE})

add_executable(RenderTexture RenderTexture.cxx)

target_link_libraries(RenderTexture
  # For v7.1.0 only
  vtkRenderingOpenGL2

  # Common for v6.1 and v7.1
  vtkCommonCore
  vtkInteractionStyle
  vtkIOGeometry
  vtkIOImage
  vtkRenderingFreeType

  # These are only for v6.1.0
  # vtkRenderingOpenGL
  # vtkRenderingFreeTypeOpenGL

  # These are for v5.8
  # vtkRendering
  # vtkImaging
  # vtkIO
  # vtkFiltering
  # vtkCommon
  # vtkGraphics
  # vtkftgl
  # vtkverdict
  # vtksys
  # vtkDICOMParser
  # vtkmetaio
)
