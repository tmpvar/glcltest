__kernel void hello(__write_only image2d_t image) {
  int2 pos = (int2)(get_global_id(0), get_global_id(1));
  float2 fpos = convert_float2(pos);
  float2 dim = convert_float2(get_image_dim(image));

  float2 center = dim/(float2)(2.0);
  float d = distance(fpos, center);

  // TODO: why is this not centered?
  write_imagef(image, pos, (float4)(d/200));
}
