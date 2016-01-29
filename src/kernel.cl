#define ival float2
#define ivec2 float4

// 1d interval arithmetic

static ival isqr(ival i) {
  ival r = i*i;
  return (ival)(min(r[0], r[1]), max(r[0], r[1]));
}

static ival isqrt(ival i) {
  return (ival)(
    sqrt(i[0]),
    sqrt(i[1])
  );
}

static ival ilength(ival i) {
  return isqrt(i*i);
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
  return isqrt(
    isqr((ival)(i[0], i[1])) +
    isqr((ival)(i[2], i[3]))
  );
}

// primitives

static ival icircle(ivec2 p, float r) {
  return isub(iv2length(p), r);
}

__kernel void block(__write_only image2d_t image, global float *shapes, uint shape_count, int block_width, int block_height) {
  int2 pos = (int2)(get_global_id(0) * block_width, get_global_id(1) * block_height);
  float2 fpos = convert_float2(pos);

  ivec2 b = (ivec2)(
    fpos[0],
    fpos[0] + block_width,
    fpos[1],
    fpos[1] + block_height
  );

  ival d = (ival)(100, 100);
  float2 shape_pos;
  float crossings = 0;
  ival r;

  for (uint i = 0; i<=shape_count; i++) {
    uint offset = i*4;
    switch (convert_uint(shapes[offset])) {
      // circle
      case 1:
        shape_pos = (float2)(shapes[offset+1], shapes[offset+2]);
        r = icircle(
          iv2sub(b, shape_pos),
          shapes[offset+3]
        );
        crossings += icontains(r, 0.0) ? 1 : 0;
        d = imin(d, r);
      break;
    }
  }

  for (int x=pos[0]; x<pos[0] + block_width; x++) {
    for (int y=pos[1]; y<pos[1] + block_height; y++) {
      if (icontains(d, 0.0)) {
        write_imagef(image, (int2)(x, y), (float4)(1.0));
      }
    }
  }

  if (d[0] > 0) {
    write_imagef(image, pos, (float4)(1.0, 0.0, 1.0, 1.0));
  } else if (d[1] < 0) {
    write_imagef(image, pos, (float4)(0.0, 0.0, 1.0, 1.0));
  }
}

__kernel void hello(__write_only image2d_t image, constant float *shapes, uint shape_count, uint block_width, uint block_height) {
  int2 pos = (int2)(get_global_id(0), get_global_id(1));
  float2 fpos = convert_float2(pos);

  ivec2 b = (ivec2)(fpos[0], fpos[0], fpos[1], fpos[1]);

  ival d = (ival)(100, -100);
  float2 shape_pos;
  float crossings = 0;
  ival r;
  for (uint i = 0; i<=shape_count; i++) {
    uint offset = i*4;
    switch (convert_uint(shapes[offset])) {
      // circle
      case 1:
        shape_pos = (float2)(shapes[offset+1], shapes[offset+2]);
        r = icircle(
          iv2sub(b, shape_pos),
          shapes[offset+3]
        );
        crossings += r[0] < 0 ? 1 : 0;
        d = imin(d, r);
      break;
    }
  }

  write_imagef(image, pos, (float4)((float)(crossings) / 5.0));
}
