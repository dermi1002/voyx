#include <voyx/dsp/InverseSynthPipeline.h>

#include <voyx/Source.h>

InverseSynthPipeline::InverseSynthPipeline(const double samplerate, const size_t framesize, const size_t hopsize, const size_t dftsize,
                                           std::shared_ptr<Source<sample_t>> source, std::shared_ptr<Sink<sample_t>> sink,
                                           std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, dftsize, source, sink),
  vocoder(samplerate, framesize, hopsize, dftsize),
  midi(midi),
  plot(plot)
{
  if (midi != nullptr)
  {
    const double concertpitch = midi->concertpitch();

    dftfreqs = $$::dft::freqs<double>(samplerate, framesize);
    dftbins = $$::dft::bins<double>(framesize);
    dftkeys = $$::midi::keys(dftfreqs, concertpitch);
    midifreqs = $$::midi::freqs<double>(concertpitch);
    midikeys = $$::midi::keys<double>();
    midibins = $$::interp(midikeys, dftkeys, dftbins);
    dftfreqs = $$::interp(dftbins, midibins, midifreqs);
  }

  if (plot != nullptr)
  {
    plot->xmap(samplerate / 2);
    plot->xlim(0, 2.5e3);
    plot->ylim(0, 1);
  }
}

void InverseSynthPipeline::operator()(const size_t index,
                                      const voyx::vector<sample_t> signal,
                                      voyx::matrix<phasor_t> dfts)
{
  if (midi != nullptr)
  {
    for (auto dft : dfts)
    {
      const auto midimask = midi->mask();
      const auto dftmask = $$::interp(dftbins, midibins, midimask);

      for (size_t i = 0; i < dft.size(); ++i)
      {
        dft[i].real(dftmask[i]);
        dft[i].imag(dftfreqs[i]);
      }

      if (plot != nullptr)
      {
        plot->plot(dftmask);
      }
    }
  }
  else
  {
    vocoder.encode(dfts);
  }

  vocoder.decode(dfts);
}
