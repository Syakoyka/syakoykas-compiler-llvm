#include "lexer.hpp"
#include <iostream>
#include <vector>

int main() {
    std::string sourceCode = "def calculate x 10 if x else 0";

    std::cout << "Lexer Test" << std::endl;
    std::cout << "Source code: " << sourceCode << std::endl;

    std::vector<Token> tokens = Lexer::tokenize(sourceCode);

    std::cout << "Tokens found: " << tokens.size() << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  [Pos: " << token.position << "] -> Value: \"" << token.value << "\" (Type: ";
        
        switch (token.type) {
            case TokenType::TOKEN_DEF: std::cout << "DEF"; break;
            case TokenType::TOKEN_IF: std::cout << "IF"; break;
            case TokenType::TOKEN_ELSE: std::cout << "ELSE"; break;
            case TokenType::TOKEN_IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::TOKEN_NUMBER: std::cout << "NUMBER"; break;
            case TokenType::TOKEN_OPERATOR: std::cout << "OPERATOR"; break;
            default: std::cout << "ERROR/UNKNOWN"; break;
        }
        std::cout << ")" << std::endl;
    }

    return 0;
}