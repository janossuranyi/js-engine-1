#include "graphics/VertexArray.hpp"

namespace jse {


	void VertexArrayAttrib::Copy(const VertexArrayAttrib& other)
	{
		type = other.type;
		size = other.size;
		index = other.index;
		stride = other.stride;
		basePointer = other.basePointer;
		mBuffer = other.mBuffer;
	}

	VertexArrayAttrib::~VertexArrayAttrib()
	{
	}

	VertexArrayAttrib::VertexArrayAttrib(const VertexArrayAttrib& other)
	{
		Copy(other);
	}

	VertexArrayAttrib::VertexArrayAttrib(VertexArrayAttrib&& other) noexcept
	{
		Copy(other);
	}

	void VertexArrayAttributes::AddVertexAttrib(const VertexArrayAttrib& aAttr)
	{
		if ((1U << aAttr.index) & mVtxFlags) return;

		mVtxFlags |= (1U << aAttr.index);

		//VertexArrayAttrib newAttr = aAttr;

		vertexAttributes.push_back(aAttr);

	}

	void VertexArrayAttributes::AddVertexAttrib(const VertexBufferElement aIndex, const VertexAttribType aType, const int aSize, const int aStride, const unsigned int aBase, const BufferObject* aBuffer)
	{
		const VertexArrayAttrib attr(aIndex, aType, aSize, aStride, aBase, aBuffer);
		AddVertexAttrib(attr);
	}

	VertexArrayAttrib& VertexArrayAttrib::operator=(const VertexArrayAttrib& other)
	{
		Copy(other);

		return *this;
	}

	VertexArrayAttrib& VertexArrayAttrib::operator=(VertexArrayAttrib&& other) noexcept
	{
		Copy(other);

		return *this;
	}
}