#pragma once

#include <GL/glew.h>

#include "Shader.hpp"
#include "Error.hpp"


#include <vector>
#include <string>

namespace Vicetrice
{
	struct VertexBufferElement
	{
		unsigned int	type;
		unsigned int	count;
		unsigned char	normalized;
		unsigned int	ByteSize;
	};

	struct BufferInfo
	{
		unsigned int OpenglID; // OpenGL ID for the buffer
		unsigned int BuffCapacity; // Capacity of the buffer (in bytes)
		unsigned int BuffSize; // Current size (number of bytes stored)
	};

	enum class RendererType
	{
		ATTS_IN_ONE_BUFFER,
		ATTS_IN_SEPARATED_BUFFERS,
		ATTS_IN_MIXED_BUFFERS
	};

	class Renderer
	{
	public:
		Renderer(unsigned int VBOcapacity, unsigned int EBOcapacity, const std::vector<VertexBufferElement>& layout, RendererType rendertype);
		~Renderer();


		//---------------------------------------- MULTI BUFFER RENDERER

		void DrawEBO(unsigned int EBOid, unsigned int count, const Shader& shader , unsigned int offset = 0) const;

		void UpdateVBO(unsigned int attrib, const void* data, size_t size, unsigned int offset = 0);

		void UpdateEBO(unsigned int EBOindex, const void* data, size_t size, unsigned int offset = 0);

		inline void UpdateVBOByteSize(unsigned int attrib, unsigned int NewSize)
		{
			m_vertexBuff[attrib].BuffSize = NewSize;
		}

		inline void UpdateEBOByteSize(unsigned int EBOindex, unsigned int NewSize)
		{
			m_indexBuff[EBOindex].BuffSize = NewSize;
		}


		void addEBO(unsigned int EBOcapacity);

		void addAttrib(const VertexBufferElement &element);


		inline unsigned int VBOByteSize(unsigned int attrib) const
		{
			return m_vertexBuff[attrib].BuffSize;
		}

		inline unsigned int EBOByteSize(unsigned int EBOindex) const
		{
			return m_indexBuff[EBOindex].BuffSize;
		}

		inline unsigned int VBOByteCapacity(unsigned int attrib) const
		{
			return m_vertexBuff[attrib].BuffCapacity;
		}

		inline unsigned int EBOByteCapacity(unsigned int EBOindex) const
		{
			return m_indexBuff[EBOindex].BuffCapacity;
		}

		//---------------------------------------- SINGLE BUFFER RENDERER

		void DrawVBO(unsigned int count, const Shader& shader, unsigned int offset = 0) const;

		void UpdateVBO(const void* data, size_t size, unsigned int offset = 0);

		void UpdateEBO(const void* data, size_t size, unsigned int offset = 0);

		inline void UpdateVBOByteSize(unsigned int NewSize)
		{
			m_vertexBuff[0].BuffSize = NewSize;
		}

		inline void UpdateEBOByteSize( unsigned int NewSize)
		{
			m_indexBuff[0].BuffSize = NewSize;
		}

		inline unsigned int VBOByteSize() const
		{
			return m_vertexBuff[0].BuffSize;
		}

		inline unsigned int EBOByteSize() const
		{
			return m_indexBuff[0].BuffSize;
		}

		inline unsigned int VBOByteCapacity() const
		{
			return m_vertexBuff[0].BuffCapacity;
		}

		inline unsigned int EBOByteCapacity() const
		{
			return m_indexBuff[0].BuffCapacity;
		}


	private:
		unsigned int m_VAO;
		std::vector<VertexBufferElement> m_layout;

		std::vector<BufferInfo> m_indexBuff; // Stores information about element buffer objects
		std::vector<BufferInfo> m_vertexBuff; // Stores information about vertex buffer objects

		RendererType m_renderType;
		unsigned int m_VerticesCapacity;
		unsigned int m_ElementsCapacity;

		void SetupSingleBuffer(unsigned int VBOcapacity, unsigned int EBOcapacity);
		void SetupSeparatedBuffers(unsigned int VBOcapacity, unsigned int EBOcapacity);
	};
}