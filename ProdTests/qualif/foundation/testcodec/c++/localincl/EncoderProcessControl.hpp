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

#ifndef INCL_ENCODER_PROCESS_CONTROL_HPP
#define INCL_ENCODER_PROCESS_CONTROL_HPP

#include <iostream>

#include <Foundation/orbsupport/CORBA.hpp>
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Foundation/testutils/Testable.hpp>

#include "testcodec/TestCodec.stub.hpp"



namespace Cdmw
{
namespace TestCodec
{

class EncoderProcessControl : public Cdmw::CdmwInit::ProcessControl,
                              virtual Cdmw::TestUtils::Testable 
{
    
  public:

    EncoderProcessControl (CORBA::ORB_ptr orb)
        throw(CORBA::SystemException);
    
    ~EncoderProcessControl()
        throw();
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
     * operation
     */
    virtual  
    void on_initialise (const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/run:1.0
     * operation
     */
    virtual 
    void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
	          CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/stop:1.0
     * operation
     */
    virtual 
    void on_stop()
        throw(CORBA::SystemException);

        
  private:
  
    //
    // Testable operation
    //
    void do_tests();

    /**
    * The orb reference.
    */
    CORBA::ORB_var m_orb;
    
    Cdmw::TestCodec::DataTransfer_var m_dataTransfer;
    
    
    
};  // End class EncoderProcessControl

}; // End namespace TestCodec
}; // End namespace Cdmw

#endif // INCL_HELLO_CLIENT_PROCESS_CONTROL_HPP
