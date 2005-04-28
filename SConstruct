import os

TargetSignatures('content')

env = Environment(toolpath=[Dir('toolspec').abspath])

if env['PLATFORM'] != 'win32':
    env['ENV'] = os.environ
else:
    env.Append(CXXFLAGS=['/GX', '/GR'])

if env.subst('$CC') == 'gcc':
    env.Append(CCFLAGS=['-Wall', '-g', '-Werror'],
               LINKFLAGS=['-g'])

Export('env')
BuildDir('build', '.', duplicate=0)
SConscript(dirs=['build/src',
                 'build/test'])
