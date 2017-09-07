#version 150

in vec2 texCoord;
uniform sampler2D texUnit;


float filterKernel[25] = float[](1 ,4 ,6 ,4 ,1,
				 4, 16, 24, 16, 4,
				 6, 24, 36, 24, 6,
				 4, 16, 24, 16, 4,
				 1, 4, 6, 4, 1);
			

out vec4 out_Color;
			
void main(void)
{
    int divider = 256;
    vec2 texOffSet = 1.0 / textureSize(texUnit, 0);
    
    vec4 result = vec3(0.0, 0.0, 0.0, 0.0);
    
    for( int row = -2; row < 3; row++)
    {
      for ( int col = -2; col < 3; col++)
      {
	result += texture(texUnit, texCoord + vec2(texOffSet.x*row, texOffSet.y * y*col)) * filterKernel[(row+2) + 5*(col+2)];
      }
     }
	
}