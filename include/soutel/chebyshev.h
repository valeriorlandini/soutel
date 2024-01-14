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

#ifndef CHEBYSHEV_H_
#define CHEBYSHEV_H_

#include "interp.h"

#if __cplusplus >= 202002L
#include<concepts>
#endif

namespace soutel
{

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample chebyshev(const TSample &input, const unsigned int &order)
{
    TSample output = (TSample)0.0;

    switch (order)
    {
    case 0:
        output = (TSample)1.0;
        break;
    case 1:
        output = input;
        break;
    case 2:
        output = (TSample)2.0 * pow(input, (TSample)2.0) -
                 (TSample)1.0;
        break;
    case 3:
        output = (TSample)4.0 * pow(input, (TSample)3.0) -
                 (TSample)3.0 * input;
        break;
    case 4:
        output = (TSample)8.0 * pow(input, (TSample)4.0) -
                 (TSample)8.0 * pow(input, (TSample)2.0) +
                 (TSample)1.0;
        break;
    case 5:
        output = (TSample)16.0 * pow(input, (TSample)5.0) -
                 (TSample)20.0 * pow(input, (TSample)3.0) +
                 (TSample)5.0 * input;
        break;
    case 6:
        output = (TSample)32.0 * pow(input, 6.0) -
                 (TSample)48.0 * pow(input, (TSample)4.0) +
                 (TSample)18.0 * pow(input, (TSample)2.0) -
                 (TSample)1.0;
        break;
    case 7:
        output = (TSample)64.0 * pow(input, (TSample)7.0) -
                 (TSample)112.0 * pow(input, (TSample)5.0) +
                 (TSample)56.0 * pow(input, (TSample)3.0) -
                 (TSample)7.0 * input;
        break;
    case 8:
        output = (TSample)128.0 * pow(input, (TSample)8.0) -
                 (TSample)256.0 * pow(input, (TSample)6.0) +
                 (TSample)160.0 * pow(input, (TSample)4.0) -
                 (TSample)32.0 * pow(input, (TSample)2.0) +
                 (TSample)1.0;
        break;
    case 9:
        output = (TSample)256.0 * pow(input, (TSample)9.0) -
                 (TSample)576.0 * pow(input, (TSample)7.0) +
                 (TSample)432.0 * pow(input, (TSample)5.0) -
                 (TSample)120.0 * pow(input, (TSample)3.0) +
                 (TSample)9.0 * input;
        break;
    case 10:
        output = (TSample)512.0 * pow(input, (TSample)10.0) -
                 (TSample)1280.0 * pow(input, (TSample)8.0) +
                 (TSample)1120.0 * pow(input, (TSample)6.0) -
                 (TSample)400.0 * pow(input, (TSample)4.0) +
                 (TSample)50.0 * pow(input, (TSample)2.0) -
                 (TSample)1.0;
        break;
    }

    return output;
}

template <typename TSample>
#if __cplusplus >= 202002L
requires std::floating_point<TSample>
#endif
inline TSample chebyshev(const TSample &input, const TSample &order)
{
    TSample out_1 = chebyshev(input, (unsigned int)floor(order));
    TSample out_2 = chebyshev(input, (unsigned int)ceil(order));

    return cosip(out_1, out_2, order - floor(order));
}

}

#endif // CHEBYSHEV_H_
