#ifdef _WIN32
  #include <direct.h>
  #define CHDIR(p) _chdir(p);
#else
  #include <unistd.h>
  #define CHDIR(p) chdir(p);
#endif

#include "global.hpp"
#include "engine/gui/gui.hpp"
#include "engine/Camera.hpp"
#include "engine/ShadersWatcher.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/LightPoint.hpp"
#include "engine/mesh/sphere/SphereSegmented.hpp"
#include "engine/environment.hpp"
#include "utils/clrp.hpp"

using global::window;

void GLAPIENTRY MessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
  static const clrp::clrp_t clrpError{clrp::ATTRIBUTE::BOLD, clrp::FG::RED};
  static const clrp::clrp_t clrpWarning{clrp::ATTRIBUTE::BOLD, clrp::FG::YELLOW};

  clrp::clrp_t clrpFinal = clrpError;
  bool stop = true;

  switch (source) {
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return; // Handled by the Shader class itself
    case GL_DEBUG_SOURCE_API:
      clrpFinal = clrpWarning; // "SIMD32 shader inefficient", skipping since occurs only on my laptop
      stop = false;
      break;
  }

  fprintf(
    stderr, "GL CALLBACK: %s source = 0x%x, id = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, id, type, severity, clrp::format(message, clrpFinal).c_str()
  );

  if (stop)
    exit(1);
}

int main() {
  // Assuming the executable is launching from its own directory
  CHDIR("../../..");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  // Window init
  window = glfwCreateWindow(1600, 900, "MyProgram", NULL, NULL);
  global::profiler = new ProfilerManager(300);
  ivec2 winSize = global::getWinSize();
  dvec2 winCenter = dvec2(winSize) / 2.;

  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2 * !global::guiFocused);
  glfwSetCursorPos(window, winCenter.x, winCenter.y);

  // GLAD init
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  gui::init();

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("res/shaders");

  Shader lightShader("light.vert", "light.frag");
  Shader linesShader("lines.vert", "lines.frag");
  Shader sphereShader("sphere/segmented.vert", "pbr.frag", "tbn.geom");
  Shader skyboxHdrShader("skybox-hdr.vert", "skybox-hdr.frag");

  ShadersWatcher::add(&lightShader);
  ShadersWatcher::add(&linesShader);
  ShadersWatcher::add(&sphereShader);
  ShadersWatcher::add(&skyboxHdrShader);

  // ===== Cameras ============================================== //

  Camera cameraSpectate({85.f, 77.f, 76.f}, -2.385f, -0.582f);
  cameraSpectate.setFarPlane(1000.f);
  cameraSpectate.setSpeedDefault(50.f);

  // ===== Inputs Handler ======================================= //

  InputsHandler::mousePos = global::getWinCenter();
  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);
  glfwSetCursorPosCallback(window, InputsHandler::cursorPosCallback);

  // ============================================================ //

  LightPoint light0({50.f, 100.f, 50.f}, vec3(1.f), vec3(3000.f));
  LightPoint light1({100.f, 0.f, 0.f}, global::red, vec3(3000.f));
  LightPoint light2({0.f, 100.f, 0.f}, global::green, vec3(3000.f));
  LightPoint light3({0.f, 0.f, 100.f}, global::blue, vec3(3000.f));

  SphereSegmented sphere(128, 10.f);
  sphere.loadMaterial("res/tex/materials/lava-and-rock");

  environment::init();
  environment::loadFromImageEquirectangularHDR("res/tex/env/cedar_bridge_sunset_1_4k.hdr");

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  gui::camPtr = &cameraSpectate;
  gui::lightPtr = &light0;
  gui::spherePtr = &sphere;

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    global::dt = currTime - prevTime;

    // FPS cap
    if (global::dt < fpsLimit) continue;
    else prevTime = currTime;

    global::time += global::dt;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      InputsHandler::process(cameraSpectate);
      cameraSpectate.update();
    } else
      glfwSetCursorPos(window, winCenter.x, winCenter.y);

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      gui::fps = static_cast<u16>(1.f / global::dt);
      titleTimer = currTime;
    }

    global::profiler->clearTasks();

    ShadersWatcher::check();

    light0.update();
    light1.update();
    light2.update();
    light3.update();

    light0.setUniforms(sphereShader);
    light1.setUniforms(sphereShader);
    light2.setUniforms(sphereShader);
    light3.setUniforms(sphereShader);

    glViewport(0, 0, winSize.x, winSize.y);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + !global::wireframeMode);

    environment::draw(&cameraSpectate, skyboxHdrShader);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    environment::texIrradianceCubemapHDR.bind(7);
    sphere.draw(&cameraSpectate, sphereShader);

    glDisable(GL_CULL_FACE);

    light0.draw(&cameraSpectate, lightShader);
    light1.draw(&cameraSpectate, lightShader);
    light2.draw(&cameraSpectate, lightShader);
    light3.draw(&cameraSpectate, lightShader);

    if (global::drawGlobalAxis) {
      Mesh::drawDebugDirectionLine(&cameraSpectate, {}, {1e6f, 0.f, 0.f}, global::red);
      Mesh::drawDebugDirectionLine(&cameraSpectate, {}, {0.f, 1e6f, 0.f}, global::green);
      Mesh::drawDebugDirectionLine(&cameraSpectate, {}, {0.f, 0.f, 1e6f}, global::blue);
    }

    // ============================================================ //

    gui::draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  gui::shutdown();
  glfwTerminate();

  return 0;
}

