// Author: Hakan Yıldız
// Shared under MIT License. See the file LICENSE for more info.

/// @file diff_checker_base.ipp
/// Implements the diff_checker_base function template, which implements a
/// checker program. The expected parameters to the checker are:
///     <input_file> <claimed_output_file> <correct_output_file> <hidden>
/// This checker checks if <claimed_output_file> matches <correct_output_file>
/// by reading the files with the provided Tokenizer. <input_file> is ignored.
/// <hidden> parameter, which should be 0 or 1, conveys if the test case is
/// hidden. Content output (when applicable) is supressed if so. The output is
/// in the following format:
///     <grade_ratio>|<further output>

#include <fstream>
#include <iostream>
#include <string>

#include "Tokenizer.ipp"

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;


/// Implements the checker logic. See documentation of diff_checker_base.ipp.
/// @tparam Tokenizer  The tokenizer to parse the output files.
/// @tparam LookAhead  If ShowOutput is true, this integer tells how many tokens
///                    to further print after finding a mismatch.
/// @tparam ShowDiff   Whether to show the diff between the two output files.
/// @tparam ShowOutput Whether to show <claimed_output_file>.
template<typename Tokenizer, int LookAhead, bool ShowDiff, bool ShowOutput>
int diff_checker_base(int argc, char **argv)
{
    if (argc < 5)
    {
        cerr << "Invalid parameters." << endl;
        return 1;
    }

    const char *claimedOutputPath = argv[2];
    const char *correctOutputPath = argv[3];
    string argv4 = string(argv[4]);

    if (argv4 != "0" && argv4 != "1")
    {
        cerr << "Invalid test-case-hidden parameter." << endl;
        return 1;
    }

    const bool isTestCaseHidden = (argv4 == "1");

    ifstream claimedStream(claimedOutputPath);

    if (claimedStream.fail())
    {
        cout << "0|Error opening the output file." << endl;
        return 0;
    }

    ifstream correctStream(correctOutputPath);

    if (correctStream.fail())
    {
        cerr << "Error opening the ground-truth file." << endl;
        return 1;
    }

    Tokenizer claimed(claimedStream);
    Tokenizer correct(correctStream);

    stringstream checkerOutput;

    while (true)
    {
        auto claimedToken = claimed.next();
        auto correctToken = correct.next();

        if (correctToken.kind() == Tokenizer::TokenKind::Invalid)
        {
            cerr << "Ground-truth file format is invalid." << endl;
            return 1;
        }

        checkerOutput << claimedToken.sstr();

        if (!correctToken.isEqual(claimedToken))
        {
            if constexpr (ShowDiff)
            {
                if (isTestCaseHidden)
                {
                    cout << "0|Wrong output. (Mismatch intentionally hidden.)" << endl;
                }
                else
                {
                    cout << "0|Unexpected "
                         << claimedToken.lstr()
                         << " at line "
                         << claimedToken.line()
                         << ", token "
                         << claimedToken.token()
                         << ", while expecting "
                         << correctToken.lstr()
                         << "." << endl;
                }
            }
            else
            {
                cout << "0|Wrong output." << endl;
            }

            if constexpr (ShowOutput)
            {
                if (isTestCaseHidden)
                {
                    cout << "(Your output is intentionally hidden.)" << endl;
                }
                else
                {
                    cout << "Your output (as parsed):" << endl;

                    for (int i = 0; i < LookAhead; i++)
                    {
                        if (claimedToken.kind() == Tokenizer::TokenKind::EndOfFile ||
                            claimedToken.kind() == Tokenizer::TokenKind::Invalid)
                        {
                            break;
                        }
                        else
                        {
                            claimedToken = claimed.next();
                            checkerOutput << claimedToken.sstr();
                        }
                    }

                    if (claimedToken.kind() == Tokenizer::TokenKind::Invalid)
                    {
                        checkerOutput << "..?..";
                    }
                    else if (claimedToken.kind() != Tokenizer::TokenKind::EndOfFile)
                    {
                        checkerOutput << ".....";
                    }

                    for (char c : checkerOutput.str())
                    {
                        cout << c;
                    }

                    cout << endl;
                }
            }

            return 0;
        }
        else if (correctToken.kind() == Tokenizer::TokenKind::EndOfFile)
        {
            cout << "1|Correct output.";
            return 0;
        }
    }
}
