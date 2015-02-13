#ifndef QUATERNION_H
#define QUATERNION_H

#include "matrix34.h"
#include "vector3f.h"

class Quaternion {
 public:
  static Quaternion FromAxisAngle(Vector3f const & axis, float angle);

  Quaternion(){}
  explicit Quaternion(float w, float x, float y, float z)
    : w_(w), x_(x), y_(y), z_(z) {}
  explicit Quaternion(float* data)
    : w_(data[0]), x_(data[1]), y_(data[2]), z_(data[3]) {}
  Quaternion(Quaternion const & rhs);

  float& w() { return w_; }
  float w() const { return w_; }

  float& x() { return x_; }
  float x() const { return x_; }

  float& y() { return y_; }
  float y() const { return y_; }

  float& z() { return z_; }
  float z() const { return z_; }

  float* data() { return data_; }
  float const * data() const { return data_; }

  Quaternion& operator=(Quaternion const & rhs);
  Quaternion& operator+=(Quaternion const &rhs);
  Quaternion& operator-=(Quaternion const &rhs);
  Quaternion& operator*=(float scalar);

  float Magnitude() const;
  float MagnitudeSquared() const;
  Quaternion& Normalize();

  Quaternion Conjugate() const;
  Quaternion Inverse() const;

  Matrix3f ToMatrix() const;
  Vector3f Axis() const;
  float Angle() const;

 private:
  union {
    struct {
      float w_, x_, y_, z_;
    };
    float data_[4];
  };
};

#endif // QUATERNION_H
