#include <voyx/dsp/StftPitchShiftPipeline.h>

#include <voyx/Source.h>

StftPitchShiftPipeline::StftPitchShiftPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                                               std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                                               std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, dftsize, source, sink),
  core(std::make_tuple(dftsize * 2 - 2, framesize), hopsize, samplerate),
  midi(midi),
  plot(plot)
{
  if (plot != nullptr)
  {
    plot->xmap(samplerate / 2);
    plot->xlim(0, 2e3);
    plot->ylim(-120, 0);
  }

  core.factors({ 1 });
  core.quefrency(0 * 1e-3);
  core.distortion(1);
  core.normalization(true);
}

void StftPitchShiftPipeline::operator()(const size_t index,
                                        const voyx::vector<sample_t> signal,
                                        voyx::matrix<phasor_t> dfts)
{
  if (plot != nullptr)
  {
    const auto dft = dfts.front();

    std::vector<double> abs(dft.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = 20 * std::log10(std::abs(dft[i]));
    }

    plot->plot(abs);
  }

  std::vector<phasor_t> buffer(dfts.stride());

  for (auto dft : dfts)
  {
    std::copy(dft.begin(), dft.end(), buffer.begin());
    core.shiftpitch(buffer);
    std::copy(buffer.begin(), buffer.end(), dft.begin());
  }
}
