#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 m_color;
layout(location = 4) in vec2 shifting;
layout(location = 3) in int texID; 
layout(location = 2) in vec2 texCoords; 


uniform mat4 u_MP;

flat out int TexID; 
out vec4 OutColor;
out vec2 FragTexCoords; // Coordenadas de textura para el fragmento


void main()
{
    gl_Position = u_MP * (vec4(position + shifting, 0.0f, 1.0f));
    OutColor = m_color; 
    TexID = texID; 
    FragTexCoords = texCoords;
}



#shader fragment
#version 330 core
layout(location = 0) out vec4 color;


in vec4 OutColor;
flat in int TexID;
in vec2 FragTexCoords; 

uniform sampler2DArray u_Textures; 
uniform vec4 u_WinLimit;


void main()
{
    float xAccept = step(u_WinLimit.y, gl_FragCoord.x) * step(gl_FragCoord.x, u_WinLimit.x);
    float yAccept = step(u_WinLimit.w, gl_FragCoord.y) * step(gl_FragCoord.y, u_WinLimit.z);
    float accept = xAccept * yAccept;

    float validTex = step(0.0, float(TexID));

    float alphaMask = texture(u_Textures, vec3(FragTexCoords, float(TexID))).r * validTex + (1.0 - validTex);

    color = vec4(OutColor.rgb, OutColor.a * alphaMask * accept);
}

