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


import com.thalesgroup.tools.ant.util.FindUtils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.InputStream;
import java.io.IOException;

import java.util.Collection;
import java.util.Iterator;
import java.util.StringTokenizer;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.ExecTask;
import org.apache.tools.ant.taskdefs.Property;
import org.apache.tools.ant.types.Commandline;


/**
 * Find in the source directory any files that match the specified file pattern.
 */
public class FindFile extends Task {


    private String m_file = null;
    private File m_dir = null;
    private String m_property = null;
    private String m_delimiter = " ";
    private boolean m_findFirst = false;
    private boolean m_recursive = false;
    private boolean m_override = false;


    /**
     * The name of the file to find.
     */
    public void setFile(String file) {
        m_file = file;
    }


    /**
     * Getter for the "file" attribute.
     */
    public String getFile() {
        return m_file;
    }


    /**
     * Start the search in this directory.
     */
    public void setDir(File dir) {
        m_dir = dir;
    }


    /**
     * Getter for the "dir" attribute.
     */
    public File getDir() {
        return m_dir;
    }


    /**
     * Return only the first match.
     */
    public void setFindFirst(boolean findFirst) {
        m_findFirst = findFirst;
    }


    /**
     * Getter for the "findfirst" attribute.
     */
    public boolean getFindFirst() {
        return m_findFirst;
    }


    /**
     * Property to create.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Getter for the "property" attribute.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Search the directory recursively.
     */
    public void setRecursive(boolean recursive) {
        m_recursive = recursive;
    }


    /**
     * Getter for the "recursive" attribute.
     */
    public boolean getRecursive() {
        return m_recursive;
    }


    /**
     * List item delimiter.
     */
    public void setSeparator(String separator) {
        m_delimiter = separator;
    }


    /**
     * Getter for the "delimiter" attribute.
     */
    public String getSeparator() {
        return m_delimiter;
    }


    /**
     * Override property value.
     */
    public void setOverride(boolean override) {
        m_override = override;
    }


    /**
     * Getter for the "override" attribute.
     */
    public boolean getOverride() {
        return m_override;
    }


    /**
     * Run the task.
     *
     * @exception BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        String value = null;

        // If running on Linux or Unix then favor the find utility for the
        // search as it would avoid problems with symbolic links.
        if (System.getProperty("os.name").indexOf("Windows") != -1) {
            value = findFileMethod1();
        } else {
            value = findFileMethod2();
        }

        if (value != null) {
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
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_file == null) || m_file.trim().equals("")) {
            throw new BuildException(
                                     "attribute file must not be null.", getLocation());
        }

        if (m_dir == null) {
            throw new BuildException(
                                     "attribute dir must not be null.", getLocation());
        }

        if (!m_dir.exists()) {
            throw new BuildException(
                                     m_dir + " does not exist.", getLocation());
        }

        if (!m_dir.isDirectory()) {
            throw new BuildException(
                                     m_dir + " is not a directory.", getLocation());
        }

        if ((m_property == null) || m_property.trim().equals("")) {
            throw new BuildException(
                                     "attribute property must not be null.", getLocation());
        }
    }


    /**
     * Find a file using ANT.
     */
    protected String findFileMethod1() {
        String value = null;
        try {
            if (m_findFirst) {
                // Return only the first match.
                File f = FindUtils.findFile(
                                            getProject(), m_file, m_dir, m_recursive);

                if (f != null) {
                    value = new String(f.getCanonicalPath());
                }
            } else {
                // Return all the matches.
                Collection files =
                    FindUtils.findFiles(getProject(),
                                        m_file, m_dir, m_recursive);

                StringBuffer sb = new StringBuffer();
                
                Iterator iter = files.iterator();
                while (iter.hasNext()) {
                    File f = (File) iter.next();

                    sb.append(m_delimiter).append(f.getCanonicalPath());
                }

                if (sb.length() > 0) {
                    if (m_delimiter.equals(" ")) {
                        value = new String(sb.toString().trim());
                    } else {
                        value = sb.toString().substring(m_delimiter.length());
                    }
                }
            }
        } catch (IOException ioe) {
            throw new BuildException(ioe);
        }

        return value;
    }


    /**
     * Find a file using Unix find utility.
     */
    protected String findFileMethod2() {
        String value = null;

        try {
            ExecTask exec = (ExecTask) getProject().createTask("exec");
            exec.setExecutable("find");
            exec.setFailonerror(false);
            long id = System.currentTimeMillis();
            exec.setOutputproperty("find.result." + id);
            exec.setResultProperty("find.error." + id);
            exec.setLogError(false);
            
            Commandline.Argument arg = exec.createArg();
            arg.setLine(m_dir.getCanonicalPath() + " -type f -name " + m_file +
                        " -maxdepth " + (m_recursive ? "6" : "1") +
                        " -print -follow");

            exec.execute();

            String findResult = getProject().getProperty("find.result." + id);
            int error = new Integer(getProject().getProperty("find.error." + id)).intValue();

            if (findResult.length() > 0) {
                StringTokenizer st = new StringTokenizer(findResult, "\n");
	       
                if (error ==1)
                    log("but ignored", Project.MSG_ERR);


                if (m_findFirst) {

                    while (st.hasMoreTokens()) {
                        String s = st.nextToken();
                        if (s.indexOf("find:") == -1)
                            {
                                // Return only the first match.
                                File f = new File(s);
                                if (f.exists()) {
                                    value = f.getAbsolutePath();
                                    break;
                                }
                            }
                    }
                } else {

                    // Return all the matches.
                    StringBuffer sb = new StringBuffer();
                    while (st.hasMoreTokens()) {
                        String s = st.nextToken();
                        if (s.indexOf("find:") == -1)
                            {
                                
                                File f = new File(s);
                                if (f.exists()) {
                                    sb.append(m_delimiter).append(f.getAbsolutePath());
                                }
                            }
                    }

                    if (sb.length() > 0) {
                        if (m_delimiter.equals(" ")) {
                            value = new String(sb.toString().trim());
                        } else {
                            value = sb.toString().
                                substring(m_delimiter.length());
                        }
                    }
                }
            }
        } catch (IOException ioe) {
            throw new BuildException(ioe);
        }
        
        return value;
    }


}
