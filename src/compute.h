#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
  #include <GLFW/glfw3.h>
  #include <OpenGL/OpenGL.h>
  #include <OpenCL/opencl.h>
  #include <OpenCL/cl_gl.h>
#else
  #include <CL/cl.h>
#endif

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

typedef struct {
  cl_device_id device;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  cl_kernel kernel;
} glcl_job_t;


int compute_init(glcl_job_t *job) {
  char fileName[] = "../src/kernel.cl";

  FILE *fp = fopen(fileName, "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }
  char *source_str = (char*)malloc(MAX_SOURCE_SIZE);
  size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  #ifdef linux
    cl_context_properties properties[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
      0
    };
  #elif defined _WIN32
    cl_context_properties properties[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
      CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
      0
    };
  #elif defined TARGET_OS_MAC
    CGLContextObj glContext = CGLGetCurrentContext();
    CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
    cl_context_properties properties[] = {
      CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
      (cl_context_properties)shareGroup,
      0
    };
  #endif

  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;

  clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &job->device, &ret_num_devices);

  // TODO: error handling and device selection
  job->context = clCreateContext(properties, 1, &job->device, NULL, NULL, &ret);

  /* Create Command Queue */
  job->command_queue = clCreateCommandQueue(job->context, job->device, 0, &ret);

  /* Create Kernel Program from the source */
  job->program = clCreateProgramWithSource(
    job->context,
    1,
    (const char **)&source_str,
    (const size_t *)&source_size,
    &ret
  );

  // this will not be async if the `pfn_notify` arg is not a callback
  clBuildProgram(job->program, 1, &job->device, NULL, NULL, NULL);
  free(source_str);


  job->kernel = clCreateKernel(job->program, "hello", &ret);

  return 1;
}

void compute_destroy(glcl_job_t *job) {
  /* Finalization */
  clFlush(job->command_queue);
  clFinish(job->command_queue);
  clReleaseKernel(job->kernel);
  clReleaseProgram(job->program);
  clReleaseCommandQueue(job->command_queue);
  clReleaseContext(job->context);
}
