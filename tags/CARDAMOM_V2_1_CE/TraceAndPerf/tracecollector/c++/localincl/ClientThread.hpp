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


#ifndef INCL_TRACECOLLECTOR_CLIENTTHREAD_HPP 
#define INCL_TRACECOLLECTOR_CLIENTTHREAD_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include <iostream>
#include <string>

namespace Cdmw 
{
namespace Trace
{


class ClientThread : public OsSupport::Thread 
{
public:
    ClientThread(CORBA::ORB_ptr,
                 std::istream &,
                 std::ostream &);

    ~ClientThread() throw();

protected:
    void run() throw();;
    
    void run(std::istream&,std::ostream&);    
    
    // Available commands
	// Standard commands
    CORBA::Long print_help (const std::string &, std::ostream &);
    CORBA::Long exit (const std::string &, std::ostream &);
    CORBA::Long toggle_verbose(const std::string &, std::ostream &);

	// Specific commands
   

private:

    // type definition for "operation" used in command_def structure  
    typedef CORBA::Long  (ClientThread::* operation) (const std::string &, std::ostream &);
    
    struct command_def
    {
        const char* name;
        operation   op;
    };
    
    static const command_def commands[];
    static const CORBA::ULong nb_commands;
    
    CORBA::Long runOperation(const std::string &,const std::string &, std::ostream &);
    
    ClientThread();
    ClientThread(const ClientThread&);
    ClientThread& operator = (const ClientThread&);
    
    void unknown_command(std::ostream &);    
    void print_prompt(std::ostream&); 
    void print_info(std::ostream&);
     
    CORBA::ORB_var                           m_orb;
    bool                                     m_verbose;   
    std::istream &                           m_istream;   
    std::ostream &                           m_ostream;
};

}; // End namespace trace

}; // End namespace Cdmw 

#endif // INCL_TRACECOLLECTOR_CLIENTTHREAD_HPP 

