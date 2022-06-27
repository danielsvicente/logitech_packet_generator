#pragma once

#include "IPrinter.hpp"
#include "Packet.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace Logi
{
    using PacketVariant = std::variant<StartDataTransferPacket, DataPacket, StopDataTransferPacket>;
    using Packets = std::vector<PacketVariant>;

    class PacketGenerator
    {
    public:
        static constexpr int s_maxDataBytes = 59;

        PacketGenerator(uint8_t softwareId, IPrinter& printer);
        PacketGenerator(uint8_t softwareId, Endianess endianess, IPrinter& printer);

        /// Creates packets for the input data.
        ///
        /// \param buffer The buffer containing data to be encoded.
        /// \param size The size of the buffer.
        /// \return The generated packets.
        Packets createPackets(const std::byte* buffer, size_t size, const EndPacketFlags& flags);
        
        /// Creates packets for the input data.
        ///
        /// \param buffer The buffer containing data to be encoded.
        /// \return The generated packets
        Packets createPackets(const std::vector<std::byte>& buffer, const EndPacketFlags& flags);

        /// Prints the input packets.
        ///
        /// \param packets The packets to be print.
        void printPackets(const Packets& packets);

    private:

        PacketHeader createPacket(PacketType type);
        StartDataTransferPacket createPacket(uint32_t totalPayloadSize);
        DataPacket createPacket(uint8_t payloadSize, const std::byte* data);
        StopDataTransferPacket createPacket(const EndPacketFlags& flags);
        void printPacket(const StartDataTransferPacket& packet);
        void printPacket(const DataPacket& packet);
        void printPacket(const StopDataTransferPacket& packet);
        void incrementSequenceId();

        uint8_t m_softwareId{0};
        uint16_t m_packetSequenceId{0};
        bool m_swapByteOrder{false};
        IPrinter& m_printer;
    };

} // namespace Logi