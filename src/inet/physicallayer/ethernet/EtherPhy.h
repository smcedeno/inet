//
// Copyright (C) 2020 OpenSimLtd.
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

#ifndef __INET_ETHERPHY_H
#define __INET_ETHERPHY_H

#include "inet/common/INETDefs.h"

namespace inet {

namespace physicallayer {

class INET_API EtherPhy : public cSimpleModule
{
  protected:
    cGate *physInGate = nullptr;
    cGate *upperLayerInGate = nullptr;

  public:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
};

} // namespace physicallayer

} // namespace inet

#endif

