#pragma once
namespace clc
{

	// Standard blend modes - See Graphics::SetBlendMode
	enum EBlendMode { BM_MULTIPLY, BM_ADDITIVE, BM_ALPHA, BM_REPLACE };


	enum ETexSampling {
		TS_COLOR,	// border color
		TS_REPEAT	// repeat
	};

	// Forward Declarations of Graphics related classes
	class Renderable;
	class Camera;

	// forward declare the graphics resources
	namespace GL {
		class Model;
		class Texture;
		class Shader;
		class ShaderProgram;
		class FrameBuffer;
	}
}