//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MACAddressTag_m.h"
#include "inet/linklayer/common/UserPriorityTag_m.h"
#include "inet/linklayer/ieee80211/mac/contract/IContention.h"
#include "inet/linklayer/ieee80211/mac/contract/IFrameSequence.h"
#include "inet/linklayer/ieee80211/mac/contract/IRx.h"
#include "inet/linklayer/ieee80211/mac/contract/ITx.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Mac.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211SubtypeTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/physicallayer/ieee80211/packetlevel/Ieee80211ControlInfo_m.h"

namespace inet {
namespace ieee80211 {

Define_Module(Ieee80211Mac);

simsignal_t Ieee80211Mac::stateSignal = SIMSIGNAL_NULL;
simsignal_t Ieee80211Mac::radioStateSignal = SIMSIGNAL_NULL;

Ieee80211Mac::Ieee80211Mac()
{
}

Ieee80211Mac::~Ieee80211Mac()
{
    if (pendingRadioConfigMsg)
        delete pendingRadioConfigMsg;
}

void Ieee80211Mac::initialize(int stage)
{
    MACProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        mib = getModuleFromPar<Ieee80211Mib>(par("mibModule"), this);
        mib->qos = par("qosStation");
        cModule *radioModule = gate("lowerLayerOut")->getNextGate()->getOwnerModule();
        radioModule->subscribe(IRadio::radioModeChangedSignal, this);
        radioModule->subscribe(IRadio::receptionStateChangedSignal, this);
        radioModule->subscribe(IRadio::transmissionStateChangedSignal, this);
        radioModule->subscribe(IRadio::receivedSignalPartChangedSignal, this);
        radio = check_and_cast<IRadio *>(radioModule);
        ds = check_and_cast<IDs *>(getSubmodule("ds"));
        rx = check_and_cast<IRx *>(getSubmodule("rx"));
        tx = check_and_cast<ITx *>(getSubmodule("tx"));
        const char *addressString = par("address");
        if (!strcmp(addressString, "auto")) {
            // change module parameter from "auto" to concrete address
            par("address").setStringValue(MACAddress::generateAutoAddress().str().c_str());
            addressString = par("address");
        }
        mib->address.setAddress(addressString);
        modeSet = Ieee80211ModeSet::getModeSet(par("modeSet").stringValue());
        const char *fcsModeString = par("fcsMode");
        if (!strcmp(fcsModeString, "declared"))
            fcsMode = FCS_DECLARED;
        else if (!strcmp(fcsModeString, "computed"))
            fcsMode = FCS_COMPUTED;
        else
            throw cRuntimeError("Unknown fcs mode");
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        registerInterface();
        emit(NF_MODESET_CHANGED, modeSet);
        if (isOperational)
            radio->setRadioMode(IRadio::RADIO_MODE_RECEIVER);
        if (isInterfaceRegistered().isUnspecified())// TODO: do we need multi-MAC feature? if so, should they share interfaceEntry??  --Andras
            registerInterface();
    }
    else if (stage == INITSTAGE_LINK_LAYER_2) {
        rx = check_and_cast<IRx *>(getSubmodule("rx"));
        tx = check_and_cast<ITx *>(getSubmodule("tx"));
        dcf = check_and_cast<Dcf *>(getSubmodule("dcf"));
        hcf = check_and_cast<Hcf *>(getSubmodule("hcf"));
    }
}

const MACAddress& Ieee80211Mac::isInterfaceRegistered()
{
    // if (!par("multiMac").boolValue())
    //    return MACAddress::UNSPECIFIED_ADDRESS;
    IInterfaceTable *ift = findModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (!ift)
        return MACAddress::UNSPECIFIED_ADDRESS;
    cModule *interfaceModule = findModuleUnderContainingNode(this);
    if (!interfaceModule)
        throw cRuntimeError("NIC module not found in the host");
    std::string interfaceName = utils::stripnonalnum(interfaceModule->getFullName());
    InterfaceEntry *e = ift->getInterfaceByName(interfaceName.c_str());
    if (e)
        return e->getMacAddress();
    return MACAddress::UNSPECIFIED_ADDRESS;
}

InterfaceEntry *Ieee80211Mac::createInterfaceEntry()
{
    InterfaceEntry *e = new InterfaceEntry(this);
    // address
    e->setMACAddress(mib->address);
    e->setInterfaceToken(mib->address.formInterfaceIdentifier());
    e->setMtu(par("mtu").longValue());
    // capabilities
    e->setBroadcast(true);
    e->setMulticast(true);
    e->setPointToPoint(false);
    return e;
}

void Ieee80211Mac::handleMessageWhenUp(cMessage *message)
{
    if (message->arrivedOn("mgmtIn")) {
        if (!message->isPacket())
            handleUpperCommand(message);
        else
            handleMgmtPacket(check_and_cast<Packet *>(message));
    }
    else
        LayeredProtocolBase::handleMessageWhenUp(message);
}

void Ieee80211Mac::handleSelfMessage(cMessage *msg)
{
    ASSERT(false);
}

void Ieee80211Mac::handleMgmtPacket(Packet *packet)
{
    const auto& header = std::make_shared<Ieee80211MgmtHeader>();
    header->setType(packet->getTag<Ieee80211SubtypeReq>()->getSubtype());
    header->setReceiverAddress(packet->getMandatoryTag<MacAddressReq>()->getDestAddress());
    if (mib->mode == Ieee80211Mib::INFRASTRUCTURE && mib->bssStationData.stationType == Ieee80211Mib::ACCESS_POINT)
        header->setAddress3(mib->bssData.bssid);
    packet->insertHeader(header);
    const auto& trailer = std::make_shared<Ieee80211MacTrailer>();
    trailer->setFcsMode(fcsMode);
    packet->insertTrailer(trailer);
    processUpperFrame(packet, header);
}

void Ieee80211Mac::handleUpperPacket(Packet *msg)
{
    auto packet = check_and_cast<Packet *>(msg);
    if (mib->mode == Ieee80211Mib::INFRASTRUCTURE && mib->bssStationData.stationType == Ieee80211Mib::STATION && !mib->bssStationData.isAssociated) {
        EV << "STA is not associated with an access point, discarding packet " << msg << "\n";
        delete msg;
        return;
    }
    encapsulate(packet);
    const auto& header = packet->peekHeader<Ieee80211DataOrMgmtHeader>();
    if (mib->mode == Ieee80211Mib::INFRASTRUCTURE && mib->bssStationData.stationType == Ieee80211Mib::ACCESS_POINT) {
        auto receiverAddress = header->getReceiverAddress();
        if (!receiverAddress.isMulticast()) {
            auto it = mib->bssAccessPointData.stations.find(receiverAddress);
            if (it == mib->bssAccessPointData.stations.end() || it->second != Ieee80211Mib::ASSOCIATED) {
                EV << "STA with MAC address " << receiverAddress << " not associated with this AP, dropping frame\n";
                delete packet;
                return;
            }
        }
    }
    processUpperFrame(packet, header);
}

void Ieee80211Mac::handleLowerPacket(Packet *msg)
{
    auto packet = check_and_cast<Packet *>(msg);
    auto header = packet->peekHeader<Ieee80211MacHeader>();
    if (rx->lowerFrameReceived(packet, header)) {
        processLowerFrame(packet, header);
    }
    else { // corrupted frame received
        if (mib->qos)
            hcf->corruptedFrameReceived();
        else
            dcf->corruptedFrameReceived();
    }
}

void Ieee80211Mac::handleUpperCommand(cMessage *msg)
{
    if (msg->getKind() == RADIO_C_CONFIGURE) {
        EV_DEBUG << "Passing on command " << msg->getName() << " to physical layer\n";
        if (pendingRadioConfigMsg != nullptr) {
            // merge contents of the old command into the new one, then delete it
            Ieee80211ConfigureRadioCommand *oldConfigureCommand = check_and_cast<Ieee80211ConfigureRadioCommand *>(pendingRadioConfigMsg->getControlInfo());
            Ieee80211ConfigureRadioCommand *newConfigureCommand = check_and_cast<Ieee80211ConfigureRadioCommand *>(msg->getControlInfo());
            if (newConfigureCommand->getChannelNumber() == -1 && oldConfigureCommand->getChannelNumber() != -1)
                newConfigureCommand->setChannelNumber(oldConfigureCommand->getChannelNumber());
            if (std::isnan(newConfigureCommand->getBitrate().get()) && !std::isnan(oldConfigureCommand->getBitrate().get()))
                newConfigureCommand->setBitrate(oldConfigureCommand->getBitrate());
            delete pendingRadioConfigMsg;
            pendingRadioConfigMsg = nullptr;
        }

        if (rx->isMediumFree()) {    // TODO: this should be just the physical channel sense!!!!
            EV_DEBUG << "Sending it down immediately\n";
            // PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(msg->getControlInfo());
            // if (phyControlInfo)
            // phyControlInfo->setAdaptiveSensitivity(true);
            // end dynamic power
            sendDown(msg);
        }
        else {
            // TODO: waiting potentially indefinitely?! wtf?!
            EV_DEBUG << "Delaying " << msg->getName() << " until next IDLE or DEFER state\n";
            pendingRadioConfigMsg = msg;
        }
    }
    else {
        throw cRuntimeError("Unrecognized command from mgmt layer: (%s)%s msgkind=%d", msg->getClassName(), msg->getName(), msg->getKind());
    }
}

void Ieee80211Mac::encapsulate(Packet *packet)
{
    auto macAddressReq = packet->getMandatoryTag<MacAddressReq>();
    auto destAddress = macAddressReq->getDestAddress();
    const auto& header = std::make_shared<Ieee80211DataHeader>();
    if (mib->mode == Ieee80211Mib::INDEPENDENT)
        header->setReceiverAddress(destAddress);
    else if (mib->mode == Ieee80211Mib::INFRASTRUCTURE) {
        if (mib->bssStationData.stationType == Ieee80211Mib::ACCESS_POINT) {
            header->setFromDS(true);
            header->setAddress3(mib->address);
            header->setReceiverAddress(destAddress);
        }
        else if (mib->bssStationData.stationType == Ieee80211Mib::STATION) {
            header->setToDS(true);
            header->setReceiverAddress(mib->bssData.bssid);
            header->setAddress3(destAddress);
        }
        else
            throw cRuntimeError("Unknown station type");
    }
    else
        throw cRuntimeError("Unknown mode");
    if (auto userPriorityReq = packet->getTag<UserPriorityReq>()) {
        // make it a QoS frame, and set TID
        header->setType(ST_DATA_WITH_QOS);
        header->setChunkLength(header->getChunkLength() + bit(QOSCONTROL_BITS));
        header->setTid(userPriorityReq->getUserPriority());
    }
    packet->insertHeader(header);
    const auto& trailer = std::make_shared<Ieee80211MacTrailer>();
    trailer->setFcsMode(fcsMode);
    packet->insertTrailer(trailer);
}

void Ieee80211Mac::decapsulate(Packet *packet)
{
    const auto& header = packet->popHeader<Ieee80211DataOrMgmtHeader>();
    auto macAddressInd = packet->ensureTag<MacAddressInd>();
    if (mib->mode == Ieee80211Mib::INDEPENDENT) {
        macAddressInd->setSrcAddress(header->getTransmitterAddress());
        macAddressInd->setDestAddress(header->getReceiverAddress());
    }
    else if (mib->mode == Ieee80211Mib::INFRASTRUCTURE) {
        if (mib->bssStationData.stationType == Ieee80211Mib::ACCESS_POINT) {
            macAddressInd->setSrcAddress(header->getTransmitterAddress());
            macAddressInd->setDestAddress(header->getAddress3());
        }
        else if (mib->bssStationData.stationType == Ieee80211Mib::STATION) {
            macAddressInd->setSrcAddress(header->getAddress3());
            macAddressInd->setDestAddress(header->getReceiverAddress());
        }
        else
            throw cRuntimeError("Unknown station type");
    }
    else
        throw cRuntimeError("Unknown mode");
    if (header->getType() == ST_DATA_WITH_QOS) {
        auto dataHeader = std::dynamic_pointer_cast<const Ieee80211DataHeader>(header);
        int tid = dataHeader->getTid();
        if (tid < 8)
            packet->ensureTag<UserPriorityInd>()->setUserPriority(tid);
    }
    packet->ensureTag<InterfaceInd>()->setInterfaceId(interfaceEntry->getInterfaceId());
    packet->popTrailer<Ieee80211MacTrailer>(byte(4));
}

void Ieee80211Mac::receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details)
{
    Enter_Method_Silent("receiveSignal()");
    if (signalID == IRadio::receptionStateChangedSignal) {
        rx->receptionStateChanged((IRadio::ReceptionState)value);
    }
    else if (signalID == IRadio::transmissionStateChangedSignal) {
        auto oldTransmissionState = transmissionState;
        transmissionState = (IRadio::TransmissionState)value;
        bool transmissionFinished = (oldTransmissionState == IRadio::TRANSMISSION_STATE_TRANSMITTING && transmissionState == IRadio::TRANSMISSION_STATE_IDLE);
        if (transmissionFinished) {
            tx->radioTransmissionFinished();
            EV_DEBUG << "changing radio to receiver mode\n";
            configureRadioMode(IRadio::RADIO_MODE_RECEIVER); // FIXME: this is in a very wrong place!!! should be done explicitly from UpperMac!
        }
        rx->transmissionStateChanged(transmissionState);
    }
    else if (signalID == IRadio::receivedSignalPartChangedSignal) {
        rx->receivedSignalPartChanged((IRadioSignal::SignalPart)value);
    }
}

void Ieee80211Mac::configureRadioMode(IRadio::RadioMode radioMode)
{
    if (radio->getRadioMode() != radioMode) {
        ConfigureRadioCommand *configureCommand = new ConfigureRadioCommand();
        configureCommand->setRadioMode(radioMode);
        cMessage *message = new cMessage("configureRadioMode", RADIO_C_CONFIGURE);
        message->setControlInfo(configureCommand);
        sendDown(message);
    }
}

void Ieee80211Mac::sendUp(cMessage *msg)
{
    Enter_Method("sendUp(\"%s\")", msg->getName());
    take(msg);
    MACProtocolBase::sendUp(msg);
}

void Ieee80211Mac::sendUpFrame(Packet *frame)
{
    Enter_Method("sendUpFrame(\"%s\")", frame->getName());
    const auto& header = frame->peekHeader<Ieee80211DataOrMgmtHeader>();
    decapsulate(frame);
    if (!(header->getType() & 0x30))
        send(frame, "mgmtOut");
    else
        ds->processDataFrame(frame, std::dynamic_pointer_cast<const Ieee80211DataHeader>(header));
}

void Ieee80211Mac::sendDownFrame(Packet *frame)
{
    Enter_Method("sendDownFrame(\"%s\")", frame->getName());
    take(frame);
    configureRadioMode(IRadio::RADIO_MODE_TRANSMITTER);
    frame->ensureTag<PacketProtocolTag>()->setProtocol(&Protocol::ieee80211);
    sendDown(frame);
}

void Ieee80211Mac::sendDownPendingRadioConfigMsg()
{
    if (pendingRadioConfigMsg != nullptr) {
        sendDown(pendingRadioConfigMsg);
        pendingRadioConfigMsg = nullptr;
    }
}

void Ieee80211Mac::processUpperFrame(Packet *packet, const Ptr<const Ieee80211DataOrMgmtHeader>& header)
{
    Enter_Method("processUpperFrame(\"%s\")", packet->getName());
    take(packet);
    EV_INFO << "Frame " << packet << " received from higher layer, receiver = " << header->getReceiverAddress() << "\n";
    ASSERT(!header->getReceiverAddress().isUnspecified());
    if (mib->qos)
        hcf->processUpperFrame(packet, header);
    else
        dcf->processUpperFrame(packet, header);
}

void Ieee80211Mac::processLowerFrame(Packet *packet, const Ptr<const Ieee80211MacHeader>& header)
{
    Enter_Method("processLowerFrame(\"%s\")", packet->getName());
    take(packet);
    if (mib->qos)
        hcf->processLowerFrame(packet, header);
    else
        dcf->processLowerFrame(packet, header);
}

// FIXME
bool Ieee80211Mac::handleNodeStart(IDoneCallback *doneCallback)
{
    if (!doneCallback)
        return true;    // do nothing when called from initialize()

    bool ret = MACProtocolBase::handleNodeStart(doneCallback);
    radio->setRadioMode(IRadio::RADIO_MODE_RECEIVER);
    return ret;
}

// FIXME
bool Ieee80211Mac::handleNodeShutdown(IDoneCallback *doneCallback)
{
    bool ret = MACProtocolBase::handleNodeStart(doneCallback);
    handleNodeCrash();
    return ret;
}

// FIXME
void Ieee80211Mac::handleNodeCrash()
{
}

} // namespace ieee80211
} // namespace inet
