// Author: Hakan Yıldız
// Shared under MIT License. See the file LICENSE for more info.

/// @file diff_checker_real.cpp
/// Implements a program that performs the logic in diff_checker_base.ipp, with
/// the following properties:
/// - The valid tokens are finite decimal numbers.
/// - Two numbers are equal if they are within a threshold or within a ratio.
///   (See the code.)
/// - There is a look-ahead when printing output. (See the code for details.)
/// - The SHOW_DIFF and SHOW_OUTPUT macros determine whether the diff and the
///   output should be shown.

#include "diff_checker_base.ipp"

#include <cmath>
using std::isfinite;

/// Predicate for validating tokens.
bool val(const long double &a)
{
    return isfinite(a);
}

/// Predicate for comparing tokens.
bool eq(const long double &a, const long double &b)
{
    // An error of 0.00001 is allowed. (Due to six digit printing of floats.)
    if (a - b <= 0.00001L && b - a <= 0.00001L)
    {
        return true;
    }

    // An error of 1% allowed.
    if (a >= 0.0L)
    {
        return a * 0.99L <= b && b <= a * 1.01L;
    }
    else
    {
        return a * 1.01L <= b && b <= a * 0.99L;
    }
}

#ifdef SHOW_DIFF
    #define SHOW_DIFF_FLAG true
#else
    #define SHOW_DIFF_FLAG false
#endif

#ifdef SHOW_OUTPUT
    #define SHOW_OUTPUT_FLAG true
#else
    #define SHOW_OUTPUT_FLAG false
#endif

/// Implements the program. See the documentation of diff_checker_real.cpp.
int main(int argc, char **argv)
{
    return diff_checker_base<Tokenizer<long double, val, eq>,
                             3,
                             SHOW_DIFF_FLAG,
                             SHOW_OUTPUT_FLAG>(argc, argv);
}
