#ifndef __ORBIT_CAMERA__
#define __ORBIT_CAMERA__

  #include "vec.h"

  struct {
    quat rotation;
    vec3 center, v3scratch;
    float distance;
    mat4 scratch0, scratch1;
  } orbit_camera;

  static void orbit_camera_lookat(const vec3 eye, const vec3 center, const vec3 up) {
    mat4_look_at(orbit_camera.scratch0, eye, center, up);
    quat_from_mat4(orbit_camera.rotation, orbit_camera.scratch0);
    quat_norm(orbit_camera.rotation, orbit_camera.rotation);
    orbit_camera.center = center;
    orbit_camera.distance = vec3_distance(eye, center);
  }

  static void orbit_camera_init(const vec3 eye, const vec3 center, const vec3 up) {
    quat_identity(orbit_camera.rotation);
    orbit_camera_lookat(eye, center, up);
  }

  static void orbit_camera_rotate(const float sx, const float sy, const float ex, const float ey) {
    const vec3 vs = vec3_create(sx, sy, 0.0f);
    const vec3 ve = vec3_create(ex, ey, 0.0f);
    quat s, e;

    quat_from_vec3(s, vs);
    quat_from_vec3(e, ve);

    quat_invert(e, e);
    quat_mul(s, s, e);

    if(vec4_len(s) < 1e-6) {
      printf("MISS %f\n", vec4_len(s));
      return;
    }

    quat_mul(orbit_camera.rotation, orbit_camera.rotation, s);
    quat_norm(orbit_camera.rotation, orbit_camera.rotation);
  }

  static vec3 orbit_camera_unproject(const vec3 vec, const vec4 viewport, const mat4 inv) {
    float viewX = viewport[0];
    float viewY = viewport[1];
    float viewWidth = viewport[2];
    float viewHeight = viewport[3];

    float x = vec[0];
    float y = vec[1];
    float z = vec[2];

    x = x - viewX;
    y = viewHeight - y - 1;
    y = y - viewY;

    vec3 r = vec3_create(
      (2 * x) / viewWidth - 1,
      (2 * y) / viewHeight - 1,
      2 * z - 1
    );

    return vec3_transform(r, inv);
  }

  static void orbit_camera_view(mat4 view) {
    quat q;
    vec3 s = vec3_create(0.0, 0.0, -orbit_camera.distance );
    quat_conj(q, orbit_camera.rotation);
    mat4_from_rotation_translation(view, q, s);
    mat4_translate(view, vec3_negate(orbit_camera.center));
  }
#endif
