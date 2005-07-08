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


import cdmw.common.BadParameterException;


/**
 * Base class for all Servant Provider allocators
 *
 */
public class ServantProviderAllocator 
    extends ServantProviderAllocatorBase
{
    /**
     * Name of the ServantProvider class to be allocated
     */
    protected String servantProviderClassName;
    
    /**
     * Constructor.
     *
     * @param className Name of the ServantProvider class to be allocated.
     */
    public ServantProviderAllocator(String className) {
        super(className);
        this.servantProviderClassName = className;
    }

    
    /**
     * Allocates a ServantProvider for use by generated CDMW Factories
     * implementantions.
     * This is ideal place to retreive the ServantProvider state from persistence
     * support, if any.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param adapter A POA where CORBA objects incarnated by provided servants
     *  will be activated.
     * @param deactivator An ObjectDeactivator for use whenever there is a need
     * to deactiate an object.
     * @param argc Number of additional arguments
     * @param argv Additional program arguments
     * 
     * @return A pointer to a ServantProviderBase object for use by the LifeCycle Framework.
     *
     * @exception std::bad_alloc Failure to allocate memory for the servant provider
     * @exception CORBA::SystemException Any CORBA system exception
     */
    public ServantProviderBase allocate(org.omg.CORBA.ORB orb, 
                                        org.omg.PortableServer.POA adapter,
                                        ObjectDeactivator deactivator,
                                        String[] args)
        throws BadParameterException
    {
        // Ideal place to retreive state from persistence
        // support
        try {
            // signature of the constructor
            Class[] parameterTypes = {
                Class.forName("org.omg.CORBA.ORB"),
                Class.forName("org.omg.PortableServer.POA"),
                Class.forName("cdmw.lifecycle.ObjectDeactivator"),
                Class.forName("[Ljava.lang.String;")
            };
            java.lang.reflect.Constructor constructor =
                Class.forName(servantProviderClassName).getConstructor(parameterTypes);
            Object[] initArgs =
                { orb, adapter, deactivator, args };
            return (ServantProviderBase) constructor.newInstance(initArgs);

        } catch (ClassNotFoundException cnfe) {
            throw new BadParameterException(cnfe.getMessage());
        } catch (NoSuchMethodException nsme) {
            throw new BadParameterException(nsme.getMessage());
        } catch (InstantiationException ie) {
            throw new BadParameterException(ie.getMessage());
        } catch (IllegalAccessException iae) {
            throw new BadParameterException(iae.getMessage());
        } catch (java.lang.reflect.InvocationTargetException ite) {
            throw new BadParameterException(ite.getMessage());
        }
    }
    
    /**
     * This operation is called whenever the Life Cycle Framework is done
     * with the ServantProvider object.
     * This is the ideal place to persist state of the ServantProvider to a
     * persistent support.
     *
     * @param provider A pointer to the ServantProvider object
     */    
    public void dispose(ServantProviderBase provider) 
    { 
        // Ideal place to persist state
    }    
    
}


