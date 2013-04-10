#!/bin/bash
find -name "*.cc" -not -path "./tests/google/*" -not -path "./fftf/*" -not -path "./src/boost/*" -exec python cpplint.py --verbose=0 --filter=+,-build/include_alpha,-build/include_what_you_use,-whitespace/operators {} \;
find -name "*.h"  -not -name "config.h" -not -path "./fftf/*"  -not -path "./src/primitives/avx_mathfun.h" -not -path "./inc/boost/*" -not -path "./src/primitives/neon_mathfun.h" -not -path "./tests/google/*" -exec python cpplint.py --verbose=0 --filter=+,-build/include_alpha,-build/include_what_you_use,-whitespace/operators {} \;
