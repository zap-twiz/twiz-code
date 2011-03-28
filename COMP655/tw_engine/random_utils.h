
#ifndef INCLUDED_RANDOM_UTILS_H__
#define INCLUDED_RANDOM_UTILS_H__

int LCGRandom(int a, int x, int c) {
  return a * x + c;
}

#endif  // INCLUDED_RANDOM_UTILS_H__