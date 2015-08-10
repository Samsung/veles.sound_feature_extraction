# veles.sound_feature_extraction
C++ library to extract various features from audio signal. Additionally, provides some DSP functions.

Designed without any OS portability in mind, tested only on Linux. Can work on BSD/Darwin after some potentially minor adaptation. Supports Android build.

Written with heavy usage of C++11 and can be built with either GCC or Clang.
Incorporates [simd](https://github.com/Samsung/veles.simd) and [FFTF](https://github.com/Samsung/FFTF) libraries. IIR filtering is implemented on top of
[DSPFilters](https://github.com/vmarkovtsev/DSPFilters).

Employs a modular architechture, where any feature extraction pipeline is constructed from dedicated, specialized units called "transforms".
The unique feature of this library is that several different features can be extracted at the same time, resulting in a tree of transforms,
which reuse the buffer space and are properly scheduled thanks to an advanced allocation and schedule subsystem.

There are Python bindings.

### Implemented features

The list of available transforms is at [TRANSFORMS.md](https://github.com/Samsung/veles.sound_feature_extraction/blob/master/TRANSFORMS.md).
Using them, it is possible to construct many algorithms, such as speech pitch detection similar to [YAAPT](http://www.ws.binghamton.edu/zahorian/yaapt.htm),
speech features extraction, from MFCC and PLP to complex wavelet packets, prosodic features calculation, FIR and IIR filtering, song tempo detection, etc.

Some of the examples are in tests.

### Building
```
./autogen.sh
mkdir build && cd build
../configure
make -j$(getconf _NPROCESSORS_ONLN)
make test
make install DESTDIR=...
```

### Copyright
Copyright © 2013 Samsung R&D Institute Russia

### License
[Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0).

