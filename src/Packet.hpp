#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <vector>

namespace Logi
{
    enum class PacketType
    {
        StartDataTransfer = 1,
        Data              = 2,
        StopDataTransfer  = 3
    };

    struct EndPacketFlags
    {
        bool test{};
        bool verify{};
        bool reboot{};
    };

    struct PacketHeader
    {
        uint8_t softwareId{};
        uint8_t sequenceId_0{};
        uint8_t sequenceId_1{};
        uint8_t packetType{};
    };

    struct StartDataTransferPacket
    {
        PacketHeader header;
        uint8_t totalPayloadSize_0{};
        uint8_t totalPayloadSize_1{};
        uint8_t totalPayloadSize_2{};
        uint8_t totalPayloadSize_3{};
    };

    struct DataPacket
    {
        PacketHeader header;
        uint8_t payloadSize{};
        std::vector<std::byte> data;
    };

    struct StopDataTransferPacket
    {
        PacketHeader header;
        uint8_t flags{};
    };  

} // namespace Logi