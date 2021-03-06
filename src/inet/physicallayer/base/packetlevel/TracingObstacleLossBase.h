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

#ifndef __INET_TRACINGOBSTACLELOSSBASE_H
#define __INET_TRACINGOBSTACLELOSSBASE_H

#include "inet/environment/contract/IPhysicalObject.h"
#include "inet/physicallayer/contract/packetlevel/ITracingObstacleLoss.h"

namespace inet {

namespace physicallayer {

class INET_API TracingObstacleLossBase : public cModule, public ITracingObstacleLoss
{
};

} // namespace physicallayer

} // namespace inet

#endif

