
#include "matrix34.h"

Matrix3f const  Matrix3f::kZero(0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.0f);

Matrix3f const Matrix3f::kIdentity(1.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f);

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


