#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <intrin.h>
#include "KCL.h"
#include "PMP.h"

enum Error
{
	INVALID_INPUT = 0x1,
	OPEN_FILE_ERROR = 0x2,
	PMP_INVALID_MAGIC = 0x3,
};

enum class FType
{
	EMPTY = 0x0,
	KCL = 0x1,
	PMP = 0x2
};

void printError(Error err)
{
	switch (err)
	{
	case INVALID_INPUT:
		std::cout << "Invalid input!" << std::endl;
		std::cout << "Valid usage:" << std::endl << "Resort2OG.exe kcl/pmp myFile.kcl/myFile.pmp" << std::endl;
	case OPEN_FILE_ERROR:
		std::cout << "An error caused your input file to open unsuccessfully." << std::endl;
		std::cout << "Are you sure the file exists and is not being used by another process?" << std::endl;
	case PMP_INVALID_MAGIC:
		std::cout << "Your input PMP file does not have the correct PMP file magic (PMPF)" << std::endl;
		std::cout << "Are you sure this is a valid PMP?" << std::endl;
	}

	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	FType fileType = FType::EMPTY;

	// Checking arg count
	if (argc == 3)
	{
		// Checking requested filetype
		if (_strcmpi(argv[1], "KCL") == 0)		fileType = FType::KCL;
		else if (_strcmpi(argv[1], "PMP") == 0)	fileType = FType::PMP;
		else printError(INVALID_INPUT);
	}
	else printError(INVALID_INPUT);

	std::ifstream inFile;
	inFile.open(argv[2], std::ios::binary);
	if (inFile.is_open())
	{
		// Input file is said to be PMP
		if (fileType == FType::PMP)
		{
			// File magic verification
			char magic[5] = "AAAA";
			inFile.read(reinterpret_cast<char*>(magic), 4);
			if (_strcmpi(magic, "PMPF") == 1)
			{
				printError(PMP_INVALID_MAGIC);
			}

			inFile.seekg(0x10, std::ios::beg); // Move past first set of padding -> obj count

			// Allocate space for the objects
			u16 objCount;
			inFile.read(reinterpret_cast<char*>(&objCount), sizeof(objCount));
			objCount = _byteswap_ushort(objCount);
			PMP::Object* objList = new PMP::Object[objCount];

			inFile.seekg(0x40, std::ios::beg); // Move past second set of padding -> obj data offset

			// Grab object data offset
			u32 objOffset;
			inFile.read(reinterpret_cast<char*>(&objOffset), sizeof(objOffset));
			objOffset = _byteswap_ulong(objOffset);

			inFile.seekg(objOffset, std::ios::beg); // Move to object data

			// How many tree3 (0x6) objects have been converted
			// If even, the next tree3 is converted to a tree2 (0x3), else tree1 (0x2)
			int tree3Count = 0;

			// How many objects will be saved (all that aren't in group 0xB)
			u16 outObjCount = 0;

			for (int obj = 0; obj < objCount; obj++)
			{
				// Group ID
				inFile.read(reinterpret_cast<char*>(&objList[obj].groupID), sizeof(objList[obj].groupID));

				// Main Resort group 0x2 -> Main OGWS group 0x1
				if (_byteswap_ushort(objList[obj].groupID) == 0x0002)
				{
					objList[obj].groupID = _byteswap_ushort(0x0001);
					outObjCount++;
				}

				// Object ID
				inFile.read(reinterpret_cast<char*>(&objList[obj].objID), sizeof(objList[obj].objID));

				// Is current object a tree3 (0x6) object?
				if (_byteswap_ushort(objList[obj].objID) == 0x0006)
				{
					// If even, the next tree3 is converted to a tree2 (0x3), else tree1 (0x2)
					if (tree3Count % 2 == 0)
						objList[obj].objID = _byteswap_ushort(0x0003);
					else
						objList[obj].objID = _byteswap_ushort(0x0002);

					tree3Count++;
				}

				// Unknown
				inFile.read(reinterpret_cast<char*>(&objList[obj].unk1), sizeof(objList[obj].unk1));

				// Position vectors
				for (int v = 0; v < 3; v++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].posVec[v]), sizeof(objList[obj].posVec[v]));

				// Scale vectors
				for (int v = 0; v < 3; v++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].scaleVec[v]), sizeof(objList[obj].scaleVec[v]));

				// Transformation matrix column 1
				for (int v = 0; v < 3; v++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].Mtx.firstColumn[v]), sizeof(objList[obj].Mtx.firstColumn[v]));

				// Column 2
				for (int v = 0; v < 3; v++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].Mtx.secondColumn[v]), sizeof(objList[obj].Mtx.secondColumn[v]));

				// Column 3
				for (int v = 0; v < 3; v++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].Mtx.thirdColumn[v]), sizeof(objList[obj].Mtx.thirdColumn[v]));

				// Unknown
				inFile.read(reinterpret_cast<char*>(&objList[obj].unk2), sizeof(objList[obj].unk2));

				// Additional parameters
				for (int p = 0; p < 16; p++)
					inFile.read(reinterpret_cast<char*>(&objList[obj].exParams[p]), sizeof(objList[obj].exParams[p]));
			}

			// End input session
			inFile.close();

			// Output file
			std::ofstream outFile;
			outFile.open(std::string(argv[2]) + ".ogPMP", std::ios::binary);
			
			// Grab file size
			std::ifstream getFileSize;
			getFileSize.open(argv[2], std::ios::binary | std::ios::ate);
			u32 fileSize = _byteswap_ulong(getFileSize.tellg());
			getFileSize.close();

			// Padding used in the header
			std::string headerPadding1(0xC, '\x00');
			std::string headerPadding2(0x2E, '\x00');
			std::string headerPadding3(0x34, '\x00');

			// Converting vars to Big Endian as needed
			outObjCount = _byteswap_ushort(outObjCount);
			objOffset = _byteswap_ulong(objOffset);

			// Write header
			outFile << "PMPF" << headerPadding1; // Header + Padding section 1
			outFile.write(reinterpret_cast<char*>(&outObjCount), sizeof(outObjCount)); // Object count (how many were saved back)
			outFile << headerPadding2; // Null bytes because there are no routes or points + Padding section 2
			outFile.write(reinterpret_cast<char*>(&objOffset), sizeof(objOffset)); // Object data offset
			outFile.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize)); // Route data set to end of file (no routes)
			outFile.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize)); // Point data set to end of file (no points)
			outFile << headerPadding3; // Header padding section 3

			for (int obj = 0; obj < objCount; obj++)
			{
				
			}

			outFile.close();
			delete[] objList; // Free memory
		}
		// Input file is said to be KCL
		else if (fileType == FType::KCL)
		{
			

			// End input session
			inFile.close();
		}

	}
	else printError(OPEN_FILE_ERROR);


	return 0;
}