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


package com.thalesgroup.tools.ant.taskdefs;


import java.lang.reflect.Field;

import java.util.Hashtable;

import org.apache.tools.ant.BuildException;


/**
 * Extend the ANT Contrib's variable task to support the unset attribute.
 *
 * As of ANT Contrib version 0.6, the Variable task does not support such
 * an attribute, however it does so in the current Sourceforge CVS repository.
 * This is a backport of the diffs from release 1.2 to release 1.4.
 */
public class Variable extends net.sf.antcontrib.property.Variable {


    // The name of the property.
    private String m_name = null;

    // If set to true, the property is removed.
    private boolean m_unset = false;


    /**
     * Set the name of the property.
     *
     * @param name The name of the property.
     */
    public void setName(String name) {
        m_name = name;
        super.setName(name);
    }


    /**
     * Get the name of the property.
     *
     * @return The name of the property.
     */
    public String getName() {
        return m_name;
    }


    /**
     * Specify the unset flag value.
     *
     * @param unset The new value.
     */
    public void setUnset(boolean unset) {
        m_unset = unset;
    }


    /**
     * Get the unset flag value.
     */
    public boolean getUnset() {
        return m_unset;
    }


    /**
     * Constructor.
     */
    public Variable() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        if (m_unset) {
            if ((m_name == null) || (m_name.equals(""))) {
                throw new BuildException("attribute name must not be null.",
                                         getLocation());
            }

            removeProperty(m_name);
            return;
        }

        super.execute();
    }


    /**
     * Object rape: fondle the private parts of an object without it's
     * permission.
     *
     * @param thisClass The class to rape.
     * @param fieldName The field to fondle.
     *
     * @return The field value.
     *
     * @throws NoSuchFieldException
     */
    private Field getField(Class thisClass, String fieldName)
        throws NoSuchFieldException {

        if (thisClass == null) {
            throw new NoSuchFieldException("Invalid field : " + fieldName);
        }

        try {
            return thisClass.getDeclaredField(fieldName);
        } catch (NoSuchFieldException e) {
            return getField(thisClass.getSuperclass(), fieldName);
        }
    }


    /**
     * Object rape: fondle the private parts of an object without it's
     * permission.
     *
     * @param instance The object instance.
     * @param fieldName The name of the field.
     *
     * @return An object representing the value of the field.
     *
     * @throws IllegalAccessException
     * @throws NoSuchFieldException
     */
    private Object getValue(Object instance, String fieldName)
        throws IllegalAccessException, NoSuchFieldException {

        Field field = getField(instance.getClass(), fieldName);
        field.setAccessible(true);
      
        return field.get( instance );
    }


    /**
     * Remove a property from the project's property table and the userProperty
     * table.
     * Note that Ant 1.6 uses a helper for this.
     */
    protected void removeProperty(String name) {
        Hashtable properties = null;

        // Ant 1.5 stores properties in Project
        try {
            properties = (Hashtable) getValue(getProject(), "properties");

            if (properties != null) {
                properties.remove(name);
            }
        } catch (Exception e) {
            // ignore, could be Ant 1.6
        }

        try {
            properties = (Hashtable) getValue(getProject(), "userProperties");

            if (properties != null) {
                properties.remove(name);
            }
        } catch (Exception e) {
            // ignore, could be Ant 1.6
        }

        // Ant 1.6 uses a PropertyHelper, can check for it by checking for a
        // reference to "ant.PropertyHelper"
        try {
            Object property_helper =
                getProject().getReference("ant.PropertyHelper");

            if (property_helper != null) {
                try {
                    properties =
                        (Hashtable) getValue(property_helper, "properties");

                    if (properties != null) {
                        properties.remove(name);
                    }
                } catch ( Exception e ) {
                    // ignore
                }

                try {
                    properties =
                        (Hashtable) getValue(property_helper, "userProperties");

                    if (properties != null) {
                        properties.remove(name);
                    }
                } catch (Exception e) {
                    // ignore
                }
            }
        } catch (Exception e) {
            // ignore, could be Ant 1.5
        }
    }

}
