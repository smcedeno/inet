//
// Copyright (C) 2014 OpenSim Ltd.
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

package inet.physicallayer.analogmodel.bitlevel;

import inet.physicallayer.contract.packetlevel.IAnalogModel;

module LayeredScalarAnalogModel like IAnalogModel
{
    parameters:
        bool ignorePartialInterference = default(false); // enabled means interference only partially overlapping with the signal being received is ignored, otherwise the simulation stops with an error
        @display("i=block/tunnel");
        @class(LayeredScalarAnalogModel);
}

