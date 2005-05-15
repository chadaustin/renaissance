#ifndef GLSL_STATEMENT_H
#define GLSL_STATEMENT_H


#include <iostream>
#include "CodeNode.h"


namespace ren {

    inline std::ostream& beginline(std::ostream& os, int amount) {
        while (amount--) {
            os << "  ";
        }
        return os;
    }


    class Statement;
    REN_SHARED_PTR(Statement);
    typedef std::vector<StatementPtr> StatementList;
    

    struct Statement {
        // Shut up gcc.
        virtual ~Statement() { }

        virtual CodeNodePtr getExpression() const = 0;
        virtual void setExpression(CodeNodePtr node) = 0;

        virtual StatementList getChildren() const = 0;

        virtual void write(std::ostream& os, int indent) const = 0;
    };


    struct IfStatement : public Statement {
        CodeNodePtr getExpression() const {
            return condition;
        }

        void setExpression(CodeNodePtr node) {
            condition = node;
        }

        StatementList getChildren() const {
            StatementList rv(2);
            rv[0] = truePart;
            rv[1] = falsePart;
            return rv;
        }

        void write(std::ostream& os, int indent) const {
            beginline(os, indent) << "if (" << condition->asExpression() << ")\n";
            truePart->write(os, indent + 1);
            beginline(os, indent) << "else\n";
            falsePart->write(os, indent + 1);
        }

        CodeNodePtr condition;
        StatementPtr truePart;
        StatementPtr falsePart;
    };
    REN_SHARED_PTR(IfStatement);


    struct Declaration : public Statement {
        Declaration(Type type_, const string& name_)
        : type(type_)
        , name(name_) {
        }

        CodeNodePtr getExpression() const {
            return CodeNodePtr();
        }

        void setExpression(CodeNodePtr node) {
            assert(!"Declarations don't have expressions.");
        }

        StatementList getChildren() const {
            return StatementList();
        }

        void write(std::ostream& os, int indent) const {
            beginline(os, indent) << type << " " << name << ";\n";
        }

        Type type;
        string name;
    };
    REN_SHARED_PTR(Declaration);


    struct Assignment : public Statement {
        CodeNodePtr getExpression() const {
            return rhs;
        }

        void setExpression(CodeNodePtr node) {
            rhs = node;
        }

        StatementList getChildren() const {
            return StatementList();
        }

        void write(std::ostream& os, int indent) const {
            beginline(os, indent);
            if (define) {
                os << rhs->getType() << " ";
            }
            os << lhs << " = " << rhs->asExpression() << ";\n";
        }

        bool define;
        string lhs;
        CodeNodePtr rhs;
    };
    REN_SHARED_PTR(Assignment);


    struct Block : public Statement {
        CodeNodePtr getExpression() const {
            return CodeNodePtr();
        }

        void setExpression(CodeNodePtr node) {
            assert(!"Blocks don't have expressions.");
        }

        StatementList getChildren() const {
            return statements;
        }

        void write(std::ostream& os, int indent) const {
            beginline(os, indent) << "{\n";
            for (size_t i = 0; i < statements.size(); ++i) {
                statements[i]->write(os, indent + 1);
            }
            beginline(os, indent) << "}\n";
        }

        StatementList statements;
    };
    REN_SHARED_PTR(Block);
}


#endif
