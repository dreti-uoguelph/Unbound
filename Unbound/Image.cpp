#include "stdafx.h"
#include "Image.h"

namespace Unbound
{
	namespace Graphics
	{
		/*Get the width of the image, in pixels*/
		const uint32_t Unbound::Graphics::Image::getWidth() const
		{
			return width;
		}

		/*Get the height of the image, in pixels*/
		const uint32_t Image::getHeight() const
		{
			return height;
		}

		/*Get the format of the image*/
		const ImageType Image::getFormat() const
		{
			return type;
		}

		/*Get a pointer to the data in the image
		If this is not in a memory pool, it will have to be loaded in which is slow
		For this reason, it is not recommended to modify the content of an image at runtime*/
		const char* Image::getData() const
		{
			return data;
		}

		/*Get the size of the image data, in bytes*/
		const uint32_t Image::getSizeInBytes() const
		{
			char bytesPerPixel = 4;
			return width * height * bytesPerPixel;
		}

		/*Set the size of the image, in pixels*/
		void Image::setSize(const uint32_t widthToSet, const uint32_t heightToSet)
		{
			width = widthToSet;
			height = heightToSet;
		}

		/*Set the internal data pointer of the image*/
		void Image::setData(char* dataToSet)
		{
			data = dataToSet;
		}

		Unbound::Graphics::Image::Image() noexcept
		{
			ID = 0;
			data = nullptr;
			width = 0;
			height = 0;
			boundID = 0;
			type = ImageType::IMAGE_TYPE_TEXTURE_GENERAL;
		}

		Image::Image(const uint32_t widthToSet, const uint32_t heightToSet) noexcept
		{
			ID = 0;
			data = nullptr;
			width = widthToSet;
			height = heightToSet;
			boundID = 0;
			type = ImageType::IMAGE_TYPE_TEXTURE_GENERAL;
		}

		/*Get the ID of the image in the renderer*/
		const uint32_t Image::getID() const
		{
			return ID;
		}

		const uint32_t Image::getBoundID() const
		{
			return boundID;
		}

		/*Set the ID of the image, in the renderer*/
		void Image::setID(const uint32_t toSet)
		{
			ID = toSet;
		}

		void Image::setBoundID(const uint32_t toSet)
		{
			boundID = toSet;
		}

		Unbound::Graphics::Image::~Image()
		{
		}
	}
}