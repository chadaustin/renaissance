def generate(env):
    if env['PLATFORM'] == 'cygwin':
        env.Append(CPPPATH = '/usr/local/include/SDL',
                   CPPDEFINES = ['WIN32', '_WIN32'],
                   LIBS = ['SDL'])
    elif env['PLATFORM'] == 'win32':
        env.Append(LIBS=['SDL', 'SDLmain'])
    else:
        env.ParseConfig('sdl-config', ['--cflags', '--libs'])

def exists(env):
    return 1
