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


#include <DisplayFacetExecutor_impl.hpp>

#include <ServerExecutor_impl.hpp>
#include "Foundation/ossupport/OS.hpp"


namespace Hello
{

//
// IDL:acme.com/Hello/CCM_Display:1.0
//
DisplayFacetExecutor_impl::DisplayFacetExecutor_impl(ServerExecutor_impl* p_server)
    : m_pserver (p_server)
{
}

DisplayFacetExecutor_impl::~DisplayFacetExecutor_impl()
{
    std::cout << "===> DisplayFacetExecutor_impl::~DisplayFacetExecutor_impl called!" << std::endl;
}

//
// IDL:acme.com/Hello/Display/print:1.0
//
void DisplayFacetExecutor_impl::display_hello()
    throw(CORBA::SystemException)
{
    std::cout << "\n"
              << "===========================================================\n"
              << "Entering display_hello() ..."
              << std::endl;
    CORBA::String_var server_name = m_pserver->name();
    // 
    // Read state data, increment counter and then save state again
    //
    DemoFTDCPS::ServerState state = m_pserver->read_state();
    std::cout << "\n"
              << "\t<---------------------------------------->\n"
              << "\t####> HELLO from " << server_name.in() << "\n" 
              << "\tPREVIOUS STATE IS \n"
              << "\t\tcounter :" << state.counter << "\n"
              << "\t\tmessage :" << state.message << "\n"
              << "\t<---------------------------------------->"
              << std::endl;
    std::cout << "\tBusy processing for 2 seconds...\n"
              << "\t(Server could 'crash' with Ctrl-C in the mean time :-) )"
              << std::endl;
    // Simulate server processing during 2 seconds
    // (server could "crash" with Ctrl-C during this period!) 
    Cdmw::OsSupport::OS::sleep(2000);
    state.counter++;
    std::ostringstream ostr;
    ostr << "Message number " << state.counter;
    // NOTA: Splice does not follow the language mapping!
    DDS::string_free(state.message);
    state.message=DDS::string_dup(ostr.str().c_str());
    std::cout << "\tNEW STATE IS \n"
              << "\t\tcounter :" << state.counter << "\n"
              << "\t\tmessage :" << state.message << "\n"
              << "\t<---------------------------------------->"
              << std::endl;
    m_pserver->write_state(state);
    //  NOTA: Splice does not follow the language mapping!
    DDS::string_free(state.message);
    state.message=0;
    std::cout << "\n"
              << "Exiting display_hello()\n"
              << "==========================================================="
              << std::endl;
}


}; // End of namespace Hello


