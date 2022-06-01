#ifndef JSE_RENDERABLE_H
#define JSE_RENDERABLE_H

namespace jse {

	enum class RenderableType
	{
		Mesh, Light
	};

	class Renderable
	{
	public:
		virtual ~Renderable() {}
		virtual RenderableType GetType() const = 0;
		virtual void Draw() = 0;
	};
}

#endif // !JSE_RENDERABLE_H
