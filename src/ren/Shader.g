header "post_include_hpp" {
    #include "Program.h"
}

options {
    language = "Cpp";
    namespace = "ren";
    namespaceStd = "std::";
    namespaceAntlr = "antlr::";
}


//////////////////////////////////////////////////////////////////////////////


class ShaderLexer extends Lexer;

options {
    k = 2;
    defaultErrorHandler = false;
}

// Don't let ID take these over.
tokens {
    UNIFORM   = "uniform";
    ATTRIBUTE = "attribute";
}

ID: ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')* ;
LITERAL: ('0'..'9')+ ( '.' ('0'..'9')* )?;

NEWLINE: ('\n' | '\r' | '\r' '\n')    { newline(); } ;
WS     : (' ' | '\t')                 { $setType(antlr::Token::SKIP); } ;
COMMENT: ( '#' (~ ('\n' | '\r') )* )  { $setType(antlr::Token::SKIP); } ;

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

options {
    buildAST = true;
    k = 2;
    defaultErrorHandler = false;
}

program
    :
    | NEWLINE! program
    | statement (NEWLINE! program)?
    ;

statement : uniform | attribute | definition ;

uniform : UNIFORM^ ID ID ;
attribute : ATTRIBUTE^ ID ID ;

definition: leftSide IS^ expr ;
leftSide: ID^ args ;
args: (ID)* ;

expr: orExpr ;
orExpr:   xorExpr (OR^ xorExpr)* ;
xorExpr:  andExpr (XOR^ andExpr)* ;
andExpr:  eqExpr  (AND^ eqExpr)* ;
eqExpr:   cmpExpr  ( (EQ^ | NOTEQ^) cmpExpr)* ;
cmpExpr:  addExpr  ( (LESS^ | GREATER^ | LTE^ | GTE^) addExpr)* ;
addExpr:  multExpr ( (PLUS^ | MINUS^) multExpr)* ;
multExpr: app      ( (TIMES^ | OVER^) app)* ;
app
    : ID^ (value)*
    | LITERAL^ (value)*
    ;
value
    : ID 
    | LITERAL
    | LPAREN! expr RPAREN!
    ;



//////////////////////////////////////////////////////////////////////////////


class ShaderValidator extends TreeParser;

options {
    defaultErrorHandler = false;
}

program returns [ProgramPtr p] {
    p.reset(new Program);
    DefinitionPtr def;
}
    : ( def=definition {
            p->definitions.push_back(def);
        } )*
    ;

definition returns [DefinitionPtr d] {
    d.reset(new Definition);
    ArgumentList arguments;
    NodePtr e;
}
    : #(IS #(name:ID arguments=args) e=expr) {
            d->name = name->getText();
            d->arguments = arguments;
            d->expression = e;
        }
    ;

args returns [ArgumentList arglist]
    : ( arg:ID {
            Argument a;
            a.name = arg->getText();
            arglist.push_back(a);
        } )*
    ;

expr returns [NodePtr node] {
    NodePtr lhs, rhs;
    std::vector<NodePtr> v;
}
    : #(OR      lhs=expr rhs=expr) { node = makeBinaryNode("||", lhs, rhs); }
    | #(XOR     lhs=expr rhs=expr) { node = makeBinaryNode("^^", lhs, rhs); }
    | #(AND     lhs=expr rhs=expr) { node = makeBinaryNode("&&", lhs, rhs); }
    | #(EQ      lhs=expr rhs=expr) { node = makeBinaryNode("==", lhs, rhs); }
    | #(NOTEQ   lhs=expr rhs=expr) { node = makeBinaryNode("!=", lhs, rhs); }
    | #(LESS    lhs=expr rhs=expr) { node = makeBinaryNode("<",  lhs, rhs); }
    | #(GREATER lhs=expr rhs=expr) { node = makeBinaryNode(">",  lhs, rhs); }
    | #(LTE     lhs=expr rhs=expr) { node = makeBinaryNode("<=", lhs, rhs); }
    | #(GTE     lhs=expr rhs=expr) { node = makeBinaryNode(">=", lhs, rhs); }
    | #(PLUS    lhs=expr rhs=expr) { node = makeBinaryNode("+",  lhs, rhs); }
    | #(MINUS   lhs=expr rhs=expr) { node = makeBinaryNode("-",  lhs, rhs); }
    | #(TIMES   lhs=expr rhs=expr) { node = makeBinaryNode("*",  lhs, rhs); }
    | #(OVER    lhs=expr rhs=expr) { node = makeBinaryNode("/",  lhs, rhs); }
    | #(name:ID v=values) {
            node.reset(new ApplicationNode(name->getText(), v));
        }
    | #(lit:LITERAL v=values) {
            node.reset(new ApplicationNode(lit->getText(), v));
        }
    ;

values returns [std::vector<NodePtr> values] {
    NodePtr e;
}
    : ( e=expr { values.push_back(e); } )*
    ;
