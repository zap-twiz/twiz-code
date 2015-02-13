#ifndef MATRIX34_H
#define MATRIX34_H

#include "vector3f.h"

class Matrix3f {
 public:
  static Matrix3f const kZero;
  static Matrix3f const kIdentity;

  Matrix3f() {}
  explicit Matrix3f(float a11, float a12, float a13,
                    float a21, float a22, float a23,
                    float a31, float a32, float a33);
  Matrix3f(Matrix3f const &rhs);

  Matrix3f& operator=(Matrix3f const & rhs);
  bool operator==(Matrix3f const & rhs) const;

  static Matrix3f RotationX(float theta);
  static Matrix3f RotationY(float theta);
  static Matrix3f RotationZ(float theta);

  static Matrix3f FromAxisAngle(Vector3f const & axis, float angle);

  Matrix3f& operator+=(Matrix3f const & rhs);
  Matrix3f& operator-=(Matrix3f const & rhs);

  float det() const;
  Matrix3f inverse() const;
  Matrix3f transpose() const;

  Matrix3f& operator*=(Matrix3f const & rhs);
  Matrix3f& operator*=(float rhs);

  Vector3f operator*(Vector3f const & rhs) const;

 private:
  float m_[9];
};

Matrix3f operator+(Matrix3f const & lhs, Matrix3f const & rhs);
Matrix3f operator-(Matrix3f const & lhs, Matrix3f const & rhs);

Matrix3f operator*(Matrix3f const & lhs, Matrix3f const & rhs);
//Vector3f operator*(Matrix3f const & lhs, Vector3f const & rhs);
Matrix3f operator*(Matrix3f const & lhs, float rhs);

class AffineTransform {
 public:
  // TODO!

 private:
  Matrix3f orientation_;
  Vector3f translation_;
};

#endif // MATRIX34_H

