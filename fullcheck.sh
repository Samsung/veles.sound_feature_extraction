#!/bin/bash
find -name "*.cc" -not -path "./tests/google/*" -not -path "./fftf/*" -not -path "./src/boost/*" -exec python cpplint.py --verbose=0 --filter=+,-build/include_alpha,-build/include_what_you_use,-whitespace/operators {} \;
find -name "*.h"  -not -name "config.h" -not -path "./fftf/*" -not -path "./inc/boost/*" -not -path "./tests/google/*" -exec python cpplint.py --verbose=0 --filter=+,-build/include_alpha,-build/include_what_you_use,-whitespace/operators {} \;
