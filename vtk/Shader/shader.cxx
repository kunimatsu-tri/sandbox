#include <string>

#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTexture.h>

// Example:
// https://www.vtk.org/gitweb?p=VTK.git;a=blob;f=Rendering/OpenGL2/Testing/Cxx/TestCubeMap.cxx

namespace {

const bool kUseTexture = true;

std::string RemoveFileExtention(const std::string& filepath) {
  const size_t last_dot = filepath.find_last_of(".");
  if (last_dot == std::string::npos) {
    // Do nothing
  }
  return filepath.substr(0, last_dot);
}

}

int main(int argc, char* argv[]) {
  // Parse command line arguments
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " Filename(.obj)" << std::endl;
    return EXIT_SUCCESS;
  }

  VTK_AUTOINIT_CONSTRUCT(vtkRenderingOpenGL2)

  std::string filename = argv[1];
  vtkNew<vtkOBJReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();
  vtkNew<vtkOpenGLPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper.GetPointer());

  // This works for producing silhouette image.
  // mapper->SetFragmentShaderCode(
  //     "//VTK::System::Dec\n"
  //     "//VTK::Output::Dec\n"
  //     "void main() {\n"
  //     "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
  //     "}\n");


  mapper->AddShaderReplacement(
      vtkShader::Vertex,
      "//VTK::Normal::Dec", // replace the normal block
      true, // before the standard replacements
      "//VTK::Normal::Dec\n" // we still want the default
      "  varying vec3 myNormalMCVSOutput;\n", //but we add this
      false // only do it once
                               );
  mapper->AddShaderReplacement(
      vtkShader::Vertex,
      "//VTK::Normal::Impl", // replace the normal block
      true, // before the standard replacements
      "//VTK::Normal::Impl\n" // we still want the default
      "  myNormalMCVSOutput = normalMC;\n", //but we add this
      false // only do it once
                               );

  // now modify the fragment shader
  mapper->AddShaderReplacement(
      vtkShader::Fragment,  // in the fragment shader
      "//VTK::Normal::Dec", // replace the normal block
      true, // before the standard replacements
      "//VTK::Normal::Dec\n" // we still want the default
      "  varying vec3 myNormalMCVSOutput;\n", //but we add this
      false // only do it once
                               );
  mapper->AddShaderReplacement(
      vtkShader::Fragment,  // in the fragment shader
      "//VTK::Normal::Impl", // replace the normal block
      true, // before the standard replacements
      "//VTK::Normal::Impl\n" // we still want the default calc
      "  diffuseColor = abs(myNormalMCVSOutput);\n", //but we add this
      false // only do it once
                               );

  if (kUseTexture) {
    vtkNew<vtkPNGReader> texture_reader;
    // Assuming .png file has the same name as .obj file.
    texture_reader->SetFileName(
        std::string(RemoveFileExtention(filename) + ".png").c_str());
    texture_reader->Update();

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(texture_reader->GetOutputPort());
    texture->InterpolateOn();

    // Add texture
    actor->SetTexture(texture.GetPointer());
  }

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor.GetPointer());
  renderer->SetBackground(0.9, 0.5, 0.1);
  renderer->SetUseDepthPeeling(1);
  renderer->UseFXAAOn();

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer.GetPointer());
  renderWindow->SetSize(640,480);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
