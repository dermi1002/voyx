#pragma once

#include <voyx/Header.h>
#include <voyx/alg/FFT.h>
#include <voyx/etc/Convert.Window.h>

/**
 * Short-Time Fourier Transform implementation.
 **/
template <typename T, typename F>
class STFT
{

public:

  STFT(const size_t framesize, const size_t hopsize, const size_t dftsize) :
    framesize(framesize),
    hopsize(hopsize),
    dftsize(dftsize),
    fft(dftsize * 2 - /* nyquist */ 2)
  {
    voyxassert(fft.dftsize() == dftsize);
    voyxassert(fft.framesize() >= framesize);

    if (framesize == fft.framesize())
    {
      windows.analysis  = $$::window<T>(framesize);
      windows.synthesis = windows.analysis;
    }
    else
    {
      windows.analysis  = $$::asymmetric_analysis_window<T>(fft.framesize(), framesize);
      windows.synthesis = $$::asymmetric_synthesis_window<T>(fft.framesize(), framesize);
    }

    const T unitygain = hopsize / std::inner_product(
      windows.analysis.begin(), windows.analysis.end(), windows.synthesis.begin(), T(0));

    std::transform(windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(),
      [unitygain](T value) { return value * unitygain; });

    for (size_t hop = 0; (hop + framesize) < (framesize * 2); hop += hopsize)
    {
      data.hops.push_back(hop);
    }

    data.input.resize(fft.framesize()  + framesize);
    data.output.resize(fft.framesize() + framesize);
    data.frames.resize(fft.framesize() * data.hops.size());
  }

  size_t size() const
  {
    return dftsize;
  }

  const std::vector<size_t>& hops() const
  {
    return data.hops;
  }

  const voyx::vector<T> signal() const
  {
    return voyx::vector(data.input.data() + framesize, fft.framesize());
  }

  void stft(const voyx::vector<T> samples, voyx::matrix<std::complex<F>> dfts)
  {
    voyxassert(samples.size() == framesize);
    voyxassert(dfts.size() == data.hops.size());
    voyxassert(dfts.stride() == fft.dftsize());

    for (size_t i = 0; i < fft.framesize(); ++i)
    {
      const size_t j = i + framesize;

      data.input[i] = data.input[j];
    }

    for (size_t i = 0; i < framesize; ++i)
    {
      const size_t j = i + fft.framesize();

      data.input[j] = samples[i];
    }

    voyx::matrix<F> frames(data.frames, fft.framesize());

    reject(frames, data.input, data.hops, windows.analysis);

    fft.fft(frames, dfts);
  }

  void istft(const voyx::matrix<std::complex<F>> dfts, voyx::vector<T> samples)
  {
    voyxassert(dfts.size() == data.hops.size());
    voyxassert(dfts.stride() == fft.dftsize());
    voyxassert(samples.size() == framesize);

    voyx::matrix<F> frames(data.frames, fft.framesize());

    fft.ifft(dfts, frames);

    inject(frames, data.output, data.hops, windows.synthesis);

    for (size_t i = 0; i < framesize; ++i)
    {
      const size_t j = i + fft.framesize() - framesize;

      samples[i] = data.output[j];
    }

    for (size_t i = 0; i < fft.framesize(); ++i)
    {
      const size_t j = i + framesize;

      data.output[i] = data.output[j];
      data.output[j] = 0;
    }
  }

private:

  const size_t framesize;
  const size_t hopsize;
  const size_t dftsize;

  const FFT<F> fft;

  struct
  {
    std::vector<T> analysis;
    std::vector<T> synthesis;
  }
  windows;

  struct
  {
    std::vector<T> input;
    std::vector<T> output;
    std::vector<F> frames;
    std::vector<size_t> hops;
  }
  data;

  static void reject(voyx::matrix<F> frames, const voyx::vector<T> input, const std::vector<size_t>& hops, const std::vector<T>& window)
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      const auto hop = hops[i];
      auto frame = frames[i];

      for (size_t j = 0; j < window.size(); ++j)
      {
        frame[j] = input[hop + j] * window[j];
      }
    }
  }

  static void inject(const voyx::matrix<F> frames, voyx::vector<T> output, const std::vector<size_t>& hops, const std::vector<T>& window)
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      const auto hop = hops[i];
      const auto frame = frames[i];

      for (size_t j = 0; j < window.size(); ++j)
      {
        output[hop + j] += frame[j] * window[j];
      }
    }
  }

};
