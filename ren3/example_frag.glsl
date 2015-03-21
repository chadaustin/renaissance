// --------------------
//       defines
// --------------------
 
#ifdef GL_ES
precision highp float;
#endif
 
#define DIRECT_LIGHT_COUNT 3
#define POINT_LIGHT_COUNT 0
#define SPOT_LIGHT_COUNT 0
 
// --------------------
//       uniforms
// --------------------
 
uniform vec3 uDirectLightDirections[DIRECT_LIGHT_COUNT]; // x, y, z
uniform vec4 uDirectLightColors[DIRECT_LIGHT_COUNT];     // r, g, b, 1/(r*r)--(w component not used)
 
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
    vec3 diffuse;
    vec3 fresnel;
    vec3 specular;
    vec4 albedo;
    vec4 final;
};
// TODO: @@@ until custom modules are revised
#define FragmentState ShaderState
 
// --------------------
//   color functions
// --------------------
 
vec3 clampColor(vec3 c) {
    // This transform desaturates color ahead of clamping
    // which can help greatly to prevent hue changes as
    // color brightness in one or more components exceeds
    // the displayable range.
    float L = dot(vec3(0.3333), c);
    float r = max(c.r, max( c.g, c.b));
    float t = clamp((r - 1.0), 0.0, 1.0);
    c = mix(c, vec3(L), t);
    return clamp(c, 0.0, 1.0);
}
vec3 linearTosRGB(vec3 c) { //polynomial approximation of c^(1/2.2)
    vec3 onec = vec3(1.0) - c;
    vec3 c2 = onec * onec;
    vec3 c16 = c2 * c2; c16 *= c16; c16 *= c16;
    return
        0.326999 * (vec3(1.0) - c16) +
        0.249006 * (vec3(1.0) - c2) +
        0.423995 * c;
}
vec3 sRGBToLinear(vec3 c) { //polynomial approximation of c^2.2
    vec3 c2 = c * c;
    vec3 c3 = c2 * c;
    return (0.7532 * c2) + (0.2468 * c3);
}
 
// --------------------
//   light functions
// --------------------
 
float diffuseRatio(vec3 N, vec3 L) {
    float incidence = dot(N, L);
    return clamp(incidence, 0.0, 1.0);
}
void directLight(inout ShaderState ss) {
    for (int i = 0; i < DIRECT_LIGHT_COUNT; i++) {
        ss.diffuse += diffuseRatio(ss.normal, uDirectLightDirections[i].rgb) * uDirectLightColors[i].rgb;
    }
}
 
// --------------------
//   state functions
// --------------------
 
void initStateDiffuse(inout ShaderState ss) {
    ss.diffuse = vec3(0.0);
}
void varyInStateTexture(inout ShaderState ss) {
    ss.texCoord = fTexCoord;
}
void varyInStateAmbient(inout ShaderState ss) {
    ss.ambient = fAmbient;
}
void varyInStateColor(inout ShaderState ss) {
    ss.color = fColor;
    ss.albedo = ss.color;
}
void varyInStateEye(inout ShaderState ss) {
    ss.eye = fEye;
}
void varyInStateEyeNormalize(inout ShaderState ss) {
    ss.eye = normalize(fEye);
}
void varyInStatePosition(inout ShaderState ss) {
    ss.worldPosition = fWorldPos;
}
void varyInStateNormal(inout ShaderState ss) {
    ss.normal = fNormal;
}
void varyInStateNormalNormalize(inout ShaderState ss) {
    ss.normal = normalize(fNormal);
}
void varyInStateTangent(inout ShaderState ss) {
    ss.tangent = fTangent;
}
void varyInStateTangentNormalize(inout ShaderState ss) {
    ss.tangent = vec4(normalize(fTangent.xyz), fTangent.w);
}
void initStateFinal(inout ShaderState ss) {
    ss.final = vec4(vec3(0.0), 1.0);
}
void outputStateFinal(inout ShaderState ss) {
    gl_FragColor = ss.final;
}
 
// --------------------
//       modules
// --------------------
 
void LightingDiffuse(inout ShaderState ss) {
    ss.final.rgb = linearTosRGB(ss.albedo.rgb * clampColor(ss.ambient + ss.diffuse));
    ss.final.a = ss.albedo.a;
}
uniform vec3 uGameObjectFadeParam;
void GameObjectFadeByDiscarding(inout FragmentState s) {
    if (uGameObjectFadeParam[0] == 0.) {
        return; 
    }
    if (uGameObjectFadeParam[0] < 0.) {
        discard; 
    }
    float coorModX = mod(gl_FragCoord.x, uGameObjectFadeParam[1]);
    float coorModY = mod(gl_FragCoord.y, uGameObjectFadeParam[1]);
    bool discardX = (coorModX < uGameObjectFadeParam[2]);
    bool discardY = (coorModY < uGameObjectFadeParam[2]);
    if ((uGameObjectFadeParam[0] < 1.5) && discardX && discardY) {
        discard;
    } else if ((uGameObjectFadeParam[0] > 1.5) && (discardX || discardY)) {
        discard;
    }
}
uniform sampler2D tDiffuseMap0;
 
void AlbedoMap(inout ShaderState ss) {
    // @@@ for debugging texture coordinates: X is green, Y is alpha
    //vec4 texColor = vec4(0.0, ss.texCoord.x, 0.0, ss.texCoord.y);
    vec4 texColor = texture2D(tDiffuseMap0, ss.texCoord);
 
    // @@@ for debugging to unmultiply the alpha from the rgb
    //if (texColor.a > 0.0) {
    //    texColor.rgb *= 1.0 / texColor.a;
    //}
 
    ss.albedo.rgb = sRGBToLinear(ss.albedo.rgb * texColor.rgb);
    ss.albedo.a = texColor.a;
}
uniform float uRimFactor;
uniform float uRimReachStart;
uniform float uRimReachEnd;
uniform vec4 uRimColor;
 
void Rim(inout ShaderState s) {
    float dot = dot(s.eye, s.normal);
    float rimValue = 1.0 - dot;
    vec3 tmpRimColor = mix(s.final.rgb, uRimColor.rgb, uRimFactor);
    float rimReach = 1.0 - smoothstep(uRimReachStart, uRimReachEnd, dot);
    s.final.rgb = mix(s.final.rgb, tmpRimColor.rgb, rimReach * s.final.a);
}
uniform vec4 uFogColor;
 
varying float fFogIntensity;
 
void LinearFogFinal(inout ShaderState s) {
    s.final.rgb = mix(s.final.rgb, uFogColor.rgb * s.final.a, fFogIntensity);
}
 
// ====================
//       M A I N
// ====================
 
void main() {
    //
    // state initialization
    //
    ShaderState ss;
    varyInStateTexture(ss);
    varyInStateAmbient(ss);
    varyInStateColor(ss);
    varyInStateEyeNormalize(ss);
    varyInStatePosition(ss);
    varyInStateNormalNormalize(ss);
    varyInStateTangentNormalize(ss);
    initStateDiffuse(ss);
    initStateFinal(ss);
    //
    // call texturing modules
    //
    AlbedoMap(ss);
    //
    // call light functions
    //
    directLight(ss);
    //
    // call lighting modules
    //
    LightingDiffuse(ss);
    //
    // call post-lightings modules
    //
    GameObjectFadeByDiscarding(ss);
    //
    // call post-texturing modules
    //
    Rim(ss);
    LinearFogFinal(ss);
    //
    // output fragment color
    //
    outputStateFinal(ss);
}
