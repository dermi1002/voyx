#pragma once

#include <voyx/Header.h>
#include <voyx/alg/STFT.h>
#include <voyx/dsp/SyncPipeline.h>

template<typename T = sample_t>
class StftPipeline : public SyncPipeline<sample_t>
{

public:

  StftPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize, std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink) :
    SyncPipeline<sample_t>(source, sink),
    samplerate(samplerate),
    framesize(framesize),
    hopsize(hopsize),
    dftsize(dftsize),
    stft(framesize, hopsize, dftsize)
  {
    data.dfts.resize(stft.hops().size() * stft.size());
  }

protected:

  const double samplerate;
  const size_t framesize;
  const size_t hopsize;
  const size_t dftsize;

  void operator()(const size_t index, const voyx::vector<sample_t> input, voyx::vector<sample_t> output) override
  {
    voyx::matrix<phasor_t> dfts(data.dfts, stft.size());

    stft.stft(input, dfts);
    (*this)(index, stft.signal(), dfts);
    stft.istft(dfts, output);
  }

  virtual void operator()(const size_t index, const voyx::vector<sample_t> signal, voyx::matrix<phasor_t> dfts) = 0;

private:

  STFT<sample_t, phasor_t::value_type> stft;

  struct
  {
    std::vector<phasor_t> dfts;
  }
  data;

};
