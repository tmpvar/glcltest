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
#include "mouse.h"
#include "orbit-camera.h"

// Phases
#include "phase_raster_obb.h"
#include "phase_output.h"
// TODO: shapes that move need to re-upload the buffer...

#define MAX_SHAPES 100
static struct
{
  float id, x, y, r;
} shapes[MAX_SHAPES];

float zero = 0.0f;
uint8_t izero = 0;

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

  int has_texture = phase_output_resize(bwidth, bheight);

  if (!has_texture) {
    cl_int shared_texture_error;
    // Create clgl shared texture
    fb = clCreateFromGLTexture(
      job.context,
      CL_MEM_WRITE_ONLY,
      GL_TEXTURE_2D,
      0,
      phase_output_get_fb(),
      &shared_texture_error
    );

    CL_CHECK_ERROR(shared_texture_error);
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

void vec3_print(const vec3 v) {
  printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
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

  // setup orbit camera
  vec3 eye = vec3_create(0.0f, 0.0f, -5);
  vec3 center = vec3f(0.0f);
  vec3 up = vec3_create(0.0, 1.0, 0.0 );
  orbit_camera_init(eye, center, up);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // TODO: only OSX?
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window = glfwCreateWindow(640, 480, "gogo", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowSizeCallback(window, resize);

  ImVec4 clear_color = ImColor(114, 144, 154);
  glfw_imgui_init(window, true);

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  glfwSwapInterval(1);

// -- shared texture --
  compute_init(&job);
  glfwGetFramebufferSize(window, &bwidth, &bheight);
  resize(window, bwidth, bheight);

// -- end shared texture --

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

  phase_raster_obb_init();
  phase_output_init();

  while (!glfwWindowShouldClose(window)) {
    glfw_imgui_new_frame();

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      orbit_camera_rotate(0, 0, -.1, 0);
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      orbit_camera_rotate(0, 0, .1, 0);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      orbit_camera_rotate(0, 0, 0, .1);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      orbit_camera_rotate(0, 0, 0, -.1);
    }

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

    // compute MVP matrix
    mat4 projection;
    mat4_perspective(
      projection,
      M_PI/4.0,
      (float)width/(float)height,
      0.1,
      1000.0
    );
    mat4 view, mvp;
    orbit_camera_view(view);
    vec3 ro = mat4_get_eye(view);
    mat4_mul(mvp, projection, view);

    phase_output_render();
    phase_raster_obb_render(mvp);

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
