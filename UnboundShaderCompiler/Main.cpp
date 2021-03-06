#include "stdafx.h"

int main(int argc, char** argv)
{
	for (int i = 0; i < 1; i++)
	{
#ifdef _WIN32
		system("glslangValidator.exe -V -o ./Assets/Shaders/MainVertexShader.spv ./Assets/Shaders/MainVertexShader.vert");
		system("spirv-opt.exe --strip-debug --freeze-spec-const --eliminate-dead-const --unify-const ./Assets/Shaders/MainVertexShader.spv -o ./Assets/Shaders//MainVertexShader.spv");
		system("glslangValidator.exe -V -o ./Assets/Shaders/MainFragmentShader.spv ./Assets/Shaders/MainFragmentShader.frag");
		system("spirv-opt.exe --strip-debug --freeze-spec-const --eliminate-dead-const --unify-const ./Assets/Shaders/MainFragmentShader.spv -o ./Assets/Shaders//MainFragmentShader.spv");
#endif
	}

	return 0;
}