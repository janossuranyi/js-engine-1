#ifndef JSE_VERTEX_ARRAY_H
#define JSE_VERTEX_ARRAY_H

#include <vector>
#include "graphics/GraphicsTypes.hpp"
#include "graphics/Buffer.hpp"

namespace jse {

	//---------------------------------------------
	class VertexArrayAttrib
	{
	public:
		VertexArrayAttrib(const VertexBufferElement aIndex, const VertexAttribType aType, const int aSize, const int aStride, const unsigned int aBasePointer, const BufferObject* aBuffer) : 
			type(aType),
			size(aSize),
			index(aIndex),
			stride(aStride),
			basePointer(aBasePointer),
			mBuffer(aBuffer)
		{}

		VertexArrayAttrib(const VertexArrayAttrib&);
		VertexArrayAttrib(VertexArrayAttrib&&) noexcept;
		~VertexArrayAttrib();
		VertexArrayAttrib& operator=(const VertexArrayAttrib& other);
		VertexArrayAttrib& operator=(VertexArrayAttrib&& other) noexcept;
		VertexAttribType type;
		int size;
		VertexBufferElement index;
		int stride;
		unsigned int basePointer;
		const BufferObject* mBuffer;
	private:
		void Copy(const VertexArrayAttrib& other);
	};

	typedef std::vector<VertexArrayAttrib> VertexArrayAttribList;


	class VertexArrayAttributes
	{
	public:
		VertexArrayAttributes() {}
		void AddVertexAttrib(const VertexArrayAttrib& aAttr);
		void AddVertexAttrib(const VertexBufferElement aIndex, const VertexAttribType aType, const int aSize, const int aStride, const unsigned int aBase, const BufferObject* aBuffer);
		const VertexArrayAttribList GetAttributes() const { return vertexAttributes; }
	private:
		VertexArrayAttribList vertexAttributes{};
		VertexElementFlag mVtxFlags{};
	};

	class VertexArray
	{
	public:
		VertexArray(const VertexArrayAttributes& a0, const BufferObject* aIndexBuffer) : mArrayAttributes(a0), mIndexBuffer(aIndexBuffer) {}
		virtual ~VertexArray() {}
		virtual void Compile() {}
		virtual void Bind() {}
	protected:
		const VertexArrayAttributes mArrayAttributes;
		const BufferObject* mIndexBuffer;
	};
}
#endif