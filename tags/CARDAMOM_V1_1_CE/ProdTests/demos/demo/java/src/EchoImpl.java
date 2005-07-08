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

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.event.EventChannel;

public class EchoImpl extends com.thalesgroup.EchoModule.TypedEchoPOA {

    /**
     * the ORB
     */
    private org.omg.CORBA.ORB orb;

    /**
     * My POA
     */
    private org.omg.PortableServer.POA poa;

    /**
     * ref of the consumer to which msg received is pushed.
     */    
    private org.omg.CosEventComm.PushConsumer pushConsumer = null;
    
    private String serverName;
    private String channelName;
    private String completeChannelName;
    
    public String[] rcvMsgs = new String[10];
    public short nbRcvdMsg = 0;

    public EchoImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa,
        String serverName,
        String channelName,
        String completeChannelName) {

        this.orb = orb;
        this.poa = poa;
        this.serverName = serverName;
        this.channelName = channelName;
        this.completeChannelName = completeChannelName;

    }

    public void echo_string(String str) {
        
        System.out.println("Consumer in " + serverName + " has received: " + str);
        nbRcvdMsg++;
        
        try {
            if (nbRcvdMsg == 1) {
                // Get NamingInterface object from RepositoryInterface
                NamingInterface ni = RepositoryInterface.getDomainNamingInterface
                    (cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
                
                // Get Event service naming context
                org.omg.CosNaming.NamingContext nc =
                    org.omg.CosNaming.NamingContextHelper.narrow(
                        ni.resolve("EVENT"));
                                
                if (nc == null) {
                    System.err.println("NamingContext is a nil object reference");
                    return ;
                }
                
                // Connect server1 to EventChannel1 as supplier
                EventChannel eventchannel1 = 
                    new EventChannel(completeChannelName, nc);
                            
                try {
                    pushConsumer = eventchannel1.getPushConsumer(null);
                } catch (Exception e) {
                    System.err.println("impossible to get a push consumer on " + channelName);
                    return ;
                }
            }
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            System.err.println("No Name Domain " + 
                cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
            return ;
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            System.err.println("Invalid name " + 
                cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
            return ;
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            System.err.println(nf.toString());
            return ;
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            System.err.println(cp.toString());
            return ;
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            System.err.println(in.toString());
            return ;
        }
    
        org.omg.CORBA.Any data = orb.create_any();
        data.insert_string(str);
        if (pushConsumer != null) {
            try {
                pushConsumer.push(data);
            } catch(org.omg.CosEventComm.Disconnected d) {
                System.err.println(d.toString());
            }
        }
        
    }

    public void disconnect_push_consumer() {
        System.out.println("Disconnect");
        try {
            byte[] oid = poa.servant_to_id(this);
            poa.deactivate_object(oid);
        } catch(org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            ona.printStackTrace();
        } catch(org.omg.PortableServer.POAPackage.ServantNotActive sna) {
            sna.printStackTrace();
        } catch(org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            wp.printStackTrace();
        }
    }

    public void push(org.omg.CORBA.Any data) {
        System.out.println("PushConsumer in " + serverName + ": push any not implemented");
    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

}

