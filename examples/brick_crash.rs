#On Fri, 2005-07-15 at 00:55 -0500, Chad Austin wrote:
#
#>> o_O  ouch.  do you know what you changed last?
#
#
#I've played with it, and I think this line is the culprit:
#
#
#>>> > lightFactor = if EnableLighting then 1.0 - LightIntensity else 1.0
#
#
#replacing it with 
#
#lightFactor = if EnableLighting then (1.0 - LightIntensity) else 1.0
#
#makes the crash go away.
	


# One more: the attached shader crashes the compiler.  ;) 
#
#    Dirk

# Switches.

constant bool EnableLighting

# Uniforms.

uniform vec3 LightPosition
uniform vec3 BrickColor
uniform vec3 MortarColor
uniform vec2 BrickSize
uniform vec2 BrickPct


# Constants.

SpecularContribution = 1.0
DiffuseContribution = 0.0 #1.0 - SpecularContribution


# Transform.

gl_Position = ftransform

ecPosition = (gl_ModelViewMatrix * gl_Vertex).xyz
tnorm = normalize (gl_NormalMatrix * gl_Normal)


# Lighting.

lightVec   = normalize (LightPosition - ecPosition)
reflectVec = reflect (-lightVec) tnorm
viewVec    = normalize (-ecPosition)

diffuse = max (dot lightVec viewVec) 0.0
s = pow (max (dot reflectVec viewVec) 0.0) 8.0
spec = if (diffuse > 0.0) then s else 0.0
LightIntensity = DiffuseContribution * diffuse + SpecularContribution * spec

# Brick.

origposition = gl_Vertex.xy / BrickSize
condition = fract (origposition.y * 0.5) > 0.5
xoffset = if condition then 0.5 else 0.0
position = origposition + vec2 xoffset 0.0

useBrick = step (fract position) BrickPct

lightFactor = if EnableLighting then 1.0 - LightIntensity else 1.0
amount = useBrick.x * useBrick.y
color = (mix MortarColor BrickColor amount) * (vec3 lightFactor lightFactor lightFactor)

gl_FragColor = color ++ 1.0
#gl_FragColor = BrickPct ++ 0.0 ++ 1.0
