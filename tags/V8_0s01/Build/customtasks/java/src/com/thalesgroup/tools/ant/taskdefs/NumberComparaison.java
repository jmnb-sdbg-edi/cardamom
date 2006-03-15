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
public class NumberComparaison extends Task {


    // The value 1
    private String m_val1 = null;

    // The value 2
    private String m_val2 = null;

    // The property to create.
    private String m_property = null;

   
    // the size to read
    private static final int READSZ = 32;

    /**
     * Specify the val1 
     *
     * @param val1 
     */
    public void setVal1(String val1) {
        m_val1 = val1;
    }


    /**
     * Get the val1
     *
     * @return the val1
     */
    public String getVal1() {
        return m_val1;
    }

    /**
     * Specify the val2 
     *
     * @param val2 
     */
    public void setVal2(String val2) {
        m_val2 = val2;
    }


    /**
     * Get the val2
     *
     * @return the val2
     */
    public String getVal2() {
        return m_val2;
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
    public NumberComparaison() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        if(isSup(m_val1, m_val2))
            getProject().setNewProperty(m_property, "true");
    }
    
    
    public static boolean isSup(String _val1, String _val2) 
    {
        boolean result = false;
        int val1 = Integer.parseInt(_val1);
        int val2 = Integer.parseInt(_val2);
        if (val1 > val2)
            result = true;
        return result;       
    }
}

