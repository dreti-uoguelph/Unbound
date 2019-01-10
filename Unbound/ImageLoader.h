#pragma once
#include"stdafx.h"
#include"Image.h"
#include"MemoryPool.h"
#include"MemoryManager.h"
#include"Logger.h"
#include"AssetManager.h"
#include<functional>

namespace Unbound
{
	namespace IO
	{
		/*This class is responsible for image loading and will work on both raw and compressed images
		The type of image is deduced from the extension and default settings are automatically applied*/
		class ImageLoader
		{
		public:
			/*Loads an image from a file located at path. Will automatically deduce info from image extension*/
			static Graphics::Image loadFromFile(const std::string& path);
			/*Get the current imageloader instance*/
			static ImageLoader* get();
			/*Initialize the image loader*/
			void init();

			ImageLoader() noexcept;
			ImageLoader(std::string path);
			~ImageLoader();
		private:
			static ImageLoader* instance;
		};
	}
}