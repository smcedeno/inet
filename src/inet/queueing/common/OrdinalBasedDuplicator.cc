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

#include "inet/common/INETUtils.h"
#include "inet/common/Simsignals.h"
#include "inet/queueing/common/OrdinalBasedDuplicator.h"

namespace inet {
namespace queueing {

Define_Module(OrdinalBasedDuplicator);

void OrdinalBasedDuplicator::initialize(int stage)
{
    PacketDuplicatorBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numPackets = 0;
        numDuplicated = 0;
        generateFurtherDuplicates = false;

        WATCH(numPackets);
        WATCH(numDuplicated);
        WATCH(generateFurtherDuplicates);

        const char *vector = par("duplicatesVector");
        parseVector(vector);

        if (duplicatesVector.size() == 0)
            EV_WARN << "Empty duplicatesVector" << EV_ENDL;
        else {
            EV_DEBUG << EV_FIELD(duplicatesVector, vector) << EV_ENDL;
            generateFurtherDuplicates = true;
        }
    }
}

int OrdinalBasedDuplicator::getNumPacketDuplicates(Packet *packet)
{
    if (generateFurtherDuplicates) {
        if (numPackets == duplicatesVector[0]) {
            EV_DEBUG << "Duplicating packet" << EV_FIELD(ordinalNumber, numPackets) << EV_FIELD(packet) << EV_ENDL;
            numDuplicated++;
            duplicatesVector.erase(duplicatesVector.begin());
            if (duplicatesVector.size() == 0) {
                EV_DEBUG << "End of duplicatesVector reached" << EV_ENDL;
                generateFurtherDuplicates = false;
            }
            numPackets++;
            return 1;
        }
    }
    numPackets++;
    return 0;
}

void OrdinalBasedDuplicator::parseVector(const char *vector)
{
    const char *v = vector;
    while (*v) {
        // parse packet numbers
        while (isspace(*v))
            v++;
        if (!*v || *v == ';')
            break;
        if (!isdigit(*v))
            throw cRuntimeError("Syntax error in duplicatesVector: packet number expected");
        if (duplicatesVector.size() > 0 && duplicatesVector.back() >= (unsigned int)atoi(v))
            throw cRuntimeError("Syntax error in duplicatesVector: packet numbers in ascending order expected");

        duplicatesVector.push_back(atoi(v));
        while (isdigit(*v))
            v++;

        // skip delimiter
        while (isspace(*v))
            v++;
        if (!*v)
            break;
        if (*v != ';')
            throw cRuntimeError("Syntax error in duplicatesVector: separator ';' missing");
        v++;
        while (isspace(*v))
            v++;
    }
}

} // namespace queueing
} // namespace inet

