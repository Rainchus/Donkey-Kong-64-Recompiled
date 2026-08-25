#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <zlib.h>

namespace fs = std::filesystem;

constexpr uint32_t NEXT_AVAILABLE_FREE_SPACE = 0x2000000;

struct Overlay {
    std::string name;
    uint32_t codeROMAddress;
    uint32_t dataROMAddress;
    uint32_t dataCompressedSize;
    
    uint32_t codeCompressedSize{0};
    std::vector<uint8_t> codeCompressedData;
    std::vector<uint8_t> codeDecompressedData;
    std::vector<uint8_t> dataCompressedData;
    std::vector<uint8_t> dataDecompressedData;
};

std::vector<uint8_t> gzipDecompress(const std::vector<uint8_t>& compressedData) {
    z_stream strm = {};
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        throw std::runtime_error("Failed to initialize zlib inflation.");
    }

    strm.next_in = const_cast<Bytef*>(compressedData.data());
    strm.avail_in = static_cast<uInt>(compressedData.size());

    std::vector<uint8_t> decompressedData;
    constexpr size_t BUFFER_SIZE = 32768;
    std::vector<uint8_t> buffer(BUFFER_SIZE);

    int ret;
    do {
        strm.next_out = buffer.data();
        strm.avail_out = BUFFER_SIZE;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("Gzip decompression error encountered.");
        }

        size_t have = BUFFER_SIZE - strm.avail_out;
        decompressedData.insert(decompressedData.end(), buffer.data(), buffer.data() + have);
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return decompressedData;
}

std::array<uint8_t, 4> convertChunk(const std::array<uint8_t, 4>& chunk, bool byteSwapped, bool wordSwapped) {
    std::array<uint8_t, 4> output = chunk;
    if (byteSwapped) {
        // v64 format -> reverses uint16_t byte order
        output[0] = chunk[1];
        output[1] = chunk[0];
        output[2] = chunk[3];
        output[3] = chunk[2];
    }
    if (wordSwapped) {
        // n64 format -> reverses uint32_t byte order
        output[0] = chunk[3];
        output[1] = chunk[2];
        output[2] = chunk[1];
        output[3] = chunk[0];
    }
    return output;
}

void readAndDecompressOverlays(std::fstream& fr, std::vector<Overlay>& overlays) {
    for (auto& x : overlays) {
        x.codeCompressedSize = x.dataROMAddress - x.codeROMAddress;

        fr.seekg(x.codeROMAddress, std::ios::beg);
        x.codeCompressedData.resize(x.codeCompressedSize);
        fr.read(reinterpret_cast<char*>(x.codeCompressedData.data()), x.codeCompressedSize);
        x.codeDecompressedData = gzipDecompress(x.codeCompressedData);
        x.dataCompressedData.resize(x.dataCompressedSize);
        fr.read(reinterpret_cast<char*>(x.dataCompressedData.data()), x.dataCompressedSize);
        x.dataDecompressedData = gzipDecompress(x.dataCompressedData);
    }
}

uint64_t alignHex10(std::fstream& fr) {
    uint64_t current_position = (uint64_t)fr.tellp();
    constexpr uint64_t alignment = 0x10;
    uint64_t remainder = current_position % alignment;

    if (remainder != 0) {
        uint64_t offset = alignment - remainder;
        fr.seekp(offset, std::ios::cur);
    }

    return (uint64_t)fr.tellp();
}

void writeDecompressedOverlaysToROM(std::fstream& fr, std::vector<Overlay>& overlays) {
    fr.seekp(NEXT_AVAILABLE_FREE_SPACE, std::ios::beg);

    for (const auto& x : overlays) {
        uint64_t decompressedCodeStart = alignHex10(fr);
        fr.write(reinterpret_cast<const char*>(x.codeDecompressedData.data()), x.codeDecompressedData.size());
        uint64_t decompressedDataStart = alignHex10(fr);
        fr.write(reinterpret_cast<const char*>(x.dataDecompressedData.data()), x.dataDecompressedData.size());
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_rom_path>\n";
        return 1;
    }

    fs::path srcFile = argv[1];
    const fs::path dstFile = "donkeykong64.decompressed.us.z64";

    if (!fs::exists(srcFile)) {
        std::cerr << "Error: Input file does not exist: " << srcFile << "\n";
        return 1;
    }

    std::vector<Overlay> overlays = {
        {"global_asm", 0x113F0, 0xC29D4, 0x949C},
        {"menu",       0xCBE70, 0xD4554, 0x5A2},
        {"multiplayer",0xD4B00, 0xD69F8, 0xFB},
        {"minecart",   0xD6B00, 0xD98A0, 0x197},
        {"bonus",      0xD9A40, 0xDF346, 0x2AB},
        {"race",       0xDF600, 0xE649A, 0x2DB},
        {"critter",    0xE6780, 0xE9D17, 0x38C},
        {"boss",       0xEA0B0, 0xF388F, 0x90A},
        {"arcade",     0xF41A0, 0xFB42C, 0x1EC4},
        {"jetpac",     0xFD2F0, 0x1010FD,0x936},
    };

    try {
        std::ifstream inFile(srcFile, std::ios::binary | std::ios::ate);
        if (!inFile) {
            std::cerr << "Error opening input file: " << srcFile << "\n";
            return 1;
        }
        std::streamsize size = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        std::vector<uint8_t> romBuffer(size);
        if (!inFile.read(reinterpret_cast<char*>(romBuffer.data()), size)) {
            std::cerr << "Error reading input file contents.\n";
            return 1;
        }
        inFile.close();

        if ((romBuffer[0x3B] != 0x4E) || (romBuffer[0x3C] != 0x44) || (romBuffer[0x3D] != 0x4F) || (romBuffer[0x3E] != 0x45)) {
            std::cerr << "Provided ROM is not the US version.\n";
            return 1;
        }

        std::ofstream outFile(dstFile, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error creating destination file: " << dstFile << "\n";
            return 1;
        }
        outFile.write(reinterpret_cast<const char*>(romBuffer.data()), romBuffer.size());
        outFile.close();

        std::fstream fh(dstFile, std::ios::in | std::ios::out | std::ios::binary);
        if (!fh.is_open()) {
            std::cerr << "Error reopening target file: " << dstFile << "\n";
            return 1;
        }

        std::cout << "[1 / 2] Decompressing overlays...\n";
        readAndDecompressOverlays(fh, overlays);

        std::cout << "[2 / 2] Writing decompressed overlays...\n";
        writeDecompressedOverlaysToROM(fh, overlays);
        std::cout << "Decompressed ROM created at: " << dstFile << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}