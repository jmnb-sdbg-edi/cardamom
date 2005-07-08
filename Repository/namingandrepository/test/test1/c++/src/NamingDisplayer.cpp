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


#include "test1/NamingDisplayer.hpp"
#include <iostream>


std::string NamingDisplayer::tab_string(int level)
{

    std::string tab;

    for (int i = 0; i < level; i++)
    {
        tab += "    ";

    }
    
    return tab;

}


void NamingDisplayer::show_chunk(const CosNaming::BindingList & bl, CosNaming::NamingContext_ptr nc, int level)
{

    for (CORBA::ULong i = 0; i < bl.length(); ++i) {
        // Note from CosNaming specification: 
        //   In struct Binding, binding_name is incorrectly defined
        //   as a Name instead of a NameComponent. This definition is
        //   unchanged for compatibility reasons.

        std::string tab = tab_string(level);

        std::cout << tab << bl[i].binding_name[0].id.in();

        if (bl[i].binding_name[0].kind[0] != '\0')
            std::cout << " . " << bl[i].binding_name[0].kind.in();

        if (bl[i].binding_type == CosNaming::ncontext)
        {
            std::cout << " : context" << std::endl;

            try
            {
                CORBA::Object_var obj = nc->resolve(bl[i].binding_name);
                CosNaming::NamingContext_var nc2 = CosNaming::NamingContext::_narrow(obj.in());

                list_context(nc2.in(), level+1);

            }
            catch(...)
            {

            }

        }
        else
            std::cout << " : reference" << std::endl;
    }
}


void NamingDisplayer::list_context(CosNaming::NamingContext_ptr nc, int level)
{
    CosNaming::BindingIterator_var it;      // Iterator reference
    CosNaming::BindingList_var     bl;      // Binding list
    const CORBA::ULong CHUNK = 100;         // Chunk size

    nc->list(CHUNK,bl,it);                  // Get first chunk
    show_chunk(bl.in(), nc, level);                         // Print first chunk

    if (!CORBA::is_nil(it.in())) {               // More bindings?
        CORBA::Boolean more;
        do {
            more = it->next_n(CHUNK,bl);    // Get next chunk
            show_chunk(bl.in(), nc, level);      // Print chunk
        } while (more);                     // While not done
        it->destroy();                      // Clean up
    }
}


