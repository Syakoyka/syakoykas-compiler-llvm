#pragma once

#include "ast.hpp"
#include "lexer.hpp"
#include <stdexcept>
#include <string>
#include <vector>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<StmtPtr> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;
    bool hadError = false;

    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool checkOp(const std::string& op) const;
    bool checkPunct(const std::string& punct) const;
    bool match(TokenType type);
    bool matchOp(const std::string& op);
    bool matchPunct(const std::string& punct);
    const Token& consume(TokenType type, const std::string& message);
    const Token& consumePunct(const std::string& punct, const std::string& message);

    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    StmtPtr declaration();
    StmtPtr funcDecl();
    StmtPtr varDecl();
    StmtPtr statement();
    StmtPtr returnStmt();
    StmtPtr ifStmt();
    StmtPtr block();
    StmtPtr exprStmt();

    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr call();
    ExprPtr finishCall(ExprPtr callee);
    ExprPtr primary();
};