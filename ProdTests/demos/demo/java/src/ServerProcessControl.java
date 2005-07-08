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


package cdmw.resources;

import cdmw.common.Locations;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.cdmwinit.ProcessControl;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.platformmngt.PlatformInterface;

import cdmw.event.EventChannel;
import cdmw.event.TypedEventChannel;

import cdmw.resources.EchoImpl;

public class ServerProcessControl extends ProcessControl {

    public static final String INTERFACE_REF =
        "IDL:thalesgroup.com/TypedEcho:1.0";

    private org.omg.CORBA.ORB orb;
    private org.omg.PortableServer.POA poa;
    private String serverName;
    private String channelName;
    private String completeChannelName;
    private String completeTypedChannelName;

    public ServerProcessControl(org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa,
        String serverName,
        String channelName,
        String completeChannelName,
        String completeTypedChannelName) {
        this.orb = orb;
        this.poa = poa;
        this.serverName = serverName;
        this.channelName = channelName;
        this.completeChannelName = completeChannelName;
        this.completeTypedChannelName = completeTypedChannelName;
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
        throws org.omg.CORBA.SystemException {

        // This call is legal since Cdmw services are initialised
        // before calling this operation.

        /*
        try {
            // Get NamingInterface object from RepositoryInterface
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface
                (Locations.CDMW_SERVICES_NAME_DOMAIN);

            // Get Event service naming context
            org.omg.CORBA.Object obj = ni.resolve("EVENT");

            org.omg.CosNaming.NamingContext nc =
                org.omg.CosNaming.NamingContextHelper.narrow(obj);

            // Connect server1 to TypedEventChannel1 as consumer
            TypedEventChannel typedeventchannel1 =
                new TypedEventChannel(completeTypedChannelName, nc);

            EchoImpl pcImpl = new EchoImpl(orb,
                                           poa,
                                           serverName,
                                           channelName,
                                           completeChannelName);

            com.thalesgroup.TypedEcho ref = pcImpl._this(this.orb);
            typedeventchannel1.connectTypedPushConsumer(INTERFACE_REF, ref);
        } catch (Exception e) {
            System.err.println(serverName + ": unexpected exception raised!");
            throw new org.omg.CORBA.INTERNAL(
                cdmw.orbsupport.ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }
        */

    }


    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void onRun()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun,
        org.omg.CORBA.SystemException {

        /*
        try {
            // Get NamingInterface object from RepositoryInterface
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface
                (Locations.CDMW_SERVICES_NAME_DOMAIN);

            // Get Event service naming context
            org.omg.CORBA.Object obj = ni.resolve("EVENT");

            org.omg.CosNaming.NamingContext nc =
                org.omg.CosNaming.NamingContextHelper.narrow(obj);

            // Connect server1 to TypedEventChannel1 as consumer
            TypedEventChannel typedeventchannel1 =
                new TypedEventChannel(completeTypedChannelName, nc);

            EchoImpl pcImpl = new EchoImpl(orb,
                                           poa,
                                           serverName,
                                           channelName,
                                           completeChannelName);

            com.thalesgroup.EchoModule.TypedEcho ref = pcImpl._this(this.orb);
            typedeventchannel1.connectTypedPushConsumer(INTERFACE_REF, ref);
        } catch (Exception e) {
            System.err.println(serverName + ": unexpected exception raised!");
            throw new org.omg.CORBA.INTERNAL(
                cdmw.orbsupport.ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }
        */
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void onStop() throws org.omg.CORBA.SystemException {
        // do nothing
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        // do nothing
    }

}

