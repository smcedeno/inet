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

package inet.protocol.fragmentation.base;

import inet.protocol.fragmentation.contract.IFragmenterPolicy;
import inet.queueing.base.PacketPusherBase;

module FragmenterBase extends PacketPusherBase
{
    parameters:
        bool deleteSelf = default(false);
        string fragmenterPolicyClass = default("");
        string fragmenterPolicyModule = default(".fragmenterPolicy");
        @display("i=block/fork");
        @class(FragmenterBase);
    submodules:
        fragmenterPolicy: <> like IFragmenterPolicy if typename != "" {
            parameters:
                @display("p=100,100");
        }
}
