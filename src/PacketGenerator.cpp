#include "PacketGenerator.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace Logi
{
    namespace{
        auto matchEndianess = [](Endianess a, Endianess b) -> bool
        {
            return static_cast<bool>(a) || static_cast<bool>(b);
        };

        std::ostream& formatHeader(std::ostream& os, const PacketHeader& header, uint16_t sequenceId)
        {
            auto convert = [](PacketType type) -> std::string {
                switch (type)
                {
                case PacketType::Data:              return "Data";
                case PacketType::StartDataTransfer: return "StartDataTransfer";
                case PacketType::StopDataTransfer:  return "StopDataTransfer";
                }
            };

            os << "software id: " << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << +header.softwareId << "\n";
            os << "sequence id: " << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << sequenceId << "\n";
            os << "packet type: " << convert(static_cast<PacketType>(header.packetType)) << "\n";
            return os;
        };
    }

    PacketGenerator::PacketGenerator(uint8_t softwareId, IPrinter& printer) 
        : PacketGenerator(softwareId, Endianess::LittleEndian, printer)
    {}

    PacketGenerator::PacketGenerator(uint8_t softwareId, Endianess endianess, IPrinter& printer) 
        : m_softwareId{softwareId} 
        , m_swapByteOrder{matchEndianess(checkHostEndianess(), endianess)}
        , m_printer{printer}
    {}

    Packets PacketGenerator::createPackets(const std::byte* buffer, size_t size, const EndPacketFlags& flags)
    {
        Packets packets;

        packets.emplace_back(createPacket(size));  // start transfer packet

        auto remainingBytes = size;
        for (size_t offset = 0; offset < size; offset += 59)
        {   
            if (remainingBytes > s_maxDataBytes) 
            {
                packets.emplace_back(createPacket(s_maxDataBytes, buffer + offset)); // data packet
                remainingBytes -= s_maxDataBytes;
            }
            else
            {
                packets.emplace_back(createPacket(remainingBytes, buffer + offset)); // data packet
            }
        }

        packets.emplace_back(createPacket(flags));  // end transfer packet
        
        return packets;
    }
        
    Packets PacketGenerator::createPackets(const std::vector<std::byte>& buffer, const EndPacketFlags& flags)
    {
        return createPackets(buffer.data(), buffer.size(), flags);
    }

    PacketHeader PacketGenerator::createPacket(PacketType type)
    {
        PacketHeader packet;
        packet.softwareId = m_softwareId;
        packet.packetType = static_cast<uint8_t>(type);

        if (m_swapByteOrder) 
        {
            packet.sequenceId_0 = m_packetSequenceId >> 8;
            packet.sequenceId_1 = m_packetSequenceId & 0xFF;
        }
        else
        {
            packet.sequenceId_0 = m_packetSequenceId & 0xFF;
            packet.sequenceId_1 = m_packetSequenceId >> 8;
        }

        incrementSequenceId();

        return packet;
    }


    StartDataTransferPacket PacketGenerator::createPacket(uint32_t totalPayloadSize)
    {
        StartDataTransferPacket packet;
        packet.header = createPacket(PacketType::StartDataTransfer);
        
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

    DataPacket PacketGenerator::createPacket(uint8_t payloadSize, const std::byte* data)
    {
        DataPacket packet;
        packet.header = createPacket(PacketType::Data);
        packet.payloadSize = payloadSize;
        packet.data.insert(packet.data.begin(), &data[0], &data[payloadSize]);
        return packet;
    }

    StopDataTransferPacket PacketGenerator::createPacket(const EndPacketFlags& flags)
    {
        StopDataTransferPacket packet;
        packet.header = createPacket(PacketType::StopDataTransfer);
        if (flags.reboot) packet.flags |= 1 << 0;
        if (flags.verify) packet.flags |= 1 << 1;
        if (flags.test)   packet.flags |= 1 << 2;
        return packet;
    }

    void PacketGenerator::printPackets(const Packets& packets)
    {
        for (const auto& packet : packets)
        {
            std::visit(overloaded {
                [this](const DataPacket& packet)              { printPacket(packet); },
                [this](const StartDataTransferPacket& packet) { printPacket(packet); },
                [this](const StopDataTransferPacket& packet)  { printPacket(packet); }
            }, packet);
        }
    };

    void PacketGenerator::printPacket(const StartDataTransferPacket& packet)
    {
        auto sequenceId = readField16(packet.header.sequenceId_0, packet.header.sequenceId_1, m_swapByteOrder);
        auto totalSize = readField32(
            packet.totalPayloadSize_0,
            packet.totalPayloadSize_1,
            packet.totalPayloadSize_2,
            packet.totalPayloadSize_3,
            m_swapByteOrder
        );

        std::ostringstream oss;
        formatHeader(oss, packet.header, sequenceId);
        oss << "total payload size: " << std::dec << +totalSize << "\n";
        m_printer.print(oss.str());
    }

    void PacketGenerator::printPacket(const DataPacket& packet)
    {
        auto sequenceId = readField16(packet.header.sequenceId_0, packet.header.sequenceId_1, m_swapByteOrder);

        std::ostringstream oss;
        formatHeader(oss, packet.header, sequenceId);
        oss << "payload size: " << std::dec << +packet.payloadSize << "\n";
        m_printer.print(oss.str());
    }
    
    void PacketGenerator::printPacket(const StopDataTransferPacket& packet)
    {
        auto sequenceId = readField16(packet.header.sequenceId_0, packet.header.sequenceId_1, m_swapByteOrder);

        std::ostringstream oss;
        formatHeader(oss, packet.header, sequenceId);
        oss << "test: " << (readBit(packet.flags, 2) ? "true" : "false") << "\n";
        oss << "verify: " << (readBit(packet.flags, 1) ? "true" : "false") << "\n";
        oss << "reboot: " << (readBit(packet.flags, 0) ? "true" : "false") << "\n";
        m_printer.print(oss.str());
    }

    void PacketGenerator::incrementSequenceId()
    {
        if (m_packetSequenceId == 0xFFFF) 
            m_packetSequenceId = 0;
        else
            m_packetSequenceId++;
    }


}