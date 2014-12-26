
#include "vector3f.h"

#include <cmath>

Vector3f::Vector3f(Vector3f const &rhs)
    : x_(rhs.x_), y_(rhs.y_), z_(rhs.z_) {
}

Vector3f& Vector3f::operator=(Vector3f const &rhs) {
  x_ = rhs.x_;
  y_ = rhs.y_;
  z_ = rhs.z_;
  return *this;
}

Vector3f& Vector3f::operator+=(Vector3f const &rhs) {
  x_ += rhs.x_;
  y_ += rhs.y_;
  z_ += rhs.z_;

  return *this;
}

Vector3f& Vector3f::operator-=(Vector3f const &rhs) {
  x_ -= rhs.x_;
  y_ -= rhs.y_;
  z_ -= rhs.z_;

  return *this;
}

Vector3f& Vector3f::operator*=(float scalar) {
  x_ *= scalar;
  y_ *= scalar;
  z_ *= scalar;

  return *this;
}

float Vector3f::dot(Vector3f const &rhs) const{
  float sum = 0.0f;
  sum += x_ * rhs.x_;
  sum += y_ * rhs.y_;
  sum += z_ * rhs.z_;

  return sum;
}

Vector3f Vector3f::cross(Vector3f const & rhs) const {
  return Vector3f(y_*rhs.z_ - z_*rhs.y_,
                  x_*rhs.z_ - z_*rhs.x_,
                  x_*rhs.y_ - y_*rhs.x_);
}

Vector3f& Vector3f::normalize() {
  *this *= 1 / length();
  return *this;
}

float Vector3f::length() const {
  return sqrtf(lengthSquared());
}

float Vector3f::lengthSquared() const {
  return x_*x_ + y_*y_ + z_*z_;
}

Vector3f operator+(Vector3f const &lhs, Vector3f const &rhs) {
  return Vector3f(lhs) += rhs;
}

Vector3f operator-(Vector3f const &lhs, Vector3f const &rhs) {
  return Vector3f(lhs) -= rhs;
}

Vector3f operator*(Vector3f const &lhs, float scalar) {
  return Vector3f(lhs) *= scalar;
}

Vector3f operator*(float scalar, Vector3f const &rhs) {
  return Vector3f(rhs) *= scalar;
}
