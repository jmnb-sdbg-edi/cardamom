/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
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
            Cdmw::OsSupport::OS::Timeval time =
                Cdmw::OsSupport::OS::get_time();
            std::cout << "Start of Initialisation " << time.seconds << " secs "
                      << time.microseconds << " usecs" << std::endl
                      << std::flush;
            for (int i = 0; i < 100000; i++)
            {
                *it = std::make_pair(i, "hello world");
                ++it;
            }

            OsSupport::OS::sleep(10000);

            m_end_time = Cdmw::OsSupport::OS::get_time();
            std::cout << "End of Initialisation " << m_end_time.seconds << " secs "
                      << m_end_time.microseconds << " usecs" << std::endl
                      << std::flush;
        }

        Cdmw::OsSupport::OS::Timeval time_end_on_init()
        {
            return m_end_time;
        }
        

    private:

        // Hide copy constructor/assignment operator
        OnInit_impl(const OnInit_impl& rhs)
            throw();
        
        OnInit_impl&
        operator=(const OnInit_impl& rhs)
            throw();
        
        Cdmw::OsSupport::OS::Timeval m_end_time;
        

    }; // End class OnInit_impl


}; // End namespace Cdmw

#endif // INCL_ONINIT_INTERFACE_IMPL_HPP 

