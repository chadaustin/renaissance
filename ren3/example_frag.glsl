const int DIRECT_LIGHT_COUNT = 3
const int POINT_LIGHT_COUNT = 0
const int SPOT_LIGHT_COUNT = 0
 
uniform vec3 uDirectLightDirections[DIRECT_LIGHT_COUNT]; // x, y, z
uniform vec4 uDirectLightColors[DIRECT_LIGHT_COUNT];     // r, g, b, 1/(r*r)--(w component not used)
 
varying vec2 fTexCoord;
varying vec3 fAmbient;
varying vec3 fEye;
varying vec3 fWorldPos;
varying vec3 fNormal;
varying vec4 fColor;
 
struct ShaderState {
    vec3 ambient;
    vec3 eye;
    vec2 texCoord;
    vec3 worldPosition;
    vec3 normal;
    vec4 color;
    vec3 diffuse;
    vec3 fresnel;
    vec3 specular;
    vec4 albedo;
    vec4 final;
};
#define FragmentState ShaderState
 
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
 
// --------------------
//   state functions
// --------------------
 
uniform vec3 uGameObjectFadeParam;

uniform sampler2D tDiffuseMap0;
 
uniform float uRimFactor;
uniform float uRimReachStart;
uniform float uRimReachEnd;
uniform vec4 uRimColor;
 
uniform vec4 uFogColor;

varying float fFogIntensity;
 
void main() {
    ShaderState ss;
    ss.texCoord = fTexCoord;
    ss.ambient = fAmbient;
    ss.color = fColor;
    ss.albedo = ss.color;
    ss.eye = normalize(fEye);
    ss.worldPosition = fWorldPos;
    ss.normal = normalize(fNormal);
    ss.diffuse = vec3(0.0);
    ss.final = vec4(vec3(0.0), 1.0);

    // call texturing modules
    vec4 texColor = texture2D(tDiffuseMap0, ss.texCoord);
    ss.albedo.rgb = sRGBToLinear(ss.albedo.rgb * texColor.rgb);
    ss.albedo.a = texColor.a;

    // call light functions
    for (int i = 0; i < DIRECT_LIGHT_COUNT; i++) {
        ss.diffuse += diffuseRatio(ss.normal, uDirectLightDirections[i].rgb) * uDirectLightColors[i].rgb;
    }

    // call lighting modules
    ss.final.rgb = linearTosRGB(ss.albedo.rgb * clampColor(ss.ambient + ss.diffuse));
    ss.final.a = ss.albedo.a;

    // call post-lightings modules

    if (uGameObjectFadeParam[0] != 0.) {
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

    float dot = dot(ss.eye, ss.normal);
    float rimValue = 1.0 - dot;
    vec3 tmpRimColor = mix(ss.final.rgb, uRimColor.rgb, uRimFactor);
    float rimReach = 1.0 - smoothstep(uRimReachStart, uRimReachEnd, dot);
    ss.final.rgb = mix(ss.final.rgb, tmpRimColor.rgb, rimReach * ss.final.a);

    ss.final.rgb = mix(ss.final.rgb, uFogColor.rgb * ss.final.a, fFogIntensity);
    gl_FragColor = ss.final;
}
