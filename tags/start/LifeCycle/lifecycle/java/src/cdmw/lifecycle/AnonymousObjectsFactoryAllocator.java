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

import cdmw.common.Assert;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import org.omg.CORBA.ORB;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;

/**
 * Generic class for CDMW anonymous factories allocators.
 * Users may use explicit instantiation to change behaviour
 * of default allocation and disposal of factories.
 *
 */
public class AnonymousObjectsFactoryAllocator
    implements AnonymousObjectsFactoryAllocatorBase {

    
    /**
     * Name of the Factory class to be allocated
     */
    protected String factoryClassName;
        
        
    /**
     * Constructor.
     *
     * @param factoryClassName Name of the Factory class to be allocated.
     */
   public AnonymousObjectsFactoryAllocator(String factoryClassName) {
       this.factoryClassName = factoryClassName;
   }
   
        
    /**
     * Allocates an anonymous objects factory for use by the LifeCycle
     * framework. This is ideal place to retreive the factory state
     * from persistence support.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param parent Parent POA for the factory
     * @param factoryName Fully qualified name of the CDMW factory
     * @param args Program arguments
     * @param threadingPolicy The threading policy choosen for the factory
     *
     * @return The allocated CDMW factory
     *
     * @exception BadParameterException Wrong parameter value, such as
     * threading ... etc.
     * @exception InternalErrorException Internal Error
     */
    public Servant allocate(ORB orb,
                            POA parent,
                            String factoryName,
                            POAThreadingPolicy threadingPolicy,
                            String[] args)
        throws BadParameterException,
               InternalErrorException
    {

        // Ideal place to retreive state from persistence support
        try {
            // signature of the constructor
            Class[] parameterTypes = {
                Class.forName("org.omg.CORBA.ORB"),
                Class.forName("org.omg.PortableServer.POA"),
                Class.forName("java.lang.String"),
                Class.forName("cdmw.lifecycle.POAThreadingPolicy"),
                Class.forName("[Ljava.lang.String;")
            };
            java.lang.reflect.Constructor constructor =
                Class.forName(factoryClassName).getConstructor(parameterTypes);
            Object[] initArgs =
                { orb, parent, factoryName, threadingPolicy, args };
            return (Servant) constructor.newInstance(initArgs);
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
     * with the CDMW factory allocated by a derived factory allocator.
     *
     * @param factory a CDMW factory servant.
     */
    public void dispose(Servant factory) {
        Assert.check(factory != null);
        cleanup(factory);
    }

    /**
     * This operation perfoms actual disposal of the CDMW factory.
     * This is an ideal place to persist state of the factory.
     *
     * @param fac A CDMW factory.
     */
    protected void cleanup(Servant fac) {
        // Ideal place to persist state
    }

}

