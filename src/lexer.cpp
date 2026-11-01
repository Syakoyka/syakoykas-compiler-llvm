#include "lexer.hpp"
#include <cctype>
#include <unordered_map>

Lexer::CharType Lexer::getCharType(char c) {
    if (std::isalpha(c) || c == '_') return CharType::CHAR_LETTER;
    if (std::isdigit(c)) return CharType::CHAR_DIGIT;
    if (c == '+' || c == '-' || c == '*' || c == '=' || c == '<' || c == '>' || c == '!') return CharType::CHAR_OP;
    if (c == '"') return CharType::CHAR_QUOTE;
    if (c == '/') return CharType::CHAR_SLASH;
    if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';') return CharType::CHAR_PUNCT;
    if (c == '\n') return CharType::CHAR_NEWLINE;
    if (std::isspace(c)) return CharType::CHAR_WHITESPACE;
    return CharType::CHAR_INVALID;
}

TokenType Lexer::classifyWord(const std::string& word) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"def", TokenType::TOKEN_DEF},
        {"if", TokenType::TOKEN_IF},
        {"else", TokenType::TOKEN_ELSE}
    };

    auto it = keywords.find(word);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::TOKEN_IDENTIFIER;
}

bool Lexer::checkString(const std::string& s) {
    if (s.empty()) return false;
    int state = State::STATE_START;
    for (size_t i = 0; i < s.length(); i++) {
        int charType = getCharType(s[i]);
        state = Transitions[state][charType];
        if (state == State::STATE_ERROR) return false;
    }
    return IsFinalState[state];
}

int Lexer::getPrefixLengthFrom(const std::string& s, int startPos, TokenType& outType) {
    int state = State::STATE_START;
    int lastValidLength = 0;
    int lastValidState = State::STATE_ERROR;

    for (size_t i = startPos; i < s.length(); ++i) {
        int charType = getCharType(s[i]);
        int nextState = Transitions[state][charType];

        if (nextState == State::STATE_ERROR) {
            break;
        }
        
        state = nextState;
        
        if (state == State::STATE_STRING && s[i] == '"' && i > startPos) {
            lastValidLength = (i - startPos) + 1;
            lastValidState = state;
            state = State::STATE_START;
            break;
        }

        if (IsFinalState[state]) {
            lastValidLength = (i - startPos) + 1;
            lastValidState = state;
        }
    }

    if (lastValidState == STATE_NUMBER) outType = TokenType::TOKEN_NUMBER;
    else if (lastValidState == STATE_IDENTIFIER) outType = TokenType::TOKEN_IDENTIFIER;
    else if (lastValidState == STATE_STRING) outType = TokenType::TOKEN_STRING;
    else if (lastValidState == STATE_PUNCT) outType = TokenType::TOKEN_PUNCTUATION;
    else if (lastValidState == STATE_OP) outType = TokenType::TOKEN_OPERATOR;
    else if (lastValidState == STATE_COMMENT_LINE) outType = TokenType::TOKEN_ERROR;
    else outType = TokenType::TOKEN_ERROR;

    return lastValidLength;
}

std::vector<Token> Lexer::tokenize(const std::string& s) {
    std::vector<Token> tokens;
    int i = 0;
    int line = 1;
    int column = 1;

    while (i < s.length()) {
        char c = s[i];

        if (c == '\n') {
            line++;
            column = 1;
            i++;
            continue;
        }
        if (std::isspace(c)) {
            column++;
            i++;
            continue;
        }

        if (c == '/' && i + 1 < s.length() && s[i + 1] == '/') {
            while (i < s.length() && s[i] != '\n') {
                i++;
            }
            continue;
        }

        TokenType detectedType = TokenType::TOKEN_ERROR;
        int matchLen = getPrefixLengthFrom(s, i, detectedType);

        if (matchLen > 0) {
            std::string lexeme = s.substr(i, matchLen);
            
            if (lexeme.rfind("//", 0) == 0) {
                i += matchLen;
                column += matchLen;
                continue;
            }

            Token token;
            token.value = lexeme;
            token.position = i;
            token.line = line;
            token.column = column;

            if (detectedType == TokenType::TOKEN_NUMBER) {
                token.type = TokenType::TOKEN_NUMBER;
            } else if (detectedType == TokenType::TOKEN_STRING) {
                token.type = TokenType::TOKEN_STRING;
            } else if (detectedType == TokenType::TOKEN_PUNCTUATION) {
                token.type = TokenType::TOKEN_PUNCTUATION;
            } else if (detectedType == TokenType::TOKEN_OPERATOR) {
                token.type = TokenType::TOKEN_OPERATOR;
            } else {
                token.type = classifyWord(lexeme);
            }

            tokens.push_back(token);
            i += matchLen;
            column += matchLen;
        } else {
            Token errToken;
            errToken.type = TokenType::TOKEN_ERROR;
            errToken.value = std::string(1, s[i]);
            errToken.position = i;
            errToken.line = line;
            errToken.column = column;
            tokens.push_back(errToken);
            i++;
            column++;
        }
    }

    Token eofToken;
    eofToken.type = TokenType::TOKEN_EOF;
    eofToken.value = "";
    eofToken.position = s.length();
    eofToken.line = line;
    eofToken.column = column;
    tokens.push_back(eofToken);

    return tokens;
}