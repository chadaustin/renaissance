def generate(env):
    env.Append(CPPPATH=['/usr/local/include/boost-1_31'])

def exists(env):
    return 1
