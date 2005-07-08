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


import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;

import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.filters.util.ChainReaderHelper;
import org.apache.tools.ant.taskdefs.Property;
import org.apache.tools.ant.types.FilterChain;


/**
 * Select a specific token in a string. If the token number is omitted then the
 * first token is returned.
 */
public class SelectToken extends Task {


    // The property to be created.
    private String m_property = null;

    // The source string.
    private String m_string = null;

    // The token number.
    private int m_tokenNumber = 0;

    // Return the first token.
    private boolean m_firstToken = false;

    // Return the last token.
    private boolean m_lastToken = false;

    // The delimiter.
    private String m_delimiter = null;

    // Holds FilterChains.
    private Vector filterChains = new Vector();

    // Allow property overriding.
    private boolean m_override = false;


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
     * Specify the source string.
     *
     * @param string The source string.
     */
    public void setString(String string) {
        m_string = string;
    }


    /**
     * Get the source string.
     *
     * @return The source string.
     */
    public String getString() {
        return m_string;
    }


    /**
     * Specify the token number.
     *
     * @param tokenNumber The token number.
     */
    public void setTokenNumber(int tokenNumber) {
        m_tokenNumber = tokenNumber;
    }


    /**
     * Get the token number.
     *
     * @return The token number.
     */
    public int getTokenNumber() {
        return m_tokenNumber;
    }


    /**
     * Specify the delimiter.
     *
     * @param delimiter The delimiter.
     */
    public void setDelimiter(String delimiter) {
        m_delimiter = delimiter;
    }


    /**
     * Get the delimiter.
     *
     * @return The delimiter.
     */
    public String getDelimiter() {
        return m_delimiter;
    }


    /**
     * Add the FilterChain element.
     *
     * @param filter The filter chain.
     */
    public void addFilterChain(FilterChain filter) {
        filterChains.addElement(filter);
    }


    /**
     * Set first token flag.
     *
     * @param firstToken The flag value.
     */
    public void setFirstToken(boolean firstToken) {
        m_firstToken = firstToken;
    }


    /**
     * Return first token flag value.
     *
     * @return <code>true</code> or <code>false</code>
     */
    public boolean getFirstToken() {
        return m_firstToken;
    }


    /**
     * Set last token flag.
     *
     * @param lastToken The flag value.
     */
    public void setLastToken(boolean lastToken) {
        m_lastToken = lastToken;
    }


    /**
     * Return last token flag value.
     *
     * @return <code>true</code> or <code>false</code>
     */
    public boolean getLastToken() {
        return m_lastToken;
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
     * Default constructor.
     */
    public SelectToken() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        // Apply the filters to the string prior to any actions.
        applyFilters();

        String[] tokens = null;

        if ((m_delimiter == null) || m_delimiter.equals("")) {
            // No specified delimiter, defaults to space.
            tokens = m_string.split(" ");
        } else {
            tokens = m_string.split(m_delimiter);
        }

        if (tokens.length > 0) {
            String value = "";

            if (!m_lastToken) {
                value = tokens[m_tokenNumber];
            } else {
                value = tokens[tokens.length - 1];
            }

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


    /**
     * Apply the filters.
     */
    protected void applyFilters() {
        Reader instream = null;

        try {
            instream = new StringReader(m_string);

            // Apply all the filters on the string.
            ChainReaderHelper crh = new ChainReaderHelper();
            crh.setBufferSize(m_string.length());
            crh.setPrimaryReader(instream);
            crh.setFilterChains(filterChains);
            crh.setProject(getProject());
            instream = crh.getAssembledReader();

            String text = crh.readFully(instream);

            if (text != null) {
                if (text.length() > 0) {
                    m_string = text;
                }
            }
        } catch (IOException ioe) {
            throw new BuildException(ioe, getLocation());
        } finally {
            try {
                if (instream != null) {
                    instream.close();
                }
            } catch (IOException ioex) {
                //ignore
            }
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_string == null) || m_string.equals("")) {
            throw new BuildException("attribute string must not be null.");
        }

        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }

        if (m_firstToken) {
            m_tokenNumber = 0;
        }
    }


}

