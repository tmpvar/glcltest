#include "clu.h"

typedef struct {
  cl_device_id device;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  cl_kernel kernel;
} glcl_job_t;

int compute_init(glcl_job_t *job) {
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
  size_t ret_size;
  int i;

  job->context = clCreateContext(properties, 0, 0, 0, 0, 0);
  if (!job->context) {
    printf("clCreateContext failed\n");
  }

  // compute the number of devices
  ret = clGetContextInfo(job->context, CL_CONTEXT_DEVICES, 0, NULL, &ret_size);
  if(!ret_size || ret != CL_SUCCESS) {
    printf("clGetDeviceInfo failed\n");
  }
  ret_num_devices = ret_size/sizeof(cl_device_id);

  // get the device list
  cl_device_id devices[ret_num_devices];
  ret = clGetContextInfo(job->context, CL_CONTEXT_DEVICES, ret_size, devices, &ret_size);
  if(ret) {
    printf("clGetContextInfo failed\n");
  }

  // get the GPU device and queue
  for(i=0; i<ret_num_devices; i++) {
    cl_int deviceType, error;

    ret = clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, &ret_size);
    if(ret) {
      printf("clGetDeviceInfo failed\n");
    }

    if(deviceType == CL_DEVICE_TYPE_GPU) {
      job->device = devices[i];
      job->command_queue = clCreateCommandQueue(job->context, job->device, 0, &error);
      break;
    }
  }

  cl_print_device_info(job->device);

  cl_program_from_fs(job->device, job->context, "../src/kernel.cl", &job->program);

  job->kernel = clCreateKernel(job->program, "block", &ret);
  CL_CHECK_ERROR(ret);

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
