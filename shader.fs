#version 450 core
out vec4 FragColor;

in mat4 trans;
in vec3 ourPos;

uniform sampler3D volume;
uniform vec3 cameraPos;
vec3 lightPos = vec3(4.0, 4.0, 0.0);
vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);
vec4 diffuse = vec4(0.8, 0.8, 0.8, 1.0);

int num_samples = 50;
vec4 color_func[4] = {
    vec4(0.0f, 0.0f, 0.0f, -3024.0f),
    vec4(0.62f, 0.36f, 0.18f, -800.0f),
    vec4(0.88f, 0.60f, 0.29f, 0.0f),
    vec4(1.0f, 1.0f, 1.0f, 3071.0f)
};
vec2 opacity_func[4] = {
    vec2(0.0f, -3024.0f),
    vec2(0.0f, -800.0f),
    vec2(0.4f, 300.0f),
    vec2(0.8f, 3071.0f)
};
// vec4 color_func[4] = {
//     vec4(0.0f, 0.0f, 0.0f, 0.0f),
//     vec4(0.62f, 0.36f, 0.18f, 300.0f),
//     vec4(0.88f, 0.60f, 0.29f, 1000.0f),
//     vec4(1.0f, 1.0f, 1.0f, 3071.0f)
// };
// vec2 opacity_func[4] = {
//     vec2(0.0f, 0.0f),
//     vec2(0.6f, 300.0f),
//     vec2(0.8f, 1000.0f),
//     vec2(1.0f, 3071.0f)
// };


bool inVolume(vec3 pos)
{
    return pos.x >= 0.0 && pos.x <= 1.0 && pos.y >= 0.0 && pos.y <= 1.0 && pos.z >= 0.0 && pos.z <= 1.0;
}

vec4 transferFunction(float value)
{
    vec4 result = vec4(0.0f);
    for(int i = 0; i < 3; i++)
    {
        if(value >= color_func[i].w && value < color_func[i+1].w)
        {
            float t = (value - color_func[i].w) / (color_func[i+1].w - color_func[i].w);
            result.xyz = mix(color_func[i].xyz, color_func[i+1].xyz, t);
            
        }
        if(value >= opacity_func[i].y && value < opacity_func[i+1].y)
        {
            float t = (value - opacity_func[i].y) / (opacity_func[i+1].y - opacity_func[i].y);
            result.w = mix(opacity_func[i].x, opacity_func[i+1].x, t);
        }
    }

    return result;
}

vec4 raycasting()
{
    vec4 color = vec4(0.0);
    vec3 currentPos = (trans * vec4(ourPos, 1.0)).xyz;
    vec3 rayDir = normalize(currentPos - cameraPos);
    mat4 inv_trans = inverse(trans);
    float step = 1.0 / num_samples;

    for(int i = 0; i < num_samples; i++)
    {
        currentPos = currentPos + rayDir * step;
        vec3 inver_Pos = (inv_trans * vec4(currentPos, 1.0)).xyz;
        
        vec3 texCoord = inver_Pos + 0.5;

        if(!inVolume(texCoord))
            break;

        float voxel = texture(volume, texCoord).r;

        vec4 c = transferFunction(voxel);


        color.xyz = color.xyz + c.xyz * c.w *(1.0 - color.w);
        color.w = color.w + c.w * (1.0 - color.w);            
        
        if(color.w > 0.99)
            break;
    }
    return color;
}

// vec4 debug()
// {
//     vec4 color = vec4(0.0);
//     vec3 currentPos = (trans * vec4(ourPos, 1.0)).xyz;
//     vec3 rayDir = normalize(currentPos - cameraPos);
//     mat4 inv_trans = inverse(trans);
//     float step = 1.0 / num_samples;

//     vec4 maxval = vec4(0.0);
//     int i;
//     for(i = 0; i < num_samples; i++)
//     {
//         currentPos = currentPos + rayDir * step;
//         vec3 inver_Pos = (inv_trans * vec4(currentPos, 1.0)).xyz;
        
//         vec3 texCoord = inver_Pos + 0.5;

//         if(!inVolume(texCoord))
//             break;

//         float voxel = texture(volume, texCoord).r;

//         vec4 c = transferFunction(voxel);
//         // vec4 amb = ambient * c;
//         // vec3 lightDir = normalize(lightPos - currentPos);
//         // vec4 diff = diffuse * c * max(dot(lightDir, normalize(rayDir)), 0.0);
//         // c = amb + diff;

//         color.rgb = color.rgb + c.rgb * c.a * (1.0 - color.a);
//         color.rgb = color.rgb + c.a * (1.0 - color.a);            
        
        
//         if(color.a > 0.99)
//             break;
//     }
//     return vec4(color.rgb / (i + 1.0), color.a);
// }
void main()
{
    FragColor = raycasting();
    //FragColor = debug();
}