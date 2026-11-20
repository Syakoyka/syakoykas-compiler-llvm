program := declaration* EOF ;

declaration := funcDecl | varDecl | statement ;

funcDecl := "def" IDENT "(" params? ")" block ;

params := IDENT ( "," IDENT )* ;

varDecl := "let" IDENT ( "=" expression )? ";" ;

statement := returnStmt | ifStmt | block | exprStmt ;

returnStmt := "return" expression? ";" ;

ifStmt := "if" "(" expression ")" statement ( "else" statement )? ;

block := "{" declaration* "}" ;

exprStmt := expression ";" ;

expression := assignment ;

assignment := IDENT "=" assignment | equality ;

equality := comparison ( ( "==" | "!=" ) comparison )* ;

comparison := term ( ( "<" | "<=" | ">" | ">=" ) term )* ;

term := factor ( ( "+" | "-" ) factor )* ;

factor := unary ( ( "*" | "/" ) unary )* ;

unary := ( "!" | "-" ) unary | call ;

call := primary ( "(" arguments? ")" )* ;

arguments := expression ( "," expression )* ;

primary := NUMBER | STRING | IDENT | "(" expression ")" ;