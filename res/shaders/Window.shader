#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 m_color;
layout(location = 4) in vec2 shifting;

uniform mat4 u_M;


out vec4 OutColor;

void main()
{
	gl_Position = u_M * (vec4(position + shifting,0.0f,1.0f));
	OutColor = m_color; 
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
in vec4 OutColor; 

uniform vec4 u_WinLimit; 

void main()
{
  
    float xAccept = step(u_WinLimit.y, gl_FragCoord.x) * step(gl_FragCoord.x, u_WinLimit.x);
    float yAccept = step(u_WinLimit.w, gl_FragCoord.y) * step(gl_FragCoord.y, u_WinLimit.z);
    
    float accept = xAccept * yAccept;

    
    color = vec4(OutColor.rgb, OutColor.a * accept);
}
