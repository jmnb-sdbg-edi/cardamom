/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_TOOLS_REP_ADMIN_HPP
#define INCL_TOOLS_REP_ADMIN_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <iostream>
#include <string>

namespace Cdmw
{
namespace Tools
{
    
//--------------------------------------------------------------------------------------//
class RepositoryAdmin
{
public:
    
    RepositoryAdmin(CORBA::ORB_ptr, CdmwNamingAndRepository::Repository_ptr)
        throw (CORBA::SystemException);
    
    ~RepositoryAdmin() throw();

    void run(std::istream&,std::ostream&) throw();
    
protected:
    
    
    // Available commands
    CORBA::Long print_help (const std::string &, std::ostream &) throw();
    CORBA::Long exit (const std::string &, std::ostream &) throw();
    CORBA::Long factory(const std::string &, std::ostream &) throw();
    CORBA::Long list(const std::string &, std::ostream &) throw();
    CORBA::Long register_object(const std::string &, std::ostream &) throw();
    CORBA::Long release_name(const std::string &, std::ostream &) throw();
    CORBA::Long toggle_verbose(const std::string &, std::ostream &) throw();
    CORBA::Long load_file(const std::string &, std::ostream &) throw();

private:
    
    typedef CORBA::Long  (RepositoryAdmin::* operation)(const std::string &, std::ostream &);
    
    struct command_def
    {
        const char* name;
        operation   op;
    };
    
    static const command_def commands[];
    static const CORBA::ULong nb_commands;
    
    CORBA::Long runOperation(const std::string &,const std::string &, std::ostream &) throw();
    
    RepositoryAdmin();
    RepositoryAdmin(const RepositoryAdmin&);
    RepositoryAdmin& operator = (const RepositoryAdmin&);
    
    void unknown_command(std::ostream &) const throw();    
    void print_prompt(std::ostream&) const throw(); 
    void print_info(std::ostream&) const throw();
	
    CORBA::ORB_var                           m_orb;
    CdmwNamingAndRepository::Repository_var m_repository;
    bool                                     m_verbose;
        
};


} // namespace Tools
} // namespace Cdmw

#endif // INCL_TOOLS_REP_ADMIN_HPP

