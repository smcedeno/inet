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

#ifndef __INET_ETHERNETSOCKETPACKETPROCESSOR_H
#define __INET_ETHERNETSOCKETPACKETPROCESSOR_H

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/linklayer/ethernet/layered/EthernetSocketTable.h"
#include "inet/queueing/base/PacketPusherBase.h"

namespace inet {

class INET_API EthernetSocketPacketProcessor : public queueing::PacketPusherBase, public TransparentProtocolRegistrationListener
{
  protected:
    EthernetSocketTable *socketTable = nullptr;

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual void pushPacket(Packet *packet, cGate *gate) override;
    virtual cGate *getRegistrationForwardingGate(cGate *gate) override;
};

} // namespace inet

#endif

