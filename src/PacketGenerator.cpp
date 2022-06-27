#include "PacketGenerator.hpp"
#include <iostream>

namespace Logi
{
    namespace{
        auto swapByteOrder = [](Endianess a, Endianess b) -> bool
        {
            return static_cast<bool>(a) || static_cast<bool>(b);
        };
    }

    PacketGenerator::PacketGenerator(uint8_t softwareId) 
        : PacketGenerator(softwareId, Endianess::LittleEndian)
    {}

    PacketGenerator::PacketGenerator(uint8_t softwareId, Endianess endianess) 
        : m_softwareId{softwareId} 
        , m_swapByteOrder{swapByteOrder(checkHostEndianess(), endianess)}
    {}

    Packets PacketGenerator::createPackets(const std::byte* buffer, size_t size, const EndPacketFlags& flags)
    {
        Packets packets;

        packets.emplace_back(createPacket(m_packetSequenceId++, size));  // start transfer packet

        auto remainingBytes = size;
        for (size_t offset = 0; offset < size; offset += 59)
        {   
            if (remainingBytes > s_maxDataBytes) 
            {
                packets.emplace_back(createPacket(m_packetSequenceId++, s_maxDataBytes, buffer + offset)); // data packet
                remainingBytes -= s_maxDataBytes;
            }
            else
            {
                packets.emplace_back(createPacket(m_packetSequenceId++, remainingBytes, buffer + offset)); // data packet
            }
        }

        packets.emplace_back(createPacket(m_packetSequenceId++, flags));  // end transfer packet
        
        return packets;
    }
        
    Packets PacketGenerator::createPackets(const std::vector<std::byte>& buffer, const EndPacketFlags& flags)
    {
        return createPackets(buffer.data(), buffer.size(), flags);
    }

    PacketHeader PacketGenerator::createPacket(PacketType type, uint16_t sequenceId)
    {
        PacketHeader packet;
        packet.softwareId = m_softwareId;
        packet.packetType = static_cast<uint8_t>(type);

        if (m_swapByteOrder) 
        {
            packet.sequenceId_0 = sequenceId >> 8;
            packet.sequenceId_1 = sequenceId & 0xFF;
        }
        else
        {
            packet.sequenceId_0 = sequenceId & 0xFF;
            packet.sequenceId_1 = sequenceId >> 8;
        }

        return packet;
    }


    StartDataTransferPacket PacketGenerator::createPacket(uint16_t sequenceId, uint32_t totalPayloadSize)
    {
        StartDataTransferPacket packet;
        packet.header = createPacket(PacketType::StartDataTransfer, sequenceId);
        
        if (m_swapByteOrder) 
        {
            packet.totalPayloadSize_0 = totalPayloadSize >> 24;
            packet.totalPayloadSize_1 = totalPayloadSize >> 16;
            packet.totalPayloadSize_2 = totalPayloadSize >> 8;
            packet.totalPayloadSize_3 = totalPayloadSize & 0xFF;
        }
        else
        {
            packet.totalPayloadSize_0 = totalPayloadSize & 0xFF;
            packet.totalPayloadSize_1 = totalPayloadSize >> 8;
            packet.totalPayloadSize_2 = totalPayloadSize >> 16;
            packet.totalPayloadSize_3 = totalPayloadSize >> 24;
        }

        return packet;
    }

    DataPacket PacketGenerator::createPacket(uint16_t sequenceId, uint8_t payloadSize, const std::byte* data)
    {
        DataPacket packet;
        packet.header = createPacket(PacketType::Data, sequenceId);
        packet.payloadSize = payloadSize;
        packet.data.insert(packet.data.begin(), &data[0], &data[payloadSize]);
        return packet;
    }

    StopDataTransferPacket PacketGenerator::createPacket(uint16_t sequenceId, const EndPacketFlags& flags)
    {
        StopDataTransferPacket packet;
        packet.header = createPacket(PacketType::StopDataTransfer, sequenceId);
        if (flags.reboot) packet.flags |= 1 << 0;
        if (flags.verify) packet.flags |= 1 << 1;
        if (flags.test)   packet.flags |= 1 << 2;
        return packet;
    }

    void PacketGenerator::printPackets()
    {
        
    };


}