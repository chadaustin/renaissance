options {
    language = "Cpp";
    namespaceStd = "std::";
    namespaceAntlr = "antlr::";
}


//////////////////////////////////////////////////////////////////////////////


class ShaderLexer extends Lexer;

options {
    k = 2;
}

ID: ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')* ;
LITERAL: ('0'..'9')+ ;

NEWLINE: '\n' | '\r' | '\r' '\n'  { newline(); } ;

WS: (' ' | '\t') { $setType(antlr::Token::SKIP); } ;

IS: '=' ;

OR:      "||" ;
XOR:     "^^" ;
AND:     "&&" ;
EQ:      "==" ;
NOTEQ:   "!=" ;
LESS:    '<'  ;
GREATER: '>'  ;
LTE:     "<=" ;
GTE:     ">=" ;
PLUS:    '+'  ;
MINUS:   '-'  ;
TIMES:   '*'  ;
OVER:    '/'  ;

LPAREN:  '('  ;
RPAREN:  ')'  ;


//////////////////////////////////////////////////////////////////////////////


class ShaderParser extends Parser;

program: (definition)* ;

definition: leftSide IS expr NEWLINE ;
leftSide: ID args ;
args: (ID)* ;

expr: orExpr ;
orExpr:   xorExpr (OR xorExpr)* ;
xorExpr:  andExpr (XOR andExpr)* ;
andExpr:  eqExpr (AND eqExpr)* ;
eqExpr:   cmpExpr ( (EQ | NOTEQ) cmpExpr)* ;
cmpExpr:  addExpr ( (LESS | GREATER | LTE | GTE) addExpr)* ;
addExpr:  multExpr ( (PLUS | MINUS) multExpr)* ;
multExpr: term ( (TIMES | OVER) term)* ;
term:     (value)+
    |     LPAREN expr RPAREN
    ;
value:    ID
     |    LITERAL
     ;
