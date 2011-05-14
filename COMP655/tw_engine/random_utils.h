#ifndef INCLUDED_RANDOM_UTILS_H__
#define INCLUDED_RANDOM_UTILS_H__

inline int LCGRandom(int a, int x, int c) {
  // TODO:  Implement LCG random function
  return a * x + c;
}

#endif  // INCLUDED_RANDOM_UTILS_H__
