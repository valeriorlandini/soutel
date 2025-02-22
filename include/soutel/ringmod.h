/******************************************************************************
Copyright (c) 2023-2025 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef RINGMOD_H_
#define RINGMOD_H_

#include "blosc.h"
#include "utils.h"
#include "wtosc.h"

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

enum class RModulators
{
    oscillator,
    wavetable,
    input
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class RingMod
{
public:
    RingMod(const TSample &sample_rate = (TSample)44100.0, const TSample &frequency = (TSample)440.0, const RModulators &modulator = RModulators::oscillator);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_modulator(const RModulators &modulator);
    void set_modulator_wave(const BLWaveforms &waveform);
    void load_custom_wavetable(const std::vector<TSample> &wavetable);
    void set_am(const bool &am);

    TSample get_sample_rate();
    TSample get_frequency();
    BLWaveforms get_waveform();
    RModulators get_modulator();
    bool get_am();

    inline TSample run(const TSample &input, const TSample &mod_input = (TSample)0.0);
    inline void run(const TSample &input, TSample &output, const TSample &mod_input);

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    RModulators modulator_;
    bool am_;
    TSample osc_out_;
    TSample wtosc_out_;
    TSample output_;

    BLWaveforms waveform_;
    BLOsc<TSample> osc_;
    WTOsc<TSample> wtosc_;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
RingMod<TSample>::RingMod(const TSample &sample_rate, const TSample &frequency, const RModulators &modulator)
{
    set_sample_rate(sample_rate);

    set_frequency(frequency);

    set_modulator(modulator);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void RingMod<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);

    osc_.set_sample_rate(sample_rate_);
    wtosc_.set_sample_rate(sample_rate_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void RingMod<TSample>::set_frequency(const TSample &frequency)
{
    osc_.set_frequency(frequency);
    wtosc_.set_frequency(frequency);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void RingMod<TSample>::set_modulator(const RModulators &modulator)
{
    modulator_ = modulator;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void RingMod<TSample>::set_modulator_wave(const BLWaveforms &waveform)
{
    waveform_ = waveform;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void RingMod<TSample>::load_custom_wavetable(const std::vector<TSample> &wavetable)
{
    wtosc_.set_wavetable(wavetable);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample RingMod<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample RingMod<TSample>::get_frequency()
{
    return osc_.get_frequency();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
RModulators RingMod<TSample>::get_modulator()
{
    return modulator_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool RingMod<TSample>::get_am()
{
    return am_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
BLWaveforms RingMod<TSample>::get_waveform()
{
    return waveform_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample RingMod<TSample>::run(const TSample &input, const TSample &mod_input)
{
    osc_.run();
    switch (waveform_)
    {
    case BLWaveforms::sine:
        osc_out_ = osc_.get_sine();
        break;
    case BLWaveforms::triangle:
        osc_out_ = osc_.get_triangle();
        break;
    case BLWaveforms::saw:
        osc_out_ = osc_.get_saw();
        break;
    case BLWaveforms::square:
        osc_out_ = osc_.get_square();
        break;
    }

    wtosc_out_ = wtosc_.run();

    switch (modulator_)
    {
    case RModulators::oscillator:
        am_ ? output_ = input * scale(osc_out_, (TSample)-1.0, (TSample)1.0, (TSample)0.0, (TSample)1.0) : output_ = input * osc_out_;
        break;
    case RModulators::wavetable:
        am_ ? output_ = input * scale(wtosc_out_, (TSample)-1.0, (TSample)1.0, (TSample)0.0, (TSample)1.0) : output_ = input * wtosc_out_;
        break;
    case RModulators::input:
        am_ ? output_ = input * scale(mod_input, (TSample)-1.0, (TSample)1.0, (TSample)0.0, (TSample)1.0) : output_ = input * mod_input;
        break;
    }

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void RingMod<TSample>::run(const TSample &input, TSample &output, const TSample &mod_input)
{

    output_ = run(input, mod_input);
    output = output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample RingMod<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // RINGMOD_H_
