#pragma once

#include <string>
#include <vector>

enum class TokenType {
    TOKEN_EOF = -1,
    TOKEN_ERROR = 0,
    TOKEN_DEF,        
    TOKEN_IF,         
    TOKEN_ELSE,       
    TOKEN_LET,        
    TOKEN_RETURN,     
    TOKEN_IDENTIFIER, 
    TOKEN_NUMBER,    
    TOKEN_OPERATOR,
    TOKEN_STRING,
    TOKEN_PUNCTUATION
};

struct Token {
    TokenType type;
    std::string value;
    int position;
    int line;
    int column;
};

class Lexer {
public:
    static bool checkString(const std::string& s);
    static std::vector<Token> tokenize(const std::string& s);

private:
    enum CharType {
        CHAR_LETTER = 0,      
        CHAR_DIGIT = 1,       
        CHAR_OP = 2,          
        CHAR_WHITESPACE = 3,  
        CHAR_QUOTE = 4,
        CHAR_SLASH = 5,
        CHAR_PUNCT = 6,
        CHAR_NEWLINE = 7,
        CHAR_DOT = 8,
        CHAR_INVALID = 9      
    };

    enum State {
        STATE_ERROR = 0,
        STATE_START = 1,
        STATE_IDENTIFIER = 2, 
        STATE_NUMBER = 3,
        STATE_STRING = 4,
        STATE_COMMENT_LINE = 5,
        STATE_PUNCT = 6,
        STATE_OP = 7,
        STATE_STRING_END = 8,
        STATE_FLOAT_DOT = 9,
        STATE_FLOAT = 10
    };

    static CharType getCharType(char c);
    static TokenType classifyWord(const std::string& word);
    static int getPrefixLengthFrom(const std::string& s, int startPos, TokenType& outType);

    static constexpr int Transitions[11][10] = {
        {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {   2,  3,  7,  1,  4,  5,  6,  1,  6,  0 },
        {   2,  2,  0,  0,  0,  0,  0,  0,  0,  0 },
        {   0,  3,  0,  0,  0,  0,  0,  0,  9,  0 },
        {   4,  4,  4,  4,  8,  4,  4,  4,  4,  0 },
        {   5,  5,  5,  5,  5,  5,  5,  0,  5,  5 },
        {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {   0,  0,  7,  0,  0,  0,  0,  0,  0,  0 },
        {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
        {   0, 10,  0,  0,  0,  0,  0,  0,  0,  0 },
        {   0, 10,  0,  0,  0,  0,  0,  0,  0,  0 }
    };

    static constexpr bool IsFinalState[11] = {
        false,
        false,
        true,
        true,
        false,
        true,
        true,
        true,
        true,
        false,
        true
    };
};