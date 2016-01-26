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
#define CL_CHECK_ERROR(err) do{if (err) {printf("FATAL ERROR %d at " __FILE__ ":%d\n",err,__LINE__); exit(1); } } while(0)

typedef struct {
  cl_device_id device;
  cl_context context;
  cl_command_queue command_queue;
  cl_program program;
  cl_kernel kernel;
} glcl_job_t;

void cl_print_program_info(glcl_job_t *job) {
  char* log = NULL;
  size_t r = 0;
  clGetProgramBuildInfo(job->program, job->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &r);
  if (r > 1) {
    log = (char*)malloc( r + 1 );
    cl_int err = clGetProgramBuildInfo(job->program, job->device, CL_PROGRAM_BUILD_LOG, r, log, NULL);
    printf("kernel build log: %s\n", log);
    free( log );
  }
}

void cl_print_device_info_string(cl_device_id device, cl_int type, const char *fmt) {
  size_t len;
  clGetDeviceInfo(device, type, 0, NULL, &len);
  char *value = (char*) malloc(len);
  clGetDeviceInfo(device, type, len, value, NULL);
  printf(fmt, value);
  printf("\n");
  free(value);
}

void cl_print_device_extensions(cl_device_id device) {
  size_t len, last = 0;
  clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &len);
  char *value = (char*) malloc(len);
  clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, len, value, NULL);

  printf("  extensions:\n");

  for (size_t i=0; i<len; i++) {
    if (value[i] == ' ') {
      value[i] = 0;
      printf("    %s\n", &value[last]);
      last = i+1;
    }
  }
  free(value);
}


void cl_print_device_info(cl_device_id d) {
  cl_print_device_info_string(d, CL_DEVICE_NAME, "%s");

  cl_print_device_extensions(d);

  size_t tmp;
  cl_uint tuint;
  cl_ulong tulong;


  clGetDeviceInfo(d, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(tuint), &tuint, NULL);
  printf("  compute units: %u\n", tuint);

  clGetDeviceInfo(d, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(tuint), &tuint, NULL);
  printf("  max const args: %u\n", tuint);

  clGetDeviceInfo(d, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(tulong), &tulong, NULL);
  printf("  max const buffer size: %llu\n", tulong);

  clGetDeviceInfo(d, CL_DEVICE_MAX_SAMPLERS, sizeof(tulong), &tulong, NULL);
  printf("  max samplers: %llu\n", tulong);


  // workgroup stuff
  clGetDeviceInfo(d, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(tmp), &tmp, NULL);
  printf("  max workgroup size: %lu\n", tmp);

  clGetDeviceInfo(d, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(tuint), &tuint, NULL);
  printf("  max work item dimensions: %u\n", tuint);


  // 2d image max dimensions
  clGetDeviceInfo(d, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(tmp), &tmp, NULL);
  printf("  image2d max: %lu", tmp);
  clGetDeviceInfo(d, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(tmp), &tmp, NULL);
  printf(" x %lu\n", tmp);

  // 3d image max dimensions
  clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(tmp), &tmp, NULL);
  printf("  image3d max: %lu", tmp);
  clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(tmp), &tmp, NULL);
  printf(" x %lu", tmp);
  clGetDeviceInfo(d, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(tmp), &tmp, NULL);
  printf(" x %lu\n", tmp);
}

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

  /* Create Kernel Program from the source */
  job->program = clCreateProgramWithSource(
    job->context,
    1,
    (const char **)&source_str,
    (const size_t *)&source_size,
    &ret
  );

  // this will not be async if the `pfn_notify` arg is not a callback
  cl_int result = clBuildProgram(job->program, 1, &job->device, NULL, NULL, NULL);
  cl_print_program_info(job);
  result && printf("clBuildProgram: %i\n", result);
  free(source_str);


  job->kernel = clCreateKernel(job->program, "hello", &ret);
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
