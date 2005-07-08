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


#ifndef INCL_PERFORMANCE_CLIENTTHREAD_HPP 
#define INCL_PERFORMANCE_CLIENTTHREAD_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include <iostream>
#include <string>

namespace Cdmw 
{
namespace Performance
{


class ClientThread : public OsSupport::Thread 
{
public:
    ClientThread(CORBA::ORB_ptr,
                 std::istream &,
                 std::ostream &)
                 throw (CORBA::SystemException);

    ~ClientThread() throw();

protected:
    void run() throw();
    
    void run(std::istream&,std::ostream&) throw();    
    
    // Available commands
	// Standard commands
    CORBA::Long print_help (const std::string &, std::ostream &) throw();
    CORBA::Long exit (const std::string &, std::ostream &) throw();
    CORBA::Long toggle_verbose(const std::string &, std::ostream &) throw();

	// Specific commands
    CORBA::Long activate_perfo(const std::string &, std::ostream &) throw();
    CORBA::Long start_targets(const std::string &, std::ostream &) throw(); 
    CORBA::Long get_api_info(const std::string &, std::ostream &) throw();
    CORBA::Long get_function_stat(const std::string & arg, std::ostream & os) throw();
   

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
    
    CORBA::Long runOperation(const std::string &,const std::string &, std::ostream &) throw();
    
    ClientThread();
    ClientThread(const ClientThread&);
    ClientThread& operator = (const ClientThread&);
    
    void unknown_command(std::ostream &) const throw();    
    void print_prompt(std::ostream&) const throw(); 
    void print_info(std::ostream&) const throw();
     
    CORBA::ORB_var                           m_orb;
    bool                                     m_verbose;   
    std::istream &                           m_istream;   
    std::ostream &                           m_ostream;
};

}; // End namespace Performance

}; // End namespace Cdmw 

#endif // INCL_PERFORMANCE_CLIENTTHREAD_HPP 

