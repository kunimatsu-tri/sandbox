#include <string>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTexture.h>


namespace {

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

  std::string filename = argv[1];
  vtkNew<vtkOBJReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkNew<vtkPNGReader> texture_reader;
  // Assuming .png file has the same name as .obj file.
  texture_reader->SetFileName(
      std::string(RemoveFileExtention(filename) + ".png").c_str());
  texture_reader->Update();

  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(texture_reader->GetOutputPort());
  texture->InterpolateOn();

  // Visualize
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper.GetPointer());

  // Add texture
  actor->SetTexture(texture.GetPointer());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor.GetPointer());
  renderer->SetBackground(0.9, 0.5, 0.1);

  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer.GetPointer());
  renderWindow->SetSize(640,480);

  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

  renderWindow->Render();

  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
