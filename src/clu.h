#ifndef __CLU_H__
#define __CLU_H__
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

  void cl_print_program_info(cl_device_id device, cl_program program) {
    char* log = NULL;
    size_t r = 0;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &r);
    if (r > 1) {
      log = (char*)malloc( r + 1 );
      cl_int err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, r, log, NULL);
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

  void cl_print_max_allocation_size(cl_device_id d) {
    cl_ulong tulong;
    clGetDeviceInfo(d, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(tulong), &tulong, NULL);
    printf("  max allocation size: %llu\n", tulong);
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

    clGetDeviceInfo(d, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(tuint), &tuint, NULL);
    printf("  global cacheline size: %u\n", tuint);

    clGetDeviceInfo(d, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(tulong), &tulong, NULL);
    printf("  global cache size: %llu\n", tulong);

    clGetDeviceInfo(d, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(tulong), &tulong, NULL);
    printf("  max const buffer size: %llu\n", tulong);

    cl_print_max_allocation_size(d);

    clGetDeviceInfo(d, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(tulong), &tulong, NULL);
    printf("  local mem size: %llu\n", tulong);

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

  cl_int cl_program_from_string(cl_device_id device, cl_context context, const char* source_str, size_t source_size, cl_program *program) {
    cl_int ret;
    *program = clCreateProgramWithSource(
      context,
      1,
      &source_str,
      (const size_t *)&source_size,
      &ret
    );

    // this will not be async if the `pfn_notify` arg is not a callback
    cl_int result = clBuildProgram(*program, 1, &device, NULL, NULL, NULL);
    cl_print_program_info(device, *program);
    result && printf("clBuildProgram: %i\n", result);
    return result;
  }

  cl_int cl_program_from_fs(cl_device_id device, cl_context context, const char *file, cl_program *program) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
      fprintf(stderr, "Failed to load kernel.\n");
      exit(1);
    }
    char *source_str = (char*)malloc(MAX_SOURCE_SIZE);
    size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_int r = cl_program_from_string(device, context, source_str, source_size, program);
    free(source_str);
    return r;
  }
#endif
