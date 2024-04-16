/******************************************************************************
Copyright (c) 2023-2024 Valerio Orlandini

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


#ifndef ADDOSC_H_
#define ADDOSC_H_

#include <algorithm>
#include <array>
#include <cmath>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class BLWaveforms
{
    sine,
    triangle,
    saw,
    square
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class AddOsc
{
public:
    AddOsc(const TSample &sample_rate = (TSample)44100.0,
          const TSample &frequency = (TSample)0.0);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_harmonics_gain(const std::array<TSample, 32> &harmonics_gain);
    void set_harmonics_phase(const std::array<TSample, 32> &harmonics_phase);
    bool set_harmonic_state(const int &index, const TSample &gain, const TSample &phase);
    bool set_harmonic_gain(const int &index, const TSample &gain);
    bool set_harmonic_phase(const int &index, const TSample &phase);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();
    std::array<TSample, 32> get_harmonics_gain();
    std::array<TSample, 32> get_harmonics_phase();
    std::array<TSample, 2> get_harmonic_state(const int &index);

    inline TSample run();

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample step_;
    TSample ramp_;

    int harmonics_;

    TSample output_;

    std::array<TSample, 32> harmonics_gain_;
    std::array<TSample, 32> harmonics_phase_;

    const TSample double_pi_ = (TSample)(M_PI * 2.0);
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
AddOsc<TSample>::AddOsc(const TSample &sample_rate, const TSample &frequency)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    reset();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;
    inv_sample_rate_ = (TSample)1.0 / sample_rate_;

    set_frequency(frequency_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = std::clamp(frequency, half_sample_rate_ * (TSample)-0.999, half_sample_rate_ * (TSample)0.999);

    step_ = frequency_ * inv_sample_rate_;

    if (frequency != (TSample)0.0)
    {
        harmonics_ = std::min(32, int(std::floor(half_sample_rate_ / std::abs(frequency_))));
    }
    else
    {
        harmonics_ = 0;
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_harmonics_gain(const std::array<TSample, 32> &harmonics_gain)
{
    harmonics_gain_ = harmonics_gain;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_harmonics_phase(const std::array<TSample, 32> &harmonics_phase)
{
    harmonics_phase_ = harmonics_phase;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool AddOsc<TSample>::set_harmonic_state(const int &index, const TSample &gain, const TSample &phase)
{
    if (index < 32)
    {
        set_harmonic_gain(index, gain);
        set_harmonic_phase(index, phase);

        return true;
    }
    
    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool AddOsc<TSample>::set_harmonic_gain(const int &index, const TSample &gain)
{
    if (index < 32)
    {
        harmonics_gain_[index] = gain;

        return true;
    }
    
    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool AddOsc<TSample>::set_harmonic_phase(const int &index, const TSample &phase)
{
    if (index < 32)
    {
        harmonics_phase_[index] = phase;

        return true;
    }
    
    return false;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::reset()
{
    ramp_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample AddOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample AddOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<TSample, 32> AddOsc<TSample>::get_harmonics_gain()
{
    return harmonics_gain_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<TSample, 32> AddOsc<TSample>::get_harmonics_phase()
{
    return harmonics_phase_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<TSample, 2> AddOsc<TSample>::get_harmonic_state(const int &index)
{
    std::array<TSample, 2> state{(TSample)0.0, (TSample)0.0};

    if (index >= 0 && index < 32)
    {
        state[0] = harmonics_gain_[index];
        state[1] = harmonics_phase_[index];
    }

    return state;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample AddOsc<TSample>::run()
{
    ramp_ += step_;
    if (ramp_ > (TSample)1.0)
    {
        while (ramp_ > (TSample)1.0)
        {
            ramp_ -=  (TSample)1.0;
        }
    }

    output_ = (TSample)0.0;

    for (auto h = 0; h < harmonics_; h++)
    {
        TSample harmonic = (TSample)(h + 1);
        output_ += harmonics_gain_[h] * (std::sin(harmonics_phase_[h] * ramp_ * double_pi_ * harmonic) / harmonic);
    }

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample AddOsc<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // ADDOSC_H_