#ifndef __PHASE_RASTER_OBB__
#define __PHASE_RASTER_OBB__

  // #include "scene.h"

  float phase_raster_obb_vertices[108] = {
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f
  };

  static const char* phase_raster_obb_vertex_shader_text =
  "#version 330 core\n"
  "in vec3 vPos;\n"
  "uniform mat4 mvp;"
  "void main() {\n"
  "  gl_Position = mvp * vec4(vPos, 1.0);\n"
  "}\n";

  static const char* phase_raster_obb_fragment_shader_text =
  "#version 330 core\n"
  "uniform sampler2D tex;\n"
  "out vec4 fragColor;\n"
  "void main() {\n"
  "  fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
  "}\n";

  struct {
    GLuint vertex_buffer;
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    GLuint vao;
  } phase_raster_obb;


  void phase_raster_obb_init() {
    phase_raster_obb.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(phase_raster_obb.vertex_shader, 1, &phase_raster_obb_vertex_shader_text, NULL);
    gl_error();
    glCompileShader(phase_raster_obb.vertex_shader);
    gl_shader_log(phase_raster_obb.vertex_shader);
    gl_error();

    phase_raster_obb.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(phase_raster_obb.fragment_shader, 1, &phase_raster_obb_fragment_shader_text, NULL);
    glCompileShader(phase_raster_obb.fragment_shader);
    gl_shader_log(phase_raster_obb.fragment_shader);

    phase_raster_obb.program = glCreateProgram();
    glAttachShader(phase_raster_obb.program, phase_raster_obb.vertex_shader);
    glAttachShader(phase_raster_obb.program, phase_raster_obb.fragment_shader);
    glLinkProgram(phase_raster_obb.program);
    gl_error();

    glGenVertexArrays (1, &phase_raster_obb.vao);
    glBindVertexArray (phase_raster_obb.vao);

    // Create fullscreen quad
    glGenBuffers(1, &phase_raster_obb.vertex_buffer);
    gl_error();

    glBindBuffer(GL_ARRAY_BUFFER, phase_raster_obb.vertex_buffer);
    glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(phase_raster_obb_vertices),
      phase_raster_obb_vertices,
      GL_STATIC_DRAW
    );
  }

  void phase_raster_obb_render(mat4 mvp/*scene_t *scene, mat4 mvp*/) {
    glBindVertexArray (phase_raster_obb.vao);
    glBindBuffer(GL_ARRAY_BUFFER, phase_raster_obb.vertex_buffer);

    glUseProgram(phase_raster_obb.program);
    GLint vpos_location = glGetAttribLocation(phase_raster_obb.program, "vPos");

    glBindVertexArray (phase_raster_obb.vao);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(
      vpos_location,
      3,
      GL_FLOAT,
      GL_FALSE,
      0,
      (void*) 0
    );

    GLuint mvpid = glGetUniformLocation(phase_raster_obb.program, "mvp");
    glUniformMatrix4fv(mvpid, 1, GL_FALSE, mvp);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    gl_error();
  }
#endif
