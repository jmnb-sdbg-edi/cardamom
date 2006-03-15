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


import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;

import java.util.Enumeration;
import java.util.Properties;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.filters.util.ChainReaderHelper;
import org.apache.tools.ant.filters.ExpandProperties;
import org.apache.tools.ant.types.FilterChain;
import org.apache.tools.ant.types.FilterSet;


/**
 * Load a file's contents as either Ant properties or filter tokens.
 */
public class LoadProperties extends Task {


    // The source file.
    private File m_srcFile = null;

    // The filterset id to be created.
    private String m_filtersetId = null;

    // Holds filterchains.
    private final Vector m_filterChains = new Vector();

    // Encoding to use for filenames.
    private String m_encoding = null;


    /**
     * Set the file to load.
     *
     * @param srcFile The file to load.
     */
    public void setSrcFile(File srcFile) {
        m_srcFile = srcFile;
    }


    /**
     * Get the file to be loaded.
     *
     * @return The file to be loaded.
     */
    public File getSrcFile() {
        return m_srcFile;
    }


    /**
     * Set the filterset id to be created.
     *
     * @param filtersetId The filterset id to be created.
     */
    public void setFilterSetId(String filtersetId) {
        m_filtersetId = filtersetId;
    }


    /**
     * Get the filterset id.
     *
     * @return The filterset id.
     */
    public String getFilterSetId() {
        return m_filtersetId;
    }


    /**
     * Encoding to use for input, defaults to the platform's default encoding.
     *
     * @param encoding The encoding value.
     */
    public void setEncoding(String encoding) {
        m_encoding = encoding;
    }


    /**
     * Get the encoding.
     *
     * @return The encoding.
     */
    public String getEncoding() {
        return m_encoding;
    }


    /**
     * Add a FilterChain.
     *
     * @param filter The filter chain.
     */
    public void addFilterChain(FilterChain filter) {
        m_filterChains.addElement(filter);
    }


    /**
     * Default constructor.
     */
    public LoadProperties() {
        super();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_srcFile == null) {
            throw new BuildException("source file not defined.");
        }

        if (!m_srcFile.exists()) {
            throw new BuildException("source file does not exist.");
        }

        if (!m_srcFile.isFile()) {
            throw new BuildException("source file is not a file.");
        }
    }


    /**
     * read in a source file's contents and load them up as Ant properties
     *
     * @exception BuildException if something goes wrong with the build
     */
    public final void execute() throws BuildException {
        checkConfiguration();

        FileInputStream fis = null;
        BufferedInputStream bis = null;
        Reader instream = null;

        try {
            final long len = m_srcFile.length();

            // Open up the file
            fis = new FileInputStream(m_srcFile);
            bis = new BufferedInputStream(fis);
            if (m_encoding == null) {
                instream = new InputStreamReader(bis);
            } else {
                instream = new InputStreamReader(bis, m_encoding);
            }

            // Apply the filter chains.
            ChainReaderHelper crh = new ChainReaderHelper();
            crh.setPrimaryReader(instream);
            crh.setFilterChains(m_filterChains);
            crh.setProject(getProject());
            instream = crh.getAssembledReader();

            FilterSet fs = new FilterSet();
            fs.setProject(getProject());
            boolean hasFilterTokens = false;

            String text = crh.readFully(instream);

            if (text != null) {
                if (!text.endsWith("\n")) {
                    text = text + "\n";
                }

                // Apply encoding.
                ByteArrayInputStream tis = null;
                if (m_encoding == null) {
                    tis = new ByteArrayInputStream(text.getBytes());
                } else {
                    tis = new ByteArrayInputStream(text.getBytes(m_encoding));
                }

                // Convert the stream into a list of properties.
                final Properties props = new Properties();
                props.load(tis);

                final Enumeration e = props.keys();
                while (e.hasMoreElements()) {
                    final String key = (String) e.nextElement();
                    final String value = props.getProperty(key);

                    if ((key != null) && (value != null) &&
                        value.trim().length() > 0) {

                        // Either create a filter token or a property.
                        if ((m_filtersetId != null) &&
                            !m_filtersetId.equals("")) {

                            FilterSet.Filter f = new FilterSet.Filter(
                                key,getProject().replaceProperties(value));

                            fs.addFilter(f);
                            hasFilterTokens = true;
                        } else {                                                        
                            getProject().setNewProperty(key, value);
                        }
                    }
                }

                // If the filterset is not empty, then create a new reference.
                if (hasFilterTokens) {
                    getProject().addReference(m_filtersetId, fs);
                }

                // Close the stream.
                tis.close();
            }

        } catch (final IOException ioe) {
            final String message = "unable to load file: " + ioe.toString();
            throw new BuildException(message, ioe, getLocation());
        } catch (final BuildException be) {
            throw be;
        } finally {
            try {
                if (fis != null) {
                    fis.close();
                }
            } catch (IOException ioex) {
                //ignore
            }
        }
    }


}


