#ifndef __CL_ERROR_H__
#define __CL_ERROR_H__

  #ifdef __APPLE__
    #include <OpenCL/opencl.h>
    #include <GLFW/glfw3.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenCL/opencl.h>
    #include <OpenCL/cl_gl.h>
  #else
    #include <CL/cl.h>
  #endif

  void cl_error(cl_int err) {
    switch(err) {
      case CL_DEVICE_NOT_FOUND:
        printf("ERROR CL_DEVICE_NOT_FOUND:\n  clGetDeviceIDs  if no OpenCL devices that matched device_type were found.\n");
      break;
      case CL_DEVICE_NOT_AVAILABLE:
        printf("ERROR CL_DEVICE_NOT_AVAILABLE:\n  clCreateContext if a device in devices is currently not available even though the device was returned by clGetDeviceIDs.\n");
      break;
      case CL_COMPILER_NOT_AVAILABLE:
        printf("ERROR CL_COMPILER_NOT_AVAILABLE:\n  clBuildProgram  if program is created with clCreateProgramWithSource and a compiler is not available i.e. CL_DEVICE_COMPILER_AVAILABLE specified in the table of OpenCL Device Queries for clGetDeviceInfo is set to CL_FALSE.\n");
      break;
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        printf("ERROR CL_MEM_OBJECT_ALLOCATION_FAILURE:\n  if there is a failure to allocate memory for buffer object.\n");
      break;
      case CL_OUT_OF_RESOURCES:
        printf("ERROR CL_OUT_OF_RESOURCES:\n  if there is a failure to allocate resources required by the OpenCL implementation on the device.\n");
      break;
      case CL_OUT_OF_HOST_MEMORY:
        printf("ERROR CL_OUT_OF_HOST_MEMORY:\n  if there is a failure to allocate resources required by the OpenCL implementation on the host.\n");
      break;
      case CL_PROFILING_INFO_NOT_AVAILABLE:
        printf("ERROR CL_PROFILING_INFO_NOT_AVAILABLE:\n  clGetEventProfilingInfo if the CL_QUEUE_PROFILING_ENABLE flag is not set for the command-queue, if the execution status of the command identified by event is not CL_COMPLETE or if event is a user event object.\n");
      break;
      case CL_MEM_COPY_OVERLAP:
        printf("ERROR CL_MEM_COPY_OVERLAP:\n  clEnqueueCopyBuffer, clEnqueueCopyBufferRect, clEnqueueCopyImage  if src_buffer and dst_buffer are the same buffer or subbuffer object and the source and destination regions overlap or if src_buffer and dst_buffer are different sub-buffers of the same associated buffer object and they overlap. The regions overlap if src_offset ≤ to dst_offset ≤ to src_offset + size – 1, or if dst_offset ≤ to src_offset ≤ to dst_offset + size – 1.\n");
      break;
      case CL_IMAGE_FORMAT_MISMATCH:
        printf("ERROR CL_IMAGE_FORMAT_MISMATCH:\n  clEnqueueCopyImage  if src_image and dst_image do not use the same image format.\n");
      break;
      case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        printf("ERROR CL_IMAGE_FORMAT_NOT_SUPPORTED:\n  clCreateImage if the image_format is not supported.\n");
      break;
      case CL_BUILD_PROGRAM_FAILURE:
        printf("ERROR CL_BUILD_PROGRAM_FAILURE:\n  clBuildProgram  if there is a failure to build the program executable. This error will be returned if clBuildProgram does not return until the build has completed.\n");
      break;
      case CL_MAP_FAILURE:
        printf("ERROR CL_MAP_FAILURE:\n  clEnqueueMapBuffer, clEnqueueMapImage  if there is a failure to map the requested region into the host address space. This error cannot occur for image objects created with CL_MEM_USE_HOST_PTR or CL_MEM_ALLOC_HOST_PTR.\n");
      break;
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        printf("ERROR CL_MISALIGNED_SUB_BUFFER_OFFSET:\n  if a sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for device associated with queue.\n");
      break;
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
        printf("ERROR CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:\n  if the execution status of any of the events in event_list is a negative integer value.\n");
      break;
      case CL_COMPILE_PROGRAM_FAILURE:
        printf("ERROR CL_COMPILE_PROGRAM_FAILURE:\n  clCompileProgram  if there is a failure to compile the program source. This error will be returned if clCompileProgram does not return until the compile has completed.\n");
      break;
      case CL_LINKER_NOT_AVAILABLE:
        printf("ERROR CL_LINKER_NOT_AVAILABLE:\n  clLinkProgram if a linker is not available i.e. CL_DEVICE_LINKER_AVAILABLE specified in the table of allowed values for param_name for clGetDeviceInfo is set to CL_FALSE.\n");
      break;
      case CL_LINK_PROGRAM_FAILURE:
        printf("ERROR CL_LINK_PROGRAM_FAILURE:\n  clLinkProgram if there is a failure to link the compiled binaries and/or libraries.\n");
      break;
      case CL_DEVICE_PARTITION_FAILED:
        printf("ERROR CL_DEVICE_PARTITION_FAILED:\n  clCreateSubDevices   if the partition name is supported by the implementation but in_device could not be further partitioned.\n");
      break;
      case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
        printf("ERROR CL_KERNEL_ARG_INFO_NOT_AVAILABLE:\n  clGetKernelArgInfo  if the argument information is not available for kernel.\n");
      break;
      case CL_INVALID_VALUE:
        printf("ERROR CL_INVALID_VALUE:\n  clGetDeviceIDs, clCreateContext This depends on the function: two or more coupled parameters had errors.\n");
      break;
      case CL_INVALID_DEVICE_TYPE:
        printf("ERROR CL_INVALID_DEVICE_TYPE:\n  clGetDeviceIDs  if an invalid device_type is given\n");
      break;
      case CL_INVALID_PLATFORM:
        printf("ERROR CL_INVALID_PLATFORM:\n  clGetDeviceIDs  if an invalid platform was given\n");
      break;
      case CL_INVALID_DEVICE:
        printf("ERROR CL_INVALID_DEVICE:\n  clCreateContext, clBuildProgram if devices contains an invalid device or are not associated with the specified platform.\n");
      break;
      case CL_INVALID_CONTEXT:
        printf("ERROR CL_INVALID_CONTEXT:\n  if context is not a valid context.\n");
      break;
      case CL_INVALID_QUEUE_PROPERTIES:
        printf("ERROR CL_INVALID_QUEUE_PROPERTIES:\n  clCreateCommandQueue  if specified command-queue-properties are valid but are not supported by the device.\n");
      break;
      case CL_INVALID_COMMAND_QUEUE:
        printf("ERROR CL_INVALID_COMMAND_QUEUE:\n  if command_queue is not a valid command-queue.\n");
      break;
      case CL_INVALID_HOST_PTR:
        printf("ERROR CL_INVALID_HOST_PTR:\n  clCreateImage, clCreateBuffer This flag is valid only if host_ptr is not NULL. If specified, it indicates that the application wants the OpenCL implementation to allocate memory for the memory object and copy the data from memory referenced by host_ptr.CL_MEM_COPY_HOST_PTR and CL_MEM_USE_HOST_PTR are mutually exclusive.CL_MEM_COPY_HOST_PTR can be used with CL_MEM_ALLOC_HOST_PTR to initialize the contents of the cl_mem object allocated using host-accessible (e.g. PCIe) memory.\n");
      break;
      case CL_INVALID_MEM_OBJECT:
        printf("ERROR CL_INVALID_MEM_OBJECT:\n  if memobj is not a valid OpenCL memory object.\n");
      break;
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        printf("ERROR CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:\n  if the OpenGL/DirectX texture internal format does not map to a supported OpenCL image format.\n");
      break;
      case CL_INVALID_IMAGE_SIZE:
        printf("ERROR CL_INVALID_IMAGE_SIZE:\n  if an image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with queue.\n");
      break;
      case CL_INVALID_SAMPLER:
        printf("ERROR CL_INVALID_SAMPLER:\n  clGetSamplerInfo, clReleaseSampler, clRetainSampler, clSetKernelArg if sampler is not a valid sampler object.\n");
      break;
      case CL_INVALID_BINARY:
        printf("ERROR CL_INVALID_BINARY:\n  clCreateProgramWithBinary, clBuildProgram The provided binary is unfit for the selected device. if program is created with clCreateProgramWithBinary and devices listed in device_list do not have a valid program binary loaded.\n");
      break;
      case CL_INVALID_BUILD_OPTIONS:
        printf("ERROR CL_INVALID_BUILD_OPTIONS:\n  clBuildProgram  if the build options specified by options are invalid.\n");
      break;
      case CL_INVALID_PROGRAM:
        printf("ERROR CL_INVALID_PROGRAM:\n  if program is a not a valid program object.\n");
      break;
      case CL_INVALID_PROGRAM_EXECUTABLE:
        printf("ERROR CL_INVALID_PROGRAM_EXECUTABLE:\n  if there is no successfully built program executable available for device associated with command_queue.\n");
      break;
      case CL_INVALID_KERNEL_NAME:
        printf("ERROR CL_INVALID_KERNEL_NAME:\n  clCreateKernel  if kernel_name is not found in program.\n");
      break;
      case CL_INVALID_KERNEL_DEFINITION:
        printf("ERROR CL_INVALID_KERNEL_DEFINITION:\n  clCreateKernel  if the function definition for __kernel function given by kernel_name such as the number of arguments, the argument types are not the same for all devices for which the program executable has been built.\n");
      break;
      case CL_INVALID_KERNEL:
        printf("ERROR CL_INVALID_KERNEL:\n  if kernel is not a valid kernel object.\n");
      break;
      case CL_INVALID_ARG_INDEX:
        printf("ERROR CL_INVALID_ARG_INDEX:\n  clSetKernelArg, clGetKernelArgInfo  if arg_index is not a valid argument index.\n");
      break;
      case CL_INVALID_ARG_VALUE:
        printf("ERROR CL_INVALID_ARG_VALUE:\n  clSetKernelArg, clGetKernelArgInfo  if arg_value specified is not a valid value.\n");
      break;
      case CL_INVALID_ARG_SIZE:
        printf("ERROR CL_INVALID_ARG_SIZE:\n  clSetKernelArg  if arg_size does not match the size of the data type for an argument that is not a memory object or if the argument is a memory object and arg_size != sizeof(cl_mem) or if arg_size is zero and the argument is declared with the __local qualifier or if the argument is a sampler and arg_size != sizeof(cl_sampler).\n");
      break;
      case CL_INVALID_KERNEL_ARGS:
        printf("ERROR CL_INVALID_KERNEL_ARGS:\n  if the kernel argument values have not been specified.\n");
      break;
      case CL_INVALID_WORK_DIMENSION:
        printf("ERROR CL_INVALID_WORK_DIMENSION:\n  if work_dim is not a valid value (i.e. a value between 1 and 3).\n");
      break;
      case CL_INVALID_WORK_GROUP_SIZE:
        printf("ERROR CL_INVALID_WORK_GROUP_SIZE:\n  if local_work_size is specified and number of work-items specified by global_work_size is not evenly divisable by size of work-group given by local_work_size or does not match the work-group size specified for kernel using the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier in program source.if local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] *… local_work_size[work_dim – 1] is greater than the value specified by CL_DEVICE_MAX_WORK_GROUP_SIZE in the table of OpenCL Device Queries for clGetDeviceInfo.if local_work_size is NULL and the __attribute__ ((reqd_work_group_size(X, Y, Z))) qualifier is used to declare the work-group size for kernel in the program source.\n");
      break;
      case CL_INVALID_WORK_ITEM_SIZE:
        printf("ERROR CL_INVALID_WORK_ITEM_SIZE:\n  if the number of work-items specified in any of local_work_size[0], … local_work_size[work_dim – 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], …. CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim – 1].\n");
      break;
      case CL_INVALID_GLOBAL_OFFSET:
        printf("ERROR CL_INVALID_GLOBAL_OFFSET:\n  if the value specified in global_work_size + the corresponding values in global_work_offset for any dimensions is greater than the sizeof(size_t) for the device on which the kernel execution will be enqueued.\n");
      break;
      case CL_INVALID_EVENT_WAIT_LIST:
        printf("ERROR CL_INVALID_EVENT_WAIT_LIST:\n  if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.\n");
      break;
      case CL_INVALID_EVENT:
        printf("ERROR CL_INVALID_EVENT:\n  if event objects specified in event_list are not valid event objects.\n");
      break;
      case CL_INVALID_OPERATION:
        printf("ERROR CL_INVALID_OPERATION:\n  if interoperability is specified by setting CL_CONTEXT_ADAPTER_D3D9_KHR, CL_CONTEXT_ADAPTER_D3D9EX_KHR or CL_CONTEXT_ADAPTER_DXVA_KHR to a non-NULL value, and interoperability with another graphics API is also specified. (only if the cl_khr_dx9_media_sharing extension is supported).\n");
      break;
      case CL_INVALID_GL_OBJECT:
        printf("ERROR CL_INVALID_GL_OBJECT:\n  if texture is not a GL texture object whose type matches texture_target, if the specified miplevel of texture is not defined, or if the width or height of the specified miplevel is zero.\n");
      break;
      case CL_INVALID_BUFFER_SIZE:
        printf("ERROR CL_INVALID_BUFFER_SIZE:\n  clCreateBuffer, clCreateSubBuffer if size is 0.Implementations may return CL_INVALID_BUFFER_SIZE if size is greater than the CL_DEVICE_MAX_MEM_ALLOC_SIZE value specified in the table of allowed values for param_name for clGetDeviceInfo for all devices in context.\n");
      break;
      case CL_INVALID_MIP_LEVEL:
        printf("ERROR CL_INVALID_MIP_LEVEL:\n  OpenGL-functions  if miplevel is greater than zero and the OpenGL implementation does not support creating from non-zero mipmap levels.\n");
      break;
      case CL_INVALID_GLOBAL_WORK_SIZE:
        printf("ERROR CL_INVALID_GLOBAL_WORK_SIZE:\n  if global_work_size is NULL, or if any of the values specified in global_work_size[0], …global_work_size [work_dim – 1] are 0 or exceed the range given by the sizeof(size_t) for the device on which the kernel execution will be enqueued.\n");
      break;
      case CL_INVALID_PROPERTY:
        printf("ERROR CL_INVALID_PROPERTY:\n  clCreateContext Vague error, depends on the function\n");
      break;
      case CL_INVALID_IMAGE_DESCRIPTOR:
        printf("ERROR CL_INVALID_IMAGE_DESCRIPTOR:\n  clCreateImage if values specified in image_desc are not valid or if image_desc is NULL.\n");
      break;
      case CL_INVALID_COMPILER_OPTIONS:
        printf("ERROR CL_INVALID_COMPILER_OPTIONS:\n  clCompileProgram  if the compiler options specified by options are invalid.\n");
      break;
      case CL_INVALID_LINKER_OPTIONS:
        printf("ERROR CL_INVALID_LINKER_OPTIONS:\n  clLinkProgram if the linker options specified by options are invalid.\n");
      break;
      case CL_INVALID_DEVICE_PARTITION_COUNT:
        printf("ERROR CL_INVALID_DEVICE_PARTITION_COUNT:\n  clCreateSubDevices  if the partition name specified in properties is CL_DEVICE_PARTITION_ BY_COUNTS and the number of sub-devices requested exceeds CL_DEVICE_PARTITION_ MAX_SUB_DEVICES or the total number of compute units requested exceeds CL_DEVICE_PARTITION_ MAX_COMPUTE_UNITS for in_device, or the number of compute units requested for one or more sub-devices is less than zero or the number of sub-devices requested exceeds CL_DEVICE_PARTITION_ MAX_COMPUTE_UNITS for in_device.\n");
      break;
    }
  }
#endif
