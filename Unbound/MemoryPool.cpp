#include "stdafx.h"
#include "MemoryPool.h"

namespace Unbound
{
	//Initialize the memory pool and allocate memory
	void MemoryPool::init()
	{
		if (maxSize == 0)
		{
			throw std::runtime_error("Could not create memory pool, as the size was too low");
		}

		metaLabels = std::vector<MemoryLabel>();
		data = reinterpret_cast<char*>(malloc(maxSize));
	}

	//Same as above, but with the default size as an argument
	void MemoryPool::init(uint64_t initialMaxSize)
	{
		maxSize = initialMaxSize;
		init();
	}

	//Resize the memory pool and free the remaining memory
	void MemoryPool::resize(size_t newSize)
	{
		maxSize = newSize;
		data = reinterpret_cast<char*>(realloc(data, maxSize));
	}

	//Close the memory pool and free the remaining memory
	void MemoryPool::close()
	{
		free(data);
	}

	//This will copy data into the memory pool. Returns the ID of the new data section with the copied data
	uint32_t MemoryPool::addData(char* dataToAdd, uint64_t sizeOfData, uint32_t parentMemoryLabelID)
	{
		uint64_t dstOffset = findFreeSpace(sizeOfData, parentMemoryLabelID);

		if (dstOffset != UINT64_MAX)
		{
			if (dataToAdd != nullptr)
			{
				memcpy(data + dstOffset, dataToAdd, sizeOfData);
			}

			if (sections.size() == 0)
			{
				sections.push_back({ 0, sizeOfData, 0 });
				return 0;
			}

			sections.push_back({ dstOffset, sizeOfData, sections[sections.size() - 1].id + 1 });
		}

		return sections[sections.size() - 1].id;
	}

	//This will allocate an empty chunk of memory and return its address
	void* MemoryPool::addSection(uint64_t sizeOfData, uint32_t parentMemoryLabelID)
	{
		uint64_t dstOffset = findFreeSpace(sizeOfData, parentMemoryLabelID);

		if (dstOffset != UINT64_MAX)
		{
			if (sections.size() == 0)
			{
				sections.push_back({ 0, sizeOfData, 0 });
				return data;
			}

			sections.push_back({ dstOffset, sizeOfData, sections[sections.size() - 1].id + 1 });
		}

		return data + dstOffset;
	}

	//This will add a metalabel for a given section of memory. Returns the ID of the new label
	uint32_t MemoryPool::addMetalabel(std::string name, uint64_t offset, uint64_t size)
	{
		if (offset + size > maxSize)
		{
			throw std::runtime_error("Could not allocate a metalabel larger than the memory pool it is contained in");
		}

		metaLabels.push_back({ name, {offset, size, static_cast<uint32_t>(metaLabels.size()) } });
		return static_cast<uint32_t>(metaLabels.size()) - 1;
	}

	char* MemoryPool::get(uint32_t idToGet)
	{
		return data + sections[idToGet].locationOffset;
	}

	char* MemoryPool::getMetalabelData(uint32_t idToGet)
	{
		return data + getMetalabelOffset(idToGet);
	}

	char* MemoryPool::getMetalabelData(std::string nameToGet)
	{
		return data + getMetalabelOffset(nameToGet);
	}

	char* MemoryPool::getData()
	{
		return data;
	}

	//Get the location offset of a metalabel in the memory pool
	uint64_t MemoryPool::getMetalabelOffset(std::string name)
	{
		return findMetalabel(0, name).section.locationOffset;
	}

	uint64_t MemoryPool::getMetalabelOffset(uint32_t id)
	{
		return findMetalabel(id).section.locationOffset;
	}

	//Get the size of a metalabel in the memory pool
	uint64_t MemoryPool::getMetalabelSize(std::string name)
	{
		return findMetalabel(0, name).section.dataSize;
	}

	uint64_t MemoryPool::getMetalabelSize(uint32_t id)
	{
		return findMetalabel(id).section.dataSize;
	}

	//Get a metalabel's ID based on its name
	uint32_t MemoryPool::getMetalabelID(std::string labelname)
	{
		for (auto i : metaLabels)
		{
			if (i.name == labelname)
			{
				return i.section.id;
			}
		}

		return UINT32_MAX;
	}

	//This will remove the section of data represented by idToRemove from the pool and make that memory available
	void MemoryPool::remove(uint32_t idToRemove)
	{
	}

	void MemoryPool::defragment()
	{
	}

	//Returns the amount of space currently used in bytes
	uint64_t MemoryPool::getCurrentUsageBytes()
	{
		return currentUsage;
	}

	//Returns the amount of space currently used as a percent, from 0.0f to 100.0f
	float MemoryPool::getCurrentUsagePercent()
	{
		if (maxSize > 0)
		{
			return getCurrentUsageBytes() / static_cast<float>(maxSize);
		}

		return 0.0f;
	}

	MemoryPool::MemoryPool() noexcept
	{
		data = nullptr;
	}

	MemoryPool::MemoryPool(uint64_t initialSize) noexcept
	{
		maxSize = initialSize;
		data = nullptr;
	}

	MemoryPool::~MemoryPool()
	{
		close();
	}

	//Returns the offset in the pool to a section of free space of size amountNeeded
	//Returns UINT64_MAX if a section of the right size is not found
	uint64_t MemoryPool::findFreeSpace(uint64_t amountNeeded, uint32_t parentLabelID)
	{
		uint64_t offset = 0;

		if (parentLabelID != UINT32_MAX)
		{
			offset = getMetalabelOffset(parentLabelID);
		}

		if (sections.size() == 0)
		{
			if (amountNeeded + offset <= maxSize)
			{
				currentUsage += amountNeeded;
				return offset;
			}

			return UINT64_MAX;
		}

		if (offset == 0)
		{
			if (amountNeeded > maxSize || sections[sections.size() - 1].locationOffset + sections[sections.size() - 1].dataSize + amountNeeded > maxSize)
			{
				return UINT64_MAX;
			}

			currentUsage += amountNeeded;
			return sections[sections.size() - 1].locationOffset + sections[sections.size() - 1].dataSize;
		}
		else
		{
			if (amountNeeded + offset > maxSize)
			{
				return UINT64_MAX;
			}

			currentUsage += amountNeeded;
			return offset;
		}
	}

	//Finds a metalabel based on its name or id
	MemoryPool::MemoryLabel MemoryPool::findMetalabel(uint32_t id, std::string name)
	{
		if (name != "")
		{
			for (auto i : metaLabels)
			{
				if (i.name == name)
				{
					return i;
				}
			}

			throw std::runtime_error("Could not find metalabel for processing");
		}

		for (auto i : metaLabels)
		{
			if (i.section.id == id)
			{
				return i;
			}
		}

		throw std::runtime_error("Could not find metalabel for processing");
	}

	uint64_t MemoryPool::getSize()
	{
		return maxSize;
	}

	/*This will initialize the memory pool with the buffer pointed to by that variable*/
	void StagingMemoryPool::init(void* buffer)
	{
		data = reinterpret_cast<char*>(buffer);
		freesectionstart = data;
	}

	/*This will return the address of a section of memory of the desired size*/
	void* StagingMemoryPool::addData(uint64_t size)
	{
		if (currentUsage + size > maxsize)
		{
			Logger::log("Error: trying to add data to full staging memory pool. MaxSize: " + std::to_string(maxsize) + " Data size: " + std::to_string(size));
			return nullptr;
		}

		currentUsage += size;
		freesectionstart += size;
		return freesectionstart - size;
	}

	/*This will empty the enture memory pool and prepare it for refilling*/
	void StagingMemoryPool::flush()
	{
		freesectionstart = data;
		currentUsage = 0;
	}

	/*This will completely close the memory pool. You need to call init if you want to use it again*/
	void StagingMemoryPool::close()
	{
		flush();
	}

	/*This returns the start of the data block*/
	void* StagingMemoryPool::getData()
	{
		return reinterpret_cast<void*>(data);
	}

	/*Returns a pointer to the start of the free section, usable for writing*/
	void* StagingMemoryPool::getFreeSectionStart()
	{
		return reinterpret_cast<void*>(freesectionstart);
	}

	/*This returns the size of the data section*/
	uint64_t StagingMemoryPool::getSize()
	{
		return maxsize;
	}

	/*Sets the maximum size the staging pool can reach*/
	void StagingMemoryPool::setMaxSize(uint64_t sizeToSet)
	{
		maxsize = sizeToSet;
	}

	uint64_t StagingMemoryPool::getCurrentUsage()
	{
		return currentUsage;
	}

	StagingMemoryPool::StagingMemoryPool()
	{
		currentUsage = 0;
		freesectionstart = nullptr;
		maxsize = 0;
		data = nullptr;
	}

	StagingMemoryPool::~StagingMemoryPool()
	{
	}
}