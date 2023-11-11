#include <voyx/dsp/StftTestPipeline.h>

#include <voyx/Source.h>

StftTestPipeline::StftTestPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                                   std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                                   std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, dftsize, source, sink),
  vocoder(samplerate, framesize, hopsize, dftsize),
  midi(midi),
  plot(plot)
{
  if (plot != nullptr)
  {
    plot->xmap(samplerate / 2);
    plot->xlim(0, 2e3);
    plot->ylim(-120, 0);
  }
}

void StftTestPipeline::operator()(const size_t index,
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

  // vocoder test
  // vocoder.encode(dfts);
  // vocoder.decode(dfts);
}
