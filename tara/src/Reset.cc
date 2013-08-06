/*****************************************************************************\
 Tara-- <<-- Tara -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Tara is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tara is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Reset.h"
#include "Tara.h"

namespace Reset {
    void transformNet() {
        message("Reset- Transformation ist not yet working !!");
        // go through all reset-transitions
        std::map<pnapi::Transition*, bool>::iterator it;
        it = Tara::resetMap.begin();
        std::map<pnapi::Transition*, bool>::iterator end;
        end = Tara::resetMap.end();

        for(; it != end; ++it) {
            status("%s is Reset- Transition", it->first->getName().c_str());
        }
    }
}
