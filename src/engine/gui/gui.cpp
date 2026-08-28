#include "gui.hpp"

#include "imgui.h"
// #include "implot.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <filesystem>

#include "glm/gtc/type_ptr.hpp"
#include "global.hpp"
#include "../environment.hpp"

using namespace ImGui;

static bool configCollapsed = true;
static bool infoCollapsed = true;

Camera* gui::camPtr = nullptr;
LightPoint* gui::lightPtr = nullptr;
SphereSegmented* gui::spherePtr = nullptr;

u16 gui::fps = 1;

void gui::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void gui::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void gui::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}

void gui::init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(global::window, true);
  ImGui_ImplOpenGL3_Init();
}

void gui::toggleConfig() { configCollapsed = !configCollapsed; }
void gui::toggleInfo()   { infoCollapsed   = !infoCollapsed;   }

void gui::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // ::::: Config window ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
  SetNextWindowCollapsed(configCollapsed);

  auto _task = global::profiler->startScopedTask("gui::draw");

  Begin("Config");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  // ===== Spectate camera =============================================================================== //

  assert(camPtr);
  if (CollapsingHeader("Spectate camera")) {
    SliderFloat("Near##2", &camPtr->nearPlane, 0.01f, 1.f);
    SliderFloat("Far##2", &camPtr->farPlane,  10.f, 1000.f);
    SliderFloat("Speed##2", &camPtr->speedDefault, 1.f, 50.f);
    SliderFloat("FOV##2", &camPtr->fov, 45.f, 179.f);
    DragFloat("Yaw##2", &camPtr->yaw);
    DragFloat("Pitch##2", &camPtr->pitch);
    DragFloat3("Position", glm::value_ptr(camPtr->position));

    if (TreeNode("Flags")) {
      CheckboxFlags("Right", &camPtr->flags, CameraFlags_DrawRight);
      CheckboxFlags("Up", &camPtr->flags, CameraFlags_DrawUp);
      CheckboxFlags("Forward", &camPtr->flags, CameraFlags_DrawForward);

      TreePop();
    }
  }

  // ===== Material ====================================================================================== //

  assert(spherePtr);
  if (CollapsingHeader("Sphere")) {
    SliderFloat("Radius", &spherePtr->radius, 1.f, 100.f);
    DragFloat("Height scale", &spherePtr->heightScale);

    {
      static fspath selectedPath = spherePtr->material.currFolder;

      if (BeginCombo("Material", selectedPath.lexically_normal().filename().string().c_str())) {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::directory_iterator("res/tex/materials")) {
          if (entry.is_directory()) {
            bool isSelected = selectedPath == entry.path();

            if (Selectable(entry.path().lexically_normal().filename().string().c_str(), isSelected)) {
              selectedPath = entry.path();
              spherePtr->material.loadFrom(selectedPath);
            }

            if (isSelected)
              SetItemDefaultFocus();
          }
        }

        EndCombo();
      }
    }

    {
      static f0::index& selectedIdx = spherePtr->material.baseReflectivityIdx;

      if (BeginCombo("Base reflectivity", f0::all[selectedIdx].name)) {
        for (int i = 0; i < f0::IDX_COUNT; i++) {
          bool isSelected = selectedIdx == i;

          if (Selectable(f0::all[i].name, isSelected))
            selectedIdx = (f0::index)i;

          if (isSelected)
            SetItemDefaultFocus();
        }

        EndCombo();
      }
    }
  }

  // ===== Light ========================================================================================= //

  assert(lightPtr);
  if (CollapsingHeader("Light")) {
    static bool singleMultiplier = true;

    DragFloat3("Position", glm::value_ptr(lightPtr->position));
    ColorEdit3("Color", glm::value_ptr(lightPtr->color));

    Spacing();
    Checkbox("Single", &singleMultiplier);
    if (singleMultiplier) {
      static float mult = lightPtr->multiplier.x;

      if (DragFloat("Multiplier", &mult))
        lightPtr->multiplier = vec3(mult);
      } else {
        DragFloat("Red multiplier", &lightPtr->multiplier.r);
        DragFloat("Green multiplier", &lightPtr->multiplier.g);
        DragFloat("Blue multiplier", &lightPtr->multiplier.b);
    }

    Spacing();
    DragFloat("Radius", &lightPtr->radius, 1.f, 0.f);
  };

  // ===== Other ========================================================================================= //

  if (CollapsingHeader("Other")) {
    Checkbox("Show global axis", &global::drawGlobalAxis);
    {
      static fspath selectedPath = environment::_lastLoadedImage;

      if (BeginCombo("Environment", selectedPath.lexically_normal().filename().string().c_str())) {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::directory_iterator("res/tex/env")) {
          if (entry.is_regular_file()) {
            bool isSelected = selectedPath == entry.path();

            if (Selectable(entry.path().lexically_normal().filename().string().c_str(), isSelected)) {
              selectedPath = entry.path();
              environment::loadFromImageEquirectangularHDR(selectedPath);
            }

            if (isSelected)
              SetItemDefaultFocus();
          }
        }

        EndCombo();
      }
    }
  }

  End();

  _task.end();

  // ::::: Info window ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  const ImGuiViewport* viewport = GetMainViewport();
  ImVec2 posBR = viewport->WorkPos + viewport->WorkSize;

  SetNextWindowPos(posBR, ImGuiCond_Always, {1.f, 1.f});
  SetNextWindowCollapsed(infoCollapsed);

  Begin("Info");

  ImGui::Text("FPS: %d / %f.5 ms", fps, global::dt);

  assert(global::profiler);
  global::profiler->renderTasks(400, 200, 200, 0);

  End();

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gui::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  // ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

