/*! @file daubechies.h
 *  @brief Coefficients of Daubechies wavelets.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

/*
 * Note: phase.etl.go.jp is now phase.hpcc.jp, without ftp.
 */

/* Coefficients for Daubechies wavelets 1-38
 * -----------------------------------------
 * 2000.08.10 - Some more info added.
 *
 * Computed by Kazuo Hatano, Aichi Institute of Technology.
 * ftp://phase.etl.go.jp/pub/phase/wavelet/index.html
 *
 * Compiled and verified by Olli Niemitalo.
 *
 * Discrete Wavelet Transformation (DWT) breaks a signal down into
 * subbands distributed logarithimically in frequency, each sampled
 * at a rate that has a natural proportion to the frequencies in that
 * band. The traditional fourier transformation has no time domain
 * resolution at all, or when done using many short windows on a
 * longer data, equal resolution at all frequencies. The distribution
 * of samples in the time and frequency domain by DWT is of form:
 *
 * log f
 *  |XXXXXXXXXXXXXXXX  X = a sample
 *  |X X X X X X X X   f = frequency
 *  |X   X   X   X     t = time
 *  |X       X
 *  |X
 *   ----------------t
 *
 * Single
 * subband decomposition         and      reconstruction:
 *
 *     -> high -> decimate -------------> dilute -> high
 *    |   pass    by 2      high subband  by 2      pass \
 * in |                                                   + out
 *    |                                                  /  =in
 *     -> low  -> decimate -------------> dilute -> low
 *        pass    by 2      low subband   by 2      pass
 *
 * This creates two subbands from the input signal, both sampled at half
 * the original frequency. The filters approximate halfband FIR filters
 * and are determined by the choice of wavelet. Using Daubechies wavelets
 * (and most others), the data can be reconstructed to the exact original
 * even when the halfband filters are not perfect. Note that the amount
 * of information (samples) stays the same throughout the operation.
 *
 * Decimation by 2: ABCDEFGHIJKLMNOPQR -> ACEGIKMOQ
 *   Dilution by 2: ACEGIKMOQ -> A0C0E0G0I0K0M0O0Q0
 *
 * To get the logarithmic resolution in frequency, the low subband is
 * re-transformed, and again, the low subband from this transformation
 * gets the same treatment etc.
 *
 * Decomposition:
 *
 *     -> high -> decimate --------------------------------> subband0
 *    |   pass    by 2
 * in |                 -> high -> decimate ---------------> subband1
 *    |                |   pass    by 2
 *     -> low -> decim |                 -> high -> decim -> subband2
 *        pass   by 2  |                |   pass    by 2
 *                      -> low -> decim |
 *                         pass   by 2  |   .   down to what suffices
 *                                       -> .    or if periodic data,
 *                                          .     until short of data
 * Reconstruction:
 *
 * subband0 -----------------------------------> dilute -> high
 *                                               by 2      pass \
 * subband1 ------------------> dilute -> high                   + out
 *                              by 2      pass \                /  =in
 * subband2 -> dilute -> high                   + dilute -> low
 *             by 2      pass \                /  by 2      pass
 *                             + dilute -> low
 * Start   .                  /  by 2      pass
 * here!   . -> dilute -> low
 *         .    by 2      pass
 *
 * In a real-time application, the filters introduce delays, so you need
 * to compensate them by adding additional delays to less-delayed higher
 * bands, to get the summation work as intended.
 *
 * For periodic signals or windowed operation, this problem doesn't exist -
 * a single subband transformation is a matrix multiplication, with wrapping
 * implemented in the matrix:
 *
 * Decomposition:
 *
 * |L0|   |C0  C1  C2  C3                | |I0|     L = lowpass output
 * |H0|   |C3 -C2  C1 -C0                | |I1|     H = highpass output
 * |L1|   |        C0  C1  C2  C3        | |I2|     I = input
 * |H1| = |        C3 -C2  C1 -C0        | |I3|     C = coefficients
 * |L2|   |                C0  C1  C2  C3| |I4|
 * |H2|   |                C3 -C2  C1 -C0| |I5|
 * |L3|   |C2  C3                  C0  C1| |I6|
 * |H3|   |C1 -C0                  C3 -C2| |I7|     Daubechies 4-coef:
 *
 *       1+sqrt(3)         3+sqrt(3)         3-sqrt(3)         1-sqrt(3)
 * C0 = ---------   C1 = ---------   C2 = ---------   C3 = ---------
 *       4 sqrt(2)         4 sqrt(2)         4 sqrt(2)         4 sqrt(2)
 *
 * Reconstruction:
 *
 * |I0|   |C0  C3                  C2  C1| |L0|
 * |I1|   |C1 -C2                  C3 -C0| |H0|
 * |I2|   |C2  C1  C0  C3                | |L1|
 * |I3| = |C3 -C0  C1 -C2                | |H1|
 * |I4|   |        C2  C1  C0  C3        | |L2|
 * |I5|   |        C3 -C0  C1 -C2        | |H2|
 * |I6|   |                C2  C1  C0  C3| |L3|
 * |I7|   |                C3 -C0  C1 -C2| |H3|
 *
 * This file contains the lowpass FIR filter coefficients. Highpass
 * coefficients you get by reversing tap order and multiplying by
 * sequence 1,-1, 1,-1, ... Because these are orthogonal wavelets, the
 * analysis and reconstruction coefficients are the same.
 *
 * A coefficient set convolved by its reverse is an ideal halfband lowpass
 * filter multiplied by a symmetric windowing function. This creates the
 * kind of symmetry in the frequency domain that enables aliasing-free
 * reconstruction. Daubechies wavelets are the minimum-phase, minimum
 * number of taps solutions for a number of vanishing moments (seven in
 * Daub7 etc), which determines their frequency selectivity.
 */

#ifndef SRC_PRIMITIVES_DAUBECHIES_H_
#define SRC_PRIMITIVES_DAUBECHIES_H_

#ifndef WAVELET_INTERNAL_USE
#error This file should not be included directly into mid level code
#endif

extern const double kDaubechiesD[38][76];
extern const float  kDaubechiesF[38][76];

#endif  // SRC_PRIMITIVES_DAUBECHIES_H_
