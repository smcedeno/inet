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

#ifndef __INET_APSKMODULATIONBASE_H
#define __INET_APSKMODULATIONBASE_H

#include "inet/common/ShortBitVector.h"
#include "inet/physicallayer/apskradio/bitlevel/ApskSymbol.h"
#include "inet/physicallayer/contract/packetlevel/IApskModulation.h"

namespace inet {

namespace physicallayer {

/**
 * Base class for modulations using various amplitude and phase-shift keying.
 */
class INET_API ApskModulationBase : public IApskModulation
{
  protected:
    const std::vector<ApskSymbol> *constellation;
    const unsigned int codeWordSize;
    const unsigned int constellationSize;

  public:
    ApskModulationBase(const std::vector<ApskSymbol> *constellation);

    virtual std::ostream& printToStream(std::ostream& stream, int level, int evFlags = 0) const override;
    const static ApskModulationBase *findModulation(const char *name);

    virtual const std::vector<ApskSymbol> *getConstellation() const { return constellation; }
    virtual unsigned int getConstellationSize() const override { return constellationSize; }
    virtual unsigned int getCodeWordSize() const override { return codeWordSize; }

    virtual const ApskSymbol *mapToConstellationDiagram(const ShortBitVector& symbol) const;
    virtual ShortBitVector demapToBitRepresentation(const ApskSymbol *symbol) const;
};

} // namespace physicallayer

} // namespace inet

#endif

