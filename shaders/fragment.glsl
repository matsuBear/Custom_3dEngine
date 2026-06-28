#version 450 core

const int maxLightCount=16;
vec3 skyC=vec3(38, 210, 222)/255*0.3;
vec3 groundC=vec3(179, 62, 78)/255*0.3;

in vec4 vPos;
in vec4 vNormal;
uniform vec4 lightPos[maxLightCount];
uniform vec4 lightColorL[maxLightCount];
uniform int lightCount;
uniform vec4 meshColor;
uniform vec4 camPos;
uniform float ambientStrength;
uniform float shininess;
out vec4 shadedColor;

vec4 light;
vec4 lightColor;
vec3 accFrag=vec3(0.0);

void main(){
    vec3 cutNormal = normalize(vNormal.xyz);

    if (!gl_FrontFacing) {
        cutNormal = -cutNormal;
    }

    for(int i=0;i<lightCount;i++){
        light=lightPos[i];
        lightColor=lightColorL[i];
        float distance = length(light.xyz - vPos.xyz);
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * (distance * distance));
        vec3 lightDir  = normalize(light.xyz - vPos.xyz);
        vec3 viewDir   = normalize(camPos.xyz - vPos.xyz);
        vec3 diffuse = (max(dot(cutNormal, lightDir), 0.0) * lightColor.xyz*attenuation)*meshColor.xyz;
        vec3 reflecDir = reflect(-lightDir, cutNormal);
        vec3 specular  = pow(max(dot(reflecDir, viewDir), 0.0), shininess) * lightColor.xyz*attenuation;
        vec3 finalRGB = diffuse + specular;
        accFrag += finalRGB;
    }

    float mixFact=-0.5*cutNormal.y+0.5;
    accFrag+=ambientStrength*mix(skyC, groundC, mixFact);
    accFrag=(accFrag*(2.51*accFrag+0.03))/(accFrag*(2.43*accFrag+0.59)+0.14);
    shadedColor=vec4(accFrag, 1.0f);
    
}
