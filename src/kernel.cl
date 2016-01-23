float circle(float2 fpos, float r);

float circle(float2 fpos, float r) {
  return length(fpos) - r;
}

__kernel void hello(__write_only image2d_t image) {
  int2 pos = (int2)(get_global_id(0), get_global_id(1));
  float2 fpos = convert_float2(pos);
  float2 dim = convert_float2(get_image_dim(image));

  float2 center = dim/(float2)(2.0);
  float d = circle(fpos - center, 60.0);;

  write_imagef(image, pos, (float4)(d));
}
