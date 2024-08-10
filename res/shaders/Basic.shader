#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float index;

uniform mat4 u_MVP;
uniform vec4 u_VertexSpecific; 

void main()
{
    vec4 pos = position; 

    if (index == 0.0f)
    {
        pos.x += u_VertexSpecific.x;
        pos.y += u_VertexSpecific.y;
    }
    else if (index == 1.0f)
    {
        pos.x += u_VertexSpecific.z;
        pos.y += u_VertexSpecific.y;
    }
    else if (index == 2.0f)
    {
        pos.x += u_VertexSpecific.z;
        pos.y += u_VertexSpecific.w;
    }
    else if (index == 3.0f)
    {
        pos.x += u_VertexSpecific.x;
        pos.y += u_VertexSpecific.w;
    }

    gl_Position = u_MVP * pos; 
}


#shader fragment
#version 330 core
layout(location = 0) out vec4 color;


void main()
{
	color = vec4(0.8f,0.2f,0.9f,1.0f); 
}
