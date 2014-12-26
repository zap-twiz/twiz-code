#include "random.h"

#include <cstdlib>
#include <math.h>

Random::Random() {
  srand(0);
}

Random::~Random() {
}

float Random::Rand() {
  return static_cast<float>(rand()) / RAND_MAX;
}

Vector3f Random::RandVector() {
  return Vector3f(Rand(), Rand(), Rand());
}

Vector3f Random::RandSphere() {
  while (true) {
    Vector3f sample = RandVector();
    sample *= 2.0f;
    sample -= Vector3f(1.0f, 1.0f, 1.0f);

    float lengthSqr = sample.lengthSquared();
    if (lengthSqr < 1.0f) {
      return sample.normalize();
    }
  }
}

Vector3f Random::RandHemisphere() {
  Vector3f sphereSample = RandSphere();
  sphereSample.y() = fabs(sphereSample.y());

  return sphereSample;
}

