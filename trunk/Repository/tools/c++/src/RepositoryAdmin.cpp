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


#include "Repository/tools/RepositoryAdmin.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"
#include "Foundation/common/Assert.hpp"
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>


#define ECHO_CMD if (m_verbose) os 


namespace
{
    const char* INFO_MESSAGE = "Type h or help for a list of available commands.";
    const char* PROMPT_MESSAGE = "\nADMIN>";
    const char* UNKNOWN_COMMAND_MESSAGE = "Bad command!>";

    const char * TAB_VALUE = "    ";

    const CORBA::Long EXIT_ORDERED(-10);
    const CORBA::Long OP_SUCCESS = 0;
    const CORBA::Long OP_FAILURE = -1;

    
    bool not_space(char c)
    { return !isspace(c);}

    std::string tab_string(int level)
    {
        std::string tab("");

        for (int i = 0; i < level; ++i)
            tab += TAB_VALUE;
        
        return tab;
    }
    // Forward declaration
    bool 
    show_context(CosNaming::NamingContext_ptr nc, 
                 std::ostream& os, unsigned int level = 0)
        throw();

    bool show_chunk(CosNaming::NamingContext_ptr   nc, 
                    const CosNaming::BindingList & bl, 
                    std::ostream&                  os, 
                    unsigned int                   level)
        throw()
    {
        bool done = true;
        
        for (CORBA::ULong i = 0; i < bl.length(); ++i) {
            // Note from CosNaming specification: 
            //   In struct Binding, binding_name is incorrectly defined
            //   as a Name instead of a NameComponent. This definition is
            //   unchanged for compatibility reasons.
            try {
                std::string tab = tab_string(level);
                
                os << tab 
                   << Cdmw::NamingAndRepository::NamingInterface::to_string(bl[i].binding_name);
                
                if (bl[i].binding_type == CosNaming::ncontext) {
                    os << "/" << std::endl; // for contexts
                    // For contexts, first destroy then unbind
                    CORBA::Object_var obj = nc->resolve(bl[i].binding_name);
                    CosNaming::NamingContext_var __nc__
                        = CosNaming::NamingContext::_narrow(obj.in());
                    done &= show_context(__nc__.in(), os, level + 1);
                } else // object
                    os << std::endl;
            } catch (const CORBA::Exception & ) {
                // keep silent (ignore)
                done = false;
            } catch (const Cdmw::NamingAndRepository::InvalidNameException &) {
                // This is impossible ! keep silent (ignore)
                done = false;
            }
        }
        return done;
    }

    // Depth first
    bool show_context(CosNaming::NamingContext_ptr nc, 
		      std::ostream& os, unsigned int level)
        throw()
    {
        bool all_done = false;
        CosNaming::BindingIterator_var it;                      // Iterator reference
        CosNaming::BindingList_var     bl;                      // Binding list
        const CORBA::ULong LIST_CHUNK_SIZE = 500;               // Chunk size
        
        try {
            nc->list(LIST_CHUNK_SIZE,bl,it);                    // Get first chunk
            bool chunk_done = show_chunk(nc, bl.in(),os,level);      // Show first chunk
            
            if (!CORBA::is_nil(it.in())) {                           // More bindings?
                CORBA::Boolean more;
                do {
                    more = it->next_n(LIST_CHUNK_SIZE,bl);      // Get next chunk
                    chunk_done &= show_chunk(nc, bl.in(), os, level);// Show chunk
                } while (more);                                 // While not done
                it->destroy();                                  // Clean up iterator
            }
            all_done = chunk_done;
        } catch (const CORBA::SystemException &) {
            // keep silent (ignore)
        }
        return all_done;
    }


    //----------------------------------------------------------------------------
    void show_supported_objects(const CdmwLifeCycle::RepositoryIdSeq & seq, 
                                std::ostream& os)
        throw()
    {
        os << "Supported Objects :";
        for (CORBA::ULong i = 0;i<seq.length();++i)
            os << "\n\t RepositoryId = <" << seq[i] << ">";
        os << std::endl;
    }
}; // End anonymous namespace

namespace Cdmw
{
namespace Tools
{

//----------------------------------------------------------------------------
RepositoryAdmin::RepositoryAdmin(CORBA::ORB_ptr orb, 
                                 CdmwNamingAndRepository::Repository_ptr rep)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_repository(CdmwNamingAndRepository::Repository::_duplicate(rep)),
      m_verbose(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CDMW_ASSERT(!CORBA::is_nil(rep));
}

//----------------------------------------------------------------------------
RepositoryAdmin::~RepositoryAdmin() throw()
{
}
//----------------------------------------------------------------------------
const CORBA::ULong RepositoryAdmin::nb_commands = 10;

// WARNING:
// RepositoryAdmin::commands should be sorted alphabetically (by operation name)
const RepositoryAdmin::command_def RepositoryAdmin::commands[] = 
{ 
    { "exit"    , &RepositoryAdmin::exit            },
    { "factory" , &RepositoryAdmin::factory         },
    { "h"       , &RepositoryAdmin::print_help      },
    { "help"    , &RepositoryAdmin::print_help      },
    { "ln"      , &RepositoryAdmin::register_object },
    { "load"    , &RepositoryAdmin::load_file       },
    { "ls"      , &RepositoryAdmin::list            },
    { "rm"      , &RepositoryAdmin::release_name    },
    { "verbose" , &RepositoryAdmin::toggle_verbose  },
    { "x"       , &RepositoryAdmin::exit            }
};

    
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::runOperation(const std::string & op,
                                          const std::string & arg, 
                                          std::ostream &      os) 
    throw()
{
    CORBA::Long l = 0;
    CORBA::Long r = (CORBA::Long)nb_commands;
    CORBA::Long m;
    CORBA::Long res;
    
    while(l < r)
        {
            m = (l + r) / 2;
            res = strcmp(commands[m].name, op.c_str());
            if(res == 0)
                return (this->*commands[m].op)(arg,os);
            if(res > 0)
                r = m;
            else
                l = m + 1;
        }
    if(op != "") unknown_command(os);
    
    return OP_FAILURE;
}
//----------------------------------------------------------------------------
void RepositoryAdmin::print_info(std::ostream& os) const throw()
{
    os << "\n\n"
       << "\t\t--------------------------------------------------\n"
       << "\t\t Cdmw Naming & Repository administration console \n"
       << "\t\t--------------------------------------------------\n"
       << "\n\n"
       << INFO_MESSAGE
       << "\nInfo: (verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
}
//----------------------------------------------------------------------------
void RepositoryAdmin::print_prompt(std::ostream& os) const throw()
{
	os << PROMPT_MESSAGE << std::flush;
}
//----------------------------------------------------------------------------
void RepositoryAdmin::unknown_command( std::ostream & os) const throw()
{
    os << UNKNOWN_COMMAND_MESSAGE << std::endl;
}
//----------------------------------------------------------------------------
void RepositoryAdmin::run(std::istream& is,std::ostream& os) throw()
{
    print_info(os);
	print_prompt(os);

    std::string s;
    while(std::getline(is,s)) {  
        // ===============
        // Extract command        
        // ===============
        // skip leading whitespaces
        std::string::const_iterator i = s.begin();
        std::string::const_iterator end = s.end();
        i = std::find_if(i,end,not_space);
        // find end of command
        std::string::const_iterator j = std::find_if(i,end,isspace);
        // copy the characters in [i,j)
        std::string command;
        if (i != end)
            command = std::string(i,j);
        // =================
        // Extract arguments
        // =================
        // skip leading whitespaces
        i = std::find_if(j,end,not_space);
        std::string arg;
        // copy arguments
        if (i != end)
            arg = std::string(i,end);
        
        ECHO_CMD << "@cmd = <" << command << "> @args = <" << arg << ">" << std::endl;
        if (runOperation(command,arg,os) == EXIT_ORDERED) break; 
        print_prompt(os); 
    } 
}
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::exit(const std::string & arg, std::ostream & os) throw()
{	
	return EXIT_ORDERED;
}
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::toggle_verbose(const std::string & arg, std::ostream & os) throw()
{	
    m_verbose = !m_verbose;
    os << "(verbose mode is " << (m_verbose ? "on" : "off") << ")"
       << std::endl;
	return OP_SUCCESS;
}
//----------------------------------------------------------------------------
// Available commands:
//
CORBA::Long RepositoryAdmin::print_help(const std::string & arg, std::ostream & os) throw()
{
    os << " +==================+============================================+" << "\n"
       << " | COMMAND          | DESCRIPTION                                |" << "\n"
       << " +==================+============================================+" << "\n"
       << " | x | exit         | exit ADMIN mode                            |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | factory name     | list supported objects of the factory      |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | h | help         | print available commands                   |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | ln ior name      | Bind object whose object reference is 'ior'|" << "\n"
       << " |                  | to 'name' (fully qualified)                |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | load filename    | Load 'filename' batch file                 |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | ls [name_domain] | list content of 'name_domain'              |" << "\n"
       << " |                  | (default list all repository)              |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | rm name          | remove object 'name' from repository       |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << " | verbose          | toggle verbose mode on/off                 |" << "\n"
       << " +------------------+--------------------------------------------+" << "\n"
       << std::endl;

    return OP_SUCCESS;
}
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::list(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;

    try {
        CosNaming::NamingContext_var nc = CosNaming::NamingContext::_nil();

        CosNaming::NamingContext_var nc_root_context
            = m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        if (arg == "") {
            nc = nc_root_context;
        } else {
            // Check that domain_name is really a Name Domain
            CdmwNamingAndRepository::NameDomain_var dom
                = m_repository->resolve_name_domain (arg.c_str());
            // It is a name domain, so we should be able to get its naming 
            // context.
            Cdmw::NamingAndRepository::NamingInterface ni(nc_root_context.in());

            typedef Cdmw::NamingAndRepository::NamingUtil<CosNaming::NamingContext> Util;
            nc = Util::resolve_name(ni,arg);
        }
        result = show_context(nc.in(), os,0);
    } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
        os << "Not a NameDomain!" << std::endl;
    } catch (const CdmwNamingAndRepository::InvalidName & e) {
        os << "InvalidName <" << e.reason.in() << ">" << std::endl;
    } catch (const Cdmw::Common::TypeMismatchException & e) {
        os << "TypeMismatchException (Not a NamingContext) <" 
           << e.what() << ">" << std::endl;
    } catch (const CORBA::Exception & e) {
        os << "CORBA Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::factory(const std::string & arg, std::ostream & os) 
    throw()
{    
    CORBA::Long result = OP_FAILURE;

    try {
        
        CosNaming::NamingContext_var nc 
            = m_repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

        if (arg == "") {
            os << "Factory name needed." << std::endl;
        } else {        
            Cdmw::NamingAndRepository::NamingInterface ni(nc.in());

            typedef Cdmw::NamingAndRepository::NamingUtil<CdmwLifeCycle::FactoryBase> Util;
            
            CdmwLifeCycle::FactoryBase_var factory
                = Util::resolve_name(ni,arg);
            
            CdmwLifeCycle::RepositoryIdSeq_var seq = factory->get_supported_objects();

            show_supported_objects(seq.in(),os);
            result = OP_SUCCESS;
        }
    } catch (const CdmwNamingAndRepository::Repository::NoRootContext & ) {
        os << "Cannot reach DefaultRootContext!" << std::endl;
    } catch (const CosNaming::NamingContext::NotFound & ) {
        os << "CosNaming::NamingContext::NotFound exception!" << std::endl;
    } catch (const CosNaming::NamingContext::CannotProceed & ) {
        os << "Unexpected Error (CannotProceed exception)!" << std::endl;
    } catch (const CosNaming::NamingContext::InvalidName & ) {
        os << "Invalid Name <" << arg << ">!" << std::endl;
    } catch (const Cdmw::Common::TypeMismatchException & e) {
        os << "TypeMismatchException (Not a Factory object) <" 
           << e.what() << ">" << std::endl;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::release_name(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        std::string dirname  = Cdmw::NamingAndRepository::NamingInterface::dirname(arg);
        std::string basename = Cdmw::NamingAndRepository::NamingInterface::basename(arg);
        
        using namespace CdmwNamingAndRepository;
        if (dirname != "") {
            try {
                NameDomain_var dom = m_repository->resolve_name_domain(dirname.c_str());
                dom->release_name(basename.c_str());
                ECHO_CMD << "<" << arg << "> released." << std::endl;
                result = OP_SUCCESS;
            } catch (const NoNameDomain & ) {
                os << "<" << dirname << "> is not a NameDomain!" << std::endl;
            } catch (const NameDomain::NotRegistered & ) {
                os << "<" << basename << "> not registered within <" << dirname << ">!" 
                   << std::endl;
            } catch (const InvalidName &) {
                os << "BUG: InvalidName exception is received ?!" << std::endl;
            } catch (const CORBA::SystemException & e) {
                os << "CORBA System Exception : \n" << e << std::endl;
            }
        } else {
            os << "NameDomain needed!" << std::endl;
        }
    } catch (const Cdmw::NamingAndRepository::InvalidNameException & e) {
        os << "Invalid Name : <" << e.what() << ">" << std::endl;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}

//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::register_object(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;    

    // arg contains the following:
    // ior name

    // ===============
    // Extract ior        
    // ===============
    std::string::const_iterator i = arg.begin();
    std::string::const_iterator end = arg.end();
    // find end of ior
    std::string::const_iterator j = std::find_if(i,end,isspace);
    // copy the characters in [i,j)
    std::string ior;
    if (i != end)
        ior = std::string(i,j);
    else {
        os << "Object's IOR and name are missing." << std::endl;
        return result;
    }
    // =================
    // Extract name
    // =================
    // skip leading whitespaces
    i = std::find_if(j,end,not_space);
    std::string name;
    // copy arguments
    if (i != end)
        name = std::string(i,end);
    else {
        os << "Object's name is missing." << std::endl;
        return result;
    }

    try {
        CORBA::Object_var obj = m_orb->string_to_object(ior.c_str());

        std::string dirname  = Cdmw::NamingAndRepository::NamingInterface::dirname(name);
        std::string basename = Cdmw::NamingAndRepository::NamingInterface::basename(name);
        
        using namespace CdmwNamingAndRepository;
        if (dirname != "") {
            try {
                NameDomain_var dom = m_repository->resolve_name_domain(dirname.c_str());

                dom->register_new_object(basename.c_str(),obj.in());
                ECHO_CMD << "<" << name << "> registered." << std::endl;
                result = OP_SUCCESS;
            } catch (const NoNameDomain & ) {
                os << "<" << dirname << "> is not a NameDomain!" << std::endl;
            } catch (const NameDomain::AlreadyExists & ) {
                os << "<" << basename << "> already registered within <" << dirname << ">!" 
                   << std::endl;
            } catch (const InvalidName &) {
                os << "BUG: InvalidName exception is received ?!" << std::endl;
            } catch (const CORBA::SystemException & e) {
                os << "CORBA System Exception : \n" << e << std::endl;
            }
        } else {
            os << "NameDomain needed!" << std::endl;
        }
    } catch (const Cdmw::NamingAndRepository::InvalidNameException & e) {
        os << "Invalid Name : <" << e.what() << ">" << std::endl;
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------
CORBA::Long RepositoryAdmin::load_file(const std::string & arg, std::ostream & os) throw()
{	
    CORBA::Long result = OP_FAILURE;    

    try {
        ECHO_CMD << " Loading batch file \"" << arg << "\"." << std::endl;
        std::ifstream in(arg.c_str());
        
        if (!in)
            os << "Could not open file \"" << arg << "\"." << std::endl;
        else {
            RepositoryAdmin admin(m_orb.in(),m_repository.in());
            admin.run(in,os);
            ECHO_CMD << "Batch file \"" << arg << "\" done." << std::endl;
            result = OP_SUCCESS;
        }
    } catch (const CORBA::SystemException & e) {
        os << "CORBA System Exception : \n" << e << std::endl;
    }

    return result;
}
//----------------------------------------------------------------------------

} // namespace Tools
} // namespace Cdmw

