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

        virtual StatementList& getChildren() = 0;

        virtual void write(std::ostream& os, int indent) const = 0;
    };


    struct IfStatement : public Statement {
        IfStatement()
        : children(2) {
        }

        CodeNodePtr getExpression() const {
            return condition;
        }

        void setExpression(CodeNodePtr node) {
            condition = node;
        }

        void setTrue(StatementPtr st) {
            assert(children.size() == 2);
            children[0] = st;
        }

        void setFalse(StatementPtr st) {
            assert(children.size() == 2);
            children[1] = st;
        }

        StatementList& getChildren() {
            assert(children.size() == 2);
            return children;
        }

        void write(std::ostream& os, int indent) const {
            assert(children.size() == 2);

            beginline(os, indent) << "if (" << condition->asExpression() << ")\n";
            children[0]->write(os, indent + 1);
            beginline(os, indent) << "else\n";
            children[1]->write(os, indent + 1);
        }

        CodeNodePtr condition;
        StatementList children;
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

        StatementList& getChildren() {
            return children;
        }

        void write(std::ostream& os, int indent) const {
            beginline(os, indent) << type << " " << name << ";\n";
        }

        Type type;
        string name;

    private:
        StatementList children;
    };
    REN_SHARED_PTR(Declaration);


    struct Assignment : public Statement {
        CodeNodePtr getExpression() const {
            return rhs;
        }

        void setExpression(CodeNodePtr node) {
            rhs = node;
        }

        StatementList& getChildren() {
            return children;
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

    private:
        StatementList children;
    };
    REN_SHARED_PTR(Assignment);


    struct Block : public Statement {
        CodeNodePtr getExpression() const {
            return CodeNodePtr();
        }

        void setExpression(CodeNodePtr node) {
            assert(!"Blocks don't have expressions.");
        }

        StatementList& getChildren() {
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
