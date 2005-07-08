/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
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
 * =========================================================================== */


#ifndef INCL_PERFORMANCE_TARGETTHREAD_HPP 
#define INCL_PERFORMANCE_TARGETTHREAD_HPP 

#include <string>

#include "Foundation/osthreads/ThreadHandle.hpp"

namespace Cdmw 
{
namespace Performance
{


class TargetThread : public OsSupport::Thread 
{
public:
    TargetThread() throw();
	
    ~TargetThread() throw();

	int getApiNbr () throw();

	int getApiIndex (const std::string &api_name) throw();
	
	int getApiName (const int api_index, std::string &api_name) throw();

	long getApiCount (const int api_index) throw();
	
    long getApiSleepTime (const int api_index) throw();

protected:
    void run() throw();

	void TargetApi1();

	void TargetApi2();

	void TargetApi3();	

private:
	enum SIZE {APINBR = 3};
	
    static const char *ptargetApiName[APINBR];

	static const long targetApiCount[APINBR];
	
	static const long targetApiSleep[APINBR];
			
	TargetThread(const TargetThread&);
    TargetThread& operator = (const TargetThread&);    
};

}; // End namespace Performance

}; // End namespace Cdmw 

#endif // INCL_PERFORMANCE_TARGETTHREAD_HPP 

