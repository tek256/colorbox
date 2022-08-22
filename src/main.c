#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <linmath.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui/cimgui.h>
#include <cimgui/cimgui_impl.h>

static vec4 color;
static vec4 default_color;

void setup_ui(GLFWwindow* window) {
  igCreateContext(NULL);
  const char* glsl_version = "#version 150";
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void update_ui(GLFWwindow* window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  igNewFrame();

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  igBegin("Color", NULL, 0);
  igText("color:");
  igColorPicker4("##picker", (float*)&color,
                 ImGuiColorEditFlags_NoSidePreview |
                     ImGuiColorEditFlags_NoSmallPreview,
                 (float*)default_color);
  igEnd();
}

void draw_ui(void) {
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // Use the core profile, we won't be needing any extensions
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the window with GLFW
  GLFWwindow* window = glfwCreateWindow(1280, 720, "colorbox", NULL, NULL);

  if (!window) {
    printf("Unable to make window.\n");
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  int err = gl3wInit();
  if (err) {
    printf("Unable to initialize OpenGL!\n");
    return 1;
  }

  glViewport(0, 0, 1280, 720);

  vec4_set(color, 1.0f, 0.0f, 0.0f, 1.0f);
  vec4_set(default_color, 1.0f, 1.0f, 0.0f, 1.0f);

  setup_ui(window);

  bool show_color_window = true;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    update_ui(window);

    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_ui();

    glfwSwapBuffers(window);
  }

  // clean up
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
