//
// Copyright (C) 2020 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "inet/common/packet/serializer/ChunkSerializerRegistry.h"
#include "inet/linklayer/ethernet/Ethernet.h"
#include "inet/physicallayer/ethernet/EthernetPhyHeader_m.h"
#include "inet/physicallayer/ethernet/EthernetPhyHeaderSerializer.h"

namespace inet {

namespace physicallayer {

Register_Serializer(EthernetPhyHeaderBase, EthernetPhyHeaderBaseSerializer);
Register_Serializer(EthernetPhyHeader, EthernetPhyHeaderSerializer);
Register_Serializer(EthernetFragmentPhyHeader, EthernetFragmentPhyHeaderSerializer);

void EthernetPhyHeaderBaseSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const
{
    throw cRuntimeError("Invalid operation");
}

const Ptr<Chunk> EthernetPhyHeaderBaseSerializer::deserialize(MemoryInputStream& stream) const
{
    uint8_t byte = stream.getData().at(B(PREAMBLE_BYTES).get());
    if (byte == 0xD5) {
        EthernetPhyHeaderSerializer serializer;
        return serializer.deserialize(stream);
    }
    else {
        EthernetFragmentPhyHeaderSerializer serializer;
        return serializer.deserialize(stream);
    }
}

void EthernetPhyHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const
{
    stream.writeByteRepeatedly(0x55, B(PREAMBLE_BYTES).get()); // preamble
    stream.writeByte(0xD5); // SFD
}

const Ptr<Chunk> EthernetPhyHeaderSerializer::deserialize(MemoryInputStream& stream) const
{
    auto header = makeShared<EthernetPhyHeader>();
    bool preambleReadSuccessfully = stream.readByteRepeatedly(0x55, B(PREAMBLE_BYTES).get()); // preamble
    uint8_t sfd = stream.readByte();
    if (!preambleReadSuccessfully || sfd != 0xD5) {
        header->markIncorrect();
        header->markImproperlyRepresented();
    }
    return header;
}

void EthernetFragmentPhyHeaderSerializer::serialize(MemoryOutputStream& stream, const Ptr<const Chunk>& chunk) const
{
    const auto& header = staticPtrCast<const EthernetFragmentPhyHeader>(chunk);
    stream.writeByteRepeatedly(0x55, B(PREAMBLE_BYTES).get() - (header->getPreambleType() == SMD_Cx ? 1 : 0));
    switch (header->getPreambleType()) {
        case SFD:
            stream.writeByte(0xD5);
            break;
        case SMD_Verify:
            stream.writeByte(0x07);
            break;
        case SMD_Respond:
            stream.writeByte(0x19);
            break;
        case SMD_Sx: {
            int smdSxValues[] = {0xE6, 0x4C, 0x7F, 0xB3};
            stream.writeByte(smdSxValues[header->getSmdNumber()]);
            break;
        }
        case SMD_Cx: {
            int smdCxValues[] = {0x61, 0x52, 0x9E, 0x2A};
            stream.writeByte(smdCxValues[header->getSmdNumber()]);
            int fragmentNumberValues[] = {0xE6, 0x4C, 0x7F, 0xB3};
            stream.writeByte(fragmentNumberValues[header->getFragmentNumber()]);
            break;
        }
    }
}

const Ptr<Chunk> EthernetFragmentPhyHeaderSerializer::deserialize(MemoryInputStream& stream) const
{
    auto header = makeShared<EthernetFragmentPhyHeader>();
    bool preambleReadSuccessfully = stream.readByteRepeatedly(0x55, B(PREAMBLE_BYTES).get() - 1);
    if (!preambleReadSuccessfully)
        header->markIncorrect();
    uint8_t value = stream.readByte();
    if (value == 0x55) {
        uint8_t value = stream.readByte();
        if (value == 0xD5)
            header->setPreambleType(SFD);
        else {
            header->setPreambleType(SMD_Sx);
            int smdSxValues[] = {0xE6, 0x4C, 0x7F, 0xB3};
            int smdNumber = std::distance(smdSxValues, std::find(smdSxValues, smdSxValues + 4, value));
            header->setSmdNumber(smdNumber);
            header->setFragmentNumber(0);
        }
    }
    else {
        header->setPreambleType(SMD_Cx);
        uint8_t value = stream.readByte();
        int smdCxValues[] = {0x61, 0x52, 0x9E, 0x2A};
        int smdNumber = std::distance(smdCxValues, std::find(smdCxValues, smdCxValues + 4, value));
        header->setSmdNumber(smdNumber);
        int fragmentNumberValues[] = {0xE6, 0x4C, 0x7F, 0xB3};
        int fragmentNumber = std::distance(fragmentNumberValues, std::find(fragmentNumberValues, fragmentNumberValues + 4, value));
        header->setFragmentNumber(fragmentNumber);
    }
    return header;
}

} // namespace physicallayer

} // namespace inet

