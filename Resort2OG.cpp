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
		/*==========================================*/
		//					PMP
		// (Rather than only reading the bytes
		// we want to modify, since some PMP
		// objects will not be saved to the new file,
		// we read every object in full.)
		/*==========================================*/
		if (fileType == FType::PMP)
		{
			// File magic verification
			char magic[5] = "AAAA";
			inFile.read(reinterpret_cast<char*>(magic), 4);
			if (_strcmpi(magic, "PMPF") == 1)
				printError(PMP_INVALID_MAGIC);

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

			// Grab object data from PMP file
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

			// Output PMP
			std::ofstream outFile;
			outFile.open(std::string(argv[2]) + ".ogPMP", std::ios::binary | std::ios::trunc);
			
			// Calculate end of file for route/point null offsets (0x7F is header size)
			u32 fileSize = _byteswap_ulong(0x7F + outObjCount * sizeof(PMP::Object) + 0x1);

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

			// Saving the objects back into a PMP
			for (int obj = 0; obj < objCount; obj++)
			{
				// All Object members are in Big-Endian to make ifstream writes more simple,
				// so any checks will have to byteswap again
				if (_byteswap_ushort(objList[obj].groupID) == 0x0001) // We only save back objects in group 0x0001
				{
					// Refer to "Grabbing object data from PMP" if you want to see this process commented
					outFile.write(reinterpret_cast<char*>(&objList[obj].groupID), sizeof(objList[obj].groupID));
					outFile.write(reinterpret_cast<char*>(&objList[obj].objID), sizeof(objList[obj].objID));
					outFile.write(reinterpret_cast<char*>(&objList[obj].unk1), sizeof(objList[obj].unk1));
					for (int v = 0; v < 3; v++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].posVec[v]), sizeof(objList[obj].posVec[v]));

					for (int v = 0; v < 3; v++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].scaleVec[v]), sizeof(objList[obj].scaleVec[v]));

					for (int v = 0; v < 3; v++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].Mtx.firstColumn[v]), sizeof(objList[obj].Mtx.firstColumn[v]));

					for (int v = 0; v < 3; v++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].Mtx.secondColumn[v]), sizeof(objList[obj].Mtx.secondColumn[v]));

					for (int v = 0; v < 3; v++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].Mtx.thirdColumn[v]), sizeof(objList[obj].Mtx.thirdColumn[v]));

					outFile.write(reinterpret_cast<char*>(&objList[obj].unk2), sizeof(objList[obj].unk2));

					for (int p = 0; p < 16; p++)
						outFile.write(reinterpret_cast<char*>(&objList[obj].exParams[p]), sizeof(objList[obj].exParams[p]));
				}
			}

			// End output session
			outFile.close();
			// Free memory
			delete[] objList;
		}
		/*======================================*/
		//				  KCL
		// (Unlike PMP files, the output KCL
		// will always be the same file size,
		// so we only read entries in Section 3,
		// edit the flags and save them back.)
		// 
		// (The rest of the sections are read as well
		// to reconstruct the KCL file at the end.)
		/*======================================*/
		else if (fileType == FType::KCL)
		{
			u32 sec3Offset = 0; // Offset into the KCL to Section 3
			u32 sec4Offset = 0; // Offset into the KCL to Section 4
			u32 sec3TriCount = 0; // Section 3 triangle/entry count ((sec4Offset - sec3Offset) / sizeof(KCL::Triangle))

			std::ifstream inFile;
			inFile.open(argv[2], std::ios::binary | std::ios::ate);

			// Grab file size
			u32 fileSize = (u32)inFile.tellg();

			// Seek to Section 3 offset (u32)
			inFile.seekg(0x8, std::ios::beg);
			inFile.read(reinterpret_cast<char*>(&sec3Offset), sizeof(sec3Offset));
			// Section 3 is one indexed into Section 4, so the offset written is 0x10 behind the actual start of Section 3
			sec3Offset = _byteswap_ulong(sec3Offset) + 0x10;

			// Section 4 offset
			inFile.read(reinterpret_cast<char*>(&sec4Offset), sizeof(sec4Offset));
			sec4Offset = _byteswap_ulong(sec4Offset);
			sec3TriCount = (sec4Offset - sec3Offset) / sizeof(KCL::Triangle);
			// Allocate the array
			KCL::Triangle *Sec3Data = new KCL::Triangle[sec3TriCount];

			// Output KCL
			std::ofstream outFile;
			outFile.open(std::string(argv[2]) + ".ogKCL", std::ios::binary | std::ios::trunc);

			// Read sections 1 & 2
			inFile.seekg(0, std::ios::beg);
			char byte;
			for (u32 i = 0; i < sec3Offset; i++)
			{
				inFile.read(&byte, sizeof(byte));
				outFile.write(&byte, sizeof(byte));
			}

			// Read every triangle/entry and convert their flags (if needed)
			for (u32 t = 0; t < sec3TriCount; t++)
			{
				// Length
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].length), sizeof(Sec3Data[t].length));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].length), sizeof(Sec3Data[t].length));
				// Position index
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].posIndex), sizeof(Sec3Data[t].posIndex));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].posIndex), sizeof(Sec3Data[t].posIndex));
				// Direction index
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].dirIndex), sizeof(Sec3Data[t].dirIndex));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].dirIndex), sizeof(Sec3Data[t].dirIndex));
				// Normal A index
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].normalA), sizeof(Sec3Data[t].normalA));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].normalA), sizeof(Sec3Data[t].normalA));
				// Normal B index
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].normalB), sizeof(Sec3Data[t].normalB));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].normalB), sizeof(Sec3Data[t].normalB));
				// Normal C index
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].normalC), sizeof(Sec3Data[t].normalC));
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].normalC), sizeof(Sec3Data[t].normalC));
				// Collision flag
				inFile.read(reinterpret_cast<char*>(&Sec3Data[t].flag), sizeof(Sec3Data[t].flag));
				
				// Resort KCL flag -> OGWS KCL flag conversion
				switch (Sec3Data[t].flag)
				{
				case 0x0101: // Resort (Fairway)
					Sec3Data[t].flag = 0x0001; // OGWS (Fairway)
					break;
				case 0x0102: // Resort (Rough)
					Sec3Data[t].flag = 0x0002; // OGWS (Rough)
					break;
				case 0x0103: // Resort (Bunker)
					Sec3Data[t].flag = 0x0003; // OGWS (Bunker)
					break;
				case 0x0104: // Resort (OB)
					Sec3Data[t].flag = 0x0004; // OGWS (OB)
					break;
				case 0x0106: // Resort (Green)
					Sec3Data[t].flag = 0x0006; // OGWS (Green)
					break;
				case 0x0107: // Resort (Water Hazard)
					Sec3Data[t].flag = 0x0007; // OGWS (Water Hazard)
					break;
				case 0x0109: // Resort (Fringe)
					Sec3Data[t].flag = 0x0009; // OGWS (Fringe)
					break;
				case 0x010C: // Resort (Cliffside)
					Sec3Data[t].flag = 0x000C; // OGWS (Cliffside)
					break;
				case 0x0805: // Resort (Camera Lock OB)
					Sec3Data[t].flag = 0x0005; // OGWS (Camera Lock OB)
					break;
				}

				// Write the flag after it has been converted rather than after the initial read
				outFile.write(reinterpret_cast<char*>(&Sec3Data[t].flag), sizeof(Sec3Data[t].flag));
			}

			// Read Section 4 and write it to the new KCL
			// There is probably a better way to do this but I don't know
			for (u32 i = 0; i < fileSize - sec4Offset; i++)
			{
				inFile.read(&byte, sizeof(byte));
				outFile.write(&byte, sizeof(byte));
			}

			// End I/O session
			inFile.close();
			outFile.close();

			// Free memory
			delete[] Sec3Data;
		}
	}
	else printError(OPEN_FILE_ERROR);


	return 0;
}