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

#ifndef ECAOSC_H_
#define ECAOSC_H_

#include <algorithm>
#include <array>
#include <climits>
#include <random>
#include <vector>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class ECAOsc
{
public:
    ECAOsc(const TSample &sample_rate = (TSample)44100.0,
           const TSample &frequency = (TSample)1.0,
           const unsigned int &cells = 32u,
           const std::array<bool, 8> &rules = {false, true, true, false, true, true, true, false},
           const bool &randomize_cells = false);

    ECAOsc(const TSample &sample_rate = (TSample)44100.0,
           const TSample &frequency = (TSample)1.0,
           const unsigned int &cells = 32u,
           const uint8_t &rule_number = 110,
           const bool &randomize_cells = false);

    void set_sample_rate(const TSample &sample_rate);
    void set_frequency(const TSample &frequency);
    void set_cells_number(const unsigned int &cells_number, const bool &clear = true);
    void set_cells_status(const std::vector<bool> &status);
    void set_cell_status(const bool &status, const unsigned int &cell);
    void set_rules(const std::array<bool, 8> &rules);
    void set_rule_number(const uint8_t &rule_number);

    void randomize_status(const TSample &alive_chance = (TSample)0.5);

    std::array<bool, 8> get_rules();
    uint8_t get_rule_number();
    std::vector<bool> get_cells();

    TSample get_sample_rate();
    TSample get_frequency();

    void reset();

    inline TSample run();

    inline TSample get_last_sample();

    inline void step();

    inline TSample cells_to_float();

private:
    TSample sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample sample_count_;
    TSample steps_;

    std::array<bool, 8> rules_;
    std::vector<bool> cell_status_;

    bool big_endian_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<TSample> rand_dist_;

    TSample current_;
    TSample next_;
    TSample output_;
};

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
ECAOsc<TSample>::ECAOsc(const TSample &sample_rate, const TSample &frequency,
                        const unsigned int &cells, const std::array<bool, 8> &rules,
                        const bool &randomize_cells)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    set_rules(rules);

    set_cells_number(cells, true);

    gen_.seed(rd_());

    reset();

    if (randomize_cells)
    {
        randomize_status();
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
ECAOsc<TSample>::ECAOsc(const TSample &sample_rate, const TSample &frequency,
                        const unsigned int &cells, const uint8_t &rule_number,
                        const bool &randomize_cells)
{
    frequency_ = frequency;

    set_sample_rate(sample_rate);

    set_rule_number(rule_number);

    set_cells_number(cells, true);

    gen_.seed(rd_());

    reset();

    if (randomize_cells)
    {
        randomize_status();
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    sample_rate_ = std::max((TSample)1.0, sample_rate);
    half_sample_rate_ = sample_rate_ * (TSample)0.5;

    set_frequency(frequency_);
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = std::clamp(frequency, (TSample)0.001, half_sample_rate_);

    steps_ = ceil(sample_rate_ / frequency_);
    sample_count_ = (TSample)0.0;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_cells_number(const unsigned int &cells_number, const bool &clear)
{
    if (clear)
    {
        cell_status_.assign(std::max(1u, cells_number), false);
    }
    else
    {
        cell_status_.resize(std::max(1u, cells_number), false);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_rules(const std::array<bool, 8> &rules)
{
    for (int r = 0; r < 8; r++)
    {
        rules_[r] = rules[r];
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_rule_number(const uint8_t &rule_number)
{
    for (int b = 0; b < 8; b++)
    {
        rules_[b] = ((rule_number >> b) & 1);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_cells_status(const std::vector<bool> &status)
{
    for (int s = 0; s < std::min(status.size(), cell_status_.size()); s++)
    {
        set_cell_status(status.at(s), s);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::set_cell_status(const bool &status, const unsigned int &cell)
{
    if (cell < cell_status_.size())
    {
        cell_status_.at(cell) = status;
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::randomize_status(const TSample &alive_chance)
{
    for (int s = 0; s < cell_status_.size(); s++)
    {
        cell_status_.at(s) = (rand_dist_(gen_) <= alive_chance);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample ECAOsc<TSample>::get_sample_rate()
{
    return sample_rate_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
TSample ECAOsc<TSample>::get_frequency()
{
    return frequency_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::array<bool, 8> ECAOsc<TSample>::get_rules()
{
    return rules_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
uint8_t ECAOsc<TSample>::get_rule_number()
{
    char rule_number = 0;
    for (int r = 0; r < 8; r++)
    {
        rule_number += pow(2, rules_[7 - r]);
    }

    return (uint8_t)rule_number;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
std::vector<bool> ECAOsc<TSample>::get_cells()
{
    return cell_status_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
void ECAOsc<TSample>::reset()
{
    output_ = (TSample)0.0;
    current_ = (TSample)0.0;
    step();
    next_ = cells_to_float();
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample ECAOsc<TSample>::run()
{
    ++sample_count_;

    if (sample_count_ > steps_)
    {
        current_ = next_;
        step();
        next_ = cells_to_float();
        sample_count_ = (TSample)0.0;
    }

    TSample ratio = sample_count_ / steps_;

    output_ = current_ * ((TSample)1.0 - ratio) + next_ * ratio;

    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample ECAOsc<TSample>::get_last_sample()
{
    return output_;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline void ECAOsc<TSample>::step()
{
    std::vector<bool> next_gen;

    for (int c = 0; c < cell_status_.size(); c++)
    {
        unsigned int value = 0;
        value += (unsigned int)cell_status_.at(std::max(0, c - 1)) * 4;
        value += (unsigned int)cell_status_.at(c) * 2;
        value += (unsigned int)cell_status_.at((c + 1) % cell_status_.size());

        next_gen.push_back(rules_[value]);
    }

    for (int c = 0; c < cell_status_.size(); c++)
    {
        cell_status_.at(c) = next_gen.at(c);
    }
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample ECAOsc<TSample>::cells_to_float()
{
    TSample output = (TSample)0.0;
    int bits_TSample = sizeof(TSample) * CHAR_BIT;

    int last_cell = std::min(bits_TSample, (int)cell_status_.size());
    for (int b = 0; b < last_cell; b++)
    {
        if (cell_status_.at(b))
        {
            output += pow((TSample)2.0, (TSample)(last_cell - b - 1));
        }
    }

    return ((TSample)2.0 * output / (TSample)((1 << last_cell) - 1)) - (TSample)1.0;
}

}

#endif // ECAOSC_H_
