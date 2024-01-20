# soutel
SOUnd TEmplate Library - C++ template library for sound generation and processing. 

Currently available headers are:

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
A detailed tutorial is coming sometime soon. In the meanwhile, you can check how many of these classes are implemented in Max/MSP objects in my open source package [sonus](https://github.com/valeriorlandini/sonus).
