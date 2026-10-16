#include "lexer.hpp"
#include <cctype>

Lexer::CharType Lexer::getCharType(char c) {
    if (std::isalpha(c) || c == '_') {
        return CharType::CHAR_LETTER;
    }
    if (std::isdigit(c)) {
        return CharType::CHAR_DIGIT;
    }
    if (c == '+' || c == '-' || c == '*' || c == '=' || c == '<' || c == '>') {
        return CharType::CHAR_OP;
    }
    if (std::isspace(c)) {
        return CharType::CHAR_WHITESPACE;
    }
    return CharType::CHAR_INVALID;
}

TokenType Lexer::classifyWord(const std::string& word) {
    if (word == "def")  return TokenType::TOKEN_DEF;
    if (word == "if")   return TokenType::TOKEN_IF;
    if (word == "else") return TokenType::TOKEN_ELSE;
    return TokenType::TOKEN_IDENTIFIER;
}

bool Lexer::checkString(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    int state = State::STATE_START;
    for (int i = 0; i < s.length(); i++) {
        int charType = getCharType(s[i]);
        state = Transitions[state][charType];

        if (state == State::STATE_ERROR) {
            return false;
        }
    }

    return IsFinalState[state];
}

int Lexer::getPrefixLengthFrom(const std::string& s, int startPos, TokenType& outType) {
    int state = State::STATE_START;
    int lastValidLength = 0;
    int lastValidState = State::STATE_ERROR;

    for (int i = startPos; i < s.length(); ++i) {
        int charType = getCharType(s[i]);
        state = Transitions[state][charType];

        if (state == State::STATE_ERROR) {
            break;
        } else {
            if (IsFinalState[state]) {
                lastValidLength = (i - startPos) + 1;
                lastValidState = state;
            }
        }
    }

    if (lastValidState == STATE_NUMBER) {
        outType = TokenType::TOKEN_NUMBER;
    } else if (lastValidState == STATE_IDENTIFIER) {
        outType = TokenType::TOKEN_IDENTIFIER;
    } else {
        outType = TokenType::TOKEN_ERROR;
    }

    return lastValidLength;
}

std::vector<Token> Lexer::tokenize(const std::string& s) {
    std::vector<Token> tokens;
    int i = 0;

    while (i < s.length()) {
        if (std::isspace(s[i])) {
            i++;
            continue;
        }

        TokenType detectedType = TokenType::TOKEN_ERROR;
        int matchLen = getPrefixLengthFrom(s, i, detectedType);

        if (matchLen > 0) {
            std::string lexeme = s.substr(i, matchLen);
            
            Token token;
            token.value = lexeme;
            token.position = i;

            if (detectedType == TokenType::TOKEN_NUMBER) {
                token.type = TokenType::TOKEN_NUMBER;
            } else {
                token.type = classifyWord(lexeme);
            }

            tokens.push_back(token);
            i = i + matchLen; 
        } else {
            Token errToken;
            errToken.type = TokenType::TOKEN_ERROR;
            errToken.value = std::string(1, s[i]);
            errToken.position = i;
            tokens.push_back(errToken);
            i++; 
        }
    }

    return tokens;
}