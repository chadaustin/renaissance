import os

#TargetSignatures('content')

env = Environment(ENV=os.environ,
                  toolpath=[Dir('toolspec').abspath])

if env.subst('$CC') == 'gcc':
    env.Append(CCFLAGS=['-Wall', '-g'],
               LINKFLAGS=['-g'])

Export('env')
for d in ['src', 'test']:
    SConscript(dirs=d, build_dir='build/' + d, duplicate=0)
