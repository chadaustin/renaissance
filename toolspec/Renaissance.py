def generate(env):
    env.Append(CPPPATH=['#/stage/include'],
               LIBPATH=['#/stage/lib'],
               LIBS=['ren', 'antlr'])

def exists(env):
    return 1
