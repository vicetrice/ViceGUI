#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 m_color;

uniform mat4 u_M;

out vec4 OutColor;

void main()
{
    gl_Position = u_M * position;
    OutColor = m_color; 
}


#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
in vec4 OutColor; 

void main()
{
    color = OutColor; 
}
