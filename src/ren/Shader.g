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
    APPLY;  // Imaginary token.

    ATTRIBUTE = "attribute";
    CONSTANT  = "constant";
    UNIFORM   = "uniform";
}

ID: ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')* ;
LITERAL: ('0'..'9')+ ( '.' ('0'..'9')* )?;

NEWLINE: ('\n' | '\r' | '\r' '\n')    { newline(); } ;
WS     : (' ' | '\t')                 { $setType(antlr::Token::SKIP); } ;
COMMENT: ( '#' (~ ('\n' | '\r') )* )  { $setType(antlr::Token::SKIP); } ;

CONCAT: "++";

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

DOT:     '.'  ;

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

statement : input | definition ;

input     : constant | uniform | attribute ;
constant  : CONSTANT^  ID ID ;
uniform   : UNIFORM^   ID ID ;
attribute : ATTRIBUTE^ ID ID ;

definition: leftSide IS^ expr ;
leftSide: ID^ args ;
args: (ID)* ;

expr: concatExpr ;

concatExpr: orExpr (CONCAT^ orExpr)* ;
orExpr:     xorExpr (OR^ xorExpr)* ;
xorExpr:    andExpr (XOR^ andExpr)* ;
andExpr:    eqExpr  (AND^ eqExpr)* ;
eqExpr:     cmpExpr  ( (EQ^ | NOTEQ^) cmpExpr)* ;
cmpExpr:    addExpr  ( (LESS^ | GREATER^ | LTE^ | GTE^) addExpr)* ;
addExpr:    multExpr ( (PLUS^ | MINUS^) multExpr)* ;
multExpr:   signExpr ( (TIMES^ | OVER^) signExpr)* ;
signExpr:   (PLUS^ | MINUS^)? app ;
app! :
        (f:dottedvalue a:argvalues)
        {
            //if (#f) std::cout << "f: " << #f->toStringList() << "\n";
            //if (#a) std::cout << "a: " << #a->toStringList() << "\n";
            if (#a) {
                #app = #([APPLY, "@"], f, a);
            } else {
                #app = #f;
            }
            //if (#app) std::cout << "app: " << #app->toStringList() << "\n";
        }
    ;

argvalues : (dottedvalue)* ;

dottedvalue!
    : v:value { #dottedvalue = #v; }
      (
      DOT w:value { #dottedvalue = #([APPLY, "@"], w, #dottedvalue); }
      )*
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
    : (
            #(UNIFORM type:ID name:ID) {
                // TYPE=type->getText();
                p->uniforms.push_back(Uniform(VEC4, name->getText()));
            }
        |
            def=definition {
                p->definitions.push_back(def);
            }
        )*
    ;

definition returns [DefinitionPtr d] {
    d.reset(new Definition);
    ArgumentList arguments;
    SyntaxNodePtr e;
}
    : #(IS #(name:ID arguments=args) e=expr) {
            d->name = name->getText();
            d->arguments = arguments;
            d->expression = e;
        }
    ;

args returns [ArgumentList arglist]
    : ( arg:ID {
            arglist.push_back(arg->getText());
        } )*
    ;

expr returns [SyntaxNodePtr node] {
    SyntaxNodePtr e, lhs, rhs;
    SyntaxNodeList v;
}
    : #(CONCAT  lhs=expr rhs=expr) { node = makeBinaryNode("++", lhs, rhs); }
    | #(OR      lhs=expr rhs=expr) { node = makeBinaryNode("||", lhs, rhs); }
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
    | #(APPLY e=expr v=values) {
            v.insert(v.begin(), e);
            node.reset(new ApplySyntaxNode(v));
        }
    | id:ID {
            node.reset(new ValueSyntaxNode(id->getText()));
        }
    | lit:LITERAL {
            node.reset(new ValueSyntaxNode(lit->getText()));
        }
    ;

values returns [SyntaxNodeList values] {
    SyntaxNodePtr e;
}
    : ( e=expr { values.push_back(e); } )+
    ;
