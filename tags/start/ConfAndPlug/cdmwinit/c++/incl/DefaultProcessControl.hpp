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


#ifndef INCL_CDMW_INIT_DEFAULT_PROCESS_CONTROL_HPP
#define INCL_CDMW_INIT_DEFAULT_PROCESS_CONTROL_HPP

#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"

namespace Cdmw
{
namespace CdmwInit
{

/**
* Purpose:
* <p>
* The Default Process Control class
*/
class DefaultProcessControl : public ProcessControl
{

public:
    /**
     * Purpose:
     * <p> The constructor
     */ 
    DefaultProcessControl() throw ();

    /**
     * Purpose:
     * <p> The destructor.
     */ 
    virtual ~DefaultProcessControl() throw ();

    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    virtual 
    void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    virtual void on_next_step()
        throw(CdmwPlatformMngt::Process::InvalidStep, 
	      CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    virtual void on_run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun, 
	      CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    virtual void on_stop()
        throw(CORBA::SystemException);

private:
    DefaultProcessControl(const DefaultProcessControl&);
    DefaultProcessControl&
    operator=(const DefaultProcessControl&);

};


}; // End namespace CdmwInit
}; // End namespace Cdmw

#endif // INCL_CDMW_INIT_DEFAULT_PROCESS_CONTROL_HPP

