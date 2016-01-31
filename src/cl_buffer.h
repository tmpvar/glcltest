#include "clu.h"
#include <iostream>

using namespace std;

namespace cl {
  template <class T> class Buffer {
    public:
    cl_mem mem;
    cl_context context;
    size_t length;
    size_t element_size;
    size_t byte_length;

    Buffer<T>(cl_context context, size_t length) {
      this->init(context, CL_MEM_READ_WRITE, length);
    }

    Buffer<T>(cl_context context, cl_mem_flags flags, size_t length) {
      this->init(context, flags, length);
    }

    Buffer<T>(cl_context context, cl_mem_flags flags, size_t length, T *data) {
      this->init(context, flags | CL_MEM_COPY_HOST_PTR, length, data);
    }

    void init(cl_context context, cl_mem_flags flags, size_t length, T *data = NULL) {
      this->context = context;
      this->length = length;
      this->element_size = sizeof(T);
      this->byte_length = this->element_size * length;

      cl_int ret;
      this->mem = clCreateBuffer(
        this->context,
        flags,
        this->byte_length,
        data,
        &ret
      );

      CL_CHECK_ERROR(ret);
    }

    void print(cl_command_queue queue) {

      T *buf = (T *)clEnqueueMapBuffer(
        queue,
        this->mem,
        CL_TRUE,
        CL_MAP_READ,
        0,
        this->byte_length,
        0,
        NULL,
        NULL,
        NULL
      );

      for (size_t i = 0; i < this->length; i++) {
        cout << buf[i] << " ";
      }

      cout << endl;
      cout.flush();

      cl_int ret;

      ret = clEnqueueUnmapMemObject(queue, this->mem, buf, 0, NULL, NULL);
      clFinish(queue);
    }

    void bind(cl_kernel kernel, cl_uint index) {
      CL_CHECK_ERROR(
        clSetKernelArg(kernel, index, sizeof(cl_mem), &this->mem)
      );
    }
  };

  // TODO: print/slice/etc

  typedef Buffer<cl_uint> UIntArray;
  typedef Buffer<cl_int> IntArray;
  typedef Buffer<cl_float> FloatArray;
}
