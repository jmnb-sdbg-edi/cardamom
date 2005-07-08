/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ========================================================================= */

#ifndef _CDMW_CLOCK_CLOCK_DATA_2_COMMAND_ID_HPP_
#define _CDMW_CLOCK_CLOCK_DATA_2_COMMAND_ID_HPP_

#include "Time/clockservice/CdmwClockState.stub.hpp"

namespace Cdmw {
    namespace clock {
        class ClockData2CommandID;
    }
}

class Cdmw::clock::ClockData2CommandID {
public:
    inline int
    operator()(const Cdmw::clock::ControlledClockData& data)
        {
            return static_cast<int>(data.cmd);
        }
};
#endif /* _CDMW_CLOCK_CLOCK_DATA_2_COMMAND_ID_HPP_*/
