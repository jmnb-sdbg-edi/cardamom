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
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.Path;


/**
 * Build a string list out of the specified ANT Path reference id.
 */
public class IsBinary extends Task {


    // The Path reference id.
    private String m_file = null;

    // The property to create.
    private String m_property = null;

   
    // the size to read
    private static final int READSZ = 32;

    /**
     * Specify the file to check
     *
     * @param fileName 
     */
    public void setFile(String file) {
        m_file = file;
    }


    /**
     * Get the file to check
     *
     * @return the file
     */
    public String getFile() {
        return m_file;
    }

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
     * Default constructor.
     */
    public IsBinary() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        if(isBinary(m_file))
            getProject().setNewProperty(m_property, "true");
    }
    
    
    public static boolean isBinary(String _filename) {
        try {
            boolean result;
            InputStream is = new BufferedInputStream(new FileInputStream(_filename));
            result = isBinary(is);
            is.close();
            return result;
        }
        catch (FileNotFoundException fnfe) {
            throw new BuildException(fnfe);
        }
        catch (IOException ioe) {
            throw new BuildException(ioe);
        }

    }
    public static boolean isBinary(InputStream is) {
        try {            
            is.mark(READSZ);
            byte[] buf = new byte[READSZ];
            int r = is.read(buf);
            is.reset();
            for (int i=0; i<r; i++)
                if (buf[i] <= 0) return true;
            return false;
        }
        catch(IOException ioe) {
            throw new BuildException(ioe);
        }
    }

}

