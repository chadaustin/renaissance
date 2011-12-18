def generate(env):
    env.Append(LIBS=['boost_unit_test_framework-gcc'])

def exists(env):
    return 1
