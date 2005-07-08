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


package cdmw.eventadmin;

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;

import cdmw.cdmwinit.ProcessControl;
import cdmw.namingandrepository.RepositoryInterface;

public class ManagerProcessControl extends ProcessControl {

    private org.omg.CORBA.ORB orb;

    private com.thalesgroup.CdmwNamingAndRepository.NameDomain eventDomain;

    private EventChannelProfileManagerImpl profileManager;

    ManagerProcessControl(EventChannelProfileManagerImpl ecpmi) {
        this.profileManager = ecpmi;
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void  onInitialise(com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
        throws org.omg.CORBA.SystemException {

        try {
            System.out.println("INFO: cdmw_event_channel_manager init");

            // getting the default root context from the repository
            Repository repository = RepositoryInterface.getRepository();
            org.omg.CosNaming.NamingContext tempRootCtx
                = repository.resolve_root_context(DEFAULT_ROOT_CONTEXT.value);

            // getting the "CDMW/SERVICES/EVENT" name domain from the repository
            eventDomain = repository.resolve_name_domain("CDMW/SERVICES/EVENT");

            // registering profileManager in domain EVENT
            eventDomain.register_new_object(EventChannelProfileManagerImpl.name,
                profileManager._this());
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            nnd.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(nnd.getMessage());
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            nrc.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(nrc.getMessage());
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(in.getMessage());
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            System.out.println("EventChannelProfileManager already registred! Continue...");
            // Persistent Server so its not a pb!
            // continue...
        }
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        System.out.println("INFO: cdmw_event_channel_manager nextStep");
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void onRun()
        throws  com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun,
                org.omg.CORBA.SystemException {

        System.out.println("INFO: cdmw_event_channel_manager run");

        if (profileManager == null) {
            System.out.println("ERROR: EventChannelProfileManager object not intanciated");
            throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun();
        }
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void onStop() throws org.omg.CORBA.SystemException {

        System.out.println("INFO: cdmw_event_channel_manager stop");
        try {
            eventDomain.release_name(EventChannelProfileManagerImpl.name);
            eventDomain = null;
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(in.getMessage());
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
            nr.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(nr.getMessage());
        } catch (org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            throw se;
        }
    }

}


