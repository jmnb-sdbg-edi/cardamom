/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

#ifndef _CDMW_CLOCK_CLOCK_UPDATE_COMMANDS_HPP_
#define _CDMW_CLOCK_CLOCK_UPDATE_COMMANDS_HPP_


#include <Foundation/commonsvcs/federation/ParametricCommand.hpp>

#include <Time/clockservice/FederatedControlledClockDataCodec.hpp>
#include <Time/clockservice/CdmwClockState.stub.hpp>

extern "C++" {
    namespace Cdmw {
        namespace clock {
            class ControlledClockCommandHandler;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
#define DECLARE_CTRL_CLOCK_COMMAND(COMMAND_NAME) \
namespace Cdmw { namespace clock { \
class COMMAND_NAME \
    : public virtual Cdmw::CommonSvcs::ParametricCommand< ::Cdmw::clock::ControlledClockData > \
{ \
public: \
    COMMAND_NAME (::Cdmw::clock::ControlledClockCommandHandler& handler); \
    virtual ~COMMAND_NAME ();\
\
public: \
    virtual void execute (const ::Cdmw::clock::ControlledClockData& data); \
private: \
    ::Cdmw::clock::ControlledClockCommandHandler& m_handler; \
}; } }

//////////////////////////////////////////////////////////////////////////////

#define DEFINE_CTRL_CLOCK_COMMAND(COMMAND_NAME) \
::Cdmw::clock::COMMAND_NAME::COMMAND_NAME(::Cdmw::clock::ControlledClockCommandHandler& handler) \
   : m_handler(handler) { } \
\
::Cdmw::clock::COMMAND_NAME::~COMMAND_NAME() { } \
 \
void \
::Cdmw::clock::COMMAND_NAME::execute(const ::Cdmw::clock::ControlledClockData& data) 

//////////////////////////////////////////////////////////////////////////////


DECLARE_CTRL_CLOCK_COMMAND(SetTimeCommand)
DECLARE_CTRL_CLOCK_COMMAND(SetRateCommand)
DECLARE_CTRL_CLOCK_COMMAND(PauseCommand)
DECLARE_CTRL_CLOCK_COMMAND(ResumeCommand) 
DECLARE_CTRL_CLOCK_COMMAND(TerminateCommand)
DECLARE_CTRL_CLOCK_COMMAND(AlignRequestCommand)
DECLARE_CTRL_CLOCK_COMMAND(AlignResponseCommand)

#endif /* _CDMW_CLOCK_CLOCK_UPDATE_COMMANDS_HPP_ */
