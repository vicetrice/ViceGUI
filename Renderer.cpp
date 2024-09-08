#include "Renderer.hpp"

namespace Vicetrice
{

	//---------------------------------------- PUBLIC

	Renderer::Renderer(unsigned int VBOcapacity, unsigned int EBOcapacity, const std::vector<VertexBufferElement>& layout, RendererType rendertype)
		:
		m_VAO(0),
		m_layout(layout),
		m_renderType(rendertype),
		m_VerticesCapacity(VBOcapacity),
		m_ElementsCapacity(EBOcapacity)
	{


		switch (m_renderType)
		{
		case RendererType::ATTS_IN_ONE_BUFFER:
		case RendererType::ATTS_IN_MIXED_BUFFERS:
			SetupSingleBuffer(VBOcapacity, EBOcapacity);
			break;
		case RendererType::ATTS_IN_SEPARATED_BUFFERS:
			SetupSeparatedBuffers(VBOcapacity, EBOcapacity);
			break;
		default:
			break;
		}
	}

	Renderer::~Renderer()
	{

		for (const auto& vboInfo : m_vertexBuff)
		{
			GLCall(glDeleteBuffers(1, &vboInfo.OpenglID));
		}


		for (const auto& eboInfo : m_indexBuff)
		{
			GLCall(glDeleteBuffers(1, &eboInfo.OpenglID));
		}


		GLCall(glDeleteVertexArrays(1, &m_VAO));

	}



	//---------------------------------------- MULTI BUFFER RENDERER


	void Renderer::DrawEBO(unsigned int EBOid, unsigned int count, const Shader& shader, unsigned int offset) const
	{
		shader.Bind();
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff[EBOid].OpenglID));
		GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)offset));
	}

	void Renderer::UpdateVBO(unsigned int attrib, const void* data, size_t size, unsigned int offset)
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuff[attrib].OpenglID));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
		m_vertexBuff[attrib].BuffSize = std::max(m_vertexBuff[attrib].BuffSize, static_cast<unsigned int>(offset + size));
	}

	void Renderer::UpdateEBO(unsigned int EBOindex, const void* data, size_t size, unsigned int offset)
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff[EBOindex].OpenglID));
		GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data));
		m_indexBuff[EBOindex].BuffSize = std::max(m_indexBuff[EBOindex].BuffSize, static_cast<unsigned int>(offset + size));
	}

	void Renderer::addEBO(unsigned int EBOcapacity)
	{
		GLCall(glBindVertexArray(m_VAO));
		unsigned int EBO;
		GLCall(glGenBuffers(1, &EBO));
		m_indexBuff.emplace_back(BufferInfo{ EBO, EBOcapacity * static_cast<unsigned int>(sizeof(unsigned int)), 0 });
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff.back().BuffCapacity, nullptr, GL_DYNAMIC_DRAW));
	}

	void Renderer::addAttrib(const VertexBufferElement& element)
	{
		GLCall(glBindVertexArray(m_VAO));


		unsigned int VBO;
		GLCall(glGenBuffers(1, &VBO));
		m_vertexBuff.emplace_back(BufferInfo{ VBO, m_VerticesCapacity * element.ByteSize,0 });

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, m_VerticesCapacity * element.ByteSize, nullptr, GL_DYNAMIC_DRAW));
		GLCall(glVertexAttribPointer(m_layout.size(), element.count, element.type, element.normalized, 0, (const void*)0));
		GLCall(glEnableVertexAttribArray(m_layout.size()));

		m_layout.emplace_back(element);
	}



	//---------------------------------------- SINGLE BUFFER RENDERER


	void Renderer::DrawVBO(unsigned int count, const Shader& shader, unsigned int offset) const
	{
		GLCall(glBindVertexArray(m_VAO));
		shader.Bind();

		GLCall(glDrawArrays(GL_POINTS, offset, count));
	}

	void Renderer::UpdateVBO(const void* data, size_t size, unsigned int offset)
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuff[0].OpenglID));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
		m_vertexBuff[0].BuffSize = std::max(m_vertexBuff[0].BuffSize, static_cast<unsigned int>(offset + size));

	}

	void Renderer::UpdateEBO(const void* data, size_t size, unsigned int offset)
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff[0].OpenglID));
		GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data));
		m_indexBuff[0].BuffSize = std::max(m_indexBuff[0].BuffSize, static_cast<unsigned int>(offset + size));
	}

	//---------------------------------------- PRIVATE

	void Renderer::SetupSeparatedBuffers(unsigned int VBOcapacity, unsigned int EBOcapacity)
	{

		m_vertexBuff.reserve(m_layout.size());
		m_indexBuff.reserve(1);

		GLCall(glGenVertexArrays(1, &m_VAO));
		GLCall(glBindVertexArray(m_VAO));

		for (size_t i = 0; i < m_layout.size(); i++)
		{
			unsigned int VBO;
			GLCall(glGenBuffers(1, &VBO));
			m_vertexBuff.emplace_back(BufferInfo{ VBO, VBOcapacity * m_layout[i].ByteSize,0 });

			GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
			GLCall(glBufferData(GL_ARRAY_BUFFER, m_vertexBuff[i].BuffCapacity, nullptr, GL_DYNAMIC_DRAW));
			GLCall(glVertexAttribPointer(i, m_layout[i].count, m_layout[i].type, m_layout[i].normalized, 0, (const void*)0));
			GLCall(glEnableVertexAttribArray(i));
		}

		unsigned int EBO;
		GLCall(glGenBuffers(1, &EBO));
		m_indexBuff.emplace_back(BufferInfo{ EBO, EBOcapacity * static_cast<unsigned int>(sizeof(unsigned int)) ,0 });  // Initial EBO with size 0

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff[0].BuffCapacity, nullptr, GL_DYNAMIC_DRAW));

	}

	void Renderer::SetupSingleBuffer(unsigned int VBOcapacity, unsigned int EBOcapacity)
	{

		m_vertexBuff.reserve(1);
		m_indexBuff.reserve(1);


		GLCall(glGenVertexArrays(1, &m_VAO));
		GLCall(glBindVertexArray(m_VAO));


		unsigned int VBO;
		GLCall(glGenBuffers(1, &VBO));


		unsigned int totalVBOCapacity = 0;
		unsigned int vertexStride = 0;


		for (size_t i = 0; i < m_layout.size(); i++) {
			vertexStride += m_layout[i].ByteSize;
		}

		totalVBOCapacity = VBOcapacity * vertexStride;


		m_vertexBuff.emplace_back(BufferInfo{ VBO, totalVBOCapacity, 0 });
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, totalVBOCapacity, nullptr, GL_DYNAMIC_DRAW));


		unsigned int offset = 0;
		for (size_t i = 0; i < m_layout.size(); i++)
		{
			const auto& attrib = m_layout[i];


			GLCall(glVertexAttribPointer(i, attrib.count, attrib.type, attrib.normalized, vertexStride, (const void*)offset));
			GLCall(glEnableVertexAttribArray(i));


			offset += attrib.ByteSize;
		}


		unsigned int EBO;
		GLCall(glGenBuffers(1, &EBO));
		m_indexBuff.emplace_back(BufferInfo{ EBO, EBOcapacity * static_cast<unsigned int>(sizeof(unsigned int)), 0 });

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuff[0].BuffCapacity, nullptr, GL_DYNAMIC_DRAW));

	}

}//namespace Vicetrice