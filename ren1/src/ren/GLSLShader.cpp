#include <sstream>
#include "GLSLShader.h"


namespace ren {

    void countReferences(
        CodeNodePtr node,
        ReferenceMap& refs,
        const ReferencePath& path
    ) {
        refs[node].push_back(path);
        CodeNodeList children = node->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            countReferences(children[i], refs, path);
        }
    }


    static void doCountReferences(
        StatementPtr statement,
        ReferenceMap& refs,
        ReferencePath path
    ) {
        assert(statement);
        path.push_back(statement);

        if (CodeNodePtr e = statement->getExpression()) {
            countReferences(e, refs, path);
        }
        
        StatementList children = statement->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            doCountReferences(children[i], refs, path);
        }
    }


    void countReferences(StatementPtr node, ReferenceMap& refs) {
        return doCountReferences(node, refs, ReferencePath());
    }


    CodeNodePtr findMultiplyReferenced(
        CodeNodePtr node,
        const ReferenceMap& refs
    ) {
        assert(node);

        ReferenceMap::const_iterator it = refs.find(node);

        if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
            if (it != refs.end() && it->second.size() > 1) {
                return p;
            }
        } else if (REN_DYNAMIC_CAST_PTR(p, IfCodeNode, node)) {
            if (it != refs.end() && it->second.size() > 1) {
                return p;
            }
        } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
            // Do nothing.
        } else {
            assert(!"Unknown code node type");
        }

        CodeNodeList args = node->getChildren();
        for (size_t i = 0; i < args.size(); ++i) {
            CodeNodePtr c = findMultiplyReferenced(args[i], refs);
            if (c) {
                return c;
            }
        }
        return CodeNodePtr();
    }


    CodeNodePtr findMultiplyReferenced(
        StatementPtr stmt,
        const ReferenceMap& refs
    ) {
        assert(stmt);

        if (CodeNodePtr e = stmt->getExpression()) {
            if (CodeNodePtr f = findMultiplyReferenced(e, refs)) {
                return f;
            }
        }

        StatementList children = stmt->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            if (CodeNodePtr f = findMultiplyReferenced(children[i], refs)) {
                return f;
            }
        }

        return CodeNodePtr();
    }


    CodeNodePtr findInterpolatable(CodeNodePtr node) {
        if (REN_DYNAMIC_CAST_PTR(p, CallCodeNode, node)) {
            if (p->canInterpolate()) {
                return p;
            }
        } else if (REN_DYNAMIC_CAST_PTR(p, NameCodeNode, node)) {
            if (p->getInputType() == ValueNode::BUILTIN &&
                p->getFrequency() == VERTEX
            ) {
                return p;
            }
        }

        CodeNodeList args = node->getChildren();
        for (size_t i = 0; i < args.size(); ++i) {
            CodeNodePtr c = findInterpolatable(args[i]);
            if (c) {
                return c;
            }
        }

        return CodeNodePtr();
    }


    CodeNodePtr findInterpolatable(StatementPtr stmt) {
        if (CodeNodePtr e = stmt->getExpression()) {
            if (CodeNodePtr f = findInterpolatable(e)) {
                return f;
            }
        }

        StatementList children = stmt->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            if (CodeNodePtr f = findInterpolatable(children[i])) {
                return f;
            }
        }

        return CodeNodePtr();        
    }


    IfCodeNodePtr findBranch(CodeNodePtr node) {
        assert(node);

        if (REN_DYNAMIC_CAST_PTR(p, IfCodeNode, node)) {
            return p;
        }

        CodeNodeList args = node->getChildren();
        for (size_t i = 0; i < args.size(); ++i) {
            IfCodeNodePtr c = findBranch(args[i]);
            if (c) {
                return c;
            }
        }
        return IfCodeNodePtr();        
    }


    IfCodeNodePtr findBranch(StatementPtr stmt, StatementPtr& ref) {
        assert(stmt);

        if (CodeNodePtr e = stmt->getExpression()) {
            if (IfCodeNodePtr f = findBranch(e)) {
                ref = stmt;
                return f;
            }
        }

        StatementList children = stmt->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            if (IfCodeNodePtr f = findBranch(children[i], ref)) {
                return f;
            }
        }

        return IfCodeNodePtr();        
    }



    void replace(CodeNodePtr in, CodeNodePtr node, CodeNodePtr with) {
        assert(in);
        assert(node);
        assert(with);
        
        CodeNodeList& args = in->getChildren();
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == node) {
                args[i] = with;
            } else {
                replace(args[i], node, with);
            }
        }
    }


    void replace(StatementPtr st, CodeNodePtr node, CodeNodePtr with) {
        assert(st);
        assert(node);
        assert(with);

        if (CodeNodePtr e = st->getExpression()) {
            if (e == node) {
                st->setExpression(with);
            } else {
                replace(e, node, with);
            }
        }

        StatementList children = st->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            replace(children[i], node, with);
        }       
    }


    void replace(StatementPtr& in, StatementPtr st, StatementPtr with) {
        assert(in);
        assert(st);
        assert(with);

        if (in == st) {
            in = with;
            return;
        }

        StatementList& children = in->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            replace(children[i], st, with);
        }       
    }


    ReferencePath getCommonPrefix(
        const ReferencePath& p1,
        const ReferencePath& p2
    ) {
        size_t i = 0;
        while (i < p1.size() && i < p2.size() && p1[i] == p2[i]) {
            ++i;
        }
        return ReferencePath(p1.begin(), p1.begin() + i);
    }


    ReferencePath getCommonPrefix(const ReferenceList& rl) {
        assert(rl.size() >= 2);
        ReferencePath prefix = getCommonPrefix(rl[0], rl[1]);
        for (size_t i = 2; i < rl.size(); ++i) {
            prefix = getCommonPrefix(prefix, rl[i]);
        }
        return prefix;
    }


    void removeRedundantBlocks(BlockPtr b) {
        StatementList& children = b->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            if (REN_DYNAMIC_CAST_PTR(ib, Block, children[i])) {
                removeRedundantBlocks(ib);
                StatementList sl = ib->getChildren();
                children.erase(children.begin() + i);
                children.insert(children.begin() + i, sl.begin(), sl.end());
                --i;
            }
        }
    }


    GLSLShader::GLSLShader()
    : main(new Block)
    , _varying(0)
    , _register(0) {
    }

    template<typename VecType>
    void writeGlobalArray(std::ostream& os, const string& prefix, const VecType& g) {
        for (size_t i = 0; i < g.size(); ++i) {
            os << prefix << g[i].type << " " << g[i].name << ";\n";
        }
    }


    void GLSLShader::generate(std::ostream& os) {
        // Turn shared expression nodes into precalculated variables.
        share();

        // First, we need to turn If nodes into separate statements.
        splitBranches();

        // Sharing set up a lot of redundant (and incorrectly
        // scope-restricting) block nodes.  Let's remove those.
        removeRedundantBlocks(main);

        output(os);
    }


    bool compare(const GLSLShader::Global& a, const GLSLShader::Global& b) {
        return a.name < b.name;
    }


    void GLSLShader::output(std::ostream& os) {
        std::sort(constants .begin(), constants .end(), compare);
        std::sort(uniforms  .begin(), uniforms  .end(), compare);
        std::sort(attributes.begin(), attributes.end(), compare);
        std::sort(varyings  .begin(), varyings  .end(), compare);

        // Write constants with initializers.
        for (size_t i = 0; i < constants.size(); ++i) {
            Constant& c = constants[i];
            string init = c.value->asString();
            os << "const " << c.type << " " << c.name << " = "
               << init << ";\n";
        }

        writeGlobalArray(os, "uniform ", uniforms);
        writeGlobalArray(os, "attribute ", attributes);
        writeGlobalArray(os, "varying ", varyings);

        if (!main->statements.empty()) {
            os << "void main()\n";
            main->write(os, 0);
        }
    }


    void GLSLShader::splitBranches() {
        // Conversion to StatementPtr overload is ambiguous otherwise.
        StatementPtr main_stmt(main);

        StatementPtr ref;
        while (IfCodeNodePtr p = findBranch(main_stmt, ref)) {

            CodeNodePtr condition = p->getChildren()[0];
            CodeNodePtr truePart  = p->getChildren()[1];
            CodeNodePtr falsePart = p->getChildren()[2];

            string name = newRegisterName();

            CodeNodePtr nameReference(
                    new NameCodeNode(
                        name,
                        p->getType(),
                        p->getFrequency(),
                        NullValue,
                        ValueNode::BUILTIN)); // suitable substitute for local

            replace(main_stmt, p, nameReference);
            
            DeclarationPtr decl(new Declaration(p->getType(), name));

            AssignmentPtr assignTrue(new Assignment);
            assignTrue->define = false;
            assignTrue->lhs = name;
            assignTrue->rhs = truePart;

            AssignmentPtr assignFalse(new Assignment);
            assignFalse->define = false;
            assignFalse->lhs = name;
            assignFalse->rhs = falsePart;

            IfStatementPtr ifst(new IfStatement);
            ifst->condition = condition;
            ifst->setTrue(assignTrue);
            ifst->setFalse(assignFalse);

            BlockPtr newBlock(new Block);
            newBlock->statements.push_back(decl);
            newBlock->statements.push_back(ifst);
            newBlock->statements.push_back(ref);

            if (main_stmt == ref) {
                REN_DYNAMIC_CAST_PTR(main, Block, ref);
                assert(main);
                main_stmt = main;
            } else {
                replace(main_stmt, ref, newBlock);
            }
        }
    }


    void GLSLShader::share() {
        ReferenceMap rm;
        countReferences(main, rm);

//        std::cout << "++++\n";
//        output(std::cout);

        // Conversion to StatementPtr overload is ambiguous otherwise.
        StatementPtr main_stmt(main);
        while (CodeNodePtr p = findMultiplyReferenced(main_stmt, rm)) {
            ReferenceList rl = rm[p];
            ReferencePath prefix(getCommonPrefix(rl));
            assert(prefix[0] == main);

            string name = newRegisterName();

            CodeNodePtr nameReference(
                    new NameCodeNode(
                        name,
                        p->getType(),
                        p->getFrequency(),
                        NullValue,
                        ValueNode::BUILTIN)); // suitable substitute for local

            replace(main_stmt, p, nameReference);

            AssignmentPtr ns(new Assignment);
            ns->define = true;
            ns->lhs = name;
            ns->rhs = p;

            assert(!prefix.empty());
            StatementPtr lastInCommonPrefix(prefix[prefix.size() - 1]);

            // Make a new block.
            BlockPtr newBlock(new Block);
            newBlock->statements.push_back(ns);
            newBlock->statements.push_back(lastInCommonPrefix);

            if (main_stmt == lastInCommonPrefix) {
                main = newBlock;
                main_stmt = main;
            } else {
                replace(main_stmt, lastInCommonPrefix, newBlock);
            }

            rm.clear();
            countReferences(main, rm);

//            std::cout << "++++\n";
//            output(std::cout);
        }
    }


    string GLSLShader::newVaryingName() {
        std::ostringstream os;
        os << "_ren_v" << _varying++;
        return os.str();
    }


    string GLSLShader::newRegisterName() {
        std::ostringstream os;
        os << "_ren_r" << _register++;
        return os.str();
    }

}
