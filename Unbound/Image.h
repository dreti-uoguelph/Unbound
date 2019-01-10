#pragma once
#include"stdafx.h"

namespace Unbound
{
	namespace Graphics
	{
		/*Images can have different types, which affect how they are loaded into memory and the quality at which they can be compressed*/
		enum ImageType { IMAGE_TYPE_TEXTURE_GENERAL, IMAGE_TYPE_TEXTURE_HQ, IMAGE_TYPE_TEXTURE_LQ, IMAGE_TYPE_NORMAL_MAP, IMAGE_TYPE_GUI, IMAGE_TYPE_PROCEDURAL_TEXTURE };
		/*Memory properties affect how textures are kept in memory an whether they can be streamed in and out*/
		enum ImageMemoryProperty { IMAGE_MEMORY_PROPERTY_GENERAL, IMAGE_MEMORY_PROPERTY_PERSISTENT, IMAGE_MEMORY_PROPERTY_STREAMING_LOW_PRIORITY, IMAGE_MEMORY_PROPRTY_STREAMING_HIGH_PRIORITY };

		/*This class stores information about an image. The actual data can be stored in a variety of places, such as
		A memory pool, on the GPU or in a staging buffer. In case the data needs to be manipulated manually, it is loaded automatically*/
		class Image
		{
		public:

			/*Get the width of the image, in pixels*/
			const uint32_t getWidth() const;
			/*Get the height of the image, in pixels*/
			const uint32_t getHeight() const;
			/*Get the format of the image*/
			const ImageType getFormat() const;
			/*Get the size of the image data, in bytes*/
			const uint32_t getSizeInBytes() const;
			/*Get the ID of the image in the renderer*/
			const uint32_t getID() const;
			/*Get the bound ID of the image within the renderer*/
			const uint32_t getBoundID() const;

			/*Get a pointer to the data in the image*/
			const char* getData() const;

			/*Set the size of the image, in pixels*/
			void setSize(const uint32_t widthToSet, const uint32_t heightToSet);
			/*Set the internal data pointer of the image*/
			void setData(char* dataToSet);
			/*Set the ID of the image, in the renderer*/
			void setID(const uint32_t toSet);
			/*Set the ID of the image within the renderer*/
			void setBoundID(const uint32_t toSet);

			Image() noexcept;
			Image(const uint32_t width, const uint32_t height) noexcept;
			~Image();
		private:
			char* data; //The pointer to the actual image data in memory
			uint32_t width, height, ID, boundID; //The width, height and ID of the image
			ImageType type; //The type of image this is
			ImageMemoryProperty memProperty; //The memory details relating to this image
		};
	}
}