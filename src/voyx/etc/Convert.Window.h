#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<typename T>
  std::vector<T> window(const size_t size)
  {
    std::vector<T> window(size);

    const T pi = T(2) * std::acos(T(-1)) / size;

    for (size_t i = 0; i < size; ++i)
    {
      window[i] = T(0.5) - T(0.5) * std::cos(pi * i);
    }

    return window;
  }

  template<typename T>
  std::vector<T> asymmetric_analysis_window(const size_t analysis_window_size, const size_t synthesis_window_size)
  {
    const auto n = analysis_window_size;
    const auto m = synthesis_window_size / 2;

    const auto left = $$::window<T>(2 * n - 2 * m);
    const auto right = $$::window<T>(2 * m);

    std::vector<T> window(analysis_window_size);

    for (size_t i = 0; i < n - m; ++i)
    {
      window[i] = left[i];
    }

    for (size_t i = 0; i < m; ++i)
    {
      window[i + n - m] = right[i + m];
    }

    return window;
  }

  template<typename T>
  std::vector<T> asymmetric_synthesis_window(const size_t analysis_window_size, const size_t synthesis_window_size)
  {
    const auto n = analysis_window_size;
    const auto m = synthesis_window_size / 2;

    const auto left = $$::window<T>(2 * n - 2 * m);
    const auto right = $$::window<T>(2 * m);

    std::vector<T> window(analysis_window_size);

    for (size_t i = 0; i < m; ++i)
    {
      window[i + n - m - m] = right[i] / left[i + n - m - m];
    }

    for (size_t i = 0; i < m; ++i)
    {
      window[i + n - m] = right[i + m];
    }

    return window;
  }
}
