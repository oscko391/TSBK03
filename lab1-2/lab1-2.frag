#version 150
// bump mapping should be calculated
// 1) in view coordinates
// 2) in texture coordinates

in vec2 outTexCoord;
in vec3 out_Normal;
in vec3 Ps;
in vec3 Pt;
in vec3 pixPos;  // Needed for specular reflections
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
    vec3 light = vec3(0.0, 0.7, 0.7); // Light source in view coordinates
	
    // Calculate gradients here
    float offset = 1.0 / 256.0; // texture size, same in both directions
    
    /* PART 1 */
    vec3 bs = texture(texUnit, outTexCoord + vec2(offset, 0.0)).xyz - texture(texUnit, outTexCoord).xyz;
    vec3 bt = texture(texUnit, outTexCoord + vec2(0.0, offset)).xyz - texture(texUnit, outTexCoord).xyz;
    
    vec3 normal = normalize(out_Normal);
    vec3 normalPrim = normalize(normal + bs * cross(Pt,normal) + bt * cross(Ps, normal));
    
    /* PART 2*/
    mat3 mvt = mat3(Ps, Pt, normal);
    
    //light = light * mvt;
    
    normal = normalize(vec3(-bs.x, -bt.x, 1.0));
    
    // Simplified lighting calculation.
    // A full solution would include material, ambient, specular, light sources, multiply by texture.
    out_Color = vec4( dot(normalPrim, light)) ;
}
