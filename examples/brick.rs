# Uniforms.

uniform vec3 LightPosition
uniform vec3 BrickColor
uniform vec3 MortarColor
uniform vec2 BrickSize
uniform vec2 BrickPct


# Constants.

SpecularContribution = 0.3
DiffuseContribution = 1.0 - SpecularContribution


# Transform.

gl_Position = ftransform
ecPosition = (gl_ModielViewMatrix * gl_Vertex).xyz
tnorm = normalize (gl_NormalMatrix * gl_Normal)


# Lighting.

lightVec   = normalize (LightPosition - ecPosition)
reflectVec = reflect (-lightVec) tnorm
viewVec    = normalize (-ecPosition)

diffuse = max (dot lightVec viewVec) 0.0
spec = if (diffuse > 0.0) then s else 0.0
       where s = pow (max (dot reflectVec viewVec) 0.0) 16.0
LightIntensity = DiffuseContribution * diffuse + SpecularContribution * specular

# Brick.

position = gl_Vertex.xy / BrickSize + vec2 xoffset 0.0
           where xoffset = if (fract (position.y * 0.5) > 0.5) then 0.5 else 0.0
useBrick = step (fract position) BrickPct

color = mix MortarColor BrickColor amount
        where amount = useBrick.x * useBrick.y * LightIntensity
gl_FragColor = color ++ 1.0
