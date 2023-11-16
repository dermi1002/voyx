#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Vocoder.h>
#include <voyx/dsp/StftPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

#include <StftPitchShift/StftPitchShiftCore.h>

class StftPitchShiftPipeline : public StftPipeline<double>
{

public:

  StftPitchShiftPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                         std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                         std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  const voyx::vector<sample_t> signal,
                  voyx::matrix<phasor_t> dfts) override;

private:

  stftpitchshift::StftPitchShiftCore<double> core;

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
