#ifndef REN_PROGRAM_SCOPE_H
#define REN_PROGRAM_SCOPE_H


#include <map>
#include <boost/enable_shared_from_this.hpp>
#include "Program.h"
#include "Scope.h"


namespace ren {

    typedef std::pair<std::string, Type> Signature;


    class ProgramScope
        : public Scope
        , public boost::enable_shared_from_this<ProgramScope> {
    private:
        ProgramScope(ProgramPtr program)
        : _program(program) {
        }

    public:
        typedef boost::shared_ptr<ProgramScope> Ptr;

        static Ptr create(ProgramPtr program) {
            return Ptr(new ProgramScope(program));
        }

        ConcreteNodePtr lookup(const string& name, Type argTypes);

    private:
        ConcreteNodePtr cache(Signature sig, ConcreteNodePtr value) {
            assert(value);
            _lookupCache[sig] = value;
            return value;
        }

        ConcreteNodePtr instantiate(SyntaxNodePtr syntaxNode, ScopePtr scope);

        ProgramPtr _program;

        typedef std::map<Signature, bool> RecursionCheck;
        RecursionCheck _recursionCheck;

        typedef std::map<Signature, ConcreteNodePtr> LookupCache;
        LookupCache _lookupCache;

        friend class RecursionGuard;
    };

}


#endif
