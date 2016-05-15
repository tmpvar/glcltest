#ifndef LINMATH_H
#define LINMATH_H

#include <math.h>
#include <math.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

typedef float mat3[9];
typedef float mat4[16];
// typedef __m128 quat;// __attribute__((vector_size(32)));
typedef __m128 vec3;// __attribute__((vector_size(32)));
// typedef __m128 vec4;// __attribute__((vector_size(32)));
typedef float vec4[4], quat[4];
vec3 _zero = {0.0f, 0.0f, 0.0f, 0.0f};
vec3 _negative_zero = {-0.0f, -0.0f, -0.0f, -0.0f};

#define LINMATH_H_DEFINE_VEC(n) \
static inline void vec##n##_add(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] + b[i]; \
} \
static inline void vec##n##_sub(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] - b[i]; \
} \
static inline void vec##n##_scale(vec##n r, vec##n const v, float const s) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = v[i] * s; \
} \
static inline float vec##n##_mul_inner(vec##n const a, vec##n const b) \
{ \
  float p = 0.; \
  int i; \
  for(i=0; i<n; ++i) \
  p += b[i]*a[i]; \
  return p; \
} \
static inline float vec##n##_len(vec##n const v) \
{ \
  return sqrtf(vec##n##_mul_inner(v,v)); \
} \
static inline float vec##n##_distance(vec##n const a, vec##n const b) \
{ \
  vec##n scratch; \
  vec##n##_sub(scratch, a, b); \
  return vec##n##_len(scratch); \
}

// LINMATH_H_DEFINE_VEC(2)
// LINMATH_H_DEFINE_VEC(3)
LINMATH_H_DEFINE_VEC(4)

/* ok then */

static inline vec3 vec3_create(const float x, const float y, const float z) {
  return _mm_setr_ps(x, y, z, 0.0f);
}

static inline vec3 vec3f(const float x) {
  return _mm_set1_ps(x);
}

static inline float vec3_len(vec3 const v) {
  return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0x71)));
}

static inline float vec3_distance(vec3 const a, vec3 const b) {
  return vec3_len(a - b);
}

static inline vec3 vec3_max(const vec3 a, const vec3 b) {
  return _mm_max_ps(a, b);
}

static inline vec3 vec3_min(const vec3 a, const vec3 b) {
  return _mm_min_ps(a, b);
}

static inline vec3 vec3_copy(const vec3 a) {
  return vec3_create(a[0], a[1], a[2]);
}

static inline vec3 vec3_sign(const vec3 a) {
  return _mm_cvtepi32_ps((a < _zero) - (a > _zero));
}

static inline vec3 vec3_abs(const vec3 a) {
  return _mm_max_ps(_mm_sub_ps(_mm_setzero_ps(), a), a);
}

static inline vec3 vec3_mul_cross(const vec3 a, const vec3 b) {
  return vec3_create(
    a[1]*b[2] - a[2]*b[1],
    a[2]*b[0] - a[0]*b[2],
    a[0]*b[1] - a[1]*b[0]
  );
}

static inline vec3 vec3_transform(const vec3 a, const mat4 m) {
  float x = a[0];
  float y = a[1];
  float z = a[2];

  float w = m[3] * x + m[7] * y + m[11] * z + m[15];
  if (w == 0.0) {
    w = 1.0;
  }

  return vec3_create(
    (m[0] * x + m[4] * y + m[8] * z + m[12]) / w,
    (m[1] * x + m[5] * y + m[9] * z + m[13]) / w,
    (m[2] * x + m[6] * y + m[10] * z + m[14]) / w
  );
}

static inline vec3 vec3_reciprocal(vec3 const v) {
  return _mm_rcp_ps(v);
}

static inline vec3 vec3_negate(vec3 const v) {
  return -v;
}

static  vec3 vec3_norm(vec3 const v) {
  float len = vec3_len(v);
  if (len < 0.000000001) {
    return vec3_create(0, 0, 0);
  }
  float i = 1.0 / len;
  return v * vec3_create(i, i, i);
}

static inline void vec4_norm(vec4 r, vec4 const v) {
  float len = vec4_len(v);
  float k = 1.0 / len;
  vec4_scale(r, v, k);
}


// static inline void vec3_reflect(vec3 r, vec3 const v, vec3 const n)
// {
//   float p  = 2.f*vec3_mul_inner(v, n);
//   int i;
//   for(i=0;i<3;++i)
//   r[i] = v[i] - p*n[i];
// }

static inline void vec4_mul_cross(vec4 r, const vec4 a, const vec4 b)
{
  r[0] = a[1]*b[2] - a[2]*b[1];
  r[1] = a[2]*b[0] - a[0]*b[2];
  r[2] = a[0]*b[1] - a[1]*b[0];
  r[3] = 1.f;
}

static inline void vec4_reflect(vec4 r, vec4 v, vec4 n)
{
  const float p  = 2.f*vec4_mul_inner(v, n);
  int i;
  for(i=0;i<4;++i)
  r[i] = v[i] - p*n[i];
}

static inline void mat3_from_mat4(mat3 m, const mat4 a) {
  m[0] = a[0];
  m[1] = a[1];
  m[2] = a[2];
  m[3] = a[4];
  m[4] = a[5];
  m[5] = a[6];
  m[6] = a[8];
  m[7] = a[9];
  m[8] = a[10];
}

static inline void mat4_set(mat4 m,
  float a11, float a21, float a31, float a41,
  float a12, float a22, float a32, float a42,
  float a13, float a23, float a33, float a43,
  float a14, float a24, float a34, float a44
) {
  m[0]  = a11;
  m[1]  = a21;
  m[2]  = a31;
  m[3]  = a41;
  m[4]  = a12;
  m[5]  = a22;
  m[6]  = a32;
  m[7]  = a42;
  m[8]  = a13;
  m[9]  = a23;
  m[10] = a33;
  m[11] = a43;
  m[12] = a14;
  m[13] = a24;
  m[14] = a34;
  m[15] = a44;
}

static inline uint8_t mat4_invert(mat4 r, const mat4 a) {
  const float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
        a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],
        a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
        a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

  const float b00 = a00 * a11 - a01 * a10;
  const float b01 = a00 * a12 - a02 * a10;
  const float b02 = a00 * a13 - a03 * a10;
  const float b03 = a01 * a12 - a02 * a11;
  const float b04 = a01 * a13 - a03 * a11;
  const float b05 = a02 * a13 - a03 * a12;
  const float b06 = a20 * a31 - a21 * a30;
  const float b07 = a20 * a32 - a22 * a30;
  const float b08 = a20 * a33 - a23 * a30;
  const float b09 = a21 * a32 - a22 * a31;
  const float b10 = a21 * a33 - a23 * a31;
  const float b11 = a22 * a33 - a23 * a32;

  // Calculate the determinant
  float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  if (!det) {
    return 0;
  }

  det = 1.0 / det;

  mat4_set(r,
    (a11 * b11 - a12 * b10 + a13 * b09) * det,
    (a02 * b10 - a01 * b11 - a03 * b09) * det,
    (a31 * b05 - a32 * b04 + a33 * b03) * det,
    (a22 * b04 - a21 * b05 - a23 * b03) * det,
    (a12 * b08 - a10 * b11 - a13 * b07) * det,
    (a00 * b11 - a02 * b08 + a03 * b07) * det,
    (a32 * b02 - a30 * b05 - a33 * b01) * det,
    (a20 * b05 - a22 * b02 + a23 * b01) * det,
    (a10 * b10 - a11 * b08 + a13 * b06) * det,
    (a01 * b08 - a00 * b10 - a03 * b06) * det,
    (a30 * b04 - a31 * b02 + a33 * b00) * det,
    (a21 * b02 - a20 * b04 - a23 * b00) * det,
    (a11 * b07 - a10 * b09 - a12 * b06) * det,
    (a00 * b09 - a01 * b07 + a02 * b06) * det,
    (a31 * b01 - a30 * b03 - a32 * b00) * det,
    (a20 * b03 - a21 * b01 + a22 * b00) * det
  );
  return 1;
}

static inline vec3 mat4_get_eye(const mat4 m) {
  mat4 scratch;
  mat4_invert(scratch, m);
  return vec3_create(
    scratch[12],
    scratch[13],
    scratch[14]
  );
}

static inline void mat4_mul(mat4 r, mat4 a, mat4 b) {
  const float a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3],
        a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7],
        a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11],
        a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

  // Cache only the current line of the second matrix
  float b0  = b[0], b1 = b[1], b2 = b[2], b3 = b[3];
  r[0] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
  r[1] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
  r[2] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
  r[3] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

  b0 = b[4]; b1 = b[5]; b2 = b[6]; b3 = b[7];
  r[4] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
  r[5] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
  r[6] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
  r[7] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

  b0 = b[8]; b1 = b[9]; b2 = b[10]; b3 = b[11];
  r[8] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
  r[9] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
  r[10] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
  r[11] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

  b0 = b[12]; b1 = b[13]; b2 = b[14]; b3 = b[15];
  r[12] = b0*a00 + b1*a10 + b2*a20 + b3*a30;
  r[13] = b0*a01 + b1*a11 + b2*a21 + b3*a31;
  r[14] = b0*a02 + b1*a12 + b2*a22 + b3*a32;
  r[15] = b0*a03 + b1*a13 + b2*a23 + b3*a33;

}

static inline void mat4_perspective(mat4 m, const float fovy, const float aspect, const float near, const float far) {
  const float f = 1.0 / tanf(fovy / 2);
  const float nf = 1 / (near - far);
  const float a = f / aspect;

  const float b = (far + near) * nf;
  const float c = (2.0 * far * near) * nf;

  mat4_set(m,
    a, 0, 0,  0,
    0, f, 0,  0,
    0, 0, b, -1,
    0, 0, c,  0
  );
}

static inline void mat4_transpose(mat4 r, const mat4 m) {
  mat4_set(r,
    m[0], m[4],  m[8], m[12],
    m[1], m[5],  m[9], m[13],
    m[2], m[6], m[10], m[14],
    m[3], m[7], m[11], m[15]
  );
}

static inline void mat4_identity(mat4 m) {
  mat4_set(m,
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1
  );
}

static inline void mat4_from_rotation_translation(mat4 m, const quat q, const vec3 v) {
  const float x = q[0], y = q[1], z = q[2], w = q[3],
      x2 = x + x,
      y2 = y + y,
      z2 = z + z,

      xx = x * x2,
      xy = x * y2,
      xz = x * z2,
      yy = y * y2,
      yz = y * z2,
      zz = z * z2,
      wx = w * x2,
      wy = w * y2,
      wz = w * z2;

  mat4_set(m,
    1 - (yy + zz),
    xy + wz,
    xz - wy,
    0,
    xy - wz,
    1 - (xx + zz),
    yz + wx,
    0,
    xz + wy,
    yz - wx,
    1 - (xx + yy),
    0,
    v[0],
    v[1],
    v[2],
    1
  );
}

static inline void mat4_look_at(mat4 m, const vec3 eye, const vec3 center, const vec3 up) {
  float x0, x1, x2, y0, y1, y2, z0, z1, z2, len;
  const float eyex = eye[0];
  const float eyey = eye[1];
  const float eyez = eye[2];
  const float upx = up[0];
  const float upy = up[1];
  const float upz = up[2];
  const float centerx = center[0];
  const float centery = center[1];
  const float centerz = center[2];

  if (eyex == centerx && eyey == centery && eyez == centerz) {
    return mat4_identity(m);
  }

  //vec3.direction(eye, center, z);
  z0 = eyex - centerx;
  z1 = eyey - centery;
  z2 = eyez - centerz;

  // normalize (no check needed for 0 because of early return)
  len = 1 / sqrtf(z0 * z0 + z1 * z1 + z2 * z2);
  z0 *= len;
  z1 *= len;
  z2 *= len;

  //vec3.normalize(vec3.cross(up, z, x));
  x0 = upy * z2 - upz * z1;
  x1 = upz * z0 - upx * z2;
  x2 = upx * z1 - upy * z0;
  len = sqrt(x0 * x0 + x1 * x1 + x2 * x2);
  if (!len) {
    x0 = 0;
    x1 = 0;
    x2 = 0;
  } else {
    len = 1 / len;
    x0 *= len;
    x1 *= len;
    x2 *= len;
  }

  //vec3.normalize(vec3.cross(z, x, y));
  y0 = z1 * x2 - z2 * x1;
  y1 = z2 * x0 - z0 * x2;
  y2 = z0 * x1 - z1 * x0;

  len = sqrt(y0 * y0 + y1 * y1 + y2 * y2);
  if (!len) {
    y0 = 0;
    y1 = 0;
    y2 = 0;
  } else {
    len = 1 / len;
    y0 *= len;
    y1 *= len;
    y2 *= len;
  }

  mat4_set(m,
    x0, y0, z0, 0,
    x1, y1, z1, 0,
    x2, y2, z2, 0,
    -(x0 * eyex + x1 * eyey + x2 * eyez),
    -(y0 * eyex + y1 * eyey + y2 * eyez),
    -(z0 * eyex + z1 * eyey + z2 * eyez), 1
  );
}


static inline void quat_identity(quat q)
{
  q[0] = q[1] = q[2] = 0.f;
  q[3] = 1.f;
}
static inline void quat_add(quat r, quat a, quat b)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] + b[i];
}
static inline void quat_sub(quat r, quat a, quat b)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] - b[i];
}
static inline void quat_mul(quat r, const quat a, const quat b)
{
  const float ax = a[0], ay = a[1], az = a[2], aw = a[3],
      bx = b[0], by = b[1], bz = b[2], bw = b[3];

  r[0] = ax * bw + aw * bx + ay * bz - az * by;
  r[1] = ay * bw + aw * by + az * bx - ax * bz;
  r[2] = az * bw + aw * bz + ax * by - ay * bx;
  r[3] = aw * bw - ax * bx - ay * by - az * bz;
}
static inline void quat_scale(quat r, const quat v, const float s)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = v[i] * s;
}
static inline float quat_inner_product(quat a, quat b) {
  float p = 0.f;
  int i;
  for(i=0; i<4; ++i)
  p += b[i]*a[i];
  return p;
}

static inline void quat_conj(quat r, const quat q) {
  r[0] = -q[0];
  r[1] = -q[1];
  r[2] = -q[2];
  r[3] =  q[3];
}

static inline void quat_rotate(quat r, const float angle, const vec3 axis) {
  float halfAngle = angle / 2.0f;
  float sinha = sinf(halfAngle);
  vec3 v = axis * vec3_create(sinha, sinha, sinha);
  int i;
  for(i=0; i<3; ++i)
    r[i] = v[i];
  r[3] = cosf(halfAngle);
}
#define quat_norm vec4_norm

static inline void quat_invert(quat r, const quat a) {

  float a0 = a[0];
  float a1 = a[1];
  float a2 = a[2];
  float a3 = a[3];
  float dot = a0 * a0 + a1 * a1 + a2 * a2 + a3 * a3;
  float invDot = dot ? 1.0f / dot : 0.0f;

  // TODO: Would be faster to return [0,0,0,0] immediately if dot == 0

  r[0] = -a0 * invDot;
  r[1] = -a1 * invDot;
  r[2] = -a2 * invDot;
  r[3] = a3 * invDot;
}

static inline void quat_from_mat4(quat q, const mat4 m4) {
  mat3 m;
  mat3_from_mat4(m, m4);

   // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".
  float fTrace = m[0] + m[4] + m[8];
  float fRoot;

  if ( fTrace > 0.0 ) {
    // |w| > 1/2, may as well choose w > 1/2
    fRoot = sqrtf(fTrace + 1.0);  // 2w
    q[3] = 0.5 * fRoot;
    fRoot = 0.5/fRoot;  // 1/(4w)
    q[0] = (m[5]-m[7])*fRoot;
    q[1] = (m[6]-m[2])*fRoot;
    q[2] = (m[1]-m[3])*fRoot;
  } else {
    // |w| <= 1/2
    int i = 0;
    if ( m[4] > m[0] )
      i = 1;
    if ( m[8] > m[i*3+i] )
      i = 2;
    int j = (i+1)%3;
    int k = (i+2)%3;

    fRoot = sqrtf(m[i*3+i]-m[j*3+j]-m[k*3+k] + 1.0);
    q[i] = 0.5 * fRoot;
    fRoot = 0.5 / fRoot;
    q[3] = (m[j*3+k] - m[k*3+j]) * fRoot;
    q[j] = (m[j*3+i] + m[i*3+j]) * fRoot;
    q[k] = (m[k*3+i] + m[i*3+k]) * fRoot;
  }
}

static inline void mat4_from_quat(mat4 m, quat q)
{
  float a = q[3];
  float b = q[0];
  float c = q[1];
  float d = q[2];
  float a2 = a*a;
  float b2 = b*b;
  float c2 = c*c;
  float d2 = d*d;

  mat4_set(m,
    a2 + b2 - c2 - d2,
    2.f*(b*c + a*d),
    2.f*(b*d - a*c),
    0.f,
    2*(b*c - a*d),
    a2 - b2 + c2 - d2,
    2.f*(c*d + a*b),
    0.f,
    2.f*(b*d + a*c),
    2.f*(c*d - a*b),
    a2 - b2 - c2 + d2,
    0.,
    0.f,
    0.f,
    0.f,
    1.f
  );
}

static inline void quat_from_vec3(quat q, const vec3 vec) {
  float x = vec[0];
  float y = vec[1];
  float z = vec[2];
  float s = x*x + y*y;
  if(s > 1.0) {
    s = 1.0;
  }

  if (z == 0.0) {
    z = sqrtf(1.0 - s);
  }

  q[0] = -x;
  q[1] =  y;
  q[2] =  z;
  q[3] =  0.0;
}

static inline void mat4_translate(mat4 m, vec3 vec) {
  float x = vec[0], y = vec[1], z = vec[2];
  m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
  m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
  m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
  m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}

#endif
