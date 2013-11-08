/*---------------------------------------------------------------------------*\
Original Copyright
	FILE........: AK2LSPD.H
	TYPE........: Turbo C header file
	COMPANY.....: Voicetronix
	AUTHOR......: James Whitehall
	DATE CREATED: 21/11/95

Modified by Vadim Markovtsev <v.markovtsev@samsung.com>

    This file contains functions for converting Linear Prediction
    Coefficients (LPC) to Line Spectral Pair (LSP) and back. LSP coefficients
    are first calculated in the x domain of the unit circle (Chebyshev polynom
    variable) and then converted to frequency domain (radians) after applying
    acos().

\*---------------------------------------------------------------------------*/
/*!
 *  @file lsp.h
 *  @brief Line Spectral Pair (LSP) functions.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_PRIMITIVES_LSP_H_
#define SRC_PRIMITIVES_LSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Converts Linear Prediction Coefficients (LPC) to more robust
/// Line Spectral Pairs (LSP) representation.
/// @author David Rowe
/// @date 24/2/93
/// @param simd Value indicating whether to use SIMD acceleration.
/// @param lpc LPC coefficients.
/// @param length The size of lpc (in float-s, not in bytes).
/// @param bisects The number of sub-intervals (e.g., 4) for root refinement.
/// @param delta The grid spacing interval (e.g., 0.02). The smaller the value
/// is, the less probability is to skip a root, but the slower calculations.
/// @param freq The resulting LSP frequencies in w domain.
/// @return The number of roots found.
/// @details
///  Introduction to Line Spectrum Pairs (LSPs)
///  ------------------------------------------
///
///  LSPs are used to encode the LPC filter coefficients {lpc} for
///  transmission over the channel.  LSPs have several properties (like
///  less sensitivity to quantisation noise) that make them superior to
///  direct quantization of {lpc}.
///
///  A(z) is a polynomial of order length with {lpc} as the coefficients.
///
///  A(z) is transformed to P(z) and Q(z) (using a substitution and some
///  algebra), to obtain something like:
///
///    A(z) = 0.5[P(z)(z+z^-1) + Q(z)(z-z^-1)]  (1)
///
///  As you can imagine A(z) has complex zeros all over the z-plane. P(z)
///  and Q(z) have the very neat property of only having zeros _on_ the
///  unit circle.  So to find them we take a test point z=exp(jw) and
///  evaluate P (exp(jw)) and Q(exp(jw)) using a grid of points between 0
///  and pi.
///
///  The zeros (roots) of P(z) also happen to alternate, which is why we
///  swap coefficients as we find roots.  So the process of finding the
///  LSP frequencies is basically finding the roots of 5th order
///  polynomials.
///
///  The root so P(z) and Q(z) occur in symmetrical pairs at +/-w, hence
///  the name Line Spectrum Pairs (LSPs).
///
///  To convert back to lpc we just evaluate (1), "clocking" an impulse
///  through it length times gives us the impulse response of A(z) which is
///  {lpc}.
/// @see The Computation of Line Spectral Frequencies Using Chebyshev
/// Polynomials, IEEE Transactions On Acoustics, Speech and Signal Processing,
/// vol. ASSP-34, no. 6, December 1986, by Peter Kabal and Ravi Prakash
/// Ramachandran.
int lpc_to_lsp(int simd, const float *lpc, int length, int bisects, float delta,
               float *freq);

/// @brief Converts LSP coefficients to LPC coefficients.
/// @author David Rowe
/// @date 24/2/93
/// @param simd Value indicating whether to use SIMD acceleration.
/// @param freq The array of LSP frequencies in the w domain.
/// @param lpc The resulting array of LPC coefficients.
/// @param length The number of LPC coefficients.
void lsp_to_lpc(int simd, const float *freq, int length, float *lpc);

/// @brief Ensures the LSPs are stable.
/// @author Jean-Marc Valin
void lsp_enforce_margin(int length, float margin, float *lsp);

#ifdef __cplusplus
}
#endif

#endif	/* SRC_PRIMITIVES_LSP_H_ */
