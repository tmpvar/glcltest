#define ival float2
#define ivec2 float4

// 1d interval arithmetic

static ival isqr(ival i) {
  return i[0] >= 0.0 ? (ival)(i[0] * i[0], i[1] * i[1]) :
         i[1] < 0.0 ? (ival)(i[1] * i[1], i[0] * i[0]) :
          (ival)(0.0, max(i[0] * i[0], i[1] * i[1]));
}

static ival ilength(ival i) {
  ival pa = isqr(i[0]);
  ival pb = isqr(i[1]);

  return (ival)(
    sqrt(pa[0] + pb[0]),
    sqrt(pa[1] + pb[1])
  );
}

static ival isub(ival i, float v) {
  return (ival)(i[0] - v, i[1] - v);
}

static ival imin(ival i, ival j) {
  return (ival)(
    min(i[0], j[0]),
    min(i[1], j[1])
  );
}

static bool icontains(ival i, float v) {
  return (v >= i[0]) && (v <= i[1]);
}

/// interval vector
static ivec2 iv2sub(ivec2 i, float2 p) {
  return (ivec2)(
    i[0] - p[0],
    i[1] - p[0],
    i[2] - p[1],
    i[3] - p[1]
  );
}

static ival iv2length(ivec2 i) {
  ival pa = isqr(i[0] + i[1]);
  ival pb = isqr(i[2] + i[3]);

  return (ival)(
    sqrt(pa[0] + pb[0]),
    sqrt(pa[1] + pb[1])
  );
}

// primitives

static ival icircle(ivec2 p, float r) {
  return isub(iv2length(p), r);
}

static float circle(float2 p, float r) {
  return length(p) - r;
}

__kernel void hello(__write_only image2d_t image, constant float *shapes, uint shape_count) {
  int2 pos = (int2)(get_global_id(0), get_global_id(1));
  float2 fpos = convert_float2(pos);

  ivec2 b = (ivec2)(fpos[0], fpos[0], fpos[1], fpos[1]);

  ival d = (ival)(100, -100);
  float2 shape_pos;
  for (uint i = 0; i<=shape_count; i++) {
    uint offset = i*4;
    switch (convert_uint(shapes[offset])) {
      // circle
      case 1:
        shape_pos = (float2)(shapes[offset+1], shapes[offset+2]);

        d = imin(d, icircle(
          iv2sub(b, shape_pos),
          shapes[offset+3]
        ));
      break;
    }
  }

  write_imagef(image, pos, d[0] < 0 ? (float4)(0.0) : (float4)(1.0));
}
