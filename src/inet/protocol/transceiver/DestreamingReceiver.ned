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

package inet.protocol.transceiver;

import inet.protocol.transceiver.base.PacketReceiverBase;
import inet.protocol.transceiver.contract.IPacketReceiver;

//
// This module receives signals from the transmission medium (wire) as a stream.
// It receives the signal start and signal end separately. It sends packets to
// the upper layer as a whole.
//
simple DestreamingReceiver extends PacketReceiverBase like IPacketReceiver
{
    parameters:
        @class(DestreamingReceiver);
}

