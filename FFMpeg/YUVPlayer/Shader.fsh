#version 330 core
in vec2 TexCoord;//输入分量

uniform sampler2D SamplerY;
uniform sampler2D SamplerU;
uniform sampler2D SamplerV;

out vec4 color;//输出分量

//着色器的开头总是要声明版本，接着是输入和输出变量、uniform和main函数

void main(void)
{
    vec3 yuv;
    vec3 rgb;
    
    yuv.x = texture(SamplerY, TexCoord).r;
    yuv.y = texture(SamplerU, TexCoord).r - 0.5;
    yuv.z = texture(SamplerV, TexCoord).r - 0.5;
    
    rgb = mat3( 1.0      ,1.0       ,      1.0,
                0      ,-0.39465,2.03211,
                1.13983,-0.58060,      0) * yuv;
    
    color = vec4(rgb,0);
}
