#pragma once

#include <voyx/Header.h>

template<typename T>
class FIR
{

public:

  FIR(const std::vector<T>& b) :
    b(b), x(b.size())
  {
  }

  T operator()(const T input)
  {
    x.pop_back();
    x.push_front(input);

    T y = T(0);

    for (size_t i = 0; i < b.size(); ++i)
    {
      y += b[i] * x[i];
    }

    return y;
  }

  void operator()(const voyx::vector<T> input, voyx::vector<T> output)
  {
    voyxassert(input.size() == output.size());

    for (size_t i = 0; i < input.size(); ++i)
    {
      output[i] = (*this)(input[i]);
    }
  }

private:

  const std::vector<T> b;

  std::deque<T> x;

};
