// Author: Hakan Yıldız
// Shared under MIT License. See the file LICENSE for more info.

/// @file Tokenizer.ipp
/// Implements the Tokenizer class and dependencies.

#pragma once

#include <cstdint>
#include <stdexcept>
#include <sstream>

using std::logic_error;
using std::istream;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::uint64_t;

/// Represents a token read by a Tokenizer.
/// @tparam T Base character type for the Token.
/// @tparam EQ Equality predicate for the base characters.
template<typename T, bool EQ(const T &, const T &)>
class Token 
{
    public:
        
        enum Kind /// Represents the kind of a Token.
        {
            Valid,     ///< A valid token (other than newline/space/EOF).
            Space,     ///< Space.
            Newline,   ///< Newline.
            EndOfFile, ///< EOF.
            Invalid    ///< Invalid format.
        };

        typedef uint64_t Pos; ///< The integral type to store token positions.

    private:
        Kind mKind; ///< The underlying field for kind().
        T mValue;   ///< The underlying field for value().
        Pos mLine;  ///< The underlying field for line().
        Pos mToken; ///< The underlying field for token().

    public:
        /// Constructs a token whose kind is different than Kind::Valid.
        /// @param kind The kind of the token. Must not be Kind::Valid.
        /// @param line The line number of the token.
        /// @param token The token number (within the line) of the token.
        Token(Kind kind, Pos line, Pos token) :
              mKind(kind), mValue(), mLine(line), mToken(token)
        {
            if (mKind == Kind::Valid)
            {
                throw new logic_error(
                    "Cannot create valid token without value.");
            }
        }

        /// Constructs a token from a character, with the kind Kind::Valid.
        /// Note that there is no validity check during this construction.
        /// The validity check is a feature of the Tokenizer class.
        /// @param value The value from which the token is constructed.
        /// @param line The line number of the token.
        /// @param token The token number (within the line) of the token.
        Token(T value, Pos line, Pos token) :
              mKind(Kind::Valid), mValue(value), mLine(line), mToken(token)
        {
        }

        /// Assigns another token to this one.
        /// @param t The token to assign from.
        Token & operator=(const Token &t) = default;

        /// The kind of this token.
        Kind kind() const
        {
            return mKind;
        }

        /// The value of this token, if kind() is Kind::Valid.
        const T & value() const 
        {
            if (mKind != Kind::Valid)
            {
                throw new logic_error(
                    "Cannot obtain value of non-valid token.");
            }

            return mValue;
        }

        /// The line number of this token.
        const Pos & line() const
        {
            return mLine;
        }

        /// The token number (within the line) of this token.
        const Pos & token() const
        {
            return mToken;
        }

        /// Returns a long string for the token, which is always printable.
        string lstr() const
        {
            stringstream ss;

            switch (mKind)
            {
                case Kind::Valid:
                    ss << mValue;
                    return "'" + ss.str() + "'";
                case Kind::Newline:
                    return "<newline>";
                case Kind::Space:
                    return "<space>";
                case Kind::EndOfFile:
                    return "<end>";
                case Kind::Invalid:
                    return "<invalid-format>";
                default:
                    throw new logic_error("Unrecognized kind.");
            }
        }

        /// Returns a short string for the token.
        string sstr() const
        {
            stringstream ss;

            switch (mKind)
            {
                case Kind::Valid:
                    ss << mValue;
                    return ss.str();
                case Kind::Newline:
                    return "\n";
                case Kind::Space:
                    return " ";
                case Kind::EndOfFile:
                    return "<end>";
                case Kind::Invalid:
                    return "<invalid-format>";
                default:
                    throw new logic_error("Unrecognized kind.");
            }
        }

        /// Checks if this token is equal to another.
        /// @param t The token to check against.
        bool isEqual(const Token &t) const
        {
            if (mKind != t.mKind)
            {
                return false;
            }
            else if (mKind == Kind::Valid)
            {
                if (EQ(mValue, t.mValue))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return true;
            }
        }
};

/// A class to read tokens from an std::istream. The main functionality is
/// skipping the following whitespace characters during tokenization:
/// - A space that precedes a newline.
/// - A space that precedes the end of the file.
/// - A newline that precedes the end of the file.
/// Also, the class can check for validity of the tokens (e.g., whether they are
/// in ASCII range) with a predicate supplied as a template parameter. The
/// tokenization stops upon an invalid symbol, returning an invalid token, and
/// invalidating the tokenizer.
/// @tparam The base character type for the Token objects produced.
/// @tparam The predicate that checks whether a given character is valid.
/// @tparam The equality predicate for the Token objects produced.
template<typename T,
         bool VAL(const T &),
         bool EQ(const T &, const T &)>
class Tokenizer
{
    public:
        /// The Token type instantiated in this class.
        typedef Token<T, EQ> TokenType;
        /// The Token::Kind type instantiated in this class.
        typedef TokenType::Kind TokenKind;
        /// The Token::Pos type instantiated in this class.
        typedef TokenType::Pos TokenPos;

    private:
        istream & mInput; ///< The internal istream, provided upon construction.
        bool mIsValid;    ///< The underlying field for isValid().
        TokenPos mLine;   ///< The line number for the next token.
        TokenPos mToken;  ///< The token number for the next token.

        /// Consumes an expected (peeked) character from the internal istream.
        void consume(istream::int_type c)
        {
            if (mInput.get() != c)
            {
                mIsValid = false;
                throw runtime_error("Tokenizer failed unexpectedly. (consume)");
            }
        }

    public:
        /// Constructs a tokenizer on a given istream.
        Tokenizer(istream & input) :
              mInput(input), mIsValid(true), mLine(1), mToken(1)
        {
        }

        /// Whether this tokenizer is valid.
        bool isValid() const
        {
            return mIsValid;
        }

        /// Produces the next token.
        TokenType next()
        {
            if (mIsValid == false)
            {
                throw logic_error("Tokenizer is not valid anymore.");
            }

            int peekc;

            peekc = mInput.peek();

            if (peekc == EOF)
            {
                consume(EOF);
                mIsValid = false;
                return TokenType(TokenKind::EndOfFile, mLine, mToken);
            }
            else if (peekc == '\n')
            {
                consume('\n');

                peekc = mInput.peek();

                if (peekc == EOF)
                {
                    consume(EOF);
                    mIsValid = false;
                    return TokenType(TokenKind::EndOfFile, mLine, mToken);
                }
                else
                {
                    TokenType t(TokenKind::Newline, mLine, mToken);
                    mLine++;
                    mToken = 1;
                    return t;
                }
            }
            else if (peekc == ' ')
            {
                consume(' ');

                peekc = mInput.peek();

                if (peekc == EOF)
                {
                    consume(EOF);
                    mIsValid = false;
                    return TokenType(TokenKind::EndOfFile, mLine, mToken);
                }
                else if (peekc == '\n')
                {
                    consume('\n');

                    peekc = mInput.peek();

                    if (peekc == EOF)
                    {
                        consume(EOF);
                        mIsValid = false;
                        return TokenType(TokenKind::EndOfFile, mLine, mToken);
                    }
                    else
                    {
                        TokenType t(TokenKind::Newline, mLine, mToken);
                        mLine++;
                        mToken = 1;
                        return t;
                    }
                }
                else
                {
                    return TokenType(TokenKind::Space, mLine, mToken++);
                }
            }
            else
            {
                T value;

                mInput >> value;

                if (mInput.fail())
                {
                    mIsValid = false;
                    return TokenType(TokenKind::Invalid, mLine, mToken);
                }
                else
                {
                    if (VAL(value))
                    {
                        return TokenType(value, mLine, mToken++);
                    }
                    else
                    {
                        mIsValid = false;
                        return TokenType(TokenKind::Invalid, mLine, mToken);
                    }
                }
            }
        }
};
