gl_Position = ftransform

uniform float CoolestTemp
uniform float TempRange
uniform vec3 CoolestColor
uniform vec3 HottestColor

attribute float VertexTemp

Temperature = (VertexTemp - CoolestTemp) / TempRange
color = mix CoolestColor HottestColor Temperature
gl_FragColor = color ++ 1.0
