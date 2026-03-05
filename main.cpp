#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include <zlib.h>

struct Data
{
    std::optional<std::string> origin;
    std::optional<std::string> level;
    std::optional<std::string> message;
};

std::string getTimestamp()
{
   return std::to_string(time(0));
};

std::string compressData(Data data)
{
    std::string json = "{";
    if (data.origin.has_value()) {
        json += "\"origin\": \"" + data.origin.value() + "\",";
    }
    if (data.level.has_value()) {
        json += "\"level\": \"" + data.level.value() + "\",";
    }
    if (data.message.has_value()) {
        json += "\"message\": \"" + data.message.value() + "\",";
    }
    json += "\"timestamp\": \"" + getTimestamp() + "\"}";
    std::vector<unsigned char> compressed(1000);
    uLongf compressedSize = compressed.size();
    if (compress((Bytef*)compressed.data(), &compressedSize, (Bytef*)json.c_str(), json.size()) != Z_OK) {
        return "";
    }
    return std::string((char*)compressed.data(), compressedSize);
};

int writeLogFile(Data data)
{
    std::ofstream logFile;
    logFile.open("./log.txt", std::ios::binary | std::ios::app);

    if (!logFile.is_open())
    {
        return 1;
    }

    std::string compressedData = compressData(data);
    if (compressedData.empty()) {
        return 1;
    }
    uint32_t size = compressedData.size();

    logFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    logFile.write(compressedData.c_str(), compressedData.size());
    logFile.close();

    return 0;
};

std::string readLogFile()
{
    std::ifstream logFile;
    logFile.open("./log.txt", std::ios::binary);
    if (!logFile.is_open()) {
        return "";
    }
    std::string fileContent((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    logFile.close();
    return fileContent;
};

std::string decompressData()
{
    std::ifstream logFile("./log.txt", std::ios::binary);
    if (!logFile.is_open()) return "";

    std::string result;
    uint32_t size;
    while (logFile.read(reinterpret_cast<char*>(&size), sizeof(size))) {
        std::vector<char> compressed(size);
        logFile.read(compressed.data(), size);

        std::vector<unsigned char> decompressed(1000);
        uLongf decompressedSize = decompressed.size();
        if (uncompress((Bytef*)decompressed.data(), &decompressedSize,
                       (Bytef*)compressed.data(), size) == Z_OK) {
            std::string json((char*)decompressed.data(), decompressedSize);
            result += json + "\n";
        }
    }
    logFile.close();
    return result;
};

int main()
{
    writeLogFile(
        Data
        {
            .origin = "Test",
            .level = "Info",
            .message = "This is a test message"
        }
    );

    std::string decompressedData = decompressData();
    std::cout << decompressedData << std::endl;
    return 0;
};