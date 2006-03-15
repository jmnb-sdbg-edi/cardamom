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


import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.Path;


/**
 * Build a string list out of the specified ANT Path reference id.
 */
public class PathToList extends Task {


    // The Path reference id.
    private String m_pathRefId = null;

    // The property to be created.
    private String m_property = null;

    // The list separator.
    private String m_separator = " ";


    /**
     * Specify the property to be created.
     *
     * @param property The property to be created.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Get the property to be created.
     *
     * @return The property to be created.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Specify the Path reference id.
     *
     * @param pathRefId The Path reference id.
     */
    public void setPathRefId(String pathRefId) {
        m_pathRefId = pathRefId;
    }


    /**
     * Get the Path reference id.
     *
     * @return The Path reference id.
     */
    public String getPathRefId() {
        return m_pathRefId;
    }


    /**
     * Set the list separator.
     *
     * @param separator The list separator.
     */
    public void setSeparator(String separator) {
        m_separator = separator;
    }


    /**
     * Get the list separator.
     *
     * @return The list separator.
     */
    public String getSeparator() {
        return m_separator;
    }


    /**
     * Default constructor.
     */
    public PathToList() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        Object path =  getProject().getReference(m_pathRefId);
        if (path == null)
            throw new BuildException("unknown reference: " + m_pathRefId);

        if (!(path instanceof Path))
            throw new BuildException(m_pathRefId + " is not a path");
        
        String[] sources = ((Path) path).list();

        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < sources.length; i++) {
            sb.append(sources[i]);
            sb.append(m_separator);
        }

        getProject().setNewProperty(m_property, sb.toString().trim());
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }

        if ((m_pathRefId == null) || m_pathRefId.equals("")) {
            throw new BuildException("attribute pathRefId must not be null.");
        }
    }


}

