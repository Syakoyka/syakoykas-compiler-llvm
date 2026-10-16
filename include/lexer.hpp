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
    TOKEN_OPERATOR   
};

struct Token {
    TokenType type;
    std::string value;
    int position;
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
        CHAR_INVALID = 4      
    };

    enum State {
        STATE_ERROR = 0,
        STATE_START = 1,
        STATE_IDENTIFIER = 2, 
        STATE_NUMBER = 3      
    };

    static CharType getCharType(char c);

    static TokenType classifyWord(const std::string& word);

    static int getPrefixLengthFrom(const std::string& s, int startPos, TokenType& outType);

    // Таблица переходов DFA с числовыми индексами состояний:
    // Строки: 0 (ERROR), 1 (START), 2 (IDENTIFIER), 3 (NUMBER)
    // Столбцы: 0 (LETTER), 1 (DIGIT), 2 (OP), 3 (WHITESPACE), 4 (INVALID)
    static constexpr int Transitions[4][5] = {
        {     0,      0,      0,      0,        0 },  
        {     2,      3,      0,      1,        0 },  
        {     2,      2,      0,      0,        0 },  
        {     0,      3,      0,      0,        0 }   
    };

    static constexpr bool IsFinalState[4] = {
        false, 
        false, 
        true,  
        true   
    };
};