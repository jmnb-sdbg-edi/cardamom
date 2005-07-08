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
import java.io.FileInputStream;
import java.io.IOException;

import java.util.Collection;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.StringTokenizer;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Parse Data4Inspection.xml and build the list of files to install to inspect.
 */
public class Data4Inspection extends Task {


    private File m_file = null;
    private HashSet m_cscis = new HashSet();
    private String m_type = null;
    private String m_prefix = null;


    /**
     * Set the Data4Inspection.xml file location.
     *
     * @param file The file location.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * @return the location of the file to parse.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Restrict the list of CSCIs to process.
     *
     * @param cscis The list of CSCIs to process.
     */
    public void setCscis(String cscis) {
        try {
            StringTokenizer st = new StringTokenizer(cscis);
            while (st.hasMoreTokens()) {
                m_cscis.add(st.nextToken());
            }
        } catch (NullPointerException e) {
            // Ignore, m_cscis will be an empty set.
        }
    }


    /**
     * @return the list of CSCIs to process.
     */
    public Collection getCscis() {
        return m_cscis;
    }


    /**
     * Set the type of the inspection.
     *
     * @param type The type of the inspection.
     */
    public void setType(String type) {
        m_type = type;
    }


    /**
     * @return the type of the inspection.
     */
    public String getType() {
        return m_type;
    }


    /**
     * Set a prefix for all the properties created by this task.
     *
     * @param prefix The property prefix.
     */
    public void setPrefix(String prefix) {
        m_prefix = prefix;
    }


    /**
     * @return the property prefix.
     */
    public String getPrefix() {
        return m_prefix;
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_file == null) {
            throw new BuildException(
                "attribute file has no value",
                getLocation());
        }

        if (!m_file.exists() || m_file.isDirectory()) {
            throw new BuildException(
                m_file + " is not a file or does not exist",
                getLocation());
        }

        if ((m_type == null) || m_type.equals("")) {
            throw new BuildException(
                "attribute type has no value",
                getLocation());
        }

        if (!m_type.equals("validation") && !m_type.equals("qualification")) {
            throw new BuildException(
                "invalid type value: " + m_type +
                ", it must be either validation or qualification",
                getLocation());
        }

        if ((m_prefix != null) && !m_prefix.trim().equals("")) {
            if (!m_prefix.endsWith(".")) {
                m_prefix += ".";
            }
        } else {
            // Use the type as prefix by default.
            m_prefix = m_type + ".";
        }
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        FileInputStream fis = null;

        try {
            fis = new FileInputStream(m_file);

            // Set up a DOM tree to query.
            InputSource in = new InputSource(fis);
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // XPath expr to select the validation or qualification elements.
            String xpath = "";
            if (m_cscis.isEmpty()) {
                xpath = "//fileset";
            } else {
                xpath = "//fileset[@dirtype='csci' and (";
                Iterator it = m_cscis.iterator();
                while (it.hasNext()) {
                    xpath += "@dir='" + it.next() + "'";
                    if (it.hasNext()) xpath += " or ";
                }
                xpath += ")]";
            }
            xpath += "/" + m_type;

            // Find the files to inspect for each validation or
            // qualification id.
            Hashtable ht = new Hashtable();
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
            Node node;
            while ((node = nodes.nextNode()) != null) {
                // Validation or qualification id.
                XObject xobj = XPathAPI.eval(node, "@id");
                String id = xobj.str().trim();

                // Source directory.
                xobj = XPathAPI.eval(node, "../@dirtype");
                String dirtype = xobj.str();

                xobj = XPathAPI.eval(node, "../@dir");
                String dirvalue = xobj.str();
                String dir = "";

                // Expand the dir value according to its type.
                if (dirtype.equals("csci")) {
                    dir = getProject().getProperty(
                            "config." + dirvalue + ".location");
                } else if (dirtype.equals("tool")) {
                    dir = getProject().getProperty(
                            "config." + dirvalue + ".install.dir");
                } else {
                    File d = new File(dirvalue);
                    if (d.exists() && d.isDirectory()) {
                        try {
                            dir = d.getCanonicalPath();
                        } catch (IOException ioe) {
                            dir = null;
                        }
                    } else {
                        dir = null;
                    }
                }

                if (dir != null) {
                    HashSet hs = new HashSet();
                    if (ht.containsKey(id)) {
                        hs = (HashSet) ht.get(id);
                    }

                    xobj = XPathAPI.eval(node, "files");
                    StringTokenizer st = new StringTokenizer(xobj.str());
                    while (st.hasMoreTokens()) {
                        File f = new File(dir + File.separator + st.nextToken());

                        if (f.exists() && f.isFile()) {
                            hs.add(f.getAbsolutePath());
                        } else {
                            log("Could not find file " + f.toString(),
                                Project.MSG_WARN);
                        }
                    }

                    ht.put(id, hs);
                } else {
                    log("Ignore", Project.MSG_WARN);
                }
            }

            Iterator it = ht.keySet().iterator();
            while (it.hasNext()) {
                String key = (String) it.next();
                Collection col = (Collection) ht.get(key);
                Iterator it_ = col.iterator();
                StringBuffer sb = new StringBuffer();
                while (it_.hasNext()) {
                    sb.append(it_.next()).append(" ");
                }

                getProject().setNewProperty(
                    m_prefix + key + ".files", sb.toString().trim());
            }
        } catch (Exception ex) {
            throw new BuildException(ex, getLocation());
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException ioe) {
                    // Ignore.
                }
            }
        }
    }


}
