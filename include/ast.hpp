#pragma once

#include "lexer.hpp"
#include <memory>
#include <string>
#include <vector>

struct NumberExpr;
struct StringExpr;
struct IdentExpr;
struct UnaryExpr;
struct BinaryExpr;
struct CallExpr;
struct AssignExpr;

struct VarDeclStmt;
struct ReturnStmt;
struct IfStmt;
struct BlockStmt;
struct ExprStmt;
struct FuncDeclStmt;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(const NumberExpr& expr) = 0;
    virtual void visit(const StringExpr& expr) = 0;
    virtual void visit(const IdentExpr& expr) = 0;
    virtual void visit(const UnaryExpr& expr) = 0;
    virtual void visit(const BinaryExpr& expr) = 0;
    virtual void visit(const CallExpr& expr) = 0;
    virtual void visit(const AssignExpr& expr) = 0;

    virtual void visit(const VarDeclStmt& stmt) = 0;
    virtual void visit(const ReturnStmt& stmt) = 0;
    virtual void visit(const IfStmt& stmt) = 0;
    virtual void visit(const BlockStmt& stmt) = 0;
    virtual void visit(const ExprStmt& stmt) = 0;
    virtual void visit(const FuncDeclStmt& stmt) = 0;
};

struct ASTNode {
    int line;
    int column;

    ASTNode(int l, int c) : line(l), column(c) {}
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

struct Expr : public ASTNode {
    using ASTNode::ASTNode;
};

struct Stmt : public ASTNode {
    using ASTNode::ASTNode;
};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

struct NumberExpr : public Expr {
    std::string value;

    NumberExpr(Token tok) : Expr(tok.line, tok.column), value(std::move(tok.value)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct StringExpr : public Expr {
    std::string value;

    StringExpr(Token tok) : Expr(tok.line, tok.column), value(std::move(tok.value)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct IdentExpr : public Expr {
    std::string name;

    IdentExpr(Token tok) : Expr(tok.line, tok.column), name(std::move(tok.value)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct UnaryExpr : public Expr {
    Token op;
    ExprPtr operand;

    UnaryExpr(Token o, ExprPtr r)
        : Expr(o.line, o.column), op(std::move(o)), operand(std::move(r)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct BinaryExpr : public Expr {
    ExprPtr left;
    Token op;
    ExprPtr right;

    BinaryExpr(ExprPtr l, Token o, ExprPtr r)
        : Expr(o.line, o.column), left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct CallExpr : public Expr {
    ExprPtr callee;
    std::vector<ExprPtr> args;

    CallExpr(ExprPtr c, std::vector<ExprPtr> a, int line, int col)
        : Expr(line, col), callee(std::move(c)), args(std::move(a)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct AssignExpr : public Expr {
    std::string name;
    ExprPtr value;

    AssignExpr(Token tok, ExprPtr val)
        : Expr(tok.line, tok.column), name(std::move(tok.value)), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct VarDeclStmt : public Stmt {
    std::string name;
    ExprPtr initializer;

    VarDeclStmt(Token tok, ExprPtr init)
        : Stmt(tok.line, tok.column), name(std::move(tok.value)), initializer(std::move(init)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct ReturnStmt : public Stmt {
    ExprPtr value;

    ReturnStmt(int l, int c, ExprPtr val)
        : Stmt(l, c), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct IfStmt : public Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(int l, int c, ExprPtr cond, StmtPtr thenB, StmtPtr elseB)
        : Stmt(l, c), condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct BlockStmt : public Stmt {
    std::vector<StmtPtr> statements;

    BlockStmt(int l, int c, std::vector<StmtPtr> stmts)
        : Stmt(l, c), statements(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct ExprStmt : public Stmt {
    ExprPtr expression;

    ExprStmt(ExprPtr expr)
        : Stmt(expr->line, expr->column), expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

struct FuncDeclStmt : public Stmt {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockStmt> body;

    FuncDeclStmt(Token tok, std::vector<std::string> p, std::unique_ptr<BlockStmt> b)
        : Stmt(tok.line, tok.column), name(std::move(tok.value)), params(std::move(p)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};