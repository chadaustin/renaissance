import os

TargetSignatures('content')

env = Environment(ENV=os.environ,
                  toolpath=[Dir('toolspec').abspath])

if env.subst('$CC') == 'gcc':
    env.Append(CCFLAGS=['-Wall', '-g'],
               LINKFLAGS=['-g'])

Export('env')
SConscript(dirs=['src'])
