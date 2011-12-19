#include <ren/ren.h>
using namespace ren;

attribute vec4  vBoneWeights;
attribute vec4  vBoneIndices;

int main() {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    array<mat4x3, 60> bones;

    attribute<vec4> position;
    uniform<int> maxInfluenceCount;
    attribute<ivec4> influenceIndices;
    attribute<vec4> influenceWeights;

    vec4 P = position 
    
    ren::VertexShader2 vs;
    vs.output.position = projectionMatrix * viewMatrix * modelMatrix * P;

    printf("%s\n", vs.generateGLSL().c_str());
}

#if 0
void	boneTransform( inout vec4 position, inout vec3 normal ) {
    //bones are passed as 3x4 matrices, row-major, to save constant registers
    ivec4 i = ivec4( 3.0*vBoneIndices );
    vec4 row0 = uBones[ i.x ];
    vec4 row1 = uBones[ i.x+1 ];
    vec4 row2 = uBones[ i.x+2 ];
    
    if( vBoneWeights.y > 0.0 ) {
        row0 *= vBoneWeights.x;
        row1 *= vBoneWeights.x;
        row2 *= vBoneWeights.x;
    
        row0 += uBones[ i.y ] * vBoneWeights.y +
                uBones[ i.z ] * vBoneWeights.z +
                uBones[ i.w ] * vBoneWeights.w;
                
        row1 += uBones[ i.y+1 ] * vBoneWeights.y +
                uBones[ i.z+1 ] * vBoneWeights.z +
                uBones[ i.w+1 ] * vBoneWeights.w;
                
        row2 += uBones[ i.y+2 ] * vBoneWeights.y +
                uBones[ i.z+2 ] * vBoneWeights.z +
                uBones[ i.w+2 ] * vBoneWeights.w;
    }
    
    vec3 n;
    n.x = dot( row0.xyz, normal );
    n.y = dot( row1.xyz, normal );
    n.z = dot( row2.xyz, normal );
    normal = n;
    
    vec3 p;
    p.x = dot( row0, position );
    p.y = dot( row1, position );
    p.z = dot( row2, position );
    position.xyz = p;
}

void main() {
    vec4 P = vPosition;
    
    boneTransform( P, N );

    //model transform
    P = uModelMatrix * P;
    
    //view and projection transform
    gl_Position = uViewProjMatrix * P;
}
#endif
