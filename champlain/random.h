#ifndef RANDOM_H
#define RANDOM_H

#include "vector3f.h"

class Random {
 public:
  Random();
  ~Random();

  float Rand();
  Vector3f RandVector();

  Vector3f RandSphere();
  Vector3f RandHemisphere();
};

#endif // RANDOM_H
