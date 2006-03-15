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


#ifndef INCL_ONINIT_INTERFACE_IMPL_HPP 
#define INCL_ONINIT_INTERFACE_IMPL_HPP 

#include <FaultTolerance/ftstatemanager/DataStoreInitialiser.hpp>


namespace Cdmw
{

    template <class Oid, class Data, class OidCompare=std::less<Oid> > 
    class OnInit_impl : public Cdmw::FT::DataStoreInitialiser<Oid, Data, OidCompare>
    {

    public:
        

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */        
        OnInit_impl()
        {
        }
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        ~OnInit_impl() throw() 
        {
        }
        
    
        typedef typename Cdmw::FT::DataStore<Oid,Data, OidCompare>::DataInsertIterator DataInsertIterator;
    
        void on_init(DataInsertIterator it, bool cold_restart = true) 
            throw (Cdmw::InitErrorException)
        {

            if (cold_restart)
            {
                std::cout<<" -------------- ColdRestart -------------- "<<std::endl;
                std::ofstream os("ColdRestartFile.result");
                os << "ColdRestartFile";
                os.close();
            } else {

                std::cout<<" -------------- HotRestart -------------- "<<std::endl;
                std::ofstream os("HotRestartFile.result");
                os << "HotRestartFile";
                os.close();
            }
            
        }

    private:

        // Hide copy constructor/assignment operator
        OnInit_impl(const OnInit_impl& rhs)
            throw();
        
        OnInit_impl&
        operator=(const OnInit_impl& rhs)
            throw();
        


    }; // End class OnInit_impl


}; // End namespace Cdmw

#endif // INCL_ONINIT_INTERFACE_IMPL_HPP 

