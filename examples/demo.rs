constant bool texturing
uniform sampler2D texture

gl_Position = ftransform

white = vec4 1.0 1.0 1.0 1.0
texel = if texturing then texture2D texture gl_MultiTexCoord0.xy else white
gl_FragColor = texel * gl_Color
