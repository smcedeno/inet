//
// Copyright (C) 2013 OpenSim Ltd.
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

#include "inet/physicallayer/base/packetlevel/NoiseBase.h"

namespace inet {

namespace physicallayer {

NoiseBase::NoiseBase(simtime_t startTime, simtime_t endTime) :
    startTime(startTime),
    endTime(endTime)
{
}

std::ostream& NoiseBase::printToStream(std::ostream& stream, int level, int evFlags) const
{
    if (level <= PRINT_LEVEL_TRACE)
        stream << EV_FIELD(startTime)
               << EV_FIELD(endTime);
    return stream;
}

} // namespace physicallayer

} // namespace inet

