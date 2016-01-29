#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#include <GL/glu.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "gl.h"
#include "compute.h"
#include "glfw-imgui.h"

static const struct
{
  float x, y;
} vertices[6] =
{
  { -1.0f,  1.0f },
  {  1.0f,  1.0f },
  { -1.0f, -1.0f },
  {  1.0f, -1.0f },
  { -1.0f, -1.0f },
  {  1.0f,  1.0f },
};

// TODO: shapes that move need to re-upload the buffer...

#define MAX_SHAPES 100
static struct
{
  float id, x, y, r;
} shapes[MAX_SHAPES];

float zero = 0.0f;
uint8_t izero = 0;

static const char* vertex_shader_text =
"#version 330 core\n"
"in vec2 vPos;\n"
"out vec2 pos;\n"
"void main() {\n"
"  pos = vPos * 0.5 + 0.5;\n"
"  gl_Position = vec4(vPos, 0.0, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"uniform sampler2D tex;\n"
"in vec2 pos;\n"
"out vec4 fragColor;\n"
"void main() {\n"
"  fragColor = texture(tex, pos.xy);\n"
"}\n";

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }

  // propagate key events into imgui
  // TODO: see if there's a way to denote "handled" so we can fall back
  //       to global key bindings instead
  glfw_imgui_key_callback(window, key, scancode, action, mods);
}

cl_mem fb;
GLuint fb_texture = 0;
int has_texture = 0;
size_t global_threads[2];
glcl_job_t job;
int bwidth, bheight;

void resize(GLFWwindow *window, int width, int height) {
  glfwGetFramebufferSize(window, &bwidth, &bheight);

  cl_int block_width = 20;
  cl_int block_height = 20;

  global_threads[0] = ceil((float)bwidth / (float)block_width);
  global_threads[1] = ceil((float)bheight / (float)block_height);


  clSetKernelArg(job.kernel, 3, sizeof(cl_int), &block_width);
  clSetKernelArg(job.kernel, 4, sizeof(cl_int), &block_height);

  if (!has_texture) {
    glGenTextures(1, &fb_texture);
  }

  glBindTexture(GL_TEXTURE_2D, fb_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA8,
    bwidth,
    bheight,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    0
  );

  if (!has_texture) {
    // Create clgl shared texture
    fb = clCreateFromGLTexture(
      job.context,
      CL_MEM_WRITE_ONLY,
      GL_TEXTURE_2D,
      0,
      fb_texture,
      NULL
    );

    has_texture = 1;
  }

  // fill the image with black
  size_t fill_origin[3] = { 0, 0, 0 };
  size_t fill_region[3] = { bwidth, bheight, 1 };
  uint8_t fill_color[4] = { 0, 0, 0, 255 };

  cl_int fill_fb_result = clEnqueueFillImage(
    job.command_queue,
    fb,
    &fill_color,
    fill_origin,
    fill_region,
    0,
    NULL,
    NULL
  );


}

int main(void) {
  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint vpos_location;

  for (float i=0; i<MAX_SHAPES; i++) {
    shapes[(int)i].id = 1.0f;
    shapes[(int)i].x = (i - 1) * 5.0f * i;
    shapes[(int)i].y = 100.0f;
    shapes[(int)i].r = 10.0f * i;
  }

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // TODO: only OSX?
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowSizeCallback(window, resize);

  ImVec4 clear_color = ImColor(114, 144, 154);
  glfw_imgui_init(window, true);

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  glfwSwapInterval(1);

  // Create fullscreen quad
  glGenBuffers(1, &vertex_buffer);
  gl_error();
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// -- shared texture --
  compute_init(&job);
  glfwGetFramebufferSize(window, &bwidth, &bheight);
  resize(window, bwidth, bheight);

// -- end shared texture --

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  gl_error();
  glCompileShader(vertex_shader);
  gl_shader_log(vertex_shader);
  gl_error();

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);
  gl_shader_log(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  gl_error();

  glUseProgram(program);
  vpos_location = glGetAttribLocation(program, "vPos");
  GLint texture_location = glGetUniformLocation(program, "tex");

  GLuint vao = 0;
  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
              sizeof(float) * 2, (void*) 0);

  // fill the shape buffer with shapes
  cl_int errcode;
  cl_mem shape_buffer = clCreateBuffer(
    job.context,
    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
    sizeof(shapes),
    (void *)&shapes,
    &errcode
  );

  CL_CHECK_ERROR(errcode);


  clFlush(job.command_queue);

  cl_uint shape_count = MAX_SHAPES;
  clSetKernelArg(job.kernel, 0, sizeof(fb), &fb);
  clSetKernelArg(job.kernel, 1, sizeof(shape_buffer), &shape_buffer);
  clSetKernelArg(job.kernel, 2, sizeof(cl_uint), &shape_count);

  while (!glfwWindowShouldClose(window)) {
    glfw_imgui_new_frame();

// -- compute! --
    glFinish();
    clEnqueueAcquireGLObjects(job.command_queue, 1,  &fb, 0, 0, NULL);

    clEnqueueNDRangeKernel(
      job.command_queue,
      job.kernel,
      2,
      NULL,
      global_threads,
      NULL,
      0,
      NULL, // no waitlist
      NULL  // no callback
    );

    clEnqueueReleaseGLObjects(job.command_queue, 1,  &fb, 0, 0, NULL);
    clFlush(job.command_queue);
// -- end compute --

    float ratio;
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glUniform1i(texture_location, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    {
        static float f = 0.0f;
        ImGui::Text(
          "%.3f ms @ (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate,
          ImGui::GetIO().Framerate
        );
        ImGui::Text(
          "shape count: %u",
          shape_count
        );
    }

    ImGui::Render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  compute_destroy(&job);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
