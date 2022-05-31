#ifndef JSE_GRAPHICS_TYPES_H
#define JSE_GRAPHICS_TYPES_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "system/SystemTypes.hpp"

namespace jse {

	enum MaterialType
	{
		MaterialType_Diffuse,
		MaterialType_Specular,
		MaterialType_Emissive,
		MaterialType_ZPass,
		MaterialType_LastEnum
	};

	enum ColorMask
	{
		ColorMask_None = 0,
		ColorMask_Red = 1,
		ColorMask_Green = 2,
		ColorMask_Blue = 4,
		ColorMask_Alpha = 8,
		ColorMask_RGBA = 15
	};

	enum FrontFace
	{
		FrontFace_CW,
		FrontFace_CCW,
		FontFace_LastEnum
	};

	enum CullFace
	{
		CullFace_Front,
		CullFace_Back,
		CullFace_FrontAndBack,
		CullFace_LastEnum
	};

	enum PrimitiveType
	{
		PrimitiveType_Points,
		PrimitiveType_LineStrip,
		PrimitiveType_LineLoop,
		PrimitiveType_Lines,
		PrimitiveType_TriangleStrip,
		PrimitiveType_TriangleFan,
		PrimitiveType_Triangles,
		PrimitiveType_LastEnum
	};

	enum TextureUnit
	{
		TextureUnit_0,
		TextureUnit_1,
		TextureUnit_2,
		TextureUnit_3,
		TextureUnit_4,
		TextureUnit_5,
		TextureUnit_6,
		TextureUnit_7,
		TextureUnit_8,
		TextureUnit_9,
		TextureUnit_10,
		TextureUnit_11,
		TextureUnit_12,
		TextureUnit_13,
		TextureUnit_14,
		TextureUnit_15,
		TextureUnit_LastEnum
	};

	enum TextureType
	{
		TextureType_1D,
		TextureType_2D,
		TextureType_3D,
		TextureType_CubeMap,
		TextureType_LastEnum
	};

	enum TextureFilter
	{
		TextureFilter_Nearest,
		TextureFilter_Linear,
		TextureFilter_Nearest_Mipmap_Nearest,
		TextureFilter_Linear_Mipmap_Nearest,
		TextureFilter_Nearest_Mipmap_Linear,
		TextureFilter_Linear_Mipmap_Linear,
		TextureFilter_LastEnum
	};

	enum TextureWrapParam
	{
		TextureWrapParam_ClampToEdge,
		TextureWrapParam_ClampToBorder,
		TextureWrapParam_MirroredRepeat,
		TextureWrapParam_Repeat,
		TextureWrapParam_MirrorClampToEdge,
		TextureWrapParam_LastEnum
	};

	enum TextureParam
	{
		TextureParam_MinFilter,
		TextureParam_MagFilter,
		TextureParam_Wrap_S,
		TextureParam_Wrap_T,
		TextureParam_Wrap_R,
		TextureParam_LastEnum
	};

	enum VertexBufferElement
	{
		VertexBufferElement_Position,
		VertexBufferElement_Normal,
		VertexBufferElement_Tangent,
		VertexBufferElement_BiTangent,
		VertexBufferElement_Texture0,
		VertexBufferElement_Color0,
		VertexBufferElement_Color1,
		VertexBufferElement_Texture1,
		VertexBufferElement_Texture2,
		VertexBufferElement_Texture3,
		VertexBufferElement_Texture4,
		VertexBufferElement_User0,
		VertexBufferElement_User1,
		VertexBufferElement_User2,
		VertexBufferElement_User3,
		VertexBufferElement_LastEnum,
	};

	typedef Flag VertexElementFlag;

	#define VertexElementFlag_Position	(1U << 0)
	#define VertexElementFlag_Normal	(1U << 1)
	#define VertexElementFlag_Tangent	(1U << 2)
	#define VertexElementFlag_BiTangent	(1U << 3)
	#define VertexElementFlag_Texture0	(1U << 4)
	#define VertexElementFlag_Color0	(1U << 5)
	#define VertexElementFlag_Color1	(1U << 6)
	#define VertexElementFlag_Texture1	(1U << 7)
	#define VertexElementFlag_Texture2	(1U << 8)
	#define VertexElementFlag_Texture3	(1U << 7)
	#define VertexElementFlag_Texture4	(1U << 9)
	#define VertexElementFlag_User0		(1U << 10)
	#define VertexElementFlag_User1		(1U << 11)
	#define VertexElementFlag_User2		(1U << 12)
	#define VertexElementFlag_User3		(1U << 13)

	enum BufferAccesType
	{
		BufferAccess_Readonly,
		BufferAccess_WriteOnly,
		BufferAccess_ReadWrite,
		BufferAccess_LastEnum
	};

	enum GpuShaderStageType
	{
		ShaderStage_Vertex,
		ShaderStage_Fragment,
		ShaderStage_Geometry,
		ShaderStage_Compute,
		ShaderStage_Tesselation,
		ShaderStage_LastEnum
	};

	enum VertexAttribType
	{
		VtxAttribType_Float,
		VtxAttribType_UByte,
		VtxAttribType_UInt,
		VtxAttribType_LastEnum
	};

	const int VertexAttribSizes[] = {4, 1, 4, 0};
	const bool VertexAttribNormalize[] = { false, true , false, false };

    enum GpuProgramFormat {
        GpuProgramFormat_GLSL,
        GpuProgramFormat_LastEnum
    };

	enum PixelFormat
	{
		PixelFormat_Unknown,
		PixelFormat_Alpha,
		PixelFormat_Luminance,
		PixelFormat_LuminanceAlpha,
		PixelFormat_RGB,
		PixelFormat_RGBA,
		PixelFormat_BGR,
		PixelFormat_BGRA,
		PixelFormat_DXT1,
		PixelFormat_DXT2,
		PixelFormat_DXT3,
		PixelFormat_DXT4,
		PixelFormat_DXT5,
		PixelFormat_Depth16,
		PixelFormat_Depth24,
		PixelFormat_Depth32,
		PixelFormat_Alpha16,
		PixelFormat_Luminance16,
		PixelFormat_LuminanceAlpha16,
		PixelFormat_RGB16,
		PixelFormat_RGBA16,
		PixelFormat_Alpha32,
		PixelFormat_Luminance32,
		PixelFormat_LuminanceAlpha32,
		PixelFormat_RGB32,
		PixelFormat_RGBA32,
		PixelFormat_LastEnum,
	};

    enum GraphicsCaps {
        GraphicsCaps_MaxTextureImageUnits,
        GraphicsCaps_TextureCompression,
		GraphicCaps_ShaderModel_4,
		GraphicCaps_ShaderModel_5,
		GraphicCaps_AutoGenerateMipMaps,
		GraphicCaps_TextureCompression_DXTC,
		GraphicCaps_RenderToTexture,
		GraphicCaps_MaxAnisotropicFiltering,
		GraphicsCaps_LastEnum
    };

    typedef Flag ClearFBFlags;

	#define  ClearFBFlags_Color		(1U << 0)
	#define  ClearFBFlags_Depth		(1U << 1)
	#define  ClearFBFlags_Stencil	(1U << 2)
   
    enum DepthTestFunc {
		DepthTestFunc_Never,
		DepthTestFunc_Less,
		DepthTestFunc_LessOrEqual,
		DepthTestFunc_Greater,
		DepthTestFunc_GreaterOrEqual, 
		DepthTestFunc_Equal, 
		DepthTestFunc_NotEqual, 
		DepthTestFunc_Always, 
		DepthTestFunc_LastEnum
    };

	enum BlendFunc
	{
		BlendFunc_Zero,
		BlendFunc_One,
		BlendFunc_SrcColor,
		BlendFunc_OneMinusSrcColor,
		BlendFunc_DestColor,
		BlendFunc_OneMinusDestColor,
		BlendFunc_SrcAlpha,
		BlendFunc_OneMinusSrcAlpha,
		BlendFunc_DestAlpha,
		BlendFunc_OneMinusDestAlpha,
		BlendFunc_SrcAlphaSaturate,
		BlendFunc_LastEnum
	};

	enum BufferTarget
	{
		BufferTarget_Vertex,
		BufferTarget_Index,
		BufferTarget_Uniform,
		BufferTarget_Query,
		BufferTarget_LastEnum
	};

	enum BufferUsage
	{
		BufferUsage_StaticDraw,
		BufferUsage_StaticRead,
		BufferUsage_StaticCopy,
		BufferUsage_StreamDraw,
		BufferUsage_StreamRead,
		BufferUsage_StreamCopy,
		BufferUsage_DynaDraw,
		BufferUsage_DynaRead,
		BufferUsage_DynaCopy,
		BufferUsage_LastEnum
	};
    //--------------------------------------------------

    typedef glm::ivec2 Vector2l;
	typedef glm::ivec3 Vector3l;
	typedef glm::vec2 Vector2f;
	typedef glm::vec2 vec2;
	typedef glm::vec3 Vector3f;
	typedef glm::vec3 vec3;
	typedef glm::vec3 Color3;
	typedef glm::vec4 Color;
	typedef glm::vec4 Vector4f;
	typedef glm::vec4 vec4;
	typedef glm::mat4 Matrix;
	typedef glm::mat4 mat4;
	typedef glm::mat3 Matrix3x3;
	typedef glm::mat3 mat3;
	typedef glm::quat Quaternion;
	typedef glm::quat Quat;

	struct Color32
	{
		union {
			struct {
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			};
			unsigned char v[4];
		};
	};

	enum AnimationTrackType
	{
		AnimationTrackType_Position,
		AnimationTrackType_Rotation,
		AnimationTrackType_LastEnum
	};

	struct Keyframe
	{
		union {
			Vector3f v;
			Quat q;
			float value[4];
		};
		float time;
	};
}

#endif