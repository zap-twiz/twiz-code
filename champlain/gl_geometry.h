#ifndef GL_GEOMETRY_H
#define GL_GEOMETRY_H

#include <glu.h>

class GLGeometry {
 public:
  virtual ~GLGeometry() {}

  virtual void draw() const = 0;

  // consider bounding box routines
};

class GLPlane : public GLGeometry {
 public:
  GLPlane(float width, float height) : width_(width), height_(height) {}

  virtual void draw() const;
 private:
  float width_, height_;
};

class GLSphere : public GLGeometry {
 public:
  explicit GLSphere(float radius);
  virtual ~GLSphere();

  virtual void draw() const;

 private:
  float radius_;
  GLUquadric* quad_;
};

class GLCube : public GLGeometry {
 public:
  GLCube(float width, float height, float depth)
    : width_(width), height_(height), depth_(depth) {}

  virtual void draw() const;

 private:
  float width_, height_, depth_;
};

class GLCylinder : public GLGeometry {
 public:
  GLCylinder(float height, float radius);
  virtual ~GLCylinder();

  virtual void draw() const;

 private:
  float height_, radius_;
  GLUquadric* quad_;
};

#endif // GL_GEOMETRY_H

