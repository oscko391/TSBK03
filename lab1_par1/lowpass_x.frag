#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;

out vec4 out_Color;

void main(void)
{
    int kernelSize = 4;
    
    vec2 offSet = 1.0 / textureSize(texUnit,0);
    
    vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
    
    result += 1.0 * texture(texUnit, outTexCoord - vec2(offSet.x, 0));
    result += 2.0 * texture(texUnit, outTexCoord);
    result += 1.0 * texture(texUnit, outTexCoord + vec2(offSet.x, 0));
    
    out_Color = result / kernelSize;
}