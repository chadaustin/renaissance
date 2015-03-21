// shader key: <vtx:[ambientcube,transform,linearfogintensity,],attributes:[vposition,vnormal,vtangent,vtexcoord,vcolor,vboneweights,vboneindices,]>
 
// --------------------
//       defines
// --------------------
 
#ifndef GL_ES
// GLSL versions < 1.3 (such as on IMVU USSR) don't support the lowp qualifier
#define lowp
#endif
// Why have this function?  Truncation semantics are not that easy to
// implement in Direct3D shader instructions, so if the compiler can't
// prove that the index is positive, it generates poor code.
//
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb147212(v=vs.85).aspx
// http://engineering.imvu.com/2014/09/26/optimizing-webgl-shaders-by-reading-d3d-shader-assembly/
 
int convertIndex(float f) {
    f = max(f, 0.0);
    return int(f);
}
 
ivec2 convertIndex(vec2 f) {
    f = max(f, vec2(0));
    return ivec2(f);
}
 
ivec3 convertIndex(vec3 f) {
    f = max(f, vec3(0));
    return ivec3(f);
}
 
ivec4 convertIndex(vec4 f) {
    f = max(f, vec4(0));
    return ivec4(f);
}
 
// --------------------
//       uniforms
// --------------------
 
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uInvViewMatrix;
uniform mat3 uTextureMatrix;
 
// --------------------
//      attributes
// --------------------
 
attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec4 vTangent;
attribute vec2 vTexCoord;
attribute vec4 vColor;
attribute lowp vec4 vBoneWeights;
attribute lowp vec4 vBoneIndices;
 
// --------------------
//       varyings
// --------------------
 
varying vec2 fTexCoord;
varying vec3 fAmbient;
varying vec3 fEye;
varying vec3 fWorldPos;
varying vec3 fNormal;
varying vec4 fTangent;
varying vec4 fColor;
 
// --------------------
//   type definitions
// --------------------
 
struct ShaderState {
    vec3 ambient;
    vec3 eye;
    vec2 texCoord;
    vec3 worldPosition;
    vec3 normal;
    vec4 tangent;
    vec4 color;
    vec4 positionModel;
    vec4 positionWorld;
    vec4 positionView;
    vec4 positionProj;
    vec3 normalModel;
    vec3 normalWorld;
    vec4 tangentModel;
    vec4 tangentWorld;
// @@@ RESOLVE THESE
    vec3 scale;
    float rotation;
};
// TODO: @@@ until custom modules are revised
#define VertexState ShaderState
#define FragmentState ShaderState
 
// --------------------
//   state functions
// --------------------
 
void initStateAmbient(inout ShaderState ss) {
    ss.ambient = vec3(0.0);
}
void varyOutStateAmbient(inout ShaderState ss) {
    fAmbient = ss.ambient;
}
void initStatePosition(inout ShaderState ss) {
    ss.positionProj = ss.positionView = ss.positionWorld = ss.positionModel = vPosition;
}
void finishStatePosition(inout ShaderState ss) {
    ss.worldPosition = ss.positionWorld.xyz;
}
void varyOutStatePosition(inout ShaderState ss) {
    fWorldPos = ss.worldPosition;
}
void initStateNormal(inout ShaderState ss) {
    // scaled/biased bytes are mapped to [0,254] instead of [0,255],
    // so that there will be a middle value exactly corresponding to zero
    // $$$ in case the shader compiler is too dumb to make a constant out of this
    //float factor = (2.0 * (255.0/254.0));
    float factor = 2.00787401574804; 
    vec3 n = (vNormal * factor) - vec3(1.0);
    ss.normalModel = ss.normalWorld = n;
}
void finishStateNormal(inout ShaderState ss) {
    ss.normal = ss.normalWorld;
}
void varyOutStateNormal(inout ShaderState ss) {
    fNormal = ss.normal;
}
void initStateTangent(inout ShaderState ss) {
    // scaled/biased bytes are mapped to [0,254] instead of [0,255],
    // so that there will be a middle value exactly corresponding to zero
    vec4 t = (2.0 * (255.0/254.0) * vTangent) - vec4(1.0);
    ss.tangentModel = ss.tangentWorld = t;
}
void finishStateTangent(inout ShaderState ss) {
    ss.tangent = ss.tangentWorld;
}
void varyOutStateTangent(inout ShaderState ss) {
    fTangent = ss.tangent;
}
void initStateTexture(inout ShaderState ss) {
    ss.texCoord = vTexCoord.xy;
}
void varyOutStateTexture(inout ShaderState ss) {
    fTexCoord = ss.texCoord;
}
void initStateColor(inout ShaderState ss) {
    ss.color = vec4(1.0);
}
void varyOutStateColor(inout ShaderState ss) {
    fColor = ss.color;
}
void finishStateEye(inout ShaderState ss) {
    ss.eye = uInvViewMatrix[3].xyz - ss.positionWorld.xyz;
}
void varyOutStateEye(inout ShaderState ss) {
    fEye = ss.eye;
}
void outputStatePosition(inout ShaderState ss) {
    gl_Position = ss.positionProj;
}
 
// --------------------
//       modules
// --------------------
 
// This is not 7 lights but a collection of all 'low priority' lights,
// composed onto six basis vectors (with ambient light in the 7th).
uniform vec4 uCollectedLights[7];
 
void AmbientCube(inout ShaderState s) {
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
}
void Transform(inout ShaderState ss) {
    ss.normalWorld = normalize(uNormalMatrix * vec4(ss.normalModel,0.0)).xyz;
    ss.positionWorld = uModelMatrix * ss.positionModel;
    ss.positionView = uViewMatrix * ss.positionWorld;
    ss.positionProj = uProjMatrix * ss.positionView;
}
uniform float uFogMultiplier;
uniform vec4 uFogParams;
#define fogNear uFogParams.x
#define fogFar uFogParams.y
#define fogDensity uFogParams.z
#define fogInvRange 1.0/(fogFar-fogNear)
 
varying float fFogIntensity;
 
void LinearFogIntensity(inout VertexState s) {
    float fogCoord = -s.positionView.z;
    fFogIntensity = clamp(fogDensity * uFogMultiplier * (fogCoord-fogNear) * fogInvRange, 0.0, 1.0);
}
 
// ====================
//       M A I N
// ====================
 
void main() {
    //
    // state initialization
    //
    ShaderState ss;
    initStateAmbient(ss);
    initStatePosition(ss);
    initStateNormal(ss);
    initStateTangent(ss);
    initStateTexture(ss);
    initStateColor(ss);
    //
    // call pre-transform, transform, and post-transform modules
    //
    AmbientCube(ss);
    Transform(ss);
    LinearFogIntensity(ss);
    //
    // finish state
    //
    finishStatePosition(ss);
    finishStateNormal(ss);
    finishStateTangent(ss);
    finishStateEye(ss);
    //
    // output varyings
    //
    varyOutStateAmbient(ss);
    varyOutStatePosition(ss);
    varyOutStateNormal(ss);
    varyOutStateTangent(ss);
    varyOutStateTexture(ss);
    varyOutStateColor(ss);
    varyOutStateEye(ss);
    //
    // output vertex position
    //
    outputStatePosition(ss);
}
