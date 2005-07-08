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


package com.thalesgroup.tools.ant.taskdefs;


import java.io.File;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;


/**
 * Return the name of the file without extension.
 */
public class StripFileExt extends Task {


    // The property to be created.
    private String m_property = null;

    // The abstract pathname.
    private File m_file = null;


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
     * Specify the abstract pathname.
     *
     * @param file The abstract pathname.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Get the abstract pathname.
     *
     * @return The abstract pathname.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Default constructor.
     */
    public StripFileExt() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        String filename = m_file.getName();
        int pos = filename.lastIndexOf('.');

        if (pos != -1) {
            getProject().setNewProperty(m_property, filename.substring(0, pos));
        } else {
            getProject().setNewProperty(m_property, filename);
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }

        if (m_file == null) {
            throw new BuildException("attribute file must not be null.");
        }

        if (!m_file.exists()) {
            throw new BuildException("file does not exist.");
        }

        if (!m_file.isFile()) {
            throw new BuildException("the file you specified is not a file.");
        }
    }


}

