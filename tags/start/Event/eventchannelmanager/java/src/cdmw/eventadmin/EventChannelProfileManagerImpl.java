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

import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.common.InternalErrorException;
import cdmw.common.BadParameterException;

import com.thalesgroup.CdmwEventAdmin.EventChannelProperty;
import com.thalesgroup.CdmwEventAdmin.InvalidName;
import com.thalesgroup.CdmwEventAdmin.AlreadyExistant;
import com.thalesgroup.CdmwEventAdmin.NotExist;

import org.omg.CORBA.ORB;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.Policy;
import org.omg.CORBA.SystemException;
import org.omg.PortableServer.POA;

/**
 * This class implements the EventChannelProfileManager interface.
 * This is a singleton class.
 *
 */
public class EventChannelProfileManagerImpl 
    extends com.thalesgroup.CdmwEventAdmin.EventChannelProfileManagerPOA {

    /**
     * Name of the EventChannelProfileManager POA
     */ 
    public static final String EVENT_CHANNEL_PROFILE_MANAGER_POA 
        = "EventChannelProfileManagerPOA";

    /**
     * Name of the EventChannelProfileManager
     */ 
    public static final String EVENT_CHANNEL_PROFILE_MANAGER 
        = "EventChannelProfileManager";

    /**
     * Name of the EventChannelProfileManager
     */ 
    public static final String name = "EventChannelProfileManager";

    /**
     * A reference to the ORB pseudo-object
     */ 
    private ORB orb;
    
    /**
     * Its POA
     */
    private POA poa;
    
    /**
     * Map of channel profiles ordered by event channel name
     */
    private java.util.TreeMap channelProfiles = new java.util.TreeMap();

    /**
     * Singleton instance
     */    
    private static EventChannelProfileManagerImpl instance = null;
    
    /**
     * Reference on the entity responsible to read the XML configuration file
     */ 
    private EventChannelConfiguration eventChannelConf;


    /**
     * Return the single instance of the EventchannelProfileManager 
     *
     * @param orb A reference to the ORB pseudo-object
     * @param parent The parent POA of the POA to be created
     * @param XMLConfigurationFile Name of the XML configuration file
     * @param doValidate Validation option of the XML file
     * @return A new instance of EventChannelProfileManager
     *
     * @exception ConfigurationException Unable to create a new POA
     * @exception AlreadyActiveException EventChannelProfileManager 
     * already active
     * @exception InternalErrorException Internal error
     */ 
    public static EventChannelProfileManagerImpl init(
        ORB orb, 
        POA parent,
        String XMLConfigurationFile,
        boolean doValidate)
        throws ConfigurationException, 
                AlreadyActiveException,
                InternalErrorException {
        
        if (instance==null) {
            try {
                // Create policy list for simple persistence
                Policy[] pl = new Policy[3];
                
                pl[0] = parent.create_id_assignment_policy(
                    org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
                pl[1] = parent.create_lifespan_policy(
                    org.omg.PortableServer.LifespanPolicyValue.PERSISTENT);
                pl[2] = parent.create_implicit_activation_policy(
                    org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);

                // Get a reference to the POA manager.
                org.omg.PortableServer.POAManager manager = parent.the_POAManager();

                if (manager==null) {
                    // No POAmanager
                    throw new InternalErrorException();
                }

                // Create new POA
                cdmw.orbsupport.StrategyList poaStrategies 
                    = new cdmw.orbsupport.StrategyList();
                // Strategies with MULTITHREAD policy will accept Single Thread and
                // Multi Thread strategies. However, SERIALIZE policy will only allow
                // Single Thread policy.
                POA poa = null;

                try {
                    poa = cdmw.orbsupport.ORBSupport.createPOA(
                        parent,
                        EVENT_CHANNEL_PROFILE_MANAGER_POA,
                        manager, 
                        new cdmw.orbsupport.PolicyList(pl), 
                        poaStrategies); 
                } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
                    throw new ConfigurationException("POA Already Exists!");
                } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
                    throw new ConfigurationException("Invalid Policy for the new POA");
                } catch (SystemException se) {
                    throw new ConfigurationException(se.getMessage());
                }

                // Clean up
                for (int i = 0; i < pl.length; ++i) {
                   pl[i].destroy();
                }

                // create instance
                instance = new EventChannelProfileManagerImpl(
                    orb,
                    poa,
                    XMLConfigurationFile, 
                    doValidate);

                // Creation OK, activate object
                byte[] oid = EVENT_CHANNEL_PROFILE_MANAGER.getBytes();
                instance.poa.activate_object_with_id(oid, instance);

            } catch (org.omg.PortableServer.POAPackage.ObjectAlreadyActive oaa) {
                throw new AlreadyActiveException(name);
            } catch (org.omg.PortableServer.POAPackage.ServantAlreadyActive saa) {
                throw new AlreadyActiveException(name);
            } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
                throw new InternalErrorException();
            }

        }

        return instance;
        
    }

    /**
     * override this function because its poa is different from
     * the rootPOA.
     *
     * @return The default POA
     */
    public POA _default_POA() {
        return poa;
    }

    /**
     * add an event channel profile that is a list of properties.
     * 
     * @param channelName The event channel name as declared 
     * in the Cdmw Naming&Repository.
     * @param profile A set of properties associated to this event channel.
     *
     * @exception AlreadyExistant if a set of properties has already exist
     * for this channel.
     * @exception InvalidName if the event channel name has a bad format.
     * @exception SystemException Any CORBA System exception
     */ 
    //
    // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/create_profile:1.0
    //
    public void 
        create_profile(String channelName, EventChannelProperty[] profile)
        throws AlreadyExistant, InvalidName, SystemException {

        // check if a profile doesn't already exist for the specified channel
        EventChannelProperty[] channelProfile 
            = (EventChannelProperty[]) channelProfiles.get(channelName);

        if (channelProfile != null) {
            // a profile already exist for this channel
            throw new AlreadyExistant("This channel already has a profile.");
        } else {
            // check the validity of the name
            if (!isValid(channelName)) {
               throw new InvalidName("The name of the event channel has a bad format.");
            } else {
                // add profile in the map
                channelProfiles.put(channelName, profile);
            }
        }
        
    }

    /**
     * remove an event channel profile.
     * 
     * @param channelName The event channel name.
     *
     * @exception NotExist if the event channel doesn't have any profile.
     * @exception SystemException Any CORBA System exception
     */ 
    //
    // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/remove_profile:1.0
    //
    public void remove_profile(String channelName)
        throws NotExist, SystemException {
        
        // try to erase data in the map
        if (channelProfiles.remove(channelName)==null) {
            // the key doesn't exist in the map
            throw new NotExist("No profile for this event channel.");
        }
        
    }

    /**
     * update an event channel profile with a new one.
     * 
     * @param channelName The event channel name.
     *
     * @exception SystemException Any CORBA System exception
     * @exception NotExist if the event channel doesn't have any profile.
     */
    //
    // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/update_profile:1.0
    //
    public void 
        update_profile(String channelName, EventChannelProperty[] profile)
        throws NotExist, SystemException {
        
        // try to erase data in the map
        if (channelProfiles.remove(channelName)==null) {
            // the key doesn't exist in the map
            throw new NotExist("No profile for this event channel.");
        }
        // profile has been deleted, insert the new one!
        channelProfiles.put(channelName, profile);
        
    }

    /**
     * This operation return the profile associated to an event channel.
     * 
     * @param channelName The event channel name.
     *
     * @return EventChannelProperty[]  A pointer on the profile of the
     * specified event channel
     * @exception SystemException Any CORBA System exception
     * @exception NotExist if the event channel doesn't have any profile.
     */
    //
    // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/find_profile:1.0
    //
    public EventChannelProperty[] find_profile(String channelName)
        throws NotExist, SystemException {
        
        // check if a profile exists for the specified channel
        EventChannelProperty[] pProfile 
            = (EventChannelProperty[]) channelProfiles.get(channelName);

        if (pProfile == null && !channelProfiles.containsKey(channelName)) {
            throw new NotExist("No profile found for this event channel.");
        }

        return pProfile;
        
    }

    /**
     * This operation return the list of managed event channel.
     *
     * @return A set of event channel name having a profile.
     *
     * @exception SystemException Any CORBA System exception
     */
    public String[] managed_event_channels()
         throws SystemException {
         
        String[] managedEventChannels = new String[channelProfiles.size()];
        java.util.Iterator channelIterator 
            = channelProfiles.keySet().iterator();
        // the result array is sorted
        int i = 0;

        while ( channelIterator.hasNext() ) {
            managedEventChannels[i] = (String) channelIterator.next();
            i++;
        }

        return managedEventChannels;
         
    }

    /**
     * This operation save all the current managed profiles in an XML file.
     *
     * @exception AlreadyExistant if the XML file already exists
     * @exception SystemException Any CORBA System exception
     */
    public void save_profiles() 
        throws AlreadyExistant, SystemException {
        
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTNotYetImplemented, 
                CompletionStatus.COMPLETED_NO);
    }

    /**
     * This operation deactivates the servant and permanently removes its 
     * persistent state.
     *
     * @exception SystemException Any CORBA System exception
     */
    public void destroy() throws SystemException {
    
        byte[] oid = EVENT_CHANNEL_PROFILE_MANAGER.getBytes();
        try {
            poa.deactivate_object(oid);
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {}

        // Terminate event loop
        orb.shutdown(false);
    
    }

    /**
     * Constructor
     * 
     * @param orb A reference to the ORB pseudo-object
     * @param poa The parent POA of the POA to be created
     * @param XMLConfigurationFile Name of the XML configuration file
     * @param doValidate Validation option of the XML file
     * @exception InternalErrorException Internal error
     * @exception ConfigurationException Error in the configuration file
     */ 
    protected EventChannelProfileManagerImpl(ORB orb, 
        POA poa, String XMLConfigurationFile, boolean doValidate)
        throws InternalErrorException, ConfigurationException {

        this.orb = orb;
        this.poa = poa;
        int i = 0; // event channel index

        try {
            // get a reference on the entity responsible 
            // to read the XML configuration file.
            this.eventChannelConf = EventChannelConfiguration.
                                        readConfiguration(XMLConfigurationFile,
                                            doValidate);

            String channelName;
            EventChannelProperty[] channelProfile;
            java.util.Vector channelProperties;
            int nbProfile = this.eventChannelConf.getNbEventChannelProfile();

            for ( ; i < nbProfile; i++) {
                // get one by one event channel profile
                channelName = this.eventChannelConf.getNextEventChannelName(i);
                channelProperties 
                    = this.eventChannelConf.
                        getEventChannelProfileValue(channelName);

                // store data
                channelProfile 
                    = new EventChannelProperty[channelProperties.size()];

                int j =0;
                java.util.Enumeration e = channelProperties.elements();
                for (; e.hasMoreElements(); ) {
                    channelProfile[j] = (EventChannelProperty) e.nextElement();
                    j++;
                }

                this.channelProfiles.put(channelName, channelProfile);
            }
        } catch (BadParameterException bpe) {
            throw new InternalErrorException();
        }
        
    }

    /**
     * check if the name is a valid name
     *
     * @param s The string to check
     * @return <b>true</b> if the name is valid, <b>false</b> otherwise
     */ 
    protected boolean isValid(String s) {
    
        boolean valid = false;
        try {
            org.omg.CosNaming.NameComponent[] name 
                = cdmw.namingandrepository.NamingInterface.toName(s);
            if (name.length == 1) {
                valid = true;
            }
        } catch (cdmw.namingandrepository.InvalidNameException ine) {}
        return valid;
    
    }

}


