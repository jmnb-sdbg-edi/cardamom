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


import java.io.File;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.Property;


/**
 * Mimic the ANT "dirname" task but allow property overriding.
 */
public class Dirname extends Task {


    // File.
    private File m_file = null;

    // The property to set.
    private String m_property = null;

    // Allow property overriding.
    private boolean m_override = false;


    /**
     * Specify the file or directory to get base name from.
     *
     * @param file The file or directory.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Return the file or directory.
     *
     * @return The file or directory.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Specify the property to set.
     *
     * @param property The property to set.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Return the property.
     *
     * @return The property.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Set the override flag value.
     *
     * @param override The override flag value.
     */
    public void setOverride(boolean override) {
        m_override = override;
    }


    /**
     * Return the override flag value.
     *
     * @return The override flag value.
     */
    public boolean getOverride() {
        return m_override;
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_property == null) {
            throw new BuildException("property attribute required",
                                      getLocation());
        }

        if (m_file == null) {
            throw new BuildException("file attribute required",
                                     getLocation());
        }
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        String value = m_file.getParent();

        if (m_override) {
            if (getProject().getUserProperty(m_property) == null) {
                getProject().setProperty(m_property, value);
            } else {
                getProject().setUserProperty(m_property, value);
            }
        } else {
            Property p = (Property) getProject().createTask("property");
            p.setName(m_property);
            p.setValue(value);
            p.execute();
        }
    }
}


