#pragma once
#include"stdafx.h"
#include"ImageLoader.h"

namespace Unbound
{
	namespace Graphics
	{
		class Material
		{
		public:
			Image albedo;
			Image detail;
			Image normal;
			Image roughness;
			Image metalness;
		private:
		};

		class TerrainMaterial
		{
		public:
			Image albedo;
			Image normal;
			Image roughness;
		private:
		};
	}
}