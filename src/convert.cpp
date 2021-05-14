#include "convert.h"
#include <fstream>
#include <cassert>
#include "core/RPUtility/RPUtlMapData.h"
#include "core/KCol.h"
#include "core/endian.h"

void convertFile(char* p, EFileType type)
{
    assert(p);
    std::string path(p);

    // Create buffer for file
    std::ifstream is(path, std::ios::binary | std::ios::ate);

    if (!is.is_open())
    {
        std::printf("Your file (%s) could not be opened. Are you sure you entered the right filename?", path.c_str());
        return;
    }

    u32 size = is.tellg();
    char* buf = new char[size];

    // Read into buffer
    is.seekg(0, std::ios::beg);
    is.read(buf, size);
    is.close();

    // Convert KCL file
    if (type == EFileType::KCL)
    {
        KCol* kcl = new KCol(buf);
        // Swap endianness if needed
        if (!usingBigEndian()) kcl->swapEndian();
        // Convert to OGWS
        kcl->convert();

        // Swap endianess back before writing file
        if (!usingBigEndian()) kcl->swapEndian();

        // Write new file
        std::ofstream os(path + ".ogKCL", std::ios::binary);
        os.write((char*)kcl->mpHeader, size);
        os.close();
    }
    else if (type == EFileType::PMP)
    {
        // Check file magic
        RPUtlMapData::SPmpHeader* header = (RPUtlMapData::SPmpHeader*)buf;
        if (BIG_ENDIAN_32(header->mMagic) != RPUtlMapData::cPackedMapMagic)
        {
            std::printf("Your file (%s) is not a valid PMP file.", path.c_str());
            return;
        }

        RPUtlMapData* map = new RPUtlMapData(buf);
        // Swap endianness if needed
        if (!usingBigEndian()) map->swapEndian();
        // Convert to OGWS
        map->convert();

        // Get new file size
        u32 newSize = map->getFileSize();

        // Swap endianess back before writing file
        if (!usingBigEndian()) map->swapEndian();

        // Write new file
        std::ofstream os(path + ".ogPMP", std::ios::binary);
        os.write((char*)map->mpHeader, newSize);
        os.close();
    }
    else
    {
        assert(false);
    }

    delete[] buf;
}
