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


package cdmw.event;

import cdmw.common.Locations;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CdmwEvent.CreationError;
import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManager;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerHelper;
import com.thalesgroup.CdmwEventAdmin.EventChannelProperty;
import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerName;

import org.omg.CORBA.ORB;
import org.omg.CORBA.Any;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;

/**
 * This class provides utils used by event channel factory.
 *
 */
public class OrbacusEventChannelUtils {

    /**
     * Name of the IDL property
     */
    public static final String IDL_PROPERTY = "IDL";

    /**
     * Find the ORBACUS factory able to create the event channel 
     * This will read event channel profile if any and choose the 
     * corresponding OB factory
     * This method will be instanciate with typed and untyped
     * OBEventChannelFactory.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param channelName Name of the channel
     * @param defaultFactoryName Default factory name
     * @return The newly created ORBACUS factory
     * @exception CreationError Unable to create the factory,
     * check the reason for more details
     * @exception SystemException Any CORBA System Exception
     */
    public static org.omg.CORBA.Object 
        findOBFactory(ORB orb, String channelName, String defaultFactoryName)
        throws CreationError, SystemException {
            
        String URL_PROPERTY = "URL";
        EventChannelProfileManager manager = null;
        EventChannelProperty[]  channelProfile = null;
        org.omg.CORBA.Object obj = null;
            
        //
        // Get Event channel properties
        //
        try {
            // i) Get NamingInterface object from RepositoryInterface
            NamingInterface ni = 
                RepositoryInterface.getDomainNamingInterface(
                    Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN);

            // ii) Get a reference on the EventChannelProfileManager
            obj = ni.resolve(EventChannelProfileManagerName.value);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            throw new CreationError("Cdmw event service domain not specified.");
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new CreationError("Cdmw event service domain invalid.");
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            throw new CreationError("EventChannelProfileManager not found.");
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            throw new CreationError("Can not proceed resolve on EventChannelProfileManager.");
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {       
            throw new CreationError("Invalid name for EventChannelProfileManager.");
        }
            
        manager = EventChannelProfileManagerHelper.narrow(obj);
        if (manager==null) {
            throw new CreationError("EventChannelProfileManager is a nil object reference.");
        }
            
        //
        // iii) Find channel properties
        //
        try {
            channelProfile = manager.find_profile(channelName);
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            // No profile defined for this channel
            // iv) Find Factory
            // create it in the event server defined in the orb configuration.
            try {
                obj = orb.resolve_initial_references(defaultFactoryName);
                if (obj==null) {
                    throw new CreationError("No default event channel factory exists.");
                }
            } catch (org.omg.CORBA.ORBPackage.InvalidName in) {
                throw new CreationError("No default event channel factory exists.");
            }

        } catch (Exception e) {
            throw new CreationError("Unexpected exception raised!");
        }

        // iv) Find Factory
        if (channelProfile!=null) {
            String url = "";
            String propertyName;
            boolean isURLDefined = false;
            // A profile is defined for this channel, read it.

            for (int i=0; i<channelProfile.length; i++) {
                propertyName = channelProfile[i].name;
                if ( propertyName.equals(URL_PROPERTY) ) {
                    isURLDefined = true;
                    url = channelProfile[i].value;
                    // get out 'for' loop
                    break; 
                }
            }

            if (isURLDefined) {
                try {
                    System.out.println("INFO: create channel named " 
                        + channelName + " using the factory URL= " + url);

                    obj = orb.string_to_object(url);

                    if (obj==null) {
                        throw new CreationError(
                            "Invalid factory url for channel " + channelName);
                    }
                    
                } catch (org.omg.CORBA.TRANSIENT t) {
                    throw new org.omg.CORBA.TRANSIENT(
                        ExceptionMinorCodes.TRANSIENTCannotReachEventServer,
                            CompletionStatus.COMPLETED_NO);
                } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
                    throw new CreationError(
                        "Invalid factory url for channel " + channelName);
                }

            } else {
                throw new CreationError(
                    "Invalid factory url for channel " + channelName);
            }
        } else {
            // No profile defined, check if a local factory has been found
            /*
            if (factory==null) {
                throw new CreationError("Channel p is nil.");
            }
            */
        }
            
        //return factory;
        return obj;
        
    }

    /**
     * Find the supported interface defined for a given event channel. 
     * This will read event channel profile is any and extract the 
     * corresponding interface name.
     *
     * @param channelName the name of the event channel to find 
     * @return The supported interface corresponding to the given event channel
     * @exception CreationError Unalbe to find the interface, 
     * check the reason for more details
     * @exception SystemException Any CORBA System Exception
     */
    public static String findInterface(String channelName) 
        throws CreationError, SystemException {
        
        EventChannelProfileManager manager = null;
        EventChannelProperty[] channelProfile = null;

        org.omg.CORBA.Object obj = null;

        //
        // Get Event channel properties
        //
        try {
            // i) Get NamingInterface object from RepositoryInterface
            NamingInterface ni = 
                RepositoryInterface.getDomainNamingInterface(
                    Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN);

            // ii) Get a reference on the EventChannelProfileManager
            obj = ni.resolve(EventChannelProfileManagerName.value);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            throw new CreationError("Cdmw event service domain not specified.");
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new CreationError("Cdmw event service domain invalid.");
        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            throw new CreationError("EventChannelProfileManager not found.");
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            throw new CreationError("Can not proceed resolve on EventChannelProfileManager.");
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {       
            throw new CreationError("Invalid name for EventChannelProfileManager.");
        }

        manager = EventChannelProfileManagerHelper.narrow(obj);
        if (manager==null) {
            throw new CreationError("EventChannelProfileManager is a nil object reference.");
        }

        //
        // iii) Find channel properties
        //
        try {
            channelProfile = manager.find_profile(channelName);
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            // no profile defined for this channel nothing to do!      
        } catch(Exception e) {}

        // iv) Find interface key
        String propertyName;
        String interfaceKey = "";        
        boolean isInterfaceKeyDefined = false;
        
        if (channelProfile!=null) {
            // A profile is defined for this channel, read it.
            for (int i=0; i<channelProfile.length ; i++) {
                propertyName = channelProfile[i].name;
                if ( propertyName.equals(IDL_PROPERTY) ) {
                    isInterfaceKeyDefined = true;
                    interfaceKey = channelProfile[i].value;
                    // get out 'for' loop
                    break; 
                }
            }
        }
            
        return interfaceKey;
        
    }

    /**
     * Register the name objectKey within the NameDomain.
     * This will check validity of the name and whether it is already in use.
     *
     * @param domain Name domain where the name objectKey will be registered
     * @param objectKey The name to be registered
     * @return Registration ID of the objectKey
     * @exception com.thalesgroup.CdmwLifeCycle.AlreadyExistant 
     * The name objectKey is already registered within the NameDomain
     * @exception com.thalesgroup.CdmwLifeCycle.InvalidName
     * The name objectKey is invalid
     * @exception SystemException Any CORBA System Exception
     */
    public static Any registerName(NameDomain domain,String objectKey)
        throws com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
               com.thalesgroup.CdmwLifeCycle.InvalidName,
               SystemException {

        Any regId = null;
        try {
            regId = domain.new_name(objectKey);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            // Add more information to help the caller in debuging
            throw new com.thalesgroup.CdmwLifeCycle.AlreadyExistant(objectKey);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new com.thalesgroup.CdmwLifeCycle.InvalidName(in.reason);
        } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
            // Oops! CDMW Repository has been destroyed
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryNotExists,
                    CompletionStatus.COMPLETED_NO);
        } catch (SystemException se) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_NO);
        }
        return regId;
               
    }

    /**
     * Register (name,object) within the repository. The name has already been
     * registered, so this should succeed. In case of a TRANSIENT system 
     * exception, there will be a retry maxRetries times. if maxRetries == 0,
     * then this will do retry forever.
     *
     * @param domain Name domain where the (name, object) will be registered
     * @param regID Registration ID of the name that has been registered
     * @param theObject The object to be registered
     * @param maxRetries Number of max retries to do in case of failure
     * @exception SystemException Any CORBA System Exception
     */
    public static void registerObject(NameDomain domain,Any regId,
        org.omg.CORBA.Object theObject,int maxRetries)
        throws SystemException {
        
        boolean forever = (maxRetries <= 0);
        int counter = maxRetries;

        do {
            try {
                domain.register_object(regId,theObject);
                break;
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                        CompletionStatus.COMPLETED_YES);
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.InvalidRegistration ir) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                        CompletionStatus.COMPLETED_YES);
            } catch (org.omg.CORBA.TRANSIENT t) {
                // Retry
            }
        } while (forever || (--counter > 0) );

        if (!forever && (counter <= 0)) {
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                    CompletionStatus.COMPLETED_YES);
        }
        
    }

}


