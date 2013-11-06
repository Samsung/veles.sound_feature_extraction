/*---------------------------------------------------------------------------*\
Original Copyright
	FILE........: AK2LSPD.H
	TYPE........: Turbo C header file
	COMPANY.....: Voicetronix
	AUTHOR......: James Whitehall
	DATE CREATED: 21/11/95

Modified by Vadim Markovtsev <v.markovtsev@samsung.com>

    This file contains functions for converting Linear Prediction
    Coefficients (LPC) to Line Spectral Pair (LSP) and back. Note that the
    LSP coefficients are not in radians format but in the x domain of the
    unit circle.

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

/// @brief Converts Linear Prediction Coefficients (LPC) to more robust
/// Line Spectral Pairs (LSP) representation.
/// @author David Rowe
/// @date 24/2/93
/// @param lpc LPC coefficients.
/// @param length The size of lpc (in float-s, not in bytes).
/// @param freq The resulting LSP frequencies in the x domain.
/// @param nb The number of sub-intervals (e.g., 4).
/// @param delta The grid spacing interval (e.g., 0.02).
/// @return The number of roots found.
int lpc_to_lsp(const float *lpc, int length, int nb, float delta, float *freq);

/// @brief Converts LSP coefficients to LPC coefficients.
/// @author David Rowe
/// @date 24/2/93
/// @param freq The array of LSP frequencies in the x domain.
/// @param lpc The resulting array of LPC coefficients.
/// @param length The number of LPC coefficients.
void lsp_to_lpc(const float *freq, int length, float *lpc);

/// @brief Ensures the LSPs are stable.
/// @author Jean-Marc Valin
void lsp_enforce_margin(int length, float margin, float *lsp);

#endif	/* SRC_PRIMITIVES_LSP_H_ */
