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


package cdmw.lifecycle;

import cdmw.ossupport.ReadWriteLock;
import cdmw.common.Assert;

import com.thalesgroup.CdmwLifeCycle.EntityStrategy.EntityDatum;
import com.thalesgroup.CdmwLifeCycle.EntityStrategy.EntityDataHelper;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;
import com.thalesgroup.CdmwNamingAndRepository.InvalidName;
import com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered;

import org.omg.CosPropertyService.PropertySet;
import org.omg.CosPropertyService.InvalidPropertyName;
import org.omg.CosPropertyService.PropertyNotFound;

/**
 * This class provides a Cleanup Notifier for Life Cycle Framework
 * with "Entity" strategy. It maintains (object key, object Id) associations
 * of CDMW objects registered with a NameDomain. This enables unregistration
 * of the object key from the NameDomain when needed.</p>
 *
 * TODO: Add a ReadersWriter lock for Thread Safety
 *
 */
public class EntityCleanupNotifier extends CleanupNotifier {

    /**
     * Object reference of the NameDomain where CDMW objects whose
     * (oid,key) pairs are stored in self are registered.
     */
    private NameDomain domain;

    /**
     * Internal Entries Database.
     */
    private EntriesMap entries = new EntriesMap();

    /**
     * Property set for state storage/retreival
     */
    private PropertySet propertySet = null;

    /**
     * Name of the property within the property set
     */
    private String propertyName = new String();

    /**
     * Readers/Writer Lock to protect from concurrent access to internal data
     */
    private ReadWriteLock readWriteLock = new ReadWriteLock();

    /**
     * This class is used to maintain (object key, object Id) associations
     */
    class EntriesMap {

        private java.util.HashMap map = new java.util.HashMap();

        public byte[] getObjectId(String key) {
            return ( (String) map.get(key) ).getBytes();
        }

        public String getKey(byte[] oid) {
            java.util.Set entries = map.entrySet();
            String objectId = new String(oid);
            for (java.util.Iterator i = entries.iterator() ; i.hasNext() ; ) {
                java.util.Map.Entry entry = (java.util.Map.Entry) i.next();
                if ( entry.getValue().equals(objectId) ) {
                    return (String) entry.getKey();
                }
            }
            return null;
        }

        public void insertEntry(byte[] oid, String key) {
            // NB: the first value of the HashMap is the key
            map.put( key, new String(oid) );
        }

        public void removeByObjectId(byte[] oid) {
            map.remove(getKey(oid));
        }

        public void removeByKey(String key) {
            map.remove(key);
        }

        public boolean isEmpty() {
            return map.isEmpty();
        }

        public int size() {
            return map.size();
        }

        public boolean containsObjectId(byte[] oid) {
            return this.map.containsValue( new String(oid) );
        }

        public boolean containsKey(String key) {
            return this.map.containsKey( key );
        }

        public String dump() {
            java.util.Set entries = map.entrySet();
            StringBuffer output = new StringBuffer();
            for (java.util.Iterator i = entries.iterator() ; i.hasNext() ; ) {
                java.util.Map.Entry entry = (java.util.Map.Entry) i.next();
                output.append("<ObjectId = " + (String) entry.getValue() +
                    ", Key = " + (String) entry.getKey() + ">\n");
            }
            return output.toString();
        }

        public EntityDatum[] toEntityData() {
            java.util.Set entries = map.entrySet();
            EntityDatum[] ed = new EntityDatum[map.size()];
            int j = 0;
            for (java.util.Iterator i = entries.iterator() ; i.hasNext() ; ) {
                java.util.Map.Entry entry = (java.util.Map.Entry) i.next();
                byte[] oid = ((String) entry.getValue()).getBytes();
                String key = (String) entry.getKey();
                ed[j] = new EntityDatum( oid, key );
                j++;
            }
            return ed;
        }

    }


    /**
     * Constructor
     *
     * @param domain The CDMW NameDomain where stored object keys are
     * registered.
     */
    public EntityCleanupNotifier(NameDomain domain) {
        this.domain = domain;
    }

    /**
     * Unregisters the objet key associated to the specified <I>oid</I> from
     * the NameDomain and deletes its entry from the internal entries database.
     *
     * @param oid The Object Id of the CDMW object registered
     * with the NameDomain.
     */
    public void unregisterObject(byte[] oid) {

        Assert.check( entries!=null );
        readWriteLock.writeLock();
        
        boolean error = false;
        try {
            String key = entries.getKey(oid);
            if (key != null) {                
                this.domain.release_name(key);
                this.entries.removeByObjectId(oid);
            } else {
                error = true;
            }
        } catch (NotRegistered nr) {
            error = true;
            // This should not happen (TODO): ADD Trace logs
        } catch (InvalidName in) {
            error = true;
            // This should not happen either (TODO): ADD Trace logs
        } finally {
            readWriteLock.writeUnlock();
        }
        
        if (error) {
            System.out.println("Dumping table...");
            dump(System.out);
        }

    }

    /**
     * Inserts an (oid,key) pair to the internal database.
     *
     * @param oid The Object Id of a CDMW object registered
     * within the NameDomain.
     * @param key The Object Key used for the object's registration
     * in the NameDomain.
     *
     * @return <B>true</B> if inserted successfuly, otherwise <B>false</B>
     * is returned (the entry may already be there).
     */
    public boolean insertEntry(byte[] oid, String key) {

        Assert.check( entries!=null );

        readWriteLock.writeLock();
        this.entries.insertEntry(oid,key);
        readWriteLock.writeUnlock();
        return true;

    }

    /**
     * Removes the entry with the specified <I>oid</I>
     * from the internal database.
     *
     * @param oid The object Id field of the pair to be removed.
     *
     * @return <B>true</B> if removed successfuly, otherwise <B>false</B>
     * is returned (the entry may not be present).
     */
    public boolean removeEntry(byte[] oid) {

        Assert.check( entries!=null );

        readWriteLock.writeLock();
        this.entries.removeByObjectId(oid);
        readWriteLock.writeUnlock();
        return true;

    }

    /**
     * Removes the entry with the specified <I>key</I>
     * from the internal database.
     *
     * @param key The object key field of the pair to be removed.
     *
     * @return <B>true</B> if removed successfuly, otherwise <B>false</B>
     * is returned (the entry may not be present).
     */
    public boolean removeEntry(String key) {

        Assert.check( entries!=null );

        readWriteLock.writeLock();
        this.entries.removeByKey(key);
        readWriteLock.writeUnlock();
        return true;

    }

    /**
     * This tests whether a entry with the specified <I>key</I> is present
     * in the internal database.
     *
     * @param key The object key of the requested entry.
     *
     * @return <B>true</B> if an entry with the specified <I>key</I> field is
     * present in the internal database. Otherwise return <B>false</B>.
     */
    public boolean contains(String key) {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        boolean result = this.entries.containsKey(key);
        readWriteLock.readUnlock();

        return result;

    }

    /**
     * This tests whether a entry with the specified <I>oid</I> is present
     * in the internal database.
     *
     * @param oid The object Id of the requested entry.
     *
     * @return <B>true</B> if an entry with the specified <I>oid</I> field is
     * present in the internal database. Otherwise return <B>false</B>.
     */
    public boolean contains(byte[] oid) {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        boolean result = this.entries.containsObjectId(oid);
        readWriteLock.readUnlock();

        return result;

    }

    /**
     * Returns the number of entries in the internal database.
     *
     * @return The number of entries.
     */
    public int entries() {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        int result = this.entries.size();
        readWriteLock.readUnlock();

        return result;

    }

    /**
     * This dumps content of the internal database to the specified
     * output stream.
     *
     * @param out A reference to an output printstream.
     */
    public void dump(java.io.PrintStream out) {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        out.print("EntityCleanupNofifier  --- Size = " + entries() + "\n");
        out.print(this.entries.dump());
        readWriteLock.readUnlock();

    }

    /**
     * This operation initialises internal database using a property as
     * defined by the OMG CosPropertyService.
     *
     * @param propertySet  A reference to the PropertySet containing
     * the property.
     * @param propertyName Name of the property within the property set above.
     *
     * @return <B>true</B> if <I>init</I> was successful,
     * <B>false</B> otherwise.
     */
    public boolean init(PropertySet propertySet, String propertyName) {

        boolean result = false;

        if ( propertySet!=null ) {
            Assert.check(propertyName.length()!=0);

            // The follows steps are performed:
            // 1. Get value of property named "propetyName"
            // 2. For each value, insertEntry to internal entries database
            // 3. If PropertyNotFound exception is raised,
            //      it means it's first usage.
            //    Do not create a property at this time but do it
            //    in finish() before persisting state.
            try {
                this.propertySet = propertySet;
                this.propertyName = propertyName;

                org.omg.CORBA.Any any
                    = this.propertySet.get_property_value(propertyName);

                // Data is stored as sequence {oid;key}
                EntityDatum[] eData = EntityDataHelper.extract(any);

                for (int i = 0; i < eData.length; ++i) {
                    insertEntry(eData[i].object_id,eData[i].object_key);
                }
                result = true;
            } catch(PropertyNotFound pnf) {
                // Property not found, create at later!
                result = true;
            } catch(InvalidPropertyName ipn) {
                result = true;
            } catch(Exception e) {
                result = false;
            }
        }

        return result;

    }

    /**
     * This operation instructs the EntityCleanupNotifier that
     * the LifeCycle framework is done with it and can then persist its state
     * for next sessions.
     *
     * @param orb ORB reference used to create a Any object
     *
     * @return <B>true</B> if done successfuly, <B>false</B> otherwise.
     */
    public boolean finish(org.omg.CORBA.ORB orb) {

        boolean result = false;
        if (this.propertySet!=null) {
            try {
                // Create a CORBA Any containing current state
                readWriteLock.readLock();
                EntityDatum[] ed = this.entries.toEntityData();

                org.omg.CORBA.Any any = orb.create_any();
                EntityDataHelper.insert(any, ed);

                // This will either define a new property or set a new value
                // for the property.
                this.propertySet.define_property(this.propertyName, any);
                // State is persisted!
                result = true;
            } catch (org.omg.CORBA.UserException ue) {
                // Possible exceptions are:
                //        InvalidPropertyName,
                //        ConflictingProperty,
                //        UnsupportedTypeCode,
                //        UnsupportedProperty,
                //        ReadOnlyProperty

                // do nothing for this version
            } finally {
                readWriteLock.readUnlock();
            }

        }

        return result;

    }

    /**
     * This returns the <I>key</I> value of the entry with the specified
     * object Id.
     *
     * @param oid Object Id corresponding to the string returned.
     *
     * @return The object key value of the entry.
     */
    protected String objectIdToKey(byte[] oid) {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        String key = this.entries.getKey(oid);
        readWriteLock.readUnlock();

        return key;

    }

    /**
     * This returns the Object Id value of the entry with the specified
     * object key.
     *
     * @param key Object key corresponding to the Object Id returned.
     *
     * @return The Object Id value of the entry.
     */
    protected byte[] keyToObjectId(String key) {

        Assert.check( entries!=null );

        readWriteLock.readLock();
        byte[] oid = this.entries.getObjectId(key);
        readWriteLock.readUnlock();

        return oid;

    }

}

