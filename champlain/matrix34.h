#ifndef MATRIX34_H
#define MATRIX34_H

#include "vector3f.h"

class Matrix3f {
 public:
  static Matrix3f const kZero;
  static Matrix3f const kIdentity;

  explicit Matrix3f() {}
  explicit Matrix3f(float a11, float a12, float a13,
                    float a21, float a22, float a23,
                    float a31, float a32, float a33);
  explicit Matrix3f(Matrix3f const &rhs);

 private:
  float m_[9];
};

class Matrix34f {

 private:
  Matrix3f orientation_;
  Vector3f translation_;
};

#endif // MATRIX34_H

