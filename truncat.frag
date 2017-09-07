#version 150

in vec2 texCoord;

uniform sampler2D texUnit;

out vec4 out_Color;

void main(void)
{
    out_Color = max(texture(texUnit, texCoord) - 1.0 , 0.0);
}