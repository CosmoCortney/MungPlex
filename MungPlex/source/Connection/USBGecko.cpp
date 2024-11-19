#include <thread>
#include "USBGecko.hpp"
#include <vector>
#pragma comment(lib, "setupapi.lib")

boost::mutex MungPlex::USBGecko::s_AccessMutex;

MungPlex::USBGecko::~USBGecko()
{
    resetDevice();
    Disconnect();
}

FT_STATUS MungPlex::USBGecko::Init()
{
    static FT_STATUS ftStatus = 0;
        
    if ((ftStatus = resetDevice()) != FT_OK) return ftStatus; //assignments are intentional
    if ((ftStatus = purge(1)) != FT_OK) return ftStatus; //RX
    if ((ftStatus = purge(2)) != FT_OK) return ftStatus; //TX
    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::Connect()
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;

    if (_connectedAndReady)
    {
        _connectedAndReady = false;

        if ((ftStatus = Disconnect()) != FT_OK)
            return ftStatus;
    }

    if ((ftStatus = findUsbGecko()) != FT_OK) return ftStatus;   
    if ((ftStatus = openUsbGecko()) != FT_OK) return ftStatus;
    if ((ftStatus = setLatencyTimeouts(2000, 2000)) != FT_OK) return ftStatus;
    if ((ftStatus = setLatencyTimer(2)) != FT_OK) return ftStatus;
    if ((ftStatus = setUsbParameters(0x10000, 0)) != FT_OK) return ftStatus;

    if ((ftStatus = sendGeckoCommand(cmd_hook)) != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to send Hook command\n";
#endif
        return ftStatus;
    }

    wait(200);

    if ((ftStatus = sendGeckoCommand(0xCD)) != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to send lang override command (no override)\n";
#endif
        return ftStatus;
    }

    if ((ftStatus = sendGeckoCommand(0)) != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to send video patch command (no patch)\n";
#endif
        return ftStatus;
    }

    if ((ftStatus = Init()) != FT_OK)
        return ftStatus;

    wait(150);
    _connectedAndReady = true;
    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::Reset()
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;
    if ((ftStatus = resetDevice()) != FT_OK) return ftStatus; //Assignmens are intentional
    if ((ftStatus = purge(1)) != FT_OK) return ftStatus; //RX
    return purge(2); //TX
}

FT_STATUS MungPlex::USBGecko::Disconnect()
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    _connectedAndReady = false;
    return closeUsbGecko();
}

FT_STATUS MungPlex::USBGecko::Read(char* buf, const uint64_t rangeStart, const uint64_t readSize)
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;

    if(readSize > 8)
        _connectedAndReady = false;

    if ((ftStatus = Init()) != FT_OK) 
        return ftStatus;

    if ((ftStatus = dump(buf, rangeStart, rangeStart + readSize)) != FT_OK)
        return ftStatus;

    _connectedAndReady = true;
    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::Write(char* buf, const uint64_t rangeStart, const uint64_t writeSize)
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;
    _connectedAndReady = false;

    if ((ftStatus = Init()) != FT_OK)
        return ftStatus;

    if ((ftStatus = upload(buf, rangeStart, rangeStart + writeSize)) != FT_OK)
        return ftStatus;

    _connectedAndReady = true;
    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::Unfreeze()
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;
    if ((ftStatus = sendGeckoCommand(cmd_unfreeze)) != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to unfreeze/start game\n";
#endif
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::SendCommand(const uint8_t cmd)
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    return sendGeckoCommand(cmd);
}

FT_STATUS MungPlex::USBGecko::findUsbGecko()
{
    DWORD numDevices = 0;
    FT_STATUS ftStatus;

    ftStatus = FT_CreateDeviceInfoList(&numDevices);

    if (ftStatus != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "No FTDI Devices found\n";
#endif
        return ftStatus;
    }

#ifndef NDEBUG
    std::cout << numDevices << " FTDI Devices found\n";
#endif

    for (DWORD i = 0; i < numDevices; ++i)
    {
        DWORD flags, type, id, locId;
        char serialNumber[16], description[64];
        ftStatus = FT_GetDeviceInfoDetail(i, &flags, &type, &id, &locId, serialNumber, description, &_ftdiHandle);
        
        if (ftStatus != FT_OK)
        {
#ifndef NDEBUG
            std::cerr << "Failed to get device info for device " << i << '\n';
#endif
            continue;
        }

        if (strcmp(serialNumber, _serialNumber.c_str()) == 0)
        {
#ifndef NDEBUG
            std::cout << "Found device with serial number " << _serialNumber << " at index " << i << '\n';
            std::cout << "Description: " << description << ", ID: " << id << ", Location ID: " << locId << '\n';
#endif
            _ftdiDeviceNumber = i;
            return ftStatus;
        }
    }

#ifndef NDEBUG
    std::cout << "Device with serial number " << _serialNumber << " not found\n";
#endif
    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::openUsbGecko()
{
    return FT_OpenEx(_serialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &_ftdiHandle);
}

FT_STATUS MungPlex::USBGecko::closeUsbGecko()
{
    return FT_Close(_ftdiHandle);
}

FT_STATUS MungPlex::USBGecko::setLatencyTimeouts(const uint64_t readTimeout, const uint64_t writeTimeout)
{
    return FT_SetTimeouts(_ftdiHandle, readTimeout, writeTimeout);
}

FT_STATUS MungPlex::USBGecko::setLatencyTimer(const uint8_t ucLatency)
{
    return FT_SetLatencyTimer(_ftdiHandle, ucLatency);
}

FT_STATUS MungPlex::USBGecko::setUsbParameters(const uint64_t inTransferSize, const uint64_t outTransferSize)
{
    return FT_SetUSBParameters(_ftdiHandle, inTransferSize, outTransferSize);
}

FT_STATUS MungPlex::USBGecko::sendGeckoCommand(uint8_t cmd)
{
    uint64_t bytesWritten = 0;
    FT_STATUS ftStatus = FT_Write(_ftdiHandle, reinterpret_cast<LPVOID>(&cmd), 1, reinterpret_cast<LPDWORD>(&bytesWritten));

    if (ftStatus != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to send Gecko command " << cmd << '\n';
#endif
        return ftStatus;
    }

    if (bytesWritten != 1)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to process Gecko command " << cmd << '\n';
#endif
        return FT_FAILED_TO_WRITE_DEVICE;
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::resetDevice()
{
    return FT_ResetDevice(_ftdiHandle);
}

FT_STATUS MungPlex::USBGecko::purge(const uint32_t mask)
{
    return FT_Purge(_ftdiHandle, mask);
}

FT_STATUS MungPlex::USBGecko::getCommandResponce(char* out)
{
    if (out == nullptr)
    {
#ifndef NDEBUG
        std::cout << "Error: out param is nullptr\n";
#endif
        return FT_INVALID_ARGS;
    }

    //Read reply - expcecting GCACK
    uint64_t bytesReceived = 0;
    FT_STATUS ftStatus = 0;

    for (int i = 0; i < 10; ++i)
    {
        ftStatus = geckoRead(out, 1, reinterpret_cast<LPDWORD>(&bytesReceived));

        if (*out == static_cast<char>(GCACK))
            return ftStatus;

        if (i == 9)
        {
#ifndef NDEBUG
            std::cout << "Error: Did not get reply\n";
#endif
            return ftStatus;
        }
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::geckoSetMemoryRegion(const uint32_t memoryStart, const uint32_t memoryEnd)
{
    static std::vector<char> memoryRange(8); //contains first and last address as big endian each
    *reinterpret_cast<uint32_t*>(memoryRange.data()) = std::byteswap(memoryStart);
    *reinterpret_cast<uint32_t*>(&memoryRange[4]) = std::byteswap(memoryEnd);

    static uint64_t bytesWritten = 0;
    FT_STATUS ftStatus = geckoWrite(memoryRange.data(), 8, reinterpret_cast<LPDWORD>(&bytesWritten));

    if(ftStatus != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Error: Failed to write memory range\n";
#endif
        return ftStatus;
    }

    if(bytesWritten != 8)
    {
#ifndef NDEBUG
        std::cout << "Error: Memory range write incomplete\n";
#endif
        return ftStatus;
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::geckoRead(char* buf, const uint64_t readSize, LPDWORD bytesReceived)
{
    if (bytesReceived == nullptr || buf == nullptr)
    {
#ifndef NDEBUG
        std::cout << "Error: nullptr in args\n";
#endif
        return FT_INVALID_PARAMETER;
    }

    FT_STATUS ftStatus = FT_Read(_ftdiHandle, buf, readSize, bytesReceived);

    if (ftStatus != FT_OK)
    {
#ifndef NDEBUG
        std::cout << "Failed to read\n";
#endif
        return ftStatus;
    }

    if (*bytesReceived == readSize)
    {
#ifndef NDEBUG
        std::cout << "Received " << *bytesReceived << " bytes\n";
#endif
    }
    else 
    {
#ifndef NDEBUG
        std::cout << "Failed to read all bytes\n";
#endif
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::geckoWrite(char* buf, const uint64_t writeSize, LPDWORD bytesWritten)
{
    if (bytesWritten == nullptr || buf == nullptr)
    {
#ifndef NDEBUG
        std::cout << "Error: nullptr in args\n";
#endif
        return FT_INVALID_PARAMETER;
    }

    FT_STATUS ftStatus = FT_Write(_ftdiHandle, buf, writeSize, bytesWritten);

    if (*bytesWritten == writeSize)
    {
#ifndef NDEBUG
        std::cout << *bytesWritten << " bytes written\n";
#endif
    }
    else
    {
#ifndef NDEBUG
        std::cout << "Failed to write all bytes\n";
#endif
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::dump(char* buf, const uint32_t memoryStart, const uint32_t memoryEnd)
{
    FT_STATUS ftStatus = sendGeckoCommand(cmd_readmem);
    char responce = 0;
    ftStatus = getCommandResponce(&responce);

    if (responce != static_cast<char>(GCACK))
        return ftStatus;

    ftStatus = geckoSetMemoryRegion(memoryStart, memoryEnd);

    if (ftStatus != FT_OK)
        return ftStatus;

    static std::vector<char> readBuffer(_packetSize);
    uint32_t dumpSize = memoryEnd - memoryStart;
    uint32_t fullChunksCount = dumpSize / _packetSize;
    uint32_t lastChunkSize = dumpSize % _packetSize;
    static uint64_t bytesReceived = 0;

    if (fullChunksCount == 0)
    {
        uint32_t retry = 0;

        while (retry < 3)
        {
            ftStatus = geckoRead(readBuffer.data(), lastChunkSize, reinterpret_cast<LPDWORD>(&bytesReceived));

            if (ftStatus == FT_OK)
                break;

            sendGeckoCommand(GCRETRY);
            ++retry;
        }

        if (ftStatus != FT_OK)
        {
            sendGeckoCommand(GCFAIL);
#ifndef NDEBUG
            std::cout << "Error: Too many tries\n";
#endif
            return FT_OTHER_ERROR;
        }

        std::memcpy(buf, readBuffer.data(), lastChunkSize);
        ftStatus = sendGeckoCommand(GCACK);
    }
    else
    {
        for (uint32_t currentChunk = 0; currentChunk <= fullChunksCount; ++currentChunk)
        {
            uint32_t retry = 0;

            while (retry < 3)
            {
                wait(10);

                if (currentChunk < fullChunksCount)
                    ftStatus = geckoRead(readBuffer.data(), _packetSize, reinterpret_cast<LPDWORD>(&bytesReceived));
                else
                    ftStatus = geckoRead(readBuffer.data(), lastChunkSize, reinterpret_cast<LPDWORD>(&bytesReceived));

                if (ftStatus == FT_OK)
                    break;

                ftStatus = sendGeckoCommand(GCRETRY);
                ++retry;
            }

            if (ftStatus != FT_OK)
            {
                sendGeckoCommand(GCFAIL);
#ifndef NDEBUG
                std::cout << "Error: Too many tries\n";
#endif
                return FT_OTHER_ERROR;
            }

            if (currentChunk < fullChunksCount)
                std::memcpy(buf + _packetSize * currentChunk, readBuffer.data(), _packetSize);
            else
                std::memcpy(buf + _packetSize * currentChunk, readBuffer.data(), lastChunkSize);

            ftStatus = sendGeckoCommand(GCACK);
        }
    }

    return ftStatus;
}

FT_STATUS MungPlex::USBGecko::upload(char* buf, const uint32_t memoryStart, const uint32_t memoryEnd)
{
    FT_STATUS ftStatus = sendGeckoCommand(cmd_upload);
    char responce = 0;
    ftStatus = getCommandResponce(&responce);

    if (responce != static_cast<char>(GCACK))
        return ftStatus;

    ftStatus = geckoSetMemoryRegion(memoryStart, memoryEnd);

    if (ftStatus != FT_OK)
        return ftStatus;

    static std::vector<char> writeBuffer(_uplPacketSize);
    uint32_t uploadSize = memoryEnd - memoryStart;
    uint32_t fullChunksCount = uploadSize / _uplPacketSize;
    uint32_t lastChunkSize = uploadSize % _uplPacketSize;
    static uint64_t bytesWritten = 0;

    if (fullChunksCount == 0)
    {
        uint32_t retry = 0;
        std::memcpy(writeBuffer.data(), buf, lastChunkSize);

        while (retry < 3)
        {
            ftStatus = geckoWrite(writeBuffer.data(), lastChunkSize, reinterpret_cast<LPDWORD>(&bytesWritten));

            if (ftStatus == FT_OK)
                break;

            ftStatus = sendGeckoCommand(GCRETRY);
            ++retry;
        }

        if (ftStatus != FT_OK)
        {
            sendGeckoCommand(GCFAIL);
#ifndef NDEBUG
            std::cout << "Error: Too many tries\n";
#endif
            return FT_OTHER_ERROR;
        }

        ftStatus = sendGeckoCommand(GCACK);
    }
    else
    {
        for (uint32_t currentChunk = 0; currentChunk <= fullChunksCount; ++currentChunk)
        {
            uint32_t retry = 0;

            if (currentChunk < fullChunksCount)
                std::memcpy(writeBuffer.data(), buf + _packetSize * currentChunk, _packetSize);
            else
                std::memcpy(writeBuffer.data(), buf + _packetSize * currentChunk, lastChunkSize);

            while (retry < 3)
            {
                wait(10);

                if (currentChunk < fullChunksCount)
                    ftStatus = geckoWrite(writeBuffer.data(), _packetSize, reinterpret_cast<LPDWORD>(&bytesWritten));
                else
                    ftStatus = geckoWrite(writeBuffer.data(), lastChunkSize, reinterpret_cast<LPDWORD>(&bytesWritten));

                if (ftStatus == FT_OK)
                    break;

                sendGeckoCommand(GCRETRY);
                ++retry;
            }

            if (ftStatus != FT_OK)
            {
                sendGeckoCommand(GCFAIL);
#ifndef NDEBUG
                std::cout << "Error: Too many tries\n";
#endif
                return FT_OTHER_ERROR;
            }

            ftStatus = sendGeckoCommand(GCACK);
            ftStatus = sendGeckoCommand(GCDONE);
        }
    }

    return ftStatus;
}

void MungPlex::USBGecko::wait(const int milliseconds) const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

MungPlex::USBGecko::RvlStatus MungPlex::USBGecko::GetCurrentStatus()
{
    boost::lock_guard<boost::mutex> lock(s_AccessMutex);
    static FT_STATUS ftStatus = 0;

    if ((ftStatus = Init()) != FT_OK)
        return RvlStatus::Unknown;

    if ((ftStatus = sendGeckoCommand(cmd_status)) != FT_OK)
        return RvlStatus::Unknown;
   
    wait(69);

    char result = RvlStatus::Unknown;
    uint64_t bytesReceived = 0;

    if ((ftStatus = geckoRead(&result, 1, reinterpret_cast<LPDWORD>(&bytesReceived))) != FT_OK)
        return RvlStatus::Unknown;

    switch (static_cast<uint32_t>(result))
    {
        case 0: return RvlStatus::Running;
        case 1: return RvlStatus::Paused;
        case 2: return RvlStatus::Breakpoint;
        case 3: return RvlStatus::Loader;
        default: return RvlStatus::Unknown;
    }
}

bool MungPlex::USBGecko::IsConnectedAndReady()
{
    return _connectedAndReady;
}
