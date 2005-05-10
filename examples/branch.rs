uniform bool condition
p1 = ftransform
p2 = gl_ModelViewProjectionMatrix * gl_Vertex
gl_Position = if condition then p1 else p2
