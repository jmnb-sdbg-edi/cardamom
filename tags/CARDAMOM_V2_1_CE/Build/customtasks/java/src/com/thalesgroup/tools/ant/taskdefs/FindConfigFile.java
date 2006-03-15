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
import java.io.FilenameFilter;
import java.io.IOException;

import java.util.Comparator;
import java.util.TreeSet;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;


/**
 * Given a directory path, look for either the oldest or the newest site
 * properties file created by the configurator.
 */
public class FindConfigFile extends Task {


    // Config file name pattern.
    public static final String FILE_PATTERN = "Site-.*#(\\d+)\\.properties";

    // The directory to search.
    private File m_dir = null;

    // The property to set.
    private String m_property = null;

    // If true then return the oldest file found.
    private boolean m_oldest = false;


    /**
     * Specify the directory to search.
     *
     * @param dir The directory.
     */
    public void setDir(File dir) {
        m_dir = dir;
    }


    /**
     * Return the directory.
     *
     * @return The directory.
     */
    public File getDir() {
        return m_dir;
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
     * Specify whether or not the oldest file should be selected.
     *
     * @param oldest The flag value.
     */
    public void setOldest(boolean oldest) {
        m_oldest = oldest;
    }


    /**
     * Get the oldest flag value.
     *
     * @return The oldest flag value.
     */
    public boolean getOldest() {
        return m_oldest;
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException(
                "property attribute required", getLocation());
        }

        if (m_dir == null) {
            throw new BuildException(
                "dir attribute required", getLocation());
        }

        if (!m_dir.exists() || !m_dir.isDirectory()) {
            throw new BuildException(
                m_dir.getAbsolutePath() +
                    " does not exist or is not a directory",
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

        String[] filenames = m_dir.list(new ConfigFileFilter());
        
        TreeSet ts = new TreeSet(new ConfigFileComparator());

        for (int i = 0; i < filenames.length; i++) {
            ts.add(filenames[i]);
        }

        File f = null;
        if (ts.size() > 0) {

            if (m_oldest) {
                f = new File(m_dir, (String) ts.first());
            } else {
                f = new File(m_dir, (String) ts.last());
            }
        }

        if (f != null) {
            try {
                getProject().setNewProperty(m_property, f.getCanonicalPath());
            } catch (IOException ioe) {
                // Ignore.
            }
        }
    }


    /**
     * Filename filter.
     */
    protected class ConfigFileFilter implements FilenameFilter {

        /**
         * Return true if the file name matches the regex pattern.
         *
         * @param dir The directory in which the file was found.
         * @param name The name of the file.
         *
         * @return <code>true</code> or <code>false</code>
         */
        public boolean accept(File dir, String name) {
            return Pattern.matches(FILE_PATTERN, name);
        }

    }


    /**
     * File name comparator.
     */
    protected class ConfigFileComparator implements Comparator {

        /**
         * Compare two strings.
         *
         * @param o1 The first string.
         * @param o2 The second string.
         *
         * @return A negative integer, zero, or a positive integer as the
         *         first argument is less than, equal to, or greater than
         *         the second.
         */
        public int compare(Object o1, Object o2) {
            Pattern p = Pattern.compile(FILE_PATTERN);
            Matcher m1 = p.matcher((String) o1);
            Matcher m2 = p.matcher((String) o2);

            m1.matches();
            m2.matches();

            // The previous operations will always return true or else we
            // didn't implement the file filter correctly.
            
            int n1 = new Integer(m1.group(1)).intValue();
            int n2 = new Integer(m2.group(1)).intValue();

            // We can return (n1 - n2) as they are both positive integers.
            return (n1 - n2);
        }

    }


}
