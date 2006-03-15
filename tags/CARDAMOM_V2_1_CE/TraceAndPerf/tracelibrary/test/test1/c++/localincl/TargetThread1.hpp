/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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


#ifndef INCL_TRACE_TARGETTHREAD1_HPP 
#define INCL_TRACE_TARGETTHREAD1_HPP 

#include <string>

#include "Foundation/osthreads/ThreadHandle.hpp"

#include <Foundation/logging/Logger.hpp> // ECR-0169


namespace Cdmw 
{
namespace Trace
{


class TargetThread1 : public OsSupport::Thread 
{
public:
    TargetThread1() throw();
	
    ~TargetThread1() throw();

protected:
    void run() throw();

private:
	TargetThread1(const TargetThread1&);
    TargetThread1& operator = (const TargetThread1&);    

    Cdmw::Logging::Logger* m_logger;
};

}; // End namespace Trace

}; // End namespace Cdmw 

#endif // INCL_TRACE_TARGETTHREAD1_HPP 

