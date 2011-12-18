def generate(env):
    env.Append(CPPDEFINES=['GLEW_STATIC'],
               LIBS=['opengl32', 'glu32'])

def exists(env):
    return 1
