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

#ifndef ROESSLER_H_
#define ROESSLER_H_

#include <algorithm>

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
class Roessler
{
public:
    Roessler(const TSample &a = (TSample)0.2,
             const TSample &b = (TSample)0.2,
             const TSample &c = (TSample)5.7,
             const TSample &x = (TSample)0.1,
             const TSample &y = (TSample)0.1,
             const TSample &z = (TSample)0.1,
             const TSample &t = (TSample)0.099)
    {
        set_a(a);
        set_b(b);
        set_c(c);
        set_x(x);
        set_y(y);
        set_z(z);
        set_t(t);
    }

    void set_a(const TSample &a)
    {
        a_ = a;
    }

    void set_b(const TSample &b)
    {
        b_ = b;
    }

    void set_c(const TSample &c)
    {
        c_ = c;
    }

    void set_x(const TSample &x)
    {
        x_ = x;
    }

    void set_y(const TSample &y)
    {
        y_ = y;
    }

    void set_z(const TSample &z)
    {
        z_ = z;
    }

    void set_t(const TSample &t)
    {
        t_ = std::clamp(t, (TSample)0.0, (TSample)0.1);
    }

    TSample get_a()
    {
        return a_;
    }

    TSample get_b()
    {
        return b_;
    }

    TSample get_c()
    {
        return c_;
    }

    TSample get_x()
    {
        return x_;
    }

    TSample get_y()
    {
        return y_;
    }

    TSample get_z()
    {
        return z_;
    }

    inline void step()
    {
        if (t_ > 0.0)
        {
            TSample x_1 = ((TSample)-1.0 * y_) - z_;
            TSample y_1 = x_ + (a_ * y_);
            TSample z_1 = b_ + (z_ * (x_ - c_));

            x_ += t_ * x_1;
            y_ += t_ * y_1;
            z_ += t_ * z_1;

            if (isnan(x_) || isnan(y_) || isnan(z_))
            {
                x_ = (TSample)0.1;
                y_ = (TSample)0.1;
                z_ = (TSample)0.1;
            }
        }
    }

    inline void step(TSample &x, TSample &y, TSample &z)
    {
        step();

        x = x_;
        y = y_;
        z = z_;
    }

private:
    TSample a_, b_, c_;
    TSample x_, y_, z_;
    TSample t_;
};

}

#endif // ROESSLER_H_