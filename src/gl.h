#include <GLFW/glfw3.h>

GLint gl_error() {
  GLint error = glGetError();

  switch (error) {
    case GL_INVALID_ENUM:
      printf("error (%i): GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n", error);
    break;

    case GL_INVALID_VALUE:
      printf("error (%i): GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n", error);
    break;

    case GL_INVALID_OPERATION:
      printf("error (%i): GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n", error);
    break;

    case GL_STACK_OVERFLOW:
      printf("error (%i): GL_STACK_OVERFLOW: This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.\n", error);
    break;

    case GL_STACK_UNDERFLOW:
      printf("error (%i): GL_STACK_UNDERFLOW: This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.\n", error);
    break;

    case GL_OUT_OF_MEMORY:
      printf("error (%i): GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n", error);
    break;
  }
  return error;
}

void gl_shader_log(GLuint shader) {
  GLint error = glGetError();
  GLint l, m;
  if (error) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &m);
    char s[m];
    glGetShaderInfoLog(shader, m, &l, s);
    printf("shader log:\n%s\n", s);
  }
}
