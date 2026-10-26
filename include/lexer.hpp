#pragma once

#include <string>
#include <vector>

enum class TokenType {
    TOKEN_EOF = -1,
    TOKEN_ERROR = 0,
    TOKEN_DEF,        
    TOKEN_IF,         
    TOKEN_ELSE,       
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
        CHAR_INVALID = 8      
    };

    enum State {
        STATE_ERROR = 0,
        STATE_START = 1,
        STATE_IDENTIFIER = 2, 
        STATE_NUMBER = 3,
        STATE_STRING = 4,
        STATE_COMMENT_LINE = 5,
        STATE_PUNCT = 6
    };

    static CharType getCharType(char c);
    static TokenType classifyWord(const std::string& word);
    static int getPrefixLengthFrom(const std::string& s, int startPos, TokenType& outType);

    static constexpr int Transitions[7][9] = {
        // LTR  DIG  OP   WS   QUO  SLI  PNC  NL   INV
        {   0,   0,   0,   0,   0,   0,   0,   0,    0 },  // 0: ERROR
        {   2,   3,   0,   1,   4,   5,   6,   1,    0 },  // 1: START
        {   2,   2,   0,   0,   0,   0,   0,   0,    0 },  // 2: IDENTIFIER
        {   0,   3,   0,   0,   0,   0,   0,   0,    0 },  // 3: NUMBER
        {   4,   4,   4,   4,  -1,   4,   4,   4,    0 },  // 4: STRING (ожидание закрывающей кавычки)
        {   5,   5,   5,   5,   5,   5,   5,  -1,    5 },  // 5: LINE_COMMENT (до конца строки)
        {   0,   0,   0,   0,   0,   0,   0,   0,    0 }   // 6: PUNCTUATION
    };

    static constexpr bool IsFinalState[7] = {
        false,
        false,
        true,
        true,
        true,
        true,
        true
    };
};