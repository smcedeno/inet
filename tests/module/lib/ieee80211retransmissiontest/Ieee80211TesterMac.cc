//
// Copyright (C) 2015 Andras Varga
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
//

#include "Ieee80211TesterMac.h"
//#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"
#include "inet/linklayer/ieee80211/mac/contract/IRx.h"

namespace inet {

Define_Module(Ieee80211TesterMac);

void Ieee80211TesterMac::handleLowerPacket(Packet *msg)
{
    auto pk = check_and_cast<Packet *>(msg);
    actions = par("actions").stringValue();
    int len = strlen(actions);
    if (msgCounter >= len)
        throw cRuntimeError("No action is defined for this msg %s", msg->getName());
    if (actions[msgCounter] == 'A') {
        const auto& frame = pk->peekHeader<Ieee80211MacHeader>();
        if (rx->lowerFrameReceived(pk, frame))
            processLowerFrame(pk, frame);
        else { // corrupted frame received
            if (mib->qos)
                hcf->corruptedFrameReceived();
            else
                dcf->corruptedFrameReceived();
        }
    }
    else if (actions[msgCounter] == 'B')
        delete msg; // block
    else
        throw cRuntimeError("Unknown action = %c", actions[msgCounter]);
    msgCounter++;
}

} // namespace inet
