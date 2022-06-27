#include "../src/PacketGenerator.hpp"
#include "../src/Packet.hpp"
#include "../src/Utils.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <vector>

using namespace Logi;

namespace{
    void hexDump(std::ostream &os, const void* ptr, unsigned int size)
    {
        unsigned char* buf = (unsigned char*) ptr;
        os << " [size=" << std::dec << size << "]:\n";
        while (size>0)
        {
            // offset for ascii; 49 bytes
            char line[128] = {};
            int i = 0;
            std::memset(line, ' ', sizeof(line));
            for (i = 0; i < 16 && size > 0; ++i)
            {
                std::sprintf(line + i*3, "%02x ", *buf);
                std::sprintf(line + 49 + i, "%c", isprint((char)*buf) ? (char)*buf : '.');
                ++buf;
                --size;
            }
            line[i*3] = ' ';
            os << "-- " << line << '\n';
        }
    }

    std::string hexDump(const void* buf_i, unsigned int bufsize)
    {
        std::ostringstream oss;
        hexDump(oss, buf_i, bufsize);
        return oss.str();
    }

    template<typename T>
    void dumpPacket(const T& packet, int size)
    {
        auto p = reinterpret_cast<unsigned char*>(packet);
        std::cout << "Packet " << std::to_integer<int>(packet->header.sequenceId_0);
        std::cout << hexDump(p, size);
    }
}

TEST_CASE("Create packets for 1-byte buffer")
{
    PacketGenerator generator{52};

    auto buffer = generateRandomBuffer(1); //1bytes
    REQUIRE(buffer.size() == 1);

    Logi::EndPacketFlags flags{true, true, true};
    auto packets = generator.createPackets(buffer, flags);

    // With buffer size 1 byte, function should return 3 packets
    CHECK(packets.size() == 3);

    // Check StartDataTransferPacket
    REQUIRE_NOTHROW(std::get<StartDataTransferPacket>(packets.at(0)));
    auto packet0 = std::get<StartDataTransferPacket>(packets.at(0));
    CHECK(packet0.header.softwareId == 0x34);
    CHECK(packet0.header.sequenceId_0 == 0x00);
    CHECK(packet0.header.sequenceId_1 == 0x00);
    CHECK(packet0.header.packetType == static_cast<uint8_t>(PacketType::StartDataTransfer));
    CHECK(packet0.totalPayloadSize_0 == 0x01);
    CHECK(packet0.totalPayloadSize_1 == 0x00);
    CHECK(packet0.totalPayloadSize_2 == 0x00);
    CHECK(packet0.totalPayloadSize_3 == 0x00);

    // Check DataPacket
    REQUIRE_NOTHROW(std::get<DataPacket>(packets.at(1)));
    auto packet1 = std::get<DataPacket>(packets.at(1));
    CHECK(packet1.header.softwareId == 0x34);
    CHECK(packet1.header.sequenceId_0 == 0x01);
    CHECK(packet1.header.sequenceId_1 == 0x00);
    CHECK(packet1.header.packetType == static_cast<uint8_t>(PacketType::Data));
    CHECK(packet1.payloadSize == 0x01);
    CHECK(packet1.data.size() == 1);
    CHECK(packet1.data.at(0) == buffer.at(0));

    // Check StopDataTransferPacket
    REQUIRE_NOTHROW(std::get<StopDataTransferPacket>(packets.at(2)));
    auto packet2 = std::get<StopDataTransferPacket>(packets.at(2));
    CHECK(packet2.header.softwareId == 0x34);
    CHECK(packet2.header.sequenceId_0 == 0x02);
    CHECK(packet2.header.sequenceId_1 == 0x00);
    CHECK(packet2.header.packetType == static_cast<uint8_t>(PacketType::StopDataTransfer));
    CHECK(packet2.flags == 0b00000111);
}

TEST_CASE("Create packets for 1K-byte buffer")
{
    PacketGenerator generator{255};

    auto buffer = generateRandomBuffer(1000); //1000bytes
    REQUIRE(buffer.size() == 1000);

    std::cout << "BUFFER UNDER TEST:\n";
    std::cout << hexDump(buffer.data(), buffer.size()) << "\n\n";

    Logi::EndPacketFlags flags{true, true, false};
    auto packets = generator.createPackets(buffer, flags);

    // With buffer size 1000 bytes, function should return 19 packets
    CHECK(packets.size() == 19);

    // Check first packet -> StartDataTransferPacket
    REQUIRE_NOTHROW(std::get<StartDataTransferPacket>(packets.at(0)));
    auto packet0 = std::get<StartDataTransferPacket>(packets.at(0));
    // dumpPacket(&packet0, 8);
    CHECK(packet0.header.packetType == static_cast<uint8_t>(PacketType::StartDataTransfer));
    CHECK(packet0.header.softwareId == 0xFF);
    CHECK(packet0.header.sequenceId_0 == 0x00);
    CHECK(packet0.header.sequenceId_1 == 0x00);
    CHECK(packet0.totalPayloadSize_0 == 0xE8);
    CHECK(packet0.totalPayloadSize_1 == 0x03);
    CHECK(packet0.totalPayloadSize_2 == 0x00);
    CHECK(packet0.totalPayloadSize_3 == 0x00);

    // Check DataPacket
    std::vector<std::byte> returnedPayload;
    for (size_t sequenceId = 1; sequenceId < packets.size() - 2; sequenceId++)
    {
        REQUIRE_NOTHROW(std::get<DataPacket>(packets.at(sequenceId)));
        auto packetData = std::get<DataPacket>(packets.at(sequenceId));
        // dumpPacket(&packetData, 64);
        CHECK(packetData.header.softwareId == 0xFF);
        CHECK(packetData.header.sequenceId_0 == sequenceId);
        CHECK(packetData.header.sequenceId_1 == 0x00);
        CHECK(packetData.header.packetType == static_cast<uint8_t>(PacketType::Data));
        CHECK(packetData.payloadSize == 59);
        CHECK(packetData.data.size() == 59);
        returnedPayload.insert(returnedPayload.end(), packetData.data.begin(), packetData.data.end());
    }    

    // Check last DataPacket
    REQUIRE_NOTHROW(std::get<DataPacket>(packets.at(17)));
    auto packetData = std::get<DataPacket>(packets.at(17));
    // dumpPacket(&packetData, 61);
    CHECK(packetData.header.softwareId == 0xFF);
    CHECK(packetData.header.sequenceId_0 == 17);
    CHECK(packetData.header.sequenceId_1 == 0x00);
    CHECK(packetData.header.packetType == static_cast<uint8_t>(PacketType::Data));
    CHECK(packetData.payloadSize == 56);
    CHECK(packetData.data.size() == 56);
    returnedPayload.insert(returnedPayload.end(), packetData.data.begin(), packetData.data.end());

    CHECK (returnedPayload == buffer);

    // Check StopDataTransferPacket
    REQUIRE_NOTHROW(std::get<StopDataTransferPacket>(packets.at(18)));
    auto packet18 = std::get<StopDataTransferPacket>(packets.at(18));
    // dumpPacket(&packetData, 61);
    CHECK(packet18.header.softwareId == 0xFF);
    CHECK(packet18.header.sequenceId_0 == 0x12);
    CHECK(packet18.header.sequenceId_1 == 0x00);
    CHECK(packet18.header.packetType == static_cast<uint8_t>(PacketType::StopDataTransfer));
    CHECK(packet18.flags == 0b00000110);

}

TEST_CASE("Generator swaps bytes if host machine is big endian")
{
    // PacketGenerator is set to LittleEndian by default, 
    // as per requirement ("All data should be encoded in littleEndian") and
    // the tests above already validate it.
    // In order to test that PacketGenerator can swap bytes in case the host machine
    // is big endian, we can invert the requirement, since we cannot change the
    // host endianess.

    auto generator = std::make_unique<PacketGenerator>(1, Endianess::BigEndian);

    auto buffer = generateRandomBuffer(291); // 291 bytes
    REQUIRE(buffer.size() == 291);

    auto packets = generator->createPackets(buffer, {});

    // With buffer size 291 bytes, function should return 7 packets
    CHECK(packets.size() == 7);

    // Check first packet -> StartDataTransferPacket
    REQUIRE_NOTHROW(std::get<StartDataTransferPacket>(packets.at(0)));
    auto packet0 = std::get<StartDataTransferPacket>(packets.at(0));
    CHECK(packet0.header.sequenceId_0 == 0x00);
    CHECK(packet0.header.sequenceId_0 == 0x00);
    CHECK(packet0.totalPayloadSize_0 == 0x00);
    CHECK(packet0.totalPayloadSize_1 == 0x00);
    CHECK(packet0.totalPayloadSize_2 == 0x01);
    CHECK(packet0.totalPayloadSize_3 == 0x23);

    // Check DataPacket
    REQUIRE_NOTHROW(std::get<DataPacket>(packets.at(1)));
    auto packet1 = std::get<DataPacket>(packets.at(1));
    CHECK(packet1.header.sequenceId_0 == 0x00);
    CHECK(packet1.header.sequenceId_1 == 0x01);

    // Check StopDataTransferPacket
    REQUIRE_NOTHROW(std::get<StopDataTransferPacket>(packets.at(6)));
    auto packet6 = std::get<StopDataTransferPacket>(packets.at(6));
    CHECK(packet6.header.sequenceId_0 == 0x00);
    CHECK(packet6.header.sequenceId_1 == 0x06);



}

// TEST_CASE("Print packets")
// {
//     CHECK(false);
// }