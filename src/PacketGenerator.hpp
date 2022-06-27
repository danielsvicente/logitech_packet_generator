#pragma once

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

        PacketGenerator(uint8_t softwareId);
        PacketGenerator(uint8_t softwareId, Endianess endianess);

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

        void printPackets();

    private:

        PacketHeader createPacket(PacketType type, uint16_t sequenceId);

        /// Encodes data into a StartDataTransferPacket.
        ///
        /// \param softwareId ....
        /// \param sequenceId ....
        /// \param totalPayloadSize ....
        /// \return The packet of type StartDataTransferPacket.
        StartDataTransferPacket createPacket(uint16_t sequenceId, uint32_t totalPayloadSize);

        /// Encodes data into a DataPacket.
        ///
        /// \param softwareId ....
        /// \param sequenceId ....
        /// \param payloadSize ....
        /// \param data ....
        /// \return The packet of type DataPacket.
        DataPacket createPacket(uint16_t sequenceId, uint8_t payloadSize, const std::byte* data);

        /// Encodes data into a StopDataTransferPacket.
        ///
        /// \param softwareId ....
        /// \param sequenceId ....
        /// \param flags ....
        /// \return The packet of type StopDataTransferPacket.
        StopDataTransferPacket createPacket(uint16_t sequenceId, const EndPacketFlags& flags);

        uint8_t m_softwareId{0};
        uint16_t m_packetSequenceId{0};
        bool m_swapByteOrder{false};
    };

} // namespace Logi