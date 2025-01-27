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

#ifndef PULSAR_H_
#define PULSAR_H_

#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "window_functions.h"

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class PulsarWaveforms
{
    sine,
    triangle,
    saw,
    square,
    noise,
    dc,
    phasor
};

enum class PulsarWindows
{
    hann,
    hamming,
    blackman,
    nuttall,
    blackmannuttall,
    blackmanharris,
    flattop,
    bartletthann,
    rectangular
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Pulsar
{
public:
    Pulsar(const TSample &sample_rate = 44100.0,
           const TSample &frequency = 0.0,
           const TSample &duty_cycle = 0.5,
           const PulsarWaveforms &waveform = PulsarWaveforms::sine,
           const PulsarWindows &window = PulsarWindows::rectangular);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_duty_cycle(const TSample &duty_cycle);
    void set_waveform(const PulsarWaveforms &waveform);
    void set_window(const PulsarWindows &window);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();
    TSample get_duty_cycle();
    PulsarWaveforms get_waveform();
    PulsarWindows get_window();

    inline TSample run();

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;
    TSample duty_cycle_;
    TSample inv_duty_cycle_;

    TSample wave_step_;
    TSample wave_ramp_;
    TSample step_;
    TSample ramp_;
    TSample harmonics_;
    bool gen_wave_;

    TSample output_;

    PulsarWaveforms waveform_;
    PulsarWindows window_;

    const TSample double_pi_ = (TSample)(M_PI * 2.0);
    const TSample inv_rand_max2_ = (TSample)2.0 * ((TSample)1.0 / (TSample)RAND_MAX);
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
Pulsar<TSample>::Pulsar(const TSample &sample_rate,
                        const TSample &frequency,
                        const TSample &duty_cycle,
                        const PulsarWaveforms &waveform,
                        const PulsarWindows &window)
{
    frequency_ = frequency;

    if (sample_rate > 0.0)
    {
        set_sample_rate(sample_rate);
    }
    else
    {
        set_sample_rate(44100.0);
    }

    set_duty_cycle(duty_cycle);
    set_waveform(waveform);
    set_window(window);

    srand(time(0));
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;

    set_frequency(frequency_);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = std::clamp(frequency, half_sample_rate_ * (TSample)-0.999, half_sample_rate_ * (TSample)0.999);
    step_ = frequency_ * inv_sample_rate_;
    wave_step_ = frequency_ * inv_sample_rate_ * inv_duty_cycle_;

    if (frequency != (TSample)0.0)
    {
        harmonics_ = std::min((TSample)30.0, floor(half_sample_rate_ / abs(frequency_)));
    }
    else
    {
        harmonics_ = (TSample)0.0;
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::set_duty_cycle(const TSample &duty_cycle)
{
    duty_cycle_ = std::clamp(duty_cycle, (TSample)0.0, (TSample)1.0);

    if (duty_cycle_ > (TSample)0.0)
    {
        inv_duty_cycle_ = (TSample)1.0 / duty_cycle_;
    }
    else
    {
        inv_duty_cycle_ = (TSample)0.0;
    }

    wave_step_ = frequency_ * inv_sample_rate_ * inv_duty_cycle_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::set_waveform(const PulsarWaveforms &waveform)
{
    waveform_ = waveform;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::set_window(const PulsarWindows &window)
{
    window_ = window;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void Pulsar<TSample>::reset()
{
    ramp_ = 0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Pulsar<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Pulsar<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample Pulsar<TSample>::get_duty_cycle()
{
    return duty_cycle_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
PulsarWaveforms Pulsar<TSample>::get_waveform()
{
    return waveform_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
PulsarWindows Pulsar<TSample>::get_window()
{
    return window_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Pulsar<TSample>::run()
{
    output_ = 0.0;

    ramp_ += step_;
    if (ramp_ > duty_cycle_)
    {
        gen_wave_ = false;
    }
    if (abs(ramp_) > 1.0)
    {
        while (abs(ramp_) > 1.0)
        {
            ramp_ -=  1.0 * copysign(1.0, ramp_);
        }

        wave_ramp_ =  ramp_ * inv_duty_cycle_;

        gen_wave_ = true;
    }

    if (gen_wave_)
    {
        wave_ramp_ += wave_step_;
        if (abs(wave_ramp_) > (TSample)1.0)
        {
            gen_wave_ = false;
            return output_;
        }

        switch (waveform_)
        {
        case PulsarWaveforms::sine:
            output_ = sin(wave_ramp_ * double_pi_);
            break;
        case PulsarWaveforms::saw:
            for (TSample harmonic = (TSample)1.0; harmonic <= harmonics_; harmonic++)
            {
                output_ += std::sin((-wave_ramp_ + (TSample)0.5) * double_pi_ * harmonic) / harmonic;
            }
            output_ *= (TSample)0.55;
            break;
        case PulsarWaveforms::square:
            for (TSample harmonic = (TSample)1.0; harmonic <= harmonics_; harmonic += (TSample)2.0)
            {
                output_ += std::sin(wave_ramp_ * double_pi_ * harmonic) / harmonic;
            }
            output_ *= (TSample)1.07;
            break;
        case PulsarWaveforms::triangle:
            for (TSample harmonic = (TSample)1.0; harmonic <= harmonics_; harmonic += (TSample)2.0)
            {
                output_ += std::cos((wave_ramp_ + (TSample)0.75) * double_pi_ * harmonic) / (harmonic * harmonic);
            }
            output_ *= (TSample)0.82;
            break;
        case PulsarWaveforms::noise:
            output_ = ((TSample)rand() * inv_rand_max2_) - (TSample)1.0;
            break;
        case PulsarWaveforms::dc:
            output_ = (TSample)1.0;
            break;
        case PulsarWaveforms::phasor:
            output_ = wave_ramp_;
            break;
        }

        switch (window_)
        {
        case PulsarWindows::hann:
            output_ *= hann(wave_ramp_);
            break;
        case PulsarWindows::hamming:
            output_ *= hamming(wave_ramp_);
            break;
        case PulsarWindows::blackman:
            output_ *= blackman(wave_ramp_);
            break;
        case PulsarWindows::nuttall:
            output_ *= nuttall(wave_ramp_);
            break;
        case PulsarWindows::blackmannuttall:
            output_ *= blackmannuttall(wave_ramp_);
            break;
        case PulsarWindows::blackmanharris:
            output_ *= blackmanharris(wave_ramp_);
            break;
        case PulsarWindows::flattop:
            output_ *= flattop(wave_ramp_);
            break;
        case PulsarWindows::bartletthann:
            output_ *= bartletthann(wave_ramp_);
            break;
        }
    }

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample Pulsar<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // PULSAR_H_
