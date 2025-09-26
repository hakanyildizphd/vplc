// Author: Hakan Yıldız
// Shared under MIT License. See the file LICENSE for more info.

/// @file diff_checker_char.cpp
/// Implements a program that performs the logic in diff_checker_base.ipp, with
/// the following properties:
/// - The valid tokens are printable ASCII characters.
/// - There is a look-ahead when printing output. (See the code for details.)
/// - The SHOW_DIFF and SHOW_OUTPUT macros determine whether the diff and the
///   output should be shown.

#include "diff_checker_base.ipp"

/// Predicate for validating tokens.
bool val(const char &a)
{
    return '!' <= a && a <= '~';
}

/// Predicate for comparing tokens.
bool eq(const char &a, const char &b)
{
    return a == b;
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

/// Implements the program. See the documentation of diff_checker_char.cpp.
int main(int argc, char **argv)
{
    return diff_checker_base<Tokenizer<char, val, eq>,
                             10, // The LookAhead template parameter.
                             SHOW_DIFF_FLAG,
                             SHOW_OUTPUT_FLAG>(argc, argv);
}
