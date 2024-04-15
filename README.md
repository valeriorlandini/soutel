# soutel
SOUnd TEmplate Library - C++ template library for sound generation and processing. 

Currently available headers are:

* `addosc.h` Additive oscillator with 32 harmonics
* `allpass.h` Delay based allpass filter
* `biquad.h` Second order filters (lowpass, hipass, bandpass, bandreject, allpass, lowshelf, hishelf, peak)
* `blosc.h` Band limited multishape oscillator
* `chebyshev.h` Chebyshev polynomials based waveshaper
* `comb.h` Delay based comb filter (feedforward and feedback)
* `cryptoverb.h` Allpass and comb filters based reverberation
* `delay.h` Delay with sample interpolation
* `distortions.h` A collection of distortions and overdrive algorithms
* `ecaosc.h` Oscillator based on elementary cellular automata
* `interp.h` Interpolation algorithms
* `lorenz.h` Lorenz attractor based oscillator
* `pdosc.h` Phase distortion oscillator
* `pulsar.h` Pulsar oscillator
* `randsig.h` Random signal generator with controllable frequency
* `roessler.h` Rössler attractor based oscillator
* `simpleosc.h` Simple non bandlimited multishape oscillator
* `soutel.h` Utility header file that includes all the single modules
* `window_functions.h` A series of window functions
* `wtosc.h` Wavetable oscillator

### Tutorial

To use the classes of this library, you just have to include the corresponding file(s). For example, let's create a bandlimited oscillator and filter it with a lowpass filter.
Include the necessary files, and use the namespace:
```cpp
#include "blosc.h"
#include "biquad.h"
     
using namespace soutel;
```

Then, instantiate the classes, specifying the optional arguments if you wish:
```cpp
BLOsc<double> oscillator(44100.0, 440.0); // arguments: sample rate and frequency
Biquad<double> filter(44100.0, 880.0, 2.0, 1.0, BQFilters::lowpass); // arguments: sample rate, cutoff, q, gain (for shelving filters), filter type
```

Finally, call `run()` function. For the bandlimited oscillator, since it produces different waveforms, you have to ask for the desired shape or pass variables to store the different shapes:
```cpp
oscillator.run(); // alternatively, you can pass four variables to run(), where sine, triangle, saw and square output are stored
double filtered_sample = filter.run(oscillator.get_saw());
```

A more detailed tutorial is coming sometime soon. In the meanwhile, you can check how many of these classes are implemented in Max/MSP objects in my open source package [sonus](https://github.com/valeriorlandini/sonus).
