# soutel
SOUnd TEmplate Library - C++ template library for sound generation and processing. 

Currently available headers are:

* `addosc.h` Additive oscillator with up to 256 harmonics
* `allpass.h` Delay based allpass filter
* `biquad.h` Second order filters (lowpass, hipass, bandpass, bandreject, allpass, lowshelf, hishelf, peak)
* `blosc.h` Band limited multishape oscillator
* `chebyshev.h` Chebyshev polynomials based waveshaper
* `comb.h` Delay based comb filter (feedforward and feedback)
* `cryptoverb.h` Allpass and comb filters based reverberation
* `delay.h` Delay with sample interpolation
* `descriptors.h` Audio descriptors
* `distortions.h` A collection of distortions and overdrive algorithms
* `ecaosc.h` Oscillator based on elementary cellular automata
* `interp.h` Interpolation algorithms
* `lorenz.h` Lorenz attractor based oscillator
* `pdosc.h` Phase distortion oscillator
* `pulsar.h` Pulsar oscillator
* `randsig.h` Random signal generator with controllable frequency
* `ringmod.h` Ring and amplitude modulator
* `roessler.h` Rössler attractor based oscillator
* `simpleosc.h` Simple non bandlimited multishape oscillator
* `soutel.h` Utility header file that includes all the single modules
* `utils.h` Miscellaneous utility functions, such as unit conversions, zero padding, etc.
* `window_functions.h` A series of window functions
* `wtosc.h` Wavetable oscillator

### Tutorial

To use the classes from this library, simply include the corresponding file(s). For example, let's create a bandlimited oscillator and filter its output using a lowpass filter.

First, include the necessary files and import the namespace:
```cpp
#include "blosc.h"
#include "biquad.h"
     
using namespace soutel;
```

Next, instantiate the classes, providing optional arguments as needed:
```cpp
BLOsc<double> oscillator(44100.0, 440.0); // Arguments: sample rate and frequency
Biquad<double> filter(44100.0, 880.0, 2.0, 1.0, BQFilters::lowpass);  // Arguments: sample rate, cutoff frequency, Q factor, gain (for shelving filters), filter type
```

Finally, call the `run()` function. For the bandlimited oscillator, which can produce different waveforms, you can either specify the desired waveform or pass variables to capture all the available shapes:
```cpp
oscillator.run(); // Alternatively, pass four variables to run() to store the sine, triangle, saw, and square wave outputs
double filtered_sample = filter.run(oscillator.get_saw());
```

A more detailed tutorial is coming sometime soon. In the meanwhile, you can check how many of these classes are implemented in Max/MSP objects in my open source package [sonus](https://github.com/valeriorlandini/sonus).
