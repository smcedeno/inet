//
// Copyright (C) 2014 Florian Meier
// Copyright (C) 2013 OpenSim Ltd.
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

package inet.physicallayer.ieee802154.packetlevel;

import inet.physicallayer.base.packetlevel.NarrowbandReceiverBase;

module Ieee802154NarrowbandScalarReceiver extends NarrowbandReceiverBase
{
    parameters:
        modulation = default("DSSS-OQPSK-16");
        errorModel.typename = default("ApskErrorModel");
        double minInterferencePower @unit(dBm);
        @class(Ieee802154NarrowbandScalarReceiver);
}

