#include "stdafx.h"
#include "ImageLoader.h"
#ifndef STBI
#define STBI
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#endif

namespace Unbound
{
	namespace IO
	{
		ImageLoader* ImageLoader::instance = nullptr;

		ImageLoader* ImageLoader::get()
		{
			if (instance == nullptr)
			{
				instance = new ImageLoader();
			}

			return instance;
		}

		void ImageLoader::init()
		{
			instance = new ImageLoader();
		}

		Graphics::Image ImageLoader::loadFromFile(const std::string& path)
		{
			Graphics::Image toReturn;
			auto found = AssetManager::get()->imageIndex.find(static_cast<unsigned int>(std::hash<std::string>{} (path)));

			toReturn.setID(static_cast<uint32_t>(std::hash<std::string>{}(path)));
			toReturn.setData(nullptr);

			//Only load the file if the image path's hash has not yet been added
			if (found == AssetManager::get()->imageIndex.end())
			{
				int width, height, channels;
				unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
				uint32_t id = MemoryManager::get()->textureDataPool.addData(reinterpret_cast<char*>(data), width * height * 4);
				toReturn.setData(MemoryManager::get()->textureDataPool.get(id));
				toReturn.setSize(width, height);
			}

			Logger::log("Loaded image: " + path);

			return toReturn;
		}

		ImageLoader::ImageLoader() noexcept
		{
		}

		ImageLoader::ImageLoader(std::string path)
		{
		}

		ImageLoader::~ImageLoader()
		{
		}
	}
}