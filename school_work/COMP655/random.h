#ifndef INCLUDED_RANDOM_H_
#define INCLUDED_RANDOM_H_

#include "random_utils.h"

class Random {
 public:
  virtual unsigned int Generate() = 0;
  virtual void Restore(int memento) {};

  virtual Random* clone() const = 0;
};

template <int V>
class RandomConstant : public Random {
 public:
  virtual unsigned int Generate() { return V; }

  virtual Random* clone() const { return new RandomConstant; }
};

// random from [lower ... Upper)
template <int Lower, int Upper>
class RandomLinear : public Random {
 private:
  static int const kMultiplier = 1664525;
  static int const kOffset = 1013904223;
  static int const kMaxRand = 2 << 32 - 1;
 public:
  RandomLinear() : a(kMultiplier), x(0), c(kOffset) {}

  virtual unsigned int Generate() {
    x_ = LCGRandom(a_, x_, c_);
    long long intermediate = x_ * (Upper - Lower);
    return Lower + intermediate / kMaxRand;
  }

  virtual Random* clone() const { return new RandomLinear(a_, x_, c_); }

 private:
  RandomLinear(int a, int x, int c) : a_(a), x_(x), c_(c) {}
  int a_, x_, c_;
};

#endif  // INCLUDED_RANDOM_H_