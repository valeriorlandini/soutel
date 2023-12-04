/******************************************************************************
Copyright (c) 2023 Valerio Orlandini

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

#ifndef ALLPASS_H_
#define ALLPASS_H_

#include "delay.h"

namespace soutel
{

template <class TSample>
class Allpass
{
public:
    Allpass(const TSample &sample_rate = (TSample)44100.0,
            const TSample &max_delay_time = (TSample)1000.0,
            const TSample &delay_time = (TSample)1000.0,
            const TSample &gain = (TSample)0.707);

    void set_sample_rate(const TSample &sample_rate);
    void set_time(const TSample &delay_time);
    void set_max_time(const TSample &max_delay_time, bool clear = true);
    void set_gain(const TSample &gain);
    void clear();

    TSample get_sample_rate();
    TSample get_time();
    int get_samples();
    TSample get_max_time();
    TSample get_gain();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample max_delay_time_;
    TSample delay_time_;
    TSample output_;

    TSample gain_;

    Delay<TSample> ff_delay_;
    Delay<TSample> fb_delay_;
};

template <class TSample>
Allpass<TSample>::Allpass(const TSample &sample_rate,
                          const TSample &max_delay_time,
                          const TSample &delay_time,
                          const TSample &gain)
{
    set_sample_rate(sample_rate);

    set_max_time(max_delay_time);

    set_time(delay_time);

    set_gain(gain);
}

template <class TSample>
void Allpass<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);

    ff_delay_.set_sample_rate(sample_rate_);
    fb_delay_.set_sample_rate(sample_rate_);

    clear();
}

template <class TSample>
void Allpass<TSample>::set_max_time(const TSample &max_delay_time, bool clear)
{
    max_delay_time_ = std::max((TSample)1.0, max_delay_time);

    ff_delay_.set_max_time(max_delay_time_, clear);
    fb_delay_.set_max_time(max_delay_time_, clear);

    if (delay_time_ > max_delay_time_)
    {
        set_time(max_delay_time_);
    }
}

template <class TSample>
void Allpass<TSample>::set_time(const TSample &delay_time)
{
    delay_time_ = std::clamp(delay_time, (TSample)0.0, max_delay_time_);

    ff_delay_.set_time(delay_time_);
    fb_delay_.set_time(delay_time_);
}

template <class TSample>
void Allpass<TSample>::set_gain(const TSample &gain)
{
    gain_ = gain;
}

template <class TSample>
TSample Allpass<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <class TSample>
TSample Allpass<TSample>::get_time()
{
    return delay_time_;
}

template <class TSample>
TSample Allpass<TSample>::get_max_time()
{
    return max_delay_time_;
}

template <class TSample>
TSample Allpass<TSample>::get_gain()
{
    return gain_;
}

template <class TSample>
void Allpass<TSample>::clear()
{
    ff_delay_.clear();
    fb_delay_.clear();
}

template <class TSample>
inline TSample Allpass<TSample>::run(const TSample &input)
{
    TSample out = (TSample)-1.0 * gain_ * input;
    out += ff_delay_.run(input);
    out += gain_ * fb_delay_.run(output_);
    output_ = out;

    return output_;
}

template <class TSample>
inline void Allpass<TSample>::run(const TSample &input, TSample &output)
{
    output_ = run(input);
    output = output_;
}

template <class TSample>
inline TSample Allpass<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // ALLPASS_H_
