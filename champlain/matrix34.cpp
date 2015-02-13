
#include "matrix34.h"

#include <math.h>

Matrix3f const  Matrix3f::kZero(0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f);

Matrix3f const Matrix3f::kIdentity(1.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f);

Matrix3f Matrix3f::RotationX(float theta) {
  return Matrix3f(1.0f, 0.0f, 0.0f,
                  0.0f, cosf(theta), -sinf(theta),
                  0.0f, sinf(theta), cosf(theta));
}

Matrix3f Matrix3f::RotationY(float theta) {
  return Matrix3f(cosf(theta), 0.0f, -sinf(theta),
                  0.0f, 1.0f, 0.0f,
                  sinf(theta), 0.0f, cosf(theta));
}

Matrix3f Matrix3f::RotationZ(float theta) {
  return Matrix3f(cosf(theta), -sinf(theta), 0.0f,
                  sinf(theta), cosf(theta), 0.0f,
                  0.0f, 0.0f, 1.0f);

}

// Note: Assumes that axis is normalized!
Matrix3f Matrix3f::FromAxisAngle(Vector3f const & axis, float angle) {
  float c = cosf(angle);
  float s = sinf(angle);

  float t = 1 - c;
  float x = axis.x();
  float y = axis.y();
  float z = axis.z();
#if 0
  c =cos(angle)
  s = sin(angle)
  t =1 - c
  t*x*x + c	t*x*y - z*s	t*x*z + y*s
  t*x*y + z*s	t*y*y + c	t*y*z - x*s
  t*x*z - y*s	t*y*z + x*s	t*z*z + c
#endif

  return Matrix3f(t*x*x + c, t*x*y - z*s, t*x*z + y*s,
                  t*x*y + z*s, t*y*y + c, t*y*z - x*s,
                  t*x*z - y*s, t*y*z + x*s, t*z*z + c);
}

Matrix3f::Matrix3f(float a11, float a12, float a13,
                   float a21, float a22, float a23,
                   float a31, float a32, float a33) {
  m_[0] = a11; m_[1] = a12; m_[2] = a13;
  m_[3] = a21; m_[4] = a22; m_[5] = a23;
  m_[6] = a31; m_[7] = a32; m_[8] = a33;
}

Matrix3f::Matrix3f(Matrix3f const &rhs) {
  m_[0] = rhs.m_[0];
  m_[1] = rhs.m_[1];
  m_[2] = rhs.m_[2];
  m_[3] = rhs.m_[3];
  m_[4] = rhs.m_[4];
  m_[5] = rhs.m_[5];
  m_[6] = rhs.m_[6];
  m_[7] = rhs.m_[7];
  m_[8] = rhs.m_[8];
}

Matrix3f& Matrix3f::operator=(Matrix3f const & rhs) {
  if (this == &rhs) {
    return *this;
  }
  m_[0] = rhs.m_[0];
  m_[1] = rhs.m_[1];
  m_[2] = rhs.m_[2];
  m_[3] = rhs.m_[3];
  m_[4] = rhs.m_[4];
  m_[5] = rhs.m_[5];
  m_[6] = rhs.m_[6];
  m_[7] = rhs.m_[7];
  m_[8] = rhs.m_[8];
  return *this;
}

bool Matrix3f::operator==(Matrix3f const & rhs) const {
  return m_[0] == rhs.m_[0] &&
    m_[1] == rhs.m_[1] &&
    m_[2] == rhs.m_[2] &&
    m_[3] == rhs.m_[3] &&
    m_[4] == rhs.m_[4] &&
    m_[5] == rhs.m_[5] &&
    m_[6] == rhs.m_[6] &&
    m_[7] == rhs.m_[7] &&
    m_[8] == rhs.m_[8];
}

Matrix3f& Matrix3f::operator+=(Matrix3f const & rhs) {
  m_[0] += rhs.m_[0];
  m_[1] += rhs.m_[1];
  m_[2] += rhs.m_[2];
  m_[3] += rhs.m_[3];
  m_[4] += rhs.m_[4];
  m_[5] += rhs.m_[5];
  m_[6] += rhs.m_[6];
  m_[7] += rhs.m_[7];
  m_[8] += rhs.m_[8];
  return *this;
}

Matrix3f& Matrix3f::operator-=(Matrix3f const & rhs) {
  m_[0] -= rhs.m_[0];
  m_[1] -= rhs.m_[1];
  m_[2] -= rhs.m_[2];
  m_[3] -= rhs.m_[3];
  m_[4] -= rhs.m_[4];
  m_[5] -= rhs.m_[5];
  m_[6] -= rhs.m_[6];
  m_[7] -= rhs.m_[7];
  m_[8] -= rhs.m_[8];
  return *this;
}

float Matrix3f::det() const {
  float const a = m_[0];
  float const b = m_[1];
  float const c = m_[2];
  float const d = m_[3];
  float const e = m_[4];
  float const f = m_[5];
  float const g = m_[6];
  float const h = m_[7];
  float const i = m_[8];

  return a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
}

Matrix3f Matrix3f::inverse() const {
  float const determinant = det();
  if (determinant == 0.0f) {
    return kZero;
  }

  float const m11 = m_[0];
  float const m12 = m_[1];
  float const m13 = m_[2];
  float const m21 = m_[3];
  float const m22 = m_[4];
  float const m23 = m_[5];
  float const m31 = m_[6];
  float const m32 = m_[7];
  float const m33 = m_[8];

  Matrix3f result(m22*m11 - m23*m32, m13*m32 - m12*m33, m12*m23 - m13*m22,
                  m23*m31 - m21*m33, m11*m33 - m13*m31, m13*m21 - m11*m23,
                  m21*m32 - m22*m31, m12*m31 - m11*m32, m11*m22 - m12*m31);
  result *= 1.0f / determinant;
  return result;
}

Matrix3f Matrix3f::transpose() const {
  float const m11 = m_[0];
  float const m12 = m_[1];
  float const m13 = m_[2];
  float const m21 = m_[3];
  float const m22 = m_[4];
  float const m23 = m_[5];
  float const m31 = m_[6];
  float const m32 = m_[7];
  float const m33 = m_[8];

  Matrix3f result(m11, m21, m31,
                  m12, m22, m32,
                  m13, m23, m33);
  return result;
}

Matrix3f& Matrix3f::operator*=(Matrix3f const & rhs) {

  Matrix3f source = *this;

  int const kStride = 3;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j ) {
      m_[i*kStride + j] = source.m_[i*kStride] * rhs.m_[j] +
          source.m_[i*kStride + 1] * rhs.m_[j + kStride] +
          source.m_[i*kStride + 2] * rhs.m_[j+  2*kStride];
    }
  }

  return *this;
}

Vector3f Matrix3f::operator*(Vector3f const & rhs) const {
  float const * rhs_data = rhs.data();
  return Vector3f(
      m_[0] * rhs_data[0] + m_[1] * rhs_data[1] + m_[2] * rhs_data[2],
      m_[3] * rhs_data[0] + m_[4] * rhs_data[1] + m_[5] * rhs_data[2],
      m_[6] * rhs_data[0] + m_[7] * rhs_data[1] + m_[8] * rhs_data[2]);
}

Matrix3f& Matrix3f::operator*=(float rhs) {
  m_[0] *= rhs;
  m_[1] *= rhs;
  m_[2] *= rhs;
  m_[3] *= rhs;
  m_[4] *= rhs;
  m_[5] *= rhs;
  m_[6] *= rhs;
  m_[7] *= rhs;
  m_[8] *= rhs;
  return *this;
}

Matrix3f operator+(Matrix3f const & lhs, Matrix3f const & rhs) {
  Matrix3f result(lhs);
  result += rhs;
  return result;
}

Matrix3f operator-(Matrix3f const & lhs, Matrix3f const & rhs) {
  Matrix3f result(lhs);
  result -= rhs;
  return result;
}

Matrix3f operator*(Matrix3f const & lhs, Matrix3f const & rhs) {
  Matrix3f result(lhs);
  result *= rhs;
  return result;
}

#if 0
Vector3f operator*(Matrix3f const & lhs, Vector3f const & rhs) {
  return lhs.operator*(rhs);
}
#endif

Matrix3f operator*(Matrix3f const & lhs, float rhs) {
  Matrix3f result(lhs);
  result *= rhs;
  return result;
}


