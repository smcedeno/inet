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

#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ieee8021q/Ieee8021qSocketCommandProcessor.h"
#include "inet/linklayer/ieee8021q/Ieee8021qCommand_m.h"

namespace inet {

Define_Module(Ieee8021qSocketCommandProcessor);

void Ieee8021qSocketCommandProcessor::initialize(int stage)
{
    PacketFlowBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
        socketTable = getModuleFromPar<Ieee8021qSocketTable>(par("socketTableModule"), this);
}

cGate *Ieee8021qSocketCommandProcessor::getRegistrationForwardingGate(cGate *gate)
{
    if (gate == outputGate)
        return inputGate;
    else if (gate == inputGate)
        return outputGate;
    else
        throw cRuntimeError("Unknown gate");
}

void Ieee8021qSocketCommandProcessor::handleMessage(cMessage *message)
{
    if (auto request = dynamic_cast<Request *>(message))
        handleCommand(request);
    else
        PacketFlowBase::handleMessage(message);
}

void Ieee8021qSocketCommandProcessor::handleCommand(Request *request)
{
    auto controlInfo = request->getControlInfo();
    if (auto bindCommand = dynamic_cast<Ieee8021qBindCommand *>(controlInfo)) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        socketTable->addSocket(socketId, bindCommand->getProtocol(), bindCommand->getVlanId(), bindCommand->getSteal());
        delete request;
    }
    else if (dynamic_cast<Ieee8021qCloseCommand *>(controlInfo) != nullptr) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        socketTable->removeSocket(socketId);
        delete request;
// TODO: move to Ieee8021qSocketPacketProcessor module into a listener on the Ieee8021qSocketTable
//        auto indication = new Indication("closed", ETHERNET_I_SOCKET_CLOSED);
//        auto ctrl = new Ieee8021qSocketClosedIndication();
//        indication->setControlInfo(ctrl);
//        indication->addTag<SocketInd>()->setSocketId(socketId);
//        send(indication, "cmdOut");
    }
    else if (dynamic_cast<Ieee8021qDestroyCommand *>(controlInfo) != nullptr) {
        int socketId = request->getTag<SocketReq>()->getSocketId();
        socketTable->removeSocket(socketId);
        delete request;
    }
    else
        throw cRuntimeError("Unknown request");
}

} // namespace inet

