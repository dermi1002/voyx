#pragma once

#include <voyx/Header.h>

template<typename T>
class Vocoder
{

public:

  Vocoder(const double samplerate, const size_t framesize, const size_t hopsize, const std::optional<size_t> dftsize = std::nullopt) :
    framesize(framesize),
    hopsize(hopsize),
    dftsize(dftsize)
  {
    const T pi = T(2) * std::acos(T(-1));

    if (dftsize)
    {
      const size_t dftframesize = dftsize.value() * 2 - /* nyquist */ 2;

      freqinc = samplerate / dftframesize;
      phaseinc = pi * hopsize / dftframesize;

      synthesis.timeshift.resize(dftsize.value());

      for (size_t i = 0; i < dftsize.value(); ++i)
      {
        // compensate asymmetric synthesis window by virtual time shifting
        synthesis.timeshift[i] = pi * i * T(framesize) / dftsize.value();
      }

      analysis.buffer.resize(dftsize.value());
      synthesis.buffer.resize(dftsize.value());
    }
    else
    {
      const size_t framedftsize = framesize / 2 + /* nyquist */ 1;

      freqinc = samplerate / framesize;
      phaseinc = pi * hopsize / framesize;

      synthesis.timeshift.resize(framedftsize);

      analysis.buffer.resize(framedftsize);
      synthesis.buffer.resize(framedftsize);
    }
  }

  void encode(voyx::matrix<std::complex<T>> dfts)
  {
    for (auto dft : dfts)
    {
      encode(dft);
    }
  }

  void decode(voyx::matrix<std::complex<T>> dfts)
  {
    for (auto dft : dfts)
    {
      decode(dft);
    }
  }

  void encode(voyx::vector<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      phase = atan2(dft[i]);

      delta = phase - std::exchange(analysis.buffer[i], phase);

      j = wrap(delta - i * phaseinc) / phaseinc;

      frequency = (i + j) * freqinc;

      dft[i] = std::complex<T>(std::abs(dft[i]), frequency);
    }
  }

  void decode(voyx::vector<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      frequency = dft[i].imag();

      j = (frequency - i * freqinc) / freqinc;

      delta = (i + j) * phaseinc;

      phase = (synthesis.buffer[i] += delta) - synthesis.timeshift[i];

      dft[i] = std::polar<T>(dft[i].real(), phase);
    }
  }

private:

  const size_t framesize;
  const size_t hopsize;
  const std::optional<size_t> dftsize;

  T freqinc;
  T phaseinc;

  struct
  {
    std::vector<T> buffer;
  }
  analysis;

  struct
  {
    std::vector<T> timeshift;
    std::vector<T> buffer;
  }
  synthesis;

  /**
   * Converts the specified arbitrary phase value
   * to be within the interval from -pi to pi.
   **/
  inline static T wrap(const T phase)
  {
    const T pi = T(2) * T(M_PI);

    return phase - pi * std::floor(phase / pi + T(0.5));
  }

  /**
   * Approximates the phase angle of the complex number z.
   **/
  inline static T atan2(const std::complex<T>& z)
  {
    return atan2(z.imag(), z.real()); // instead of std::arg(z)
  }

  /**
   * Arctangent approximation according to [1].
   *
   * [1] Xavier Girones and Carme Julia and Domenec Puig
   *     Full Quadrant Approximations for the Arctangent Function
   *     IEEE Signal Processing Magazine (2013)
   *     https://ieeexplore.ieee.org/document/6375931
   **/
  inline static T atan2(const T y, const T x)
  {
    if (y == 0 && x == 0)
    {
      // skip approximation and return
      // zero instead of NaN in this case
      return T(0);
    }

    // extract the sign bits
    const int ys = std::signbit(y);
    const int xs = std::signbit(x);

    // determine the quadrant offset and sign
    const int q = (ys & ~xs) * 4 + xs * 2;
    const int s = (ys ^ xs) ? -1 : +1;

    // calculate the arctangent in the first quadrant
    const T a = T(0.596227);
    const T b = std::abs(a * y * x);
    const T c = b + y * y;
    const T d = b + x * x;
    const T e = c / (c + d);

    // translate it to the proper quadrant
    const T phi = q + std::copysign(e, s);

    // translate the result from [0, 4) to [0, 2pi)
    return phi * T(1.57079632679489661923);
  }

};
