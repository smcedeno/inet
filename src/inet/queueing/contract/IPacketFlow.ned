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

package inet.queueing.contract;

//
// This module interface is implemented by packet flows. A packet flow connects
// one input to one output. It can operate in both active and passive mode.
// Packets can be pushed into its input or packets can be pulled from its
// output. Packets are never dropped but the contents and/or attached meta
// information may be altered.
//
moduleinterface IPacketFlow extends IPacketSink, IPacketSource
{
    parameters:
        @display("i=block/star");
}
