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


package cdmw.orbsupport;

/**
 * This interface provides a set of methods to implement for each ORB.
 *
 * Features:
 * Thread safe.
 *
 */
public interface ORBSupportInterface {

    /**
     * Calls the underlying ORB specific ORBInit, so the semantic is as
     * defined by CORBA.
     *
     * @param args The command line arguments
     * @param strategyList A list of CDMW strategies
     * @param cdmwRootPOAPort specific port for the Cdmw root POA
     * @param rootPOAPort specific port for the root POA
     * @param orbIdentifier Optional ORB identifier
     *
     * @return A valid reference to an ORB pseudo-object
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public org.omg.CORBA.ORB ORBInit(
        String[] args,
        StrategyList strategyList,
        int cdmwRootPOAPort,
        int rootPOAPort,
        String orbIdentifier)
        throws org.omg.CORBA.SystemException;

    /**
     * Creates a POA child of the specified parentPOA. The
     *  created POA is initialised with the specified policies
     *  and with others Policy, created according to the
     *  specified CDMW StrategyList.
     *
     * @param parentPOA The parent POA of the POA to be created
     * @param adapterName Name of the new POA
     * @param aPOAManager Manager of the POA
     * @param policies List of policies to use to create the POA
     * @param strategyList List of CDMW Strategies
     *
     * @return A reference to the created POA
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     * @exception AdapterAlreadyExists A POA with the same name
     * is already created by parentPOA.
     * @exception InvalidPolicy Supplied policies are not compatible.
     *
     * @see StrategyList
     */
    public org.omg.PortableServer.POA createPOA(
        org.omg.PortableServer.POA parentPOA,
        String adapterName,
        org.omg.PortableServer.POAManager aPOAManager,
        PolicyList policies,
        StrategyList strategyList)
        throws
            org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
            org.omg.PortableServer.POAPackage.InvalidPolicy,
            org.omg.CORBA.SystemException;

    /**
     * Binds the specified object reference
     * to a name reachable by corbaloc URLs.
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name to be assigned to reach the object
     * @param o Object reference to bind to the corbalocName
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public void bindObjectToCorbaloc (
        org.omg.CORBA.ORB orb,
        String corbalocName,
        org.omg.CORBA.Object o)
        throws org.omg.CORBA.SystemException;

    /**
     * Removes a corbaloc binding.
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name of the binding to be removed.
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public void removeCorbalocBinding(
        org.omg.CORBA.ORB orb,
        String corbalocName)
        throws org.omg.CORBA.SystemException;

    /**
     * Register the object reference as specified service
     * to be reachable with "resolveInitialReference" API.
     *
     * @param orb The ORB pseudo-object
     * @param serviceName Name to be assigned as service to reach the object
     * @param object Object reference to associate to service name
     *
     * @exception BAD_PARAM Invalid parameter. The CDMW
     * <B>BAD_PARAMCorbalocBindingAlreadyExists</B> minor code indicates that
     * an object reference has already been bound to the specified name.
     */
    public void registerInitialReference(
        org.omg.CORBA.ORB orb,
        String serviceName,
        org.omg.CORBA.Object object)
        throws org.omg.CORBA.SystemException;

    /**
     * Clean up the orb 
     * 
     * @param orb The ORB pseudo-object
     * @exception BAD_INV_ORDER 
     */ 
    public void ORBCleanup(org.omg.CORBA.ORB orb) 
        throws org.omg.CORBA.BAD_INV_ORDER;

}

