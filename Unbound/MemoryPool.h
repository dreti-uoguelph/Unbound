#pragma once
#include"stdafx.h"

namespace Unbound
{
	/*A flexible class for allocating sections of a buffer*/
	class MemoryPool
	{
	public:
		/*Respresents a single data section in the memory pool. Used for tracking and retrieving data*/
		struct MemorySection
		{
			uint64_t locationOffset; //The offset of the start of the data section from the start of the pool
			uint64_t dataSize; //The size of the data section, in bytes;
			uint32_t id; //The id of this data section, which is used to track it
		};

		/*A memory label contains a section as well as a bit of metadata for higher level usage*/
		struct MemoryLabel
		{
			std::string name;
			MemorySection section;
		};

		std::string name;

		//Initialize the memory pool and allocate memory
		void init();
		void init(uint64_t initialMaxSize);

		//Resize the memory pool and free the remaining memory
		void resize(size_t newSize);

		//Close the memory pool and free the remaining memory
		void close();

		//This will copy data into the memory pool. Returns the ID of the resulting data section
		uint32_t addData(char* dataToAdd, uint64_t sizeOfData, uint32_t parentMemoryLabelID = UINT32_MAX);

		//This will allocate a chunk of memory and return its address
		void* addSection(uint64_t sizeOfData, uint32_t parentMemoryLabelID = UINT32_MAX);

		//This will add a metalabel for a given section of memory. Returns the ID of the new label
		uint32_t addMetalabel(std::string name, uint64_t offset, uint64_t size);

		//Get a pointer to a section of data represented by idToGet
		char* get(uint32_t idToGet);

		/*Get a pointer to the start of a metalabel's data section*/
		char* getMetalabelData(uint32_t idToGet);
		char* getMetalabelData(std::string nameToGet);

		//Get a pointer to the start of the data in the memory pool
		char* getData();

		//Get the location offset of a metalabel in the memory pool
		uint64_t getMetalabelOffset(std::string name);
		uint64_t getMetalabelOffset(uint32_t id);

		//Get the size of a metalabel in the memory pool
		uint64_t getMetalabelSize(std::string name);
		uint64_t getMetalabelSize(uint32_t id);

		//Finds a metalabel based on its name or id
		MemoryLabel findMetalabel(uint32_t id = 0, std::string name = "");
		//Get a metalabel's ID based on its name
		uint32_t getMetalabelID(std::string labelname);

		/*Returns the size of the memory pool*/
		uint64_t getSize();

		//This will remove the section of data represented by idToRemove from the pool and make that memory available
		void remove(uint32_t idToRemove);

		//Defragment the memory pool. As memory is written an freed, fragmentation will increase so this should be called
		void defragment();

		//Returns the amount of space currently used in bytes
		uint64_t getCurrentUsageBytes();
		//Returns the amount of space currently used as a percent, from 0.0f to 100.0f
		float getCurrentUsagePercent();

		MemoryPool() noexcept;
		MemoryPool(uint64_t initialSize) noexcept;
		~MemoryPool();
	private:
		char* data;
		bool allowAutomaticResizing = false; //Whether to allow the pool to increase its size automatically if it's full
		uint64_t maxSize = 0; //The maximum size of the pool, in bytes
		uint64_t currentUsage = 0; //The number of bytes that are currently being used
		std::vector<MemorySection> sections; //The sections of data that are currently in the pool
		std::vector<MemoryLabel> metaLabels; //Metalabels allow memory sections already referenced by a section to be accessed under a different name

		//Returns the offset in the pool to a section of free space of size amountNeeded
		uint64_t findFreeSpace(uint64_t amountNeeded, uint32_t parentLabelID = UINT32_MAX);
	};

	/*A high performace class meant for staging memory management. Flushes fast but lacks the ability to remove sections*/
	class StagingMemoryPool
	{
	public:

		/*This will initialize the memory pool with the buffer pointed to by that variable*/
		void init(void* buffer);

		/*This will return the address of a section of memory of the desired size*/
		void* addData(const uint64_t size);

		//This will set the maximum size of the staging buffer
		void setMaxSize(const uint64_t sizeToSet);

		/*This will empty the enture memory pool and prepare it for refilling*/
		void flush();

		/*This will completely close the memory pool. You need to call init if you want to use it again*/
		void close();

		/*This returns the start of the data block*/
		void* getData();

		/*Returns a pointer to the start of the free section, usable for writing*/
		void* getFreeSectionStart();

		/*This returns the size of the data section*/
		uint64_t getSize();

		/*Returns the current usage of the memory pool*/
		uint64_t getCurrentUsage();

		StagingMemoryPool();
		~StagingMemoryPool();

	private:
		char* data;
		char* freesectionstart;
		uint64_t currentUsage;
		uint64_t maxsize;
	};
}