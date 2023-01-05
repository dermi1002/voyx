#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<typename E>
  std::vector<size_t> findpeaks(E&& expression, const size_t radius = 0)
  {
    auto&& array = xt::eval(expression);

    if (!array.size())
    {
      return {};
    }

    std::vector<size_t> peaks;

    if (radius)
    {
      const ptrdiff_t r = radius;

      for (ptrdiff_t i = r; i < array.size() - r; ++i)
      {
        const auto value = array.flat(i);

        bool ispeak = true;

        for (ptrdiff_t j = i - r; j <= i + r; ++j)
        {
          if (j == i)
          {
            continue;
          }

          if (array.flat(j) > value)
          {
            ispeak = false;
            break;
          }
        }

        if (ispeak)
        {
          peaks.push_back(i);
        }
      }
    }
    else
    {
      auto value = array.flat(0);
      size_t index = 0;

      for (size_t i = 1; i < array.size(); ++i)
      {
        if (array.flat(i) > value)
        {
          value = array.flat(i);
          index = i;
        }
      }

      peaks.push_back(index);
    }

    return peaks;
  }

  // TODO: use xtensor

  template<typename value_getter_t, typename T>
  std::vector<size_t> argmax(const voyx::matrix<T> matrix, const size_t axis = 0)
  {
    using value_t = typename $$::typeofvalue<T>::type;
    const value_getter_t getvalue;

    static_assert(std::is_arithmetic<value_t>::value);

    std::vector<size_t> indices;

    if (matrix.empty())
    {
      return indices;
    }

    const size_t shape[] =
    {
      matrix.size(),
      matrix.stride()
    };

    if (axis == 0)
    {
      indices.resize(shape[1]);

      for (size_t i = 0; i < shape[1]; ++i)
      {
        value_t value = getvalue(matrix(0, i));
        size_t index = 0;

        for (size_t j = 1; j < shape[0]; ++j)
        {
          if (getvalue(matrix(j, i)) > value)
          {
            value = getvalue(matrix(j, i));
            index = j;
          }
        }

        indices[i] = index;
      }
    }
    else if (axis == 1)
    {
      indices.resize(shape[0]);

      for (size_t i = 0; i < shape[0]; ++i)
      {
        value_t value = getvalue(matrix(i, 0));
        size_t index = 0;

        for (size_t j = 1; j < shape[1]; ++j)
        {
          if (getvalue(matrix(i, j)) > value)
          {
            value = getvalue(matrix(i, j));
            index = j;
          }
        }

        indices[i] = index;
      }
    }
    else
    {
      throw std::runtime_error("Invalid axis index!");
    }

    return indices;
  }
}
