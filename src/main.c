#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glad_gl.c>

#include <stdio.h>
#include <linmath.h>

static unsigned int shader;
static unsigned int quad;
static mat4x4       proj, view;
static mat4x4 model;
static vec4 color;

static char   shader_value_buff[128];

unsigned char* get_file_contents(const char* fp) {
  printf("FP: %s\n", fp);
  FILE* f = fopen(fp, "rb+");
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  unsigned char* data = malloc(size + 1);
  fread(data, size, sizeof(unsigned char), f);

  data[size] = '\0';
  fclose(f);

  return data;
}

unsigned int get_sub_shader(const char* f, int type) {
  unsigned char* data = get_file_contents(f);
  unsigned int   id   = glCreateShader(type);

  glShaderSource(id, 1, (const char**)&data, NULL);
  glCompileShader(id);

  unsigned int success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    int maxlen = 0;
    int len;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxlen);
    char* log = malloc(maxlen);

    if (!log) {
      printf("Unable to malloc space for log.\n");
      return -1;
    }

    glGetShaderInfoLog(id, maxlen, &len, log);
    printf("%s\n", log);
    free(log);
  }

  free(data);
  return id;
}

unsigned int get_shader(const char* v, const char* f) {
  unsigned int _v = get_sub_shader(v, GL_VERTEX_SHADER);
  unsigned int _f = get_sub_shader(f, GL_FRAGMENT_SHADER);

  unsigned int id = glCreateProgram();

  glAttachShader(id, _v);
  glAttachShader(id, _f);

  glLinkProgram(id);

  unsigned int success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    int maxlen = 0;
    int len;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxlen);
    char* log = malloc(maxlen);

    if (!log) {
      printf("Unable to malloc space for log.\n");
      return -1;
    }

    glGetProgramInfoLog(id, maxlen, &len, log);
    printf("%s\n", log);
    free(log);
  }

  glDeleteShader(_v);
  glDeleteShader(_f);

  return id;
}

unsigned int get_vao() {
  unsigned int vao, vbo, vboi;
  float        verts[16] = {// pos       //tex
                     -0.5f, -0.5f, 0.f, 0.f, -0.5f, 0.5f,  0.f, 1.f,
                     0.5f,  0.5f,  1.f, 1.f, 0.5f,  -0.5f, 1.f, 0.f};

  unsigned int inds[6] = {0, 1, 2, 2, 3, 0};

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &vboi);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &verts[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboi);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &inds[0],
               GL_STATIC_DRAW);

  glBindVertexArray(0);

  return vao;
}

void setup_render() {
  shader  = get_shader("res/shader.vert", "res/shader.frag");
  quad    = get_vao();

  mat4x4_identity(proj);
  mat4x4_identity(view);

  mat4x4_ortho(proj, 0.f, 1280.f, 720.f, 0.f, -10.f, 10.f);

  vec4_set(color, 1.0f, 0.0f, 0.0f, 1.0f);

  float x_offset = (1280.f / 2.f) - (2 * 72.f);
  float y_offset = (720.f / 2.f) - (2 * 72.f);

  mat4x4_identity(model);
  mat4x4_translate(model, 0.0f, 0.0f, 1.0f);
  mat4x4_scale_aniso(model, model, 1280.f * 2.f, 720.f * 2.f, 1.0f);
}

void set_v4(unsigned int shader, const char* uniform, vec4 vec) {
  glUniform4f(glGetUniformLocation(shader, uniform), vec[0], vec[1], vec[2], vec[3]);
}

void set_m4(unsigned int shader, const char* uniform, mat4x4 m) {
  glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, m);
}

void render() {
  glUseProgram(shader);

  set_m4(shader, "proj", proj);
  set_m4(shader, "view", view);

  // Standard single render
  set_m4(shader, "model", model);
  set_v4(shader, "color", color);

  glBindVertexArray(quad);
  glEnableVertexAttribArray(0);

  // Standard single render
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    return 1;
  }
  // Target OpenGL 3.2 with forward compatability!
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
  gladLoadGL(glfwGetProcAddress);

  glViewport(0, 0, 1280, 720);

  glClearColor(0.05f, 0.05f, 0.05f, 1.f);

  setup_render();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();

    glfwSwapBuffers(window);
  }

  return 0;
}
