__kernel void hello(__write_only image2d_t image) {
  int x = get_global_id(0);
  int y = get_global_id(1);

  write_imagef(image, (int2)(x, y), (float4)(1.0, 0.0, 1.0, 1.0));
}
