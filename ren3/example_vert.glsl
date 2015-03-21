uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uInvViewMatrix;
uniform mat3 uTextureMatrix;
 
attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec4 vTangent;
attribute vec2 vTexCoord;
attribute vec4 vColor;
attribute lowp vec4 vBoneWeights;
attribute lowp vec4 vBoneIndices;
 
varying vec2 fTexCoord;
varying vec3 fAmbient;
varying vec3 fEye;
varying vec3 fWorldPos;
varying vec3 fNormal;
varying vec4 fTangent;
varying vec4 fColor;
 
// This is not 7 lights but a collection of all 'low priority' lights,
// composed onto six basis vectors (with ambient light in the 7th).
uniform vec4 uCollectedLights[7];
 
uniform float uFogMultiplier;
uniform vec4 uFogParams;
#define fogNear uFogParams.x
#define fogFar uFogParams.y
#define fogDensity uFogParams.z
#define fogInvRange 1.0/(fogFar-fogNear)
 
varying float fFogIntensity;
 
struct ShaderState {
    vec3 eye;
    vec2 texCoord;
    vec3 worldPosition;
    vec3 normal;
    vec4 tangent;
    vec4 color;
    vec4 positionModel;
    vec3 normalModel;
    vec3 normalWorld;
    vec4 tangentModel;
    vec4 tangentWorld;
    vec3 scale;
    float rotation;
};
 
void main() {
    ShaderState ss;
    vec3 ssAmbient = vec3(0.0);

    // scaled/biased bytes are mapped to [0,254] instead of [0,255],
    // so that there will be a middle value exactly corresponding to zero
    // $$$ in case the shader compiler is too dumb to make a constant out of this
    //float factor = (2.0 * (255.0/254.0));
    float factor = 2.00787401574804; 
    vec3 n = (vNormal * factor) - vec3(1.0);
    ss.normalModel = ss.normalWorld = n;

    // scaled/biased bytes are mapped to [0,254] instead of [0,255],
    // so that there will be a middle value exactly corresponding to zero
    vec4 t = (2.0 * (255.0/254.0) * vTangent) - vec4(1.0);
    ss.tangentModel = ss.tangentWorld = t;
    ss.texCoord = vTexCoord.xy;
    ss.color = vec4(1.0);

    // call pre-transform, transform, and post-transform modules
    vec3 posN = clamp(s.normalWorld, 0.0, 1.0);
    vec3 negN = clamp(-s.normalWorld, 0.0, 1.0);
    s.ambient = (
          posN.x * uCollectedLights[0].xyz
        + posN.y * uCollectedLights[1].xyz
        + posN.z * uCollectedLights[2].xyz
        + negN.x * uCollectedLights[3].xyz
        + negN.y * uCollectedLights[4].xyz
        + negN.z * uCollectedLights[5].xyz
        + uCollectedLights[6].xyz
    );

    ss.normalWorld = normalize(uNormalMatrix * vec4(ss.normalModel,0.0)).xyz;
    vec4 ssPositionWorld = uModelMatrix * ss.positionModel;
    vec4 ssPositionView = uViewMatrix * ssPositionWorld;
    gl_Position = uProjMatrix * ssPositionView;

    float fogCoord = -s.positionView.z;
    fFogIntensity = clamp(fogDensity * uFogMultiplier * (fogCoord-fogNear) * fogInvRange, 0.0, 1.0);

    // finish state
    ss.worldPosition = ssPositionWorld.xyz;
    ss.normal = ss.normalWorld;

    // output varyings
    fAmbient = ssAmbient;
    fWorldPos = ss.worldPosition;
    fNormal = ss.normal;
    fTangent = ss.tangentWorld;
    fTexCoord = ss.texCoord;
    fColor = ss.color;
    fEye = uInvViewMatrix[3].xyz - ssPositionWorld.xyz;
}
