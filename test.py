#!/usr/bin/env python

import os

tests = {
    'blank.rs': { 'lexer':     '',
                  'parser':    '',
                  'validator': '' },

    
    'basic.rs': { 'lexer':     """\
gl_Position = gl_ModelViewProjection * gl_Vertex
""",
                  
                  'parser':    """\
 ( = gl_Position ( * gl_ModelViewProjection gl_Vertex ) )
""",
                  
                  'validator': """\
Program:
  Name: gl_Position
""" },

    
    'expr.rs':  { 'lexer':     """\
foo = 1
bar x y = sin x + baz 1 sin 2
""",
                  
                  'parser':    """\
 ( = foo 1 )
 ( = ( bar x y ) ( + sin x baz 1 sin 2 ) )
""",
                  
                  'validator': """\
Program:
  Name: foo
  Name: bar
""" },

    
    'function.rs':  { 'lexer': """\
mult matrix vector = matrix * vector
gl_Position = mult gl_ModelViewProjection gl_Vertex
""",
                      
                      'parser': """\
 ( = ( mult matrix vector ) ( * matrix vector ) )
 ( = gl_Position mult gl_ModelViewProjection gl_Vertex )
""",

                      'validator': """\
Program:
  Name: mult
  Name: gl_Position
""" } }


executables = {
    'lexer'    : 'testLexer',
    'parser'   : 'testParser',
    'validator': 'testValidator' }

for script, outputs in tests.iteritems():
    print script
    for name, output in outputs.iteritems():
        exe = executables[name]

        stdin, stdout, stderr = os.popen3(exe + ' examples/' + script)
        stdout = stdout.read()
        stderr = stderr.read()

        result = 'failed'
        if stdout == output:
            result = 'passed'
        print '  %s: %s' % (name.ljust(10), result)
        if result == 'failed':
            print "---- stdout:"
            print stdout,
            print "---- expected:"
            print output,
            print "----"
