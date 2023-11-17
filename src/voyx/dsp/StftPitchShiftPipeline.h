#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Vocoder.h>
#include <voyx/dsp/SyncPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

#include <StftPitchShift/STFT.h>
#include <StftPitchShift/StftPitchShiftCore.h>

class StftPitchShiftPipeline : public SyncPipeline<>
{

public:

  StftPitchShiftPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                         std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                         std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

protected:

  void operator()(const size_t index,
                  const voyx::vector<sample_t> input,
                  voyx::vector<sample_t> output) override;

private:

  const double samplerate;
  const std::tuple<size_t, size_t> framesize;
  const size_t hopsize;

  struct
  {
    std::vector<double> input;
    std::vector<double> output;
  }
  buffer;

  std::shared_ptr<stftpitchshift::STFT<double>> stft;
  std::shared_ptr<stftpitchshift::StftPitchShiftCore<double>> core;

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
