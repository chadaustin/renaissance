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
Function: gl_Position
  Expression: (* gl_ModelViewProjection gl_Vertex)
""" },

    
    'expr.rs':  { 'lexer':     """\
foo = 1
bar = 9 8
baz = a b c
bing x y = sin x + baz 2 sin 3
""",
                  
                  'parser':    """\
 ( = foo 1 )
 ( = bar ( 9 8 ) )
 ( = baz ( a b c ) )
 ( = ( bing x y ) ( + ( sin x ) ( baz 2 sin 3 ) ) )
""",
                  
                  'validator': """\
Function: foo
  Expression: 1
Function: bar
  Expression: (9 8)
Function: baz
  Expression: (a b c)
Function: bing
  Arguments: x y
  Expression: (+ (sin x) (baz 2 sin 3))
""" },


    'function.rs':  { 'lexer': """\
mult matrix vector = matrix * vector
gl_Position = mult gl_ModelViewProjection gl_Vertex
""",

                      'parser': """\
 ( = ( mult matrix vector ) ( * matrix vector ) )
 ( = gl_Position ( mult gl_ModelViewProjection gl_Vertex ) )
""",

                      'validator': """\
Function: mult
  Arguments: matrix vector
  Expression: (* matrix vector)
Function: gl_Position
  Expression: (mult gl_ModelViewProjection gl_Vertex)
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
            print "---- STDOUT:"
            print stdout,
            print "---- EXPECTED:"
            print output,
            print "----"
    print

