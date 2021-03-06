//
// Copyright (C) 2015 OpenSim Ltd.
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

#ifndef __INET_IEEE80211OFDMSYMBOLMODEL_H
#define __INET_IEEE80211OFDMSYMBOLMODEL_H

#include "inet/common/INETDefs.h"
#include "inet/physicallayer/common/bitlevel/SignalSymbolModel.h"

namespace inet {

namespace physicallayer {

class INET_API Ieee80211OfdmTransmissionSymbolModel : public TransmissionSymbolModel
{
  public:
    Ieee80211OfdmTransmissionSymbolModel(int headerSymbolLength, double headerSymbolRate, int payloadSymbolLength, double payloadSymbolRate, const std::vector<const ISymbol *> *symbols, const IModulation *headerModulation, const IModulation *payloadModulation);
    virtual ~Ieee80211OfdmTransmissionSymbolModel();
};

class INET_API Ieee80211OfdmReceptionSymbolModel : public ReceptionSymbolModel
{
  public:
    Ieee80211OfdmReceptionSymbolModel(int headerSymbolLength, double headerSymbolRate, int payloadSymbolLength, double payloadSymbolRate, const std::vector<const ISymbol *> *symbols);
    virtual ~Ieee80211OfdmReceptionSymbolModel();
};
} /* namespace physicallayer */
} /* namespace inet */

#endif

