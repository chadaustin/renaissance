import os

env = Environment(ENV=os.environ)
if env.subst('$CC') == 'gcc':
    env.Append(CCFLAGS=['-Wall', '-g'],
               LINKFLAGS=['-g'])

Export('env')
SConscript(dirs=['src'])
