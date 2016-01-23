__kernel void hello(__global uint8* fb) {
  int base = get_global_id(0);
  fb[base+0] = 0;
  fb[base+1] = 0;
  fb[base+2] = 0;
  fb[base+3] = 0;
}
