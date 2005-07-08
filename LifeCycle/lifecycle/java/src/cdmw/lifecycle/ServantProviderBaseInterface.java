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


/**
 * Base interface for all servant providers
 *
 */
public interface ServantProviderBaseInterface {

    /**
     * This method acts as a hand shaking between the Servant Provider
     * and the generated Factory implementation. It indicates whether
     * a Servant provided by this Servant Provider may incarnate multiple
     * objects (with ObjectIds provided by this Servant Provider)
     *
     * @return <B>true</B> if a servant may incarnate multiple objects,
     * <B>false</B> otherwise.
     */
    public boolean mayIncarnateMultipleObjects();

    /**
     * This method provides a servant for the CORBA object with
     * the specified ObjectId. It is called by the Life Cycle Framework
     * whenever a servant is needed to incarnate an object.
     *
     * @param oid ObjectId of the object to be incarnated
     *
     * @return A servant incarnating the object
     *
     * @exception ForwardRequest Indicates to the Life Cycle
     * Framework ORB that the SevantProvider cannot handle this object
     * and requests can be handled to the returned IOR. If this exception
     * is translated/forwarded to the client-side ORB, the request invocation
     * will be transparently re-transmitted to the returnded object.
     */
    public org.omg.PortableServer.Servant lookupServant(byte[] oid)
        throws org.omg.PortableServer.ForwardRequest;

    /**
     * This method is called by the Life Cycle Framework whenever it is done
     * with the specified servant. The ServantProvider is free to handle it at
     * will.
     *
     * @param oid ObjectId of the object incarnated by the servant
     * @param poa The POA where the object is activated
     * @param servant The servant in question
     */
    public void releaseServant(
        byte[] oid,
        org.omg.PortableServer.POA poa,
        org.omg.PortableServer.Servant servant);

    /**
     * This method returns the RepositoryID of the object associated to the
     * <I>oid</I>.
     *
     * @param oid ObjectId of the object
     *
     * @return The RepositoryId of the object
     *
     * @exception OBJECT_NOT_EXIST The object does not exist any more.
     */
    public String getSupportedInterface(byte[] oid)
        throws org.omg.CORBA.OBJECT_NOT_EXIST;

    /**
     * This method indicates whether the ServantProvider supports objects
     * with the specified Repository Id.
     *
     * @param repositoryId RepositoryID of an object
     *
     * @return <B>true</B> if objects with the specified repository ID are
     * supported by the ServantProvider, <B>false</B> otherwise.
     */
    public boolean supports(String repositoryId);

    /**
     * This returns a sequence of all RepositoryIDs of objects supported by
     * the current ServantProvider.
     *
     * @return A sequence of RepositoryIds as a String table
     * supported by the ServantProvider.
     */
    public String[] getSupportedObjects();

    // NOTA: notifier should not be destroyed!
    /**
     * Set the current cleanup notifier with the argument
     *
     * @param notifier A CleanupNotifier object
     */
    public void setCleanupNotifier(CleanupNotifier notifier);

}

