#include <voyx/dsp/VoiceSynthPipeline.h>

#include <voyx/Source.h>

VoiceSynthPipeline::VoiceSynthPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                                       std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                                       std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, dftsize, source, sink),
  vocoder(samplerate, framesize, hopsize, dftsize),
  lifter(1e-3, samplerate, dftsize * 2 - 2),
  midi(midi),
  plot(plot)
{
  if (midi != nullptr)
  {
  }

  if (plot != nullptr)
  {
    plot->xmap(samplerate / 2);
    plot->xlim(0, 5e3);
    plot->ylim(-120, 0);
  }
}

void VoiceSynthPipeline::operator()(const size_t index,
                                    const voyx::vector<sample_t> signal,
                                    voyx::matrix<phasor_t> dfts)
{
  if (plot != nullptr)
  {
    std::vector<std::complex<sample_t>> dft = $$::fft(signal);
    std::vector<double> abs(dft.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = 20 * std::log10(std::abs(dft[i]));
    }

    plot->plot(abs);
  }

  vocoder.encode(dfts);

  const std::vector<double> factors = { 0.5, 1.25, 1.5, 2 };

  const double roi[] = { 0, samplerate / 2 };

  std::vector<double> envelope(dfts.stride());

  lifter.lowpass<$$::real>(dfts.front(), envelope);

  std::vector<phasor_t> buffer(factors.size() * dfts.stride());
  voyx::matrix<phasor_t> buffers(buffer, dfts.stride());

  for (auto dft : dfts)
  {
    lifter.divide<$$::real>(dft, envelope);

    for (size_t i = 0; i < factors.size(); ++i)
    {
      $$::interp(dft, buffers[i], factors[i]);
    }

    const auto mask = $$::argmax<$$::real>(buffers);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      const size_t j = mask[i];

      dft[i] = buffers(j, i);

      const auto frequency = dft[i].imag() * factors[j];

      dft[i].imag(frequency);

      if (frequency <= roi[0] || roi[1] <= frequency)
      {
        dft[i].real(0);
      }
    }

    lifter.multiply<$$::real>(dft, envelope);
  }

  vocoder.decode(dfts);
}
