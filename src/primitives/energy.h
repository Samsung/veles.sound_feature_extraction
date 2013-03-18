/*! @file energy.h
 *  @brief Low level energy calculation.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef ENERGY_H_
#define ENERGY_H_

#include <stddef.h>
#include "src/config.h"

#ifdef __cplusplus
extern "C" {
#endif

float calculate_energy(const float *signal, size_t length) NOTNULL(1);

#ifdef __cplusplus
}
#endif

#endif  // ENERGY_H_
