#include "parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& toks) : tokens(toks) {}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::TOKEN_EOF;
}

const Token& Parser::advance() {
    if (!isAtEnd()) ++current;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::checkOp(const std::string& op) const {
    if (isAtEnd()) return false;
    return peek().type == TokenType::TOKEN_OPERATOR && peek().value == op;
}

bool Parser::checkPunct(const std::string& punct) const {
    if (isAtEnd()) return false;
    return peek().type == TokenType::TOKEN_PUNCTUATION && peek().value == punct;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchOp(const std::string& op) {
    if (checkOp(op)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchPunct(const std::string& punct) {
    if (checkPunct(punct)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

const Token& Parser::consumePunct(const std::string& punct, const std::string& message) {
    if (checkPunct(punct)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(const Token& token, const std::string& message) {
    hadError = true;
    std::cerr << "[line " << token.line << ", col " << token.column << "] Parse error";
    if (token.type == TokenType::TOKEN_EOF) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.value << "'";
    }
    std::cerr << ": " << message << "\n";
    return ParseError(message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::TOKEN_PUNCTUATION && previous().value == ";") {
            return;
        }
        switch (peek().type) {
            case TokenType::TOKEN_DEF:
            case TokenType::TOKEN_LET:
            case TokenType::TOKEN_IF:
            case TokenType::TOKEN_RETURN:
                return;
            default:
                break;
        }
        if (checkPunct("}")) {
            return;
        }
        advance();
    }
}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    hadError = false;
    while (!isAtEnd()) {
        StmtPtr decl = declaration();
        if (decl) {
            statements.push_back(std::move(decl));
        }
    }
    return statements;
}

StmtPtr Parser::declaration() {
    try {
        if (match(TokenType::TOKEN_DEF)) return funcDecl();
        if (match(TokenType::TOKEN_LET)) return varDecl();
        return statement();
    } catch (const ParseError&) {
        synchronize();
        return nullptr;
    }
}

StmtPtr Parser::funcDecl() {
    Token nameTok = consume(TokenType::TOKEN_IDENTIFIER, "Expected function name after 'def'.");
    consumePunct("(", "Expected '(' after function name.");

    std::vector<std::string> params;
    if (!checkPunct(")")) {
        do {
            Token paramTok = consume(TokenType::TOKEN_IDENTIFIER, "Expected parameter name.");
            params.push_back(paramTok.value);
        } while (matchPunct(","));
    }
    consumePunct(")", "Expected ')' after parameters.");

    Token braceTok = consumePunct("{", "Expected '{' before function body.");
    std::vector<StmtPtr> bodyStmts;
    while (!checkPunct("}") && !isAtEnd()) {
        StmtPtr stmt = declaration();
        if (stmt) {
            bodyStmts.push_back(std::move(stmt));
        }
    }
    consumePunct("}", "Expected '}' after function body.");

    auto body = std::make_unique<BlockStmt>(braceTok.line, braceTok.column, std::move(bodyStmts));
    return std::make_unique<FuncDeclStmt>(nameTok, std::move(params), std::move(body));
}

StmtPtr Parser::varDecl() {
    Token nameTok = consume(TokenType::TOKEN_IDENTIFIER, "Expected variable name after 'let'.");
    ExprPtr initializer = nullptr;
    if (matchOp("=")) {
        initializer = expression();
    }
    consumePunct(";", "Expected ';' after variable declaration.");
    return std::make_unique<VarDeclStmt>(nameTok, std::move(initializer));
}

StmtPtr Parser::statement() {
    if (match(TokenType::TOKEN_RETURN)) return returnStmt();
    if (match(TokenType::TOKEN_IF)) return ifStmt();
    if (checkPunct("{")) {
        Token braceTok = advance();
        std::vector<StmtPtr> stmts;
        while (!checkPunct("}") && !isAtEnd()) {
            StmtPtr stmt = declaration();
            if (stmt) {
                stmts.push_back(std::move(stmt));
            }
        }
        consumePunct("}", "Expected '}' after block.");
        return std::make_unique<BlockStmt>(braceTok.line, braceTok.column, std::move(stmts));
    }
    return exprStmt();
}

StmtPtr Parser::returnStmt() {
    Token kw = previous();
    ExprPtr value = nullptr;
    if (!checkPunct(";")) {
        value = expression();
    }
    consumePunct(";", "Expected ';' after return statement.");
    return std::make_unique<ReturnStmt>(kw.line, kw.column, std::move(value));
}

StmtPtr Parser::ifStmt() {
    Token kw = previous();
    consumePunct("(", "Expected '(' after 'if'.");
    ExprPtr cond = expression();
    consumePunct(")", "Expected ')' after if condition.");

    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match(TokenType::TOKEN_ELSE)) {
        elseBranch = statement();
    }
    return std::make_unique<IfStmt>(kw.line, kw.column, std::move(cond), std::move(thenBranch), std::move(elseBranch));
}

StmtPtr Parser::exprStmt() {
    ExprPtr expr = expression();
    consumePunct(";", "Expected ';' after expression.");
    return std::make_unique<ExprStmt>(std::move(expr));
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    ExprPtr expr = equality();

    if (matchOp("=")) {
        Token equals = previous();
        ExprPtr value = assignment();

        if (auto* ident = dynamic_cast<IdentExpr*>(expr.get())) {
            Token identTok;
            identTok.type = TokenType::TOKEN_IDENTIFIER;
            identTok.value = ident->name;
            identTok.line = ident->line;
            identTok.column = ident->column;
            identTok.position = 0;
            return std::make_unique<AssignExpr>(identTok, std::move(value));
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

ExprPtr Parser::equality() {
    ExprPtr expr = comparison();
    while (checkOp("==") || checkOp("!=")) {
        Token op = advance();
        ExprPtr right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::comparison() {
    ExprPtr expr = term();
    while (checkOp("<") || checkOp("<=") || checkOp(">") || checkOp(">=")) {
        Token op = advance();
        ExprPtr right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::term() {
    ExprPtr expr = factor();
    while (checkOp("+") || checkOp("-")) {
        Token op = advance();
        ExprPtr right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::factor() {
    ExprPtr expr = unary();
    while (checkOp("*") || checkOp("/")) {
        Token op = advance();
        ExprPtr right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::unary() {
    if (checkOp("!") || checkOp("-")) {
        Token op = advance();
        ExprPtr right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return call();
}

ExprPtr Parser::call() {
    ExprPtr expr = primary();
    while (true) {
        if (matchPunct("(")) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }
    return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
    std::vector<ExprPtr> args;
    int line = callee->line;
    int col = callee->column;
    if (!checkPunct(")")) {
        do {
            args.push_back(expression());
        } while (matchPunct(","));
    }
    consumePunct(")", "Expected ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), std::move(args), line, col);
}

ExprPtr Parser::primary() {
    if (check(TokenType::TOKEN_NUMBER)) {
        return std::make_unique<NumberExpr>(advance());
    }
    if (check(TokenType::TOKEN_STRING)) {
        return std::make_unique<StringExpr>(advance());
    }
    if (check(TokenType::TOKEN_IDENTIFIER)) {
        return std::make_unique<IdentExpr>(advance());
    }
    if (matchPunct("(")) {
        ExprPtr expr = expression();
        consumePunct(")", "Expected ')' after expression.");
        return expr;
    }
    throw error(peek(), "Expected expression.");
}