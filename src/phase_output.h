#ifndef __phase_output__
#define __phase_output__
  #include <stdio.h>
  // #include "scene.h"

  static const struct
  {
    float x, y;
  } phase_output_vertices[6] =
  {
    { -1.0f,  1.0f },
    {  1.0f,  1.0f },
    { -1.0f, -1.0f },
    {  1.0f, -1.0f },
    { -1.0f, -1.0f },
    {  1.0f,  1.0f },
  };

  static const char* phase_output_vertex_shader_text =
  "#version 330 core\n"
  "in vec2 vPos;\n"
  "out vec2 pos;\n"
  "void main() {\n"
  "  pos = vPos * 0.5 + 0.5;\n"
  "  gl_Position = vec4(vPos, 0.0, 1.0);\n"
  "}\n";

  static const char* phase_output_fragment_shader_text =
  "#version 330 core\n"
  "uniform sampler2D tex;\n"
  "in vec2 pos;\n"
  "out vec4 fragColor;\n"
  "void main() {\n"
  "  fragColor = texture(tex, pos.xy);\n"
  "}\n";

  static struct {
    GLuint vertex_buffer;
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    GLuint vao;
    int has_texture;
  } phase_output;

  static GLuint fb_texture = 0;


  void phase_output_init() {

    phase_output.has_texture = 0;

    phase_output.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(phase_output.vertex_shader, 1, &phase_output_vertex_shader_text, NULL);
    gl_error();
    glCompileShader(phase_output.vertex_shader);
    gl_shader_log(phase_output.vertex_shader);
    gl_error();

    phase_output.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(phase_output.fragment_shader, 1, &phase_output_fragment_shader_text, NULL);
    glCompileShader(phase_output.fragment_shader);
    gl_shader_log(phase_output.fragment_shader);

    phase_output.program = glCreateProgram();
    glAttachShader(phase_output.program, phase_output.vertex_shader);
    glAttachShader(phase_output.program, phase_output.fragment_shader);
    glLinkProgram(phase_output.program);
    gl_error();

    glGenVertexArrays (1, &phase_output.vao);
    glBindVertexArray (phase_output.vao);

    // Create fullscreen quad
    glGenBuffers(1, &phase_output.vertex_buffer);
    gl_error();

    glBindBuffer(GL_ARRAY_BUFFER, phase_output.vertex_buffer);
    glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(phase_output_vertices),
      phase_output_vertices,
      GL_STATIC_DRAW
    );
    gl_error();

  }

  GLuint phase_output_get_fb() {
    return fb_texture;
  }

  int phase_output_resize(int width, int height) {
    int had_texture = phase_output.has_texture;

    if (!had_texture) {
      glGenTextures(1, &fb_texture);
      phase_output.has_texture = 1;
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
      width,
      height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      0
    );

    gl_error();

    return had_texture;
  }

  void phase_output_render() {
    glUseProgram(phase_output.program);
    glBindVertexArray (phase_output.vao);
    glBindBuffer(GL_ARRAY_BUFFER, phase_output.vertex_buffer);

    GLint vpos_location = glGetAttribLocation(phase_output.program, "vPos");
    GLint texture_location = glGetUniformLocation(phase_output.program, "tex");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glUniform1i(texture_location, 0);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(
      vpos_location,
      2,
      GL_FLOAT,
      GL_FALSE,
      sizeof(float) * 2,
      (void*) 0
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);
    gl_error();
  }
#endif
