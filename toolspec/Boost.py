def generate(env):
    if env['PLATFORM'] == 'win32':
        env.Append(CPPPATH=['#/third-party/vc7/Boost/include'])
    else:
        env.Append(CPPPATH=['/usr/local/include/boost-1_32'])

def exists(env):
    return 1
