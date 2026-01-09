/******************************************************************************
Copyright (c) 2023-2026 Valerio Orlandini

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

#include <array>

#include "interp.h"

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
struct harmonic_state
{
    TSample gain;
    TSample phase;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class AddOsc
{
public:
    AddOsc(const TSample &sample_rate = (TSample)44100.0,
           const TSample &frequency = (TSample)0.0,
           const int &harmonics = 16,
           const bool &normalize = true);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_normalize(const bool &normalize);
    void set_harmonics(const int &harmonics);
    void set_harmonics_gain(const std::vector<TSample> &harmonics_gain);
    void set_harmonics_phase(const std::vector<TSample> &harmonics_phase);
    bool set_harmonic_state(const int &index, const TSample &gain, const TSample &phase);
    bool set_harmonic_gain(const int &index, const TSample &gain);
    bool set_harmonic_phase(const int &index, const TSample &phase);
    void reset();

    TSample get_sample_rate();
    TSample get_frequency();
    bool get_normalize();
    int get_harmonics();
    std::vector<TSample> get_harmonics_gain();
    std::vector<TSample> get_harmonics_phase();
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

    bool normalize_;
    TSample norm_factor_ = (TSample)1.0;
    TSample past_norm_factor_;
    TSample interp_;

    TSample output_;

    std::vector<harmonic_state<TSample>> harmonics_state_;

    const TSample double_pi_ = (TSample)(M_PI * 2.0);

    inline void normalize_gains();
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
AddOsc<TSample>::AddOsc(const TSample &sample_rate, const TSample &frequency, const int &harmonics, const bool &normalize)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    if (harmonics > 0 && harmonics <= 256)
    {
        set_harmonics(harmonics);
    }
    else
    {
        set_harmonics(16);
    }

    if (!harmonics_state_.empty())
    {
        for (int h = harmonics_state_.size() - 1; h >= 0; h--)
        {
            harmonics_state_[h].gain = h ? (TSample)0.0 : (TSample)1.0;
            harmonics_state_[h].phase = (TSample)0.0;
        }
    }

    set_normalize(normalize);

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
void AddOsc<TSample>::set_harmonics(const int &harmonics)
{
    harmonic_state<TSample> def_value;
    def_value.gain = (TSample)0.0;
    def_value.phase = (TSample)0.0;

    if (harmonics > 0 && harmonics <= 256)
    {
        harmonics_state_.resize(harmonics, def_value);
    }
    else
    {
        harmonics_state_.resize(16, def_value);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_normalize(const bool &normalize)
{
    normalize_ = normalize;

    if (normalize_)
    {
        normalize_gains();
    }
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
        harmonics_ = std::min(int(harmonics_state_.size()), int(std::floor(half_sample_rate_ / std::abs(frequency_))));
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
void AddOsc<TSample>::set_harmonics_gain(const std::vector<TSample> &harmonics_gain)
{
    auto harmonics_gain_size = harmonics_gain.size();
    for (auto h = 0; h < harmonics_gain_size; h++)
    {
        if (h < harmonics_state_.size())
        {
            harmonics_state_[h].gain = harmonics_gain.at(h);
        }
    }

    if (normalize_)
    {
        normalize_gains();
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void AddOsc<TSample>::set_harmonics_phase(const std::vector<TSample> &harmonics_phase)
{
    auto harmonics_phase_size = harmonics_phase.size();
    for (auto h = 0; h < harmonics_phase_size; h++)
    {
        if (h < harmonics_state_.size())
        {
            harmonics_state_[h].phase = harmonics_phase.at(h);
        }
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
bool AddOsc<TSample>::set_harmonic_state(const int &index, const TSample &gain, const TSample &phase)
{
    if (index < harmonics_state_.size())
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
    if (index < harmonics_state_.size())
    {
        harmonics_state_[index].gain = gain;

        if (normalize_)
        {
            normalize_gains();
        }

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
    if (index < harmonics_state_.size())
    {
        harmonics_state_[index].phase = phase;

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
bool AddOsc<TSample>::get_normalize()
{
    return normalize_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
int AddOsc<TSample>::get_harmonics()
{
    return harmonics_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<TSample> AddOsc<TSample>::get_harmonics_gain()
{
    std::vector<TSample> gains;

    for (auto const &s : harmonics_state_)
    {
        gains.push_back(s.gain);
    }
    return gains;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<TSample> AddOsc<TSample>::get_harmonics_phase()
{
    std::vector<TSample> phases;

    for (auto const &s : harmonics_state_)
    {
        phases.push_back(s.phase);
    }
    return phases;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<TSample, 2> AddOsc<TSample>::get_harmonic_state(const int &index)
{
    std::array<TSample, 2> state{(TSample)0.0, (TSample)0.0};

    if (index >= 0 && index < harmonics_state_.size())
    {
        state[0] = harmonics_state_[index].gain;
        state[1] = harmonics_state_[index].phase;
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
        output_ += harmonics_state_[h].gain * std::cos(ramp_ * double_pi_ * harmonic + (harmonics_state_[h].phase * double_pi_));
    }

    if (normalize_)
    {
        interp_ = interp_ < (TSample)1.0 ? (interp_ + (TSample)0.01) : (TSample)1.0;
        output_ *= linip(past_norm_factor_, norm_factor_, interp_);
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

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void AddOsc<TSample>::normalize_gains()
{
    past_norm_factor_ = norm_factor_;
    norm_factor_ = (TSample)1.0;
    TSample total_gain_ = (TSample)0.0;

    if (harmonics_state_.size() > 0)
    {
        for (auto h = 0; h < harmonics_state_.size(); h++)
        {
            total_gain_ += harmonics_state_[h].gain;
        }
    }

    if (std::abs(total_gain_) > (TSample)1.0)
    {
        norm_factor_ = (TSample)1.0 / total_gain_;
    }

    interp_ = 0.0;
}

}

#endif // ADDOSC_H_
