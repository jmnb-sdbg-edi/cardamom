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

#include <fstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/idllib/CdmwFTCurrent.stub.hpp>

#include <testretentionid/RetentionIdTester_impl.hpp>



namespace CdmwFT
{
namespace Test
{


RetentionIdTester_impl::RetentionIdTester_impl(CORBA::ORB_ptr orb)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
{ 
}


/**
 * Sleep during a while, and send information about the request
 * (incoming time, retention id and expiration time) as return
 * type or into a MyException.
 */
RequestInfos RetentionIdTester_impl::test_request(CORBA::UShort sleep_time, 
                                                  CORBA::Boolean send_exception)
     throw(CORBA::SystemException, MyException)
{
    Cdmw::OsSupport::OS::Timeval now = Cdmw::OsSupport::OS::get_time();
    
    RequestInfos infos;
    
    // FIXME: TAO Does not seem use DCE time timebase for the expiration_time
    // but uses rather Posix time!
    // TimeVal is in sec/microsec from 1st Jan 1970 (This is different in Windows!!!)
    infos.reception_time = 
        static_cast<TimeBase::TimeT>(now.seconds ) * 10000000 +
        static_cast<TimeBase::TimeT>(now.microseconds) * 10;
    
    try {
        // get FTCurrent object
        CORBA::Object_var obj = m_orb->resolve_initial_references("FTCurrent");
        CdmwFT::Current_var ft_current = CdmwFT::Current::_narrow(obj.in());

        // get retention_id and expiration_time
        infos.retention_id = ft_current->get_retention_id();
        infos.expiration_time = ft_current->get_expiration_time();
    
    } catch (const CORBA::ORB::InvalidName& ) {
        std::cerr << "FTCurrent is an unknown ObjectID!" << std::endl;
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_NO);
    } catch (const CORBA::SystemException& e) {
        std::cerr << "CORBA::SystemException raised : " << e << std::endl;
        throw;
    }
    
    std::cout << "  |  RetentionIdTester: RequestInfos:\n" 
              << "  |       reception_time  = " << infos.reception_time << "\n"
              << "  |       retention_id    = " << infos.retention_id << "\n"
              << "  |       expiration_time = " << infos.expiration_time << "\n"
              << std::endl;

    if (sleep_time != 0) {
        std::cout << "  |  RetentionIdTester: sleeping " 
                  << sleep_time << " ms..." << std::endl;
        try {
            Cdmw::OsSupport::OS::sleep(sleep_time);
        } catch (Cdmw::Exception e) {
            std::cerr << "  |  RetentionIdTester: error during sleep:\n"
                      << e << std::endl;
        }
    }
    
    if (send_exception) {
        std::cout << "  |  RetentionIdTester: throw MyException" << std::endl;
        throw MyException(infos);
    }
    
    return infos;
}


}; // End namespace Test

}; // End namespace Cdmw

