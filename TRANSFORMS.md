Autocorrelation
===============

Find the cross-correlation of a signal with itself.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: normalize
    Description: Calculate normalized autocorrelation by dividing each value by lag 0 result (squared signal sum).
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Bandpass
========

Bandpass filter. Cuts off frequences higher than "frequency_high" and lower than "frequency_low".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: frequency_high
    Description: All frequencies higher will be cut.
    Default: 24000

    Name: frequency_low
    Description: All frequencies lower will be cut.
    Default: 1

    Name: length
    Description: Filter size in samples (order).
    Default: 256

    Name: ripple
    Description: Ripple level in dB (used by a subset of filter types).
    Default: 1.000000

    Name: rolloff
    Description: Rolloff level in dB (used by a subset of filter types).
    Default: 0.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: IIR filter type (bessel, butterworth, etc.).
    Default: chebyshevII



Beat
====

Find the tempo of a musical signal.

###Input format
float*
###Output format
FixedArray<(unsigned char)2, float>*
###Supported parameters
    Name: bands
    Description: The number of bands to sum.
    Default: 1

    Name: debug
    Description: Dump the resulting energy vectors.
    Default: 0

    Name: max_bpm
    Description: Maximal number of beats per minute.
    Default: 240.000000

    Name: min_bpm
    Description: Minimal number of beats per minute.
    Default: 60.000000

    Name: peaks
    Description: The number of the most significant peaks to record.
    Default: 3

    Name: pulses
    Description: The number of pulses for the convolution.
    Default: 3

    Name: resolution1
    Description: The bpm resolution during the first pass.
    Default: 2.000000

    Name: resolution2
    Description: The bpm resolution during the second pass.
    Default: 0.100000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



C2R
===

Converts each complex number to corresponding real numbers.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Centroid
========

Window's center of mass in frequency domain calculation.

###Input format
float*
###Output format
float
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



ComplexMagnitude
================

Calculates the magnitude of each complex number, that is,    a square root of the sum of squared real and imaginary parts.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Convolve
========

Convolve a raw signal with a window function.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: length
    Description: Filter size in samples (order).
    Default: 256

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: window
    Description: Type of the window. E.g. "rectangular" or "hamming".
    Default: rectangular



DCT
===

Performs Discrete Cosine Transform on the signal.

###Input format
float*
###Output format
float*


DWPT
====

Discrete Wavelet Packet Transform

###Input format
float*
###Output format
float*
###Supported parameters
    Name: order
    Description: The number of coefficients in the wavelet.
    Default: 8

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: tree
    Description: The wavelet packet binary tree fingerprint.
    Default: 3 3 3 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6

    Name: type
    Description: The type of wavelet to apply. Supported values are daub (Daubechies), coif (Coiflet) and sym (Symlet).
    Default: daub



Delta
=====

Get the difference between values of adjacent windows.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: rlength
    Description: The linear regression window length. Only odd values    greater than 1 are accepted.
    Default: 5

    Name: type
    Description: The algorithm to calculate the deltas. Allowed values are "simple" and "regression".
    Default: simple



Diff
====

Differentiate the signal.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: rectify
    Description: Perform half-wave rectification (retain only if the difference is positive).
    Default: 0

    Name: swt
    Description: Smoothly differentiate using Stationary Wavelet Transform (db1) of the specified level. The level must be greater than or equal to 0. If set to zero, this parameter is ignored.
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Energy
======

Sound energy calculation.

###Input format
float*
###Output format
float
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



FilterBank
==========

Converts the signal to the selected psychoacoustic scale (default is mel).

###Input format
float*
###Output format
float*
###Supported parameters
    Name: debug
    Description: Dump the precalculated filter bank.
    Default: 0

    Name: frequency_max
    Description: Maximal frequency of the filter bank.
    Default: 6854.000000

    Name: frequency_min
    Description: Minimal frequency of the filter bank.
    Default: 130.000000

    Name: number
    Description: The number of triangular filters.
    Default: 32

    Name: squared
    Description: Apply the squared filter bank.
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: The type of the scale. Supported values are "linear","mel", "bark" and "midi".
    Default: mel



Flux
====

Measure of spectral change.

###Input format
float*
###Output format
float


Fork
====

Clones the windows, increasing their number by a factor of "factor".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: factor
    Description: Windows number multiplier value.
    Default: 4



FrequencyBands
==============

Cut the frequency bands from sequential windows.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: bands
    Description: Bands configuration. Overrides "number".
    Default:

    Name: filter
    Description: IIR filter type to apply.
    Default: chebyshevII

    Name: lengths
    Description: IIR filter orders. "auto" for automatic selection.
    Default:

    Name: number
    Description: The number of bands. It should be the same as the cloning factor of Fork. You may set the bands configuration by hand with "bands" parameter, "number" will be ignored then.
    Default: 4

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Highpass
========

Highpass filter. All frequences lower than "frequency" shall not pass.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: frequency
    Description: Filter's cutoff frequency.
    Default: 1

    Name: length
    Description: Filter size in samples (order).
    Default: 256

    Name: ripple
    Description: Ripple level in dB (used by a subset of filter types).
    Default: 1.000000

    Name: rolloff
    Description: Rolloff level in dB (used by a subset of filter types).
    Default: 0.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: IIR filter type (bessel, butterworth, etc.).
    Default: chebyshevII



IDCT
====

Inverse of "performs Discrete Cosine Transform on the signal.".

###Input format
float*
###Output format
float*


ILog (float)
============

Inverse of "takes the logarithm from each real value of the signal.".

###Input format
float
###Output format
float
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



ILog (float*)
=============

Inverse of "takes the logarithm from each real value of the signal.".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



IRDFT
=====

Inverse of "performs Discrete Fourier Transform on the input signal (using real FFT).".

###Input format
float*
###Output format
float*


ISquare
=======

Inverse of "squares the signal (window floating point format).".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



IWindow (float*)
================

Inverse of "splits the raw input signal into numerous windows stepping "step" ms with length "length" ms of type "type".".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: count
    Description: The resulting amount of buffers.
    Default: 1

    Name: interleaved
    Description: Interleave windows from sequential input buffers. In inverse mode, treat the windows as interleaved.
    Default: 1

    Name: length
    Description: Window size in samples.
    Default: 512

    Name: step
    Description: Distance between sequential windows in samples.
    Default: 205



IWindow (short*)
================

Inverse of "splits the raw input signal into numerous windows stepping "step" ms with length "length" ms of type "type".".

###Input format
short*
###Output format
short*
###Supported parameters
    Name: count
    Description: The resulting amount of buffers.
    Default: 1

    Name: interleaved
    Description: Interleave windows from sequential input buffers. In inverse mode, treat the windows as interleaved.
    Default: 1

    Name: length
    Description: Window size in samples.
    Default: 512

    Name: step
    Description: Distance between sequential windows in samples.
    Default: 205



Identity
========

Copy the input to output.

###Input format
identity
###Output format
identity


LPC
===

Calculate Linear Prediction Coefficients (LPC) using Levinson-Durbin recursion from autocorrelation coefficients.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: error
    Description: Include total estimation error
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



LPCtoCC
=======

Convert Linear Prediction Coefficients (LPC) to Cepstral Coefficients.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: size
    Description: The number of cepstral coefficients. 0 means original LPC length.
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



LSP
===

Converts Linear Prediction Coefficients (LPC) to more robust Line Spectral Pairs (LSP) representation.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: bisects
    Description: The number of bisections for the root value refinement. The bigger the number, the more precise the values are and the less probability is to skip a root.
    Default: 16

    Name: intervals
    Description: The number of intervals to scan in the range [-1, 1]. The corresponding delta equals to 2 / intervals. The bigger the number, the less probability is to skip a root.
    Default: 128

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Log (float)
===========

Takes the logarithm from each real value of the signal.

###Input format
float
###Output format
float
###Supported parameters
    Name: add1
    Description: Add 1 to values before taking the logarithm. This trick avoids getting NaNs on zeros.
    Default: 1

    Name: base
    Description: Logarithm base (2, 10 or e).
    Default: e

    Name: scale
    Description: The number to multiply each value by before taking the logarithm.
    Default: 1.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Log (float*)
============

Takes the logarithm from each real value of the signal.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: add1
    Description: Add 1 to values before taking the logarithm. This trick avoids getting NaNs on zeros.
    Default: 1

    Name: base
    Description: Logarithm base (2, 10 or e).
    Default: e

    Name: scale
    Description: The number to multiply each value by before taking the logarithm.
    Default: 1.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Lowpass
=======

Lowpass filter. All frequences higher than "frequency" shall not pass.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: frequency
    Description: Filter's cutoff frequency.
    Default: 1

    Name: length
    Description: Filter size in samples (order).
    Default: 256

    Name: ripple
    Description: Ripple level in dB (used by a subset of filter types).
    Default: 1.000000

    Name: rolloff
    Description: Rolloff level in dB (used by a subset of filter types).
    Default: 0.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: IIR filter type (bessel, butterworth, etc.).
    Default: chebyshevII



Mean
====

Window means calculation.

###Input format
float*
###Output format
sound_feature_extraction::formats::FixedArray<(unsigned char)2, float>
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: types
    Description: Mean types to calculate (names separated with spaces).
    Default: arithmetic



Merge
=====

Merge all single-s to one solid array.

###Input format
float
###Output format
float*


Mix
===

Mix stereo audio channels together.

###Input format
short*
###Output format
short*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



PeakAnalysis
============

Peak analysis. This is a modified version of the algorithm from "Automatic Musical Genre Classification Of Audio Signals".

###Input format
FixedArray<(unsigned char)2, float>*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



PeakDynamicProgramming
======================

Accurate average estimation using dynamic programming approach.

###Input format
FixedArray<(unsigned char)2, float>*
###Output format
float
###Supported parameters
    Name: mind_values
    Description: Whether to favor points with higher values.
    Default: 0



Peaks
=====

Peak detection, that is, extraction of maximums and/or minimums from the signal. The first item is the normalized position, the second is the peak value.

###Input format
float*
###Output format
FixedArray<(unsigned char)2, float>*
###Supported parameters
    Name: max_pos
    Description: The real maximal value of the position of each peak.
    Default: 1.000000

    Name: min_pos
    Description: The real minimal value of the position of each peak.
    Default: 0.000000

    Name: number
    Description: The maximal number of peaks to extract. The rest of the slots will be set to (min_pos, 0).
    Default: 3

    Name: sort
    Description: The order in which to sort peaks (position, value, or both).
    Default: 3

    Name: swt_level
    Description: SWT level, that is, the number of smoothing passes."0" means do not do SWT.
    Default: 0

    Name: swt_order
    Description: The number of coefficients in the wavelet.
    Default: 4

    Name: swt_type
    Description: Type of Stationary Wavelet Transform which will be done    before taking the peaks. Supported values are daub (Daubechies), coif (Coiflet) and sym (Symlet). Please note that SWT introduces a phase shift.
    Default: daub

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: The type of the peaks: max, min or all.
    Default: 1



Preemphasis
===========

Filter the signal with a first-order high-pass filter y[n] = 1 - k * x[n - 1].

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: value
    Description: The filter coefficient from range (0..1]. The higher, the more emphasis occurs.
    Default: 0.900000



R2C
===

Converts each real number to complex number (imaginary part is set to zero).

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



RASTA
=====

Perform RASTA filtering.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: length
    Description: Filter size in samples (order).
    Default: 256

    Name: pole
    Description: Pole value. It normally lies within (1, 0.9].
    Default: 0.940000

    Name: ripple
    Description: Ripple level in dB (used by a subset of filter types).
    Default: 1.000000

    Name: rolloff
    Description: Rolloff level in dB (used by a subset of filter types).
    Default: 0.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: IIR filter type (bessel, butterworth, etc.).
    Default: chebyshevII



RDFT
====

Performs Discrete Fourier Transform on the input signal (using real FFT).

###Input format
float*
###Output format
float*


Rectify
=======

Wave rectification to decrease high-frequency content.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Reorder
=======

Reorders the values in the array according to the specified algorithm.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: algorithm
    Description: The way to reorder the values.
    Default: chroma

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Rolloff
=======

Measure of spectral shape.

###Input format
float*
###Output format
float
###Supported parameters
    Name: ratio
    Description: The ratio between the partial sum and the whole sum.
    Default: 0.850000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Rotate
======

Rotate the buffers, so that all elements with the same index get to the same buffer.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



SFM
===

Spectral Flatness Measure calculation.

###Input format
sound_feature_extraction::formats::FixedArray<(unsigned char)2, float>
###Output format
float
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



SHC
===

Apply spectral harmonics correlation.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: harmonics
    Description: The number of harmonics.
    Default: 3

    Name: max
    Description: The maximal frequency to scan.
    Default: 600

    Name: min
    Description: The minimal frequency to scan.
    Default: 50

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: window
    Description: The spectral window length, in Hz.
    Default: 40



STMSN
=====

Calculate short-time mean and scale normalized values, that is,$stmsn_n[i] =
                                                                                                                                                         rac{w_n[i] - \sum_{k=n-L/2}^{n+L/2}{w_k[i]}}{\max_{k=n-L/2}^{n+L/2}{w_k[i]} -\min_{k=n-L/2}^{n+L/2}{w_k[i]}}$.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: length
    Description: The amount of local values to average.
    Default: 300



Selector
========

Selects the specified part of input.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: from
    Description: The anchor of the selection. Can be either "left" or "right".
    Default: left

    Name: length
    Description: The resulting vector length. 0 means the length of the input.
    Default: 0

    Name: select
    Description: The length of picked up input values, The rest will be set to zero. 0 means the length of the output.
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



SpectralEnergy
==============

Calculates the squared magnitude of each complex number, that is, the sum of squared real and imaginary parts.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Square
======

Squares the signal (window floating point format).

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Stats
=====

Calculate statistical measures, such as skew or kurtosis.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: interval
    Description: "Texture" interval, must be bigger than 1. Zero means all buffers.
    Default: 0

    Name: overlap
    Description: Overlap value, cannot be greater than (interval - 1).
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: types
    Description: Stats types to calculate (names separated with spaces, "all" for all).
    Default: average stddev skewness kurtosis



Stretch
=======

Stretch the signal by an integer factor.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: center
    Description: Center the intervals, extending the corners (useful for stretching after splitting into windows.).
    Default: 0

    Name: factor
    Description: Stretch the signal by duplicating its values.
    Default: 1.000000

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



SubbandEnergy
=============

Calculates the subband energies (subbands are defined witha binary tree fingerprint identical to used in DWPT).

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: tree
    Description: The subbands binary tree fingerprint.
    Default: 3 3 3 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6



Subsample
=========

Subsample the signal by an integer factor.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: factor
    Description: Calculate normalized subsampling by dividing each value by lag 0 result (squared signal sum).
    Default: 1

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



UnpackRDFT
==========

Unpacks the result after applying RDFT.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



Window (float*)
===============

Splits the raw input signal into numerous windows stepping "step" ms with length "length" ms of type "type".

###Input format
float*
###Output format
float*
###Supported parameters
    Name: interleaved
    Description: Interleave windows from sequential input buffers. In inverse mode, treat the windows as interleaved.
    Default: 1

    Name: length
    Description: Window size in samples.
    Default: 512

    Name: step
    Description: Distance between sequential windows in samples.
    Default: 205

    Name: type
    Description: Type of the window. E.g. "rectangular" or "hamming".
    Default: hamming



Window (short*)
===============

Splits the raw input signal into numerous windows stepping "step" ms with length "length" ms of type "type".

###Input format
short*
###Output format
short*
###Supported parameters
    Name: interleaved
    Description: Interleave windows from sequential input buffers. In inverse mode, treat the windows as interleaved.
    Default: 1

    Name: length
    Description: Window size in samples.
    Default: 512

    Name: step
    Description: Distance between sequential windows in samples.
    Default: 205

    Name: type
    Description: Type of the window. E.g. "rectangular" or "hamming".
    Default: hamming



WindowFunction
==============

Applies a window function to each window.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: predft
    Description: Apply Discrete Fourier Transform to window function.
    Default: 0

    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8

    Name: type
    Description: Type of the window. E.g. "rectangular" or "hamming".
    Default: hamming



ZeroCrossings (float*)
======================

Number of time domain zero crossings of the signal.

###Input format
float*
###Output format
int
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



ZeroCrossings (short*)
======================

Number of time domain zero crossings of the signal.

###Input format
short*
###Output format
int
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8



ZeroPadding
===========

Pads signal with zeros to make it's length a power of 2.

###Input format
float*
###Output format
float*
###Supported parameters
    Name: threads_number
    Description: The maximal number of OpenMP threads.
    Default: 8


