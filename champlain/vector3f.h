#ifndef VECTOR3F_H
#define VECTOR3F_H

class Vector3f {
 public:
  Vector3f() : x_(0.0f), y_(0.0f), z_(0.0f) {}
  explicit Vector3f(float x, float y, float z) : x_(x), y_(y), z_(z) {}
  explicit Vector3f(float* data) : x_(data[0]), y_(data[1]), z_(data[2]) {}
  Vector3f(Vector3f const &rhs);

  float& x() { return x_; }
  float x() const { return x_; }

  float& y() { return y_; }
  float y() const { return y_; }

  float& z() { return z_; }
  float z() const { return z_; }

  float* data() { return data_; }
  float const * data() const { return data_; }

  Vector3f& operator=(Vector3f const &rhs);

  Vector3f& operator+=(Vector3f const &rhs);
  Vector3f& operator-=(Vector3f const &rhs);
  Vector3f& operator*=(float scalar);

  float dot(Vector3f const &rhs) const;
  Vector3f cross(Vector3f const &rhs) const;

  Vector3f& normalize();
  float length() const;
  float lengthSquared() const;

 private:
  union {
    struct {
      float x_, y_, z_;
    };
    float data_[3];
  };
};

Vector3f operator+(Vector3f const &lhs, Vector3f const &rhs);
Vector3f operator-(Vector3f const &lhs, Vector3f const &rhs);
Vector3f operator*(Vector3f const &lhs, float scalar);
Vector3f operator*(float scalar, Vector3f const &rhs);

#endif // VECTOR3F_H

