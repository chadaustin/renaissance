#include <sstream>
#include "ShadeGraph.h"
#include "CompilationContext.h"
#include "Compiler.h"
#include "ConcreteNode.h"
#include "Errors.h"

// Generated headers.
#include "ShaderLexer.hpp"
#include "ShaderParser.hpp"
#include "ShaderValidator.hpp"


namespace ren {

    ProgramPtr parse(const string& source) {
        std::istringstream is(source);

        antlr::ASTFactory parserFactory;
        ShaderLexer lexer(is);
        ShaderParser parser(lexer);
        parser.initializeASTFactory(parserFactory);
        parser.setASTFactory(&parserFactory);
        parser.program();

        if (antlr::RefAST ast = parser.getAST()) {
            ShaderValidator validator;
            return validator.program(ast);
        } else {
            // Parser didn't build an AST or throw an error.  Thus,
            // the program is empty.
            return ProgramPtr(new Program);
        }

    }


    inline void requireType(
        const string& name,
        ConcreteNodePtr node,
        Type type
    ) {
        if (node->getType() != type) {
            throw CompileError(
                name + " must have type " + type.getName() + ".  "
                "It has type: " + node->getType().getName());
        }
    }


    static CompileResult doCompile(
        const string& source,
        std::ostream& output
    ) {
        ProgramPtr program = parse(source);
        if (!program) {
            // No exceptions thrown, but no program generated.  Must be empty.
            return CompileResult(true);
        }

        CompilationContext cc(program);

        // Build code graph.

        ShadeGraph sg;
        
        if (program->hasDefinition("gl_Position")) {
            ConcreteNodePtr gl_Position = cc.instantiate("gl_Position");
            if (gl_Position) {
                requireType("gl_Position", gl_Position, VEC4);
                sg.outputs.insert(std::make_pair(ShadeGraph::Output("gl_Position"),
                                                 cc.evaluate(gl_Position)));
            }
        }

        if (program->hasDefinition("gl_FragColor")) {
            ConcreteNodePtr gl_FragColor = cc.instantiate("gl_FragColor");
            if (gl_FragColor) {
                requireType("gl_FragColor", gl_FragColor, VEC4);
                sg.outputs.insert(std::make_pair(ShadeGraph::Output("gl_FragColor"),
                                                 cc.evaluate(gl_FragColor)));
            }
        }

        // Split into vertex and fragment stages.
        GLSLShader vs, fs;
        sg.generate(vs, fs);

        // Generate GLSL.
        std::ostringstream vs_glsl;
        std::ostringstream fs_glsl;
        vs.generate(vs_glsl);
        fs.generate(fs_glsl);

        return CompileResult(
            true,
            vs_glsl.str(),
            fs_glsl.str());
    }


    CompileResult compile(const string& source, std::ostream& output) {
        static CompileResult FAILURE(false);

        try {
            return doCompile(source, output);
        }
        catch (const antlr::ANTLRException& e) {
            output << "ANTLR Exception: " << e.toString() << std::endl;
            return FAILURE;
        }
        catch (const std::exception& e) {
            output << "Exception: " << e.what() << std::endl;
            return FAILURE;
        }

    }

}
