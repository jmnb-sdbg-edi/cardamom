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


package cdmw.ccm.container;

import cdmw.common.Assert;
import cdmw.common.BadParameterException;

import cdmw.lifecycle.ObjectDeactivator;

/**
 * A template class for Home Allocators.
 *
 */
public class HomeAllocator implements HomeAllocatorBase {

    /**
     * Name of the class of the servant to instantiate
     */
    private String classname;

    /**
     * Constructor.
     *
     * @param classname Name of the class of the servant to instantiate
     */
    public HomeAllocator(String classname) {
        this.classname = classname;
    }

    /**
     * A static method to get an instance of a Home Allocator
     *
     * @param classname Name of the class of the servant to instantiate
     */
    public static HomeAllocator getAllocator(String classname) {
        return new HomeAllocator(classname);
    }

    /**
     * This operation is called by the container during homes installation
     * to allocate a servant for the home.
     *
     * @param homeExecutor Home executor acquired from the loaded library.
     * @param deactivator Object Deactivator for use
     * for home object deactivation.
     * @exception BadParameterException Wrong parameter value
     */
    public org.omg.PortableServer.Servant allocate(
        org.omg.Components.HomeExecutorBase homeExecutor,
        ObjectDeactivator deactivator)
        throws BadParameterException {

        try {
            // signature of the constructor
            Class[] parameterTypes = {
                Class.forName("org.omg.Components.HomeExecutorBase"),
                Class.forName("cdmw.lifecycle.ObjectDeactivator"),
            };
            java.lang.reflect.Method method =
                Class.forName(this.classname).getMethod(
                    "allocate", 
                    parameterTypes);
            Object[] initArgs = {
                homeExecutor,
                deactivator
            };
            return (org.omg.PortableServer.Servant)
                method.invoke(null, initArgs);
        } catch(ClassNotFoundException cnfe) {
            throw new BadParameterException("the class cannot be located");
        } catch(NoSuchMethodException nsme) {
            throw new BadParameterException("a matching method is not found " +
                "or if the name is '<init>' or '<clinit>'");
        } catch(IllegalAccessException iae) {
            throw new BadParameterException(
                "this Method object enforces Java language access control " + 
                "and the underlying method is inaccessible");
        } catch(java.lang.reflect.InvocationTargetException ite) {
            throw new BadParameterException(
                "the underlying method throws an exception");
        }

    }
    
    public org.omg.PortableServer.Servant getDelegate(
        org.omg.PortableServer.Servant servant) throws BadParameterException {
        
        try {
            // signature of the constructor
            Class[] parameterTypes = {
                Class.forName("org.omg.PortableServer.Servant")
            };
            java.lang.reflect.Method method =
                Class.forName(this.classname).getMethod(
                    "getDelegate", 
                    parameterTypes);
            Object[] initArgs = { servant };
            Object result = method.invoke(null, initArgs);
            if (result == null) {
                throw new BadParameterException(
                    "the parameter is not of the expected type");
            }
            return (org.omg.PortableServer.Servant) result;
        } catch(ClassNotFoundException cnfe) {
            throw new BadParameterException("the class cannot be located");
        } catch(NoSuchMethodException nsme) {
            throw new BadParameterException("a matching method is not found " +
                "or the name is '<init>' or '<clinit>'");
        } catch(IllegalAccessException iae) {
            throw new BadParameterException(
                "this Method object enforces Java language access control " + 
                "and the underlying method is inaccessible");
        } catch(java.lang.reflect.InvocationTargetException ite) {
            throw new BadParameterException(
                "the underlying method throws an exception");
        }
        
    }

    /**
     * This operation is called whenever the Container is done
     * with the Home servant previously allocated by the current allocator.
     *
     * @param home a Home servant.
     */
    public void dispose(org.omg.PortableServer.Servant home) {
        Assert.check( home != null);
        cleanup(home);
    }

    /**
     * This operation perfoms actual disposal of the home servant.
     * This is an ideal place to persist state of the factory.
     *
     * @param home a Home servant.
     */
    protected void cleanup(org.omg.PortableServer.Servant home) {
        // do nothing in Java
    }

}

