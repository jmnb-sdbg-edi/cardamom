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


import com.thalesgroup.tools.ant.input.InputHandlerImpl;
import com.thalesgroup.tools.ant.input.InputRequest;
import com.thalesgroup.tools.ant.input.MultipleChoiceInputRequest;
import com.thalesgroup.tools.ant.input.PathInputRequest;

import java.io.File;

import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.util.StringUtils;


/**
 * Read an input line from the console. This task supports all the attributes
 * from the genuine ANT Input task.
 */
public class Input extends Task {


    // The message to be displayed.
    private String m_message = "";

    // The possible values.
    private String m_validargs = null;

    // The property to create.
    private String m_addproperty = null;
    
    // The default input value.
    private String m_default = null;

    // Directory only flag.
    private boolean m_dirPathOnly = false;

    // Path only flag.
    private boolean m_pathOnly = false;

    // The input handler.
    private InputHandlerImpl m_inputHandler = new InputHandlerImpl();


    /**
     * Specify the message to display.
     *
     * @param message The message to be displayed.
     */
    public void setMessage (String message) {
        m_message = message;
    }


    /**
     * Get the message to display.
     *
     * @return The message to display.
     */
    public String getMessage() {
        return m_message;
    }

    
    /**
     * Specify the valid input parameters as comma separated strings.
     *
     * @param validargs A comma separated String defining valid input args.
     */
    public void setValidargs(String validargs) {
        m_validargs = validargs;
    }


    /**
     * Get the valid input paramaters.
     *
     * @return A comma separated String defining valid input args.
     */
    public String getValidargs() {
        return m_validargs;
    }


    /**
     * Specify the name of a property to be created from input.
     *
     * @param addproperty Name for the property to be created from input.
     */
    public void setAddproperty(String addproperty) {
        m_addproperty = addproperty;
    }


    /**
     * Get the property to be created.
     *
     * @return The property to be created.
     */
    public String getAddproperty() {
        return m_addproperty;
    }


    /**
     * Specify the directory only flag value.
     *
     * @param dirPathOnly The directory only flag.
     */
    public void setDirPathOnly(boolean dirPathOnly) {
        m_dirPathOnly = dirPathOnly;

        if (m_dirPathOnly) {
            m_pathOnly = true;
        }
    }


    /**
     * Get the directory only flag.
     *
     * @return The directory only flag.
     */
    public boolean getDirPathOnly() {
        return m_dirPathOnly;
    }


    /**
     * Specify the path only flag value.
     *
     * @param pathOnly The path only flag.
     */
    public void setPathOnly(boolean pathOnly) {
        m_pathOnly = pathOnly;
    }


    /**
     * Get the path only flag.
     *
     * @return The path only flag.
     */
    public boolean getPathOnly() {
        return m_pathOnly;
    }

    
    /**
     * Specify the default input value.
     *
     * @param theDefault The default input value.
     */
    public void setDefault(String theDefault) {
        m_default = theDefault;
    }


    /**
     * Get the default input value.
     *
     * @return The default input value.
     */
    public String getDefault() {
        return m_default;
    }


    /**
     * Sets a multiline message.
     */
    public void addText(String msg) {
        m_message += getProject().replaceProperties(msg);
    }


    /**
     * Default constructor.
     */
    public Input() {
        super();
    }


    /**
     * Run the task.
     * 
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        InputRequest request = null;

        // Use the custom InputHandlers to enable support for default values.
        if (m_dirPathOnly || m_pathOnly) {
            try {
                request = new PathInputRequest(
                    m_message,
                    m_dirPathOnly,
                    (m_default != null) ?
                        new File(m_default).getCanonicalPath() : null);
            } catch (java.io.IOException ioe) {
                throw new BuildException(ioe);
            }
        } else {
            if (m_validargs != null) {
                Vector accept = StringUtils.split(m_validargs, ',');

                request = new MultipleChoiceInputRequest(
                    m_message, accept, m_default);
            } else {
                request = new InputRequest(m_message, false, m_default);
            }
        }

        // Wait for the input.
        m_inputHandler.handleInput(request);

        // Create a new property.
        String value = request.getInput();
        if (value != null) {
            getProject().setNewProperty(m_addproperty, value);
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_message == null) || m_message.equals("")) {
            throw new BuildException("attribute message must not be null.");
        }

        if ((m_addproperty == null) || m_addproperty.equals("")) {
            throw new BuildException("attribute addproperty must not be null.");
        }

        if ((m_default != null) && (m_dirPathOnly || m_pathOnly)) {
            File f = new File(m_default);

            if (!f.exists()) {
                // Ignore the default location then.
                m_default = null;
            } else {
                if (f.isFile() && m_dirPathOnly) {
                    throw new BuildException(
                        "default location must be a directory.");
                }

                if (f.isDirectory() && !m_dirPathOnly) {
                    throw new BuildException(
                        "default location must be a file.");
                }
            }
        }
    }


}

