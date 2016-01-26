float circle(float2 fpos, float r);

float circle(float2 p, float r) {
  return length(p) - r;
}

__kernel void hello(__write_only image2d_t image, constant float *shapes, uint shape_count) {
  int2 pos = (int2)(get_global_id(0), get_global_id(1));
  float2 fpos = convert_float2(pos);

  float d = 100;
  for (uint i = 0; i<=shape_count; i++) {
    uint offset = i*4;
    switch (convert_uint(shapes[offset])) {
      // circle
      case 1:
        d = fmin(d, circle(
          fpos - (float2)(shapes[offset+1], shapes[offset+2]),
          shapes[offset+3]
        ));

      break;
    }
  }

  write_imagef(image, pos, (float4)(d));
}
