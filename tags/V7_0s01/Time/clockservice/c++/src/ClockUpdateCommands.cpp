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
 
#include "Time/clockservice/ClockUpdateCommands.hpp"
#include "clockservice/FederatedControlledClock_impl.hpp"

DEFINE_CTRL_CLOCK_COMMAND(SetTimeCommand)
{
    m_handler.handle_set(data.clock_status,
                         data.x0,
			 data.y0,
			 data.frozen_time,
			 data.rate);
}

DEFINE_CTRL_CLOCK_COMMAND(SetRateCommand)
{
    m_handler.handle_set_rate(data.clock_status,
                              data.x0,
                              data.y0,
                              data.frozen_time,
                              data.rate);
}

DEFINE_CTRL_CLOCK_COMMAND(PauseCommand)
{
    m_handler.handle_pause(data.clock_status,
                           data.x0,
			   data.y0,
			   data.frozen_time,
			   data.rate);
}

DEFINE_CTRL_CLOCK_COMMAND(ResumeCommand)
{
    m_handler.handle_resume(data.clock_status,
                            data.x0,
			    data.y0,
			    data.frozen_time,
			    data.rate);
}

DEFINE_CTRL_CLOCK_COMMAND(TerminateCommand)
{
    m_handler.handle_terminate();
}

DEFINE_CTRL_CLOCK_COMMAND(AlignRequestCommand)
{
    m_handler.handle_align_request();
}

DEFINE_CTRL_CLOCK_COMMAND(AlignResponseCommand)
{
    m_handler.handle_align_response(data.clock_status,
                                    data.x0,
			            data.y0,
			            data.frozen_time,
			            data.rate);
}

