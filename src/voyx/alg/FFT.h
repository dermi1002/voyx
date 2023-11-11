#pragma once

#include <voyx/Header.h>

#include <pocketfft_hdronly.h>

template<typename T>
class FFT
{

public:

  FFT(const size_t framesize) :
    fullsize(framesize),
    halfsize(framesize / 2 + /* nyquist */ 1)
  {
    voyxassert(framesize && !(framesize & (framesize - 1))); // power of two
  }

  size_t framesize() const
  {
    return fullsize;
  }

  size_t dftsize() const
  {
    return halfsize;
  }

  void fft(const voyx::vector<T> samples, voyx::vector<std::complex<T>> dft) const
  {
    voyxassert(samples.size() == framesize());
    voyxassert(dft.size() == dftsize());

    pocketfft::r2c(
      { samples.size() },
      { sizeof(T) },
      { sizeof(std::complex<T>) },
      0,
      true,
      samples.data(),
      dft.data(),
      T(1) / samples.size());
  }

  void fft(const voyx::matrix<T> samples, voyx::matrix<std::complex<T>> dfts) const
  {
    voyxassert(samples.size() == dfts.size());
    voyxassert(samples.stride() == framesize());
    voyxassert(dfts.stride() == dftsize());

    const ptrdiff_t X = samples.stride() * sizeof(T);
    const ptrdiff_t Y = dfts.stride() * sizeof(std::complex<T>);

    pocketfft::r2c(
      { samples.stride(), samples.size() },
      { sizeof(T), X },
      { sizeof(std::complex<T>), Y },
      0,
      true,
      samples.data(),
      dfts.data(),
      T(1) / samples.stride());
  }

  void ifft(const voyx::vector<std::complex<T>> dft, voyx::vector<T> samples) const
  {
    voyxassert(samples.size() == framesize());
    voyxassert(dft.size() == dftsize());

    pocketfft::c2r(
      { samples.size() },
      { sizeof(std::complex<T>) },
      { sizeof(T) },
      0,
      false,
      dft.data(),
      samples.data(),
      T(1));
  }

  void ifft(const voyx::matrix<std::complex<T>> dfts, voyx::matrix<T> samples) const
  {
    voyxassert(samples.size() == dfts.size());
    voyxassert(samples.stride() == framesize());
    voyxassert(dfts.stride() == dftsize());

    const ptrdiff_t X = dfts.stride() * sizeof(std::complex<T>);
    const ptrdiff_t Y = samples.stride() * sizeof(T);

    pocketfft::c2r(
      { samples.stride(), samples.size() },
      { sizeof(std::complex<T>), X },
      { sizeof(T), Y },
      0,
      false,
      dfts.data(),
      samples.data(),
      T(1));
  }

private:

  const size_t fullsize;
  const size_t halfsize;

};
