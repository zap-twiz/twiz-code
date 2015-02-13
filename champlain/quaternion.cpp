#include "quaternion.h"

#include "math.h"

Quaternion::Quaternion(Quaternion const & rhs)
  : w_(rhs.w_), x_(rhs.x_), y_(rhs.y_), z_(rhs.z_) {
}

Quaternion& Quaternion::operator=(Quaternion const & rhs) {
  data_[0] = rhs.data_[0];
  data_[1] = rhs.data_[1];
  data_[2] = rhs.data_[2];
  data_[3] = rhs.data_[3];

  return *this;
}

Quaternion& Quaternion::operator+=(Quaternion const &rhs) {
  w_ += rhs.w_;
  x_ += rhs.x_;
  y_ += rhs.y_;
  z_ += rhs.z_;

  return *this;
}

Quaternion& Quaternion::operator-=(Quaternion const &rhs) {
  w_ -= rhs.w_;
  x_ -= rhs.x_;
  y_ -= rhs.y_;
  z_ -= rhs.z_;

  return *this;
}

Quaternion& Quaternion::operator*=(float scalar) {
  w_ *= scalar;
  x_ *= scalar;
  y_ *= scalar;
  z_ *= scalar;

  return *this;
}

float Quaternion::Magnitude() const {
  return sqrtf(MagnitudeSquared());
}

float Quaternion::MagnitudeSquared() const {
  return x_*x_ + y_*y_ + z_*z_ + w_*w_;
}

Quaternion& Quaternion::Normalize() {
  float len = Magnitude();
  if (len != 0.0f) {
    *this *= 1.0f / len;
  }
  return *this;
}

Quaternion Quaternion::Conjugate() const {
  return Quaternion(w_, -x_, -y_, -z_);
}

Quaternion Quaternion::Inverse() const {
  float len = Magnitude();
  if (len == 0.0f) {
    return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
  }

  Quaternion result = Conjugate();
  result *= 1.0 / len;
  return result;
}

Matrix3f Quaternion::ToMatrix() const {

#if 0
  1 - 2*qy2 - 2*qz2	2*qx*qy - 2*qz*qw	2*qx*qz + 2*qy*qw
  2*qx*qy + 2*qz*qw	1 - 2*qx2 - 2*qz2	2*qy*qz - 2*qx*qw
  2*qx*qz - 2*qy*qw	2*qy*qz + 2*qx*qw	1 - 2*qx2 - 2*qy2
#endif
//  return Matrix3f(1 - 2*qy2 - 2*qz2)

}

Vector3f Quaternion::Axis() const {
  return Vector3f(x_, y_, z_).normalize();
}

float Quaternion::Angle() const {
  return 2.0f * acosf(w_);
}


