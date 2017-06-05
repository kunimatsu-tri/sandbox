// This demo creates depth map for a polydata instance by extracting
// exact ZBuffer values.

#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdint.h>
#include <stdlib.h>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkImageShiftScale.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPNGReader.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>
#include <vtkUnsignedCharArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLPolyDataReader.h>


const int kImageWidth = 640;
const int kImageHeight = 480;
double kFar = 5.0;
double kNear = 0.5;
double kA = kFar / (kFar - kNear);
double kB = (kFar * kNear) / (kNear - kFar);

double Convert(double value) {
  return kB / (value - kA);
}

int main(int argc, char *argv[]) {
  // Loads OBJ file.
  vtkNew<vtkOBJReader> obj_reader;
  obj_reader->SetFileName(
      "/home/kunimatsu/work/parse/models/bottle/meshes/disposable_bottle_pet_tea.obj");
  obj_reader->Update();
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(obj_reader->GetOutputPort());

  // Loads PNG file.
  vtkNew<vtkPNGReader> png_reader;
  png_reader->SetFileName(
      "/home/kunimatsu/work/parse/models/bottle/meshes/disposable_bottle_pet_tea.png");
  png_reader->Update();
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(png_reader->GetOutputPort());
  texture->InterpolateOn();

  vtkNew<vtkActor> actor;
  actor->SetTexture(texture.GetPointer());
  actor->SetMapper(mapper.GetPointer());

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor.GetPointer());

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetOffScreenRendering(1);
  renWin->AddRenderer(renderer.GetPointer());
  renWin->SetSize(kImageWidth, kImageHeight);
  renWin->Render();

  vtkNew<vtkWindowToImageFilter> color_filter;
  color_filter->SetInput(renWin.GetPointer());
  color_filter->SetMagnification(1);
  color_filter->SetInputBufferTypeToRGBA();
  color_filter->ReadFrontBufferOff();
  color_filter->Update();

  // Create Depth Map
  vtkNew<vtkWindowToImageFilter> depth_filter;
  depth_filter->SetInput(renWin.GetPointer());
  depth_filter->SetMagnification(1);
  depth_filter->SetInputBufferTypeToZBuffer();
  depth_filter->ReadFrontBufferOff();
  depth_filter->Update();

  vtkNew<vtkCamera> camera;
  camera->SetPosition(0., 0., 2.);
  camera->SetClippingRange(0.5, 5.0);
  renderer->SetActiveCamera(camera.GetPointer());

  std::vector<double> times;
  for (int i = 0; i < 100; ++i) {
    // skipping kinematics calculation.

    auto start = std::chrono::system_clock::now();
    vtkNew<vtkTransform> transform;
    std::vector<double> values;
    for (int k = 0; k < 7; ++k) {
      values.push_back(std::rand());
    }
    double d = std::sqrt(values[0] * values[0] +
                         values[1] * values[1] +
                         values[2] * values[2] +
                         values[3] * values[3]);
    transform->RotateWXYZ(values[0] / d, values[1] / d,
                          values[2] / d, values[3] / d);
    transform->Translate(values[4], values[5], values[6]);
    actor->SetUserTransform(transform.GetPointer());
    renWin->Render();
    color_filter->Modified();
    color_filter->Update();
    depth_filter->Modified();
    depth_filter->Update();

    // for (int x = 0; x < kImageWidth; ++x) {
    //   for (int y = 0; y < kImageHeight; ++y) {
    //     float raw_z = *static_cast<float*>(
    //         depth_filter->GetOutput()->GetScalarPointer(x, y, 0));
    //     void* color_ptr = color_filter->GetOutput()->GetScalarPointer(x, y, 0);
    //     float r = *(static_cast<uint8_t*>(color_ptr) + 0);
    //     float g = *(static_cast<uint8_t*>(color_ptr) + 1);
    //     float b = *(static_cast<uint8_t*>(color_ptr) + 2);
    //     float a = *(static_cast<uint8_t*>(color_ptr) + 3);
    //   }
    // }

    std::vector<uint8_t> image;
    int size = kImageWidth * kImageHeight * 4;
    image.resize(size);
    void* color_ptr = color_filter->GetOutput()->GetScalarPointer(0, 0, 0);
    memcpy(image.data(), color_ptr, size);

    std::vector<float> depth_image;
    depth_image.resize(kImageWidth * kImageHeight);
    void* depth_ptr = depth_filter->GetOutput()->GetScalarPointer(0, 0, 0);
    memcpy(depth_image.data(), depth_ptr, size);

    // for (int y = 0; y < kImageHeight; ++y) {
    //   for (int x = 0; x < kImageWidth; ++x) {
    //     float raw_z = depth_image.at(y * kImageWidth + x);
    //     // float r = *(static_cast<uint8_t*>(color_ptr) + 0);
    //     // float g = *(static_cast<uint8_t*>(color_ptr) + 1);
    //     // float b = *(static_cast<uint8_t*>(color_ptr) + 2);
    //     // float a = *(static_cast<uint8_t*>(color_ptr) + 3);
    //   }
    // }
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();
    times.push_back(elapsed);
  }

  for (auto time : times) {
    std::cout << time << " ms" << std::endl;
  }

  std::cout << " Average: "
            << std::accumulate(times.begin(), times.end(), 0.0) / times.size()
            << " ms" << std::endl;

  return EXIT_SUCCESS;
}
