//
// Copyright (C) 2008 Irene Ruengeler
// Copyright (C) 2010-2012 Thomas Dreibholz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include "inet/transportlayer/contract/sctp/SctpCommand_m.h"
#include "inet/transportlayer/sctp/SctpSendStream.h"

namespace inet {
namespace sctp {

SctpSendStream::SctpSendStream(SctpAssociation *assoc_, const uint16_t id)
{
    assoc = assoc_;
    streamId = id;
    nextStreamSeqNum = 0;
    bytesInFlight = 0;
    resetRequested = false;
    fragInProgress = false;

    char queueName[64];
    snprintf(queueName, sizeof(queueName), "OrderedSendQueue ID %d", id);
    streamQ = new cPacketQueue(queueName);
    snprintf(queueName, sizeof(queueName), "UnorderedSendQueue ID %d", id);
    uStreamQ = new cPacketQueue(queueName);
}

SctpSendStream::~SctpSendStream()
{
    deleteQueue();
}

void SctpSendStream::deleteQueue()
{
    SctpDataMsg *datMsg;
    SctpSimpleMessage *smsg;
    int32_t count = streamQ->getLength();
    while (!streamQ->isEmpty()) {
        datMsg = check_and_cast<SctpDataMsg *>(streamQ->pop());
        smsg = check_and_cast<SctpSimpleMessage *>(datMsg->decapsulate());
        delete smsg;
        delete datMsg;
        count--;
    }
    while (!uStreamQ->isEmpty()) {
        datMsg = check_and_cast<SctpDataMsg *>(uStreamQ->pop());
        smsg = check_and_cast<SctpSimpleMessage *>(datMsg->decapsulate());
        delete smsg;
        delete datMsg;
    }
    delete streamQ;
    delete uStreamQ;
}

uint32_t SctpSendStream::getNextStreamSeqNum() {
    return nextStreamSeqNum;
};

void SctpSendStream::setNextStreamSeqNum(const uint16_t num) {
    nextStreamSeqNum = num;
};

} // namespace sctp
} // namespace inet

