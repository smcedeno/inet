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

#include "inet/physicallayer/analogmodel/packetlevel/DimensionalNoise.h"

namespace inet {

namespace physicallayer {

DimensionalNoise::DimensionalNoise(simtime_t startTime, simtime_t endTime, Hz centerFrequency, Hz bandwidth, const Ptr<const IFunction<WpHz, Domain<simsec, Hz>>>& power) :
    NarrowbandNoiseBase(startTime, endTime, centerFrequency, bandwidth),
    power(power)
{
}

std::ostream& DimensionalNoise::printToStream(std::ostream& stream, int level, int evFlags) const
{
    stream << "DimensionalNoise";
    if (level <= PRINT_LEVEL_DEBUG)
        stream << EV_FIELD(powerMax, power->getMax())
               << EV_FIELD(powerMin, power->getMin());
    if (level <= PRINT_LEVEL_TRACE)
        stream << EV_FIELD(power);
    return NarrowbandNoiseBase::printToStream(stream, level);
}

W DimensionalNoise::computeMinPower(simtime_t startTime, simtime_t endTime) const
{
    Point<simsec> startPoint{simsec(startTime)};
    Point<simsec> endPoint{simsec(endTime)};
    W minPower = integrate<WpHz, Domain<simsec, Hz>, 0b10, W, Domain<simsec>>(power)->getMin(Interval<simsec>(startPoint, endPoint, 0b1, 0b1, 0b0));
    EV_DEBUG << "Computing minimum noise power" << EV_FIELD(startPoint) << EV_FIELD(endPoint) << EV_FIELD(minPower) << endl;
    return minPower;
}

W DimensionalNoise::computeMaxPower(simtime_t startTime, simtime_t endTime) const
{
    Point<simsec> startPoint{simsec(startTime)};
    Point<simsec> endPoint{simsec(endTime)};
    W maxPower = integrate<WpHz, Domain<simsec, Hz>, 0b10, W, Domain<simsec>>(power)->getMax(Interval<simsec>(startPoint, endPoint, 0b1, 0b1, 0b0));
    EV_DEBUG << "Computing maximum noise power" << EV_FIELD(startPoint) << EV_FIELD(endPoint) << EV_FIELD(maxPower) << endl;
    return maxPower;
}

} // namespace physicallayer

} // namespace inet

