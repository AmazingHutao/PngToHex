#include <iostream>
#include <fstream>
#include <cstdint>

int main()
{

    // 打開 PNG 文件
    std::ifstream input("image.png", std::ios::binary);
    if (!input.is_open())
    {
        std::cerr << "Unable to open input file" << std::endl;
        return -1;
    }

    // 讀取 PNG 文件標頭
    const uint8_t png_header[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    uint8_t header[sizeof(png_header)];
    input.read((char*)header, sizeof(header));
    if (memcmp(header, png_header, sizeof(png_header)) != 0)
    {
        std::cerr << "Invalid PNG header" << std::endl;
        return -1;
    }

    // 讀取 PNG 文件數據區
    while (input.good())
    {
        uint8_t chunk_header[8];
        uint32_t chunk_size;
        input.read((char*)chunk_header, sizeof(chunk_header));
        if (input.gcount() < sizeof(chunk_header))
        {
            break;
        }
        chunk_size = ((uint32_t)chunk_header[0] << 24) | ((uint32_t)chunk_header[1] << 16) | ((uint32_t)chunk_header[2] << 8) | (uint32_t)chunk_header[3];

        if (memcmp(chunk_header + 4, "IDAT", 4) == 0)
        {
            // 讀取 IDAT 數據區
            uint8_t* data = new uint8_t[chunk_size];
            input.read((char*)data, chunk_size);
            if (input.gcount() < chunk_size)
            {
                std::cerr << "Incomplete IDAT chunk" << std::endl;
                delete[] data;
                return -1;
            }

            // 轉換每個像素
            for (uint32_t i = 0; i < chunk_size; i += 3)
            {
                std::cout << "0x" << std::hex << (int)data[i] << ", 0x" << std::hex << (int)data[i + 1] << ", 0x" << std::hex << (int)data[i + 2] << ", ";
            }

            delete[] data;
        }

        // 跳過其它數據區
        input.seekg(chunk_size + 4, std::ios::cur);
    }

    input.close();

    return 0;
}
