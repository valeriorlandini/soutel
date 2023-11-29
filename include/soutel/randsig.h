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

#ifndef RANDSIG_H_
#define RANDSIG_H_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

namespace soutel
{

template <class TSample>
class Randsig
{
public:
    Randsig(const TSample &sample_rate = (TSample)44100.0,
            const TSample &frequency = (TSample)1.0);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);

    TSample get_sample_rate();
    TSample get_frequency();

    void reset();

    inline TSample run();

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample sample_count_;
    TSample steps_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<TSample> rand_dist_;

    TSample current_;
    TSample next_;
    TSample output_;
};

template <class TSample>
Randsig<TSample>::Randsig(const TSample &sample_rate, const TSample &frequency)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    gen_.seed(rd_());

    reset();
}

template <class TSample>
void Randsig<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;

    set_frequency(frequency_);
}

template <class TSample>
void Randsig<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = std::clamp(frequency, (TSample)0.001, half_sample_rate_);

    steps_ = ceil(sample_rate_ / frequency_);
    sample_count_ = (TSample)0.0;
}

template <class TSample>
TSample Randsig<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <class TSample>
TSample Randsig<TSample>::get_frequency()
{
    return frequency_;
}

template <class TSample>
void Randsig<TSample>::reset()
{
    output_ = (TSample)0.0;
    current_ = (TSample)0.0;
    next_ = (rand_dist_(gen_) * (TSample)2.0) - (TSample)1.0;
}

template <class TSample>
inline TSample Randsig<TSample>::run()
{
    ++sample_count_;

    if (sample_count_ > steps_)
    {
        current_ = next_;
        next_ = (rand_dist_(gen_) * (TSample)2.0) - (TSample)1.0;
        sample_count_ = (TSample)0.0;
    }

    TSample ratio = sample_count_ / steps_;

    output_ = current_ * ((TSample)1.0 - ratio) + next_ * ratio;

    return output_;
}

template <class TSample>
inline TSample Randsig<TSample>::get_last_sample()
{
    return output_;
}

}

#endif // RANDSIG_H_
