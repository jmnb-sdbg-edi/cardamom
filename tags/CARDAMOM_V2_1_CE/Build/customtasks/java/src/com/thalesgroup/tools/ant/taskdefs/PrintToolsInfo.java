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


import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;

import java.util.Collection;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.FilterSet;
import org.apache.tools.ant.types.FilterSetCollection;
import org.apache.tools.ant.util.StringUtils;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Prints information about the needed tools.
 */
public class PrintToolsInfo extends Task {


    // Log level.
    protected int m_logLevel = Project.MSG_WARN;

    // Source file.
    private File m_file = null;

    // The OS family name.
    private String m_osFamily = null;

    // The list of selected tools.
    private Vector m_tools = null;

    // The package description file.
    private File m_pkgDesc = null;

    // Holds FilterSets.
    private Vector m_filterSets = new Vector();

    // The OS version.
    private String m_osVer = "";

    // Tool descriptions.
    private Hashtable m_toolDescs = new Hashtable();


    /**
     * Set the file to be parsed.
     *
     * @param file The file to be parsed.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Get the file to be parsed.
     *
     * @return The file to be parsed.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Specify the OS family name.
     *
     * @param osFamily The OS family name.
     */
    public void setOsfamily(String osFamily) {
        m_osFamily = osFamily;
        m_osVer = System.getProperty("os.version");
    }


    /**
     * Get the OS family name.
     *
     * @return The OS family name.
     */
    public String getOsfamily() {
        return m_osFamily;
    }


    /**
     * Specify the list of selected tools.
     *
     * @param tools The list of selected tools.
     */
    public void setTools(String tools) {
        m_tools = StringUtils.split(tools, ' ');
    }


    /**
     * Get the list of selected tools.
     *
     * @return The list of selected tools.
     */
    public Collection getTools() {
        return (Collection) m_tools;
    }


    /**
     * Specify the package description file.
     *
     * @param pkgDesc The package description file.
     */
    public void setPkgDesc(File pkgDesc) {
        m_pkgDesc = pkgDesc;
    }


    /**
     * Return the package description file.
     *
     * @return The package description file.
     */
    public File getPkgDesc() {
        return m_pkgDesc;
    }


    /**
     * Add a FilterSet.
     *
     * @return A FilterSet. 
     */
    public FilterSet createFilterSet() {
        FilterSet filterSet = new FilterSet();
        m_filterSets.addElement(filterSet);
        return filterSet;
    }


    /**
     * Get the filtersets being applied to this operation.
     *
     * @return A collection of FilterSet objects.
     */
    protected Collection getFilterSets() {
        return m_filterSets;
    }


    /**
     * Default constructor.
     */
    public PrintToolsInfo() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        if (m_file == null) {
            throw new BuildException("attribute file must not be null.");
        }

        if (!m_file.exists()) {
            throw new BuildException("file does not exist.");
        }

        if (!m_file.isFile()) {
            throw new BuildException("source file is not a file.");
        }

        if ((m_osFamily == null) || m_osFamily.equals("")) {
            throw new BuildException("attribute osfamily must not be null");
        }

        if (m_tools.isEmpty()) {
            throw new BuildException("attribute tools must not be null");
        }

        if (m_pkgDesc == null) {
            throw new BuildException("pkgDesc must not be null.");
        }

        if (!m_pkgDesc.exists()) {
            throw new BuildException("pkgDesc does not exist.");
        }

        if (!m_pkgDesc.isFile()) {
            throw new BuildException("pkgDesc is not a file.");
        }

        String header = "\n\n----------------------" +
                        "\n List of needed tools" +
                        "\n----------------------\n";

        log(header, m_logLevel);

        String introduction =
            "It is HIGHLY ADVISABLE to install each of these tools into " +
            "separate locations\n(e.g. /tools/tool1, /tools/tool2, etc.) " +
            "so as to ease software upgrades and\nto avoid unmanageable " +
            "conflicts.\n\n";

        FileInputStream fis = null;

        log(introduction, m_logLevel);

        try {
            getToolDescriptions();

            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            InputSource in = null;

            // Print information about the tools.

            StringBuffer text = new StringBuffer();

            boolean filterSetsAvailable =
                (m_filterSets != null) && !m_filterSets.isEmpty();

            // Replace tokens in m_file before parsing.
            if (filterSetsAvailable) {
                FilterSetCollection executionFilters = 
                    new FilterSetCollection();

                Iterator iter = m_filterSets.iterator();
                while (iter.hasNext()) {
                    executionFilters.addFilterSet((FilterSet) iter.next());
                }

                BufferedReader reader = null;
                reader = new BufferedReader(new FileReader(m_file));
                
                int length;
                String newline = null;
                String line = reader.readLine();
                String LS = System.getProperty("line.separator");
                while (line != null) {
                    if (line.length() > 0) {
                        if (filterSetsAvailable) {
                            newline = executionFilters.replaceTokens(line);
                        } else {
                            newline = line;
                        }

                        text.append(newline);
                        text.append(LS);
                    }
                    
                    line = reader.readLine();
                }

                StringReader stringReader = new StringReader(text.toString());
                in = new InputSource(stringReader);
            } else {
                fis = new FileInputStream(m_file);
                in = new InputSource(fis);
            }

            // Set up a DOM tree to query.
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // Print information about the tools.
            Iterator iter = m_tools.iterator();
            while (iter.hasNext()) {
                String toolName = (String) iter.next();

                // Get the required version for this tool.
                String property =
                    "require." + m_osFamily + "." + toolName + ".version";
                String toolver = getProject().getProperty(property);
                if (toolver == null) {
                    throw new BuildException(
                        "property " + property + " not defined.");
                }

                // Information about a specific version of the tool.
                String xpath =
                    "/config/tool[@name='" + toolName + "' and @version='" +
                    toolver + "']";

                Node node = XPathAPI.selectSingleNode(doc, xpath);
                if (node != null) {
                    printToolInfo(node);
                } else {
                    xpath = "/config/tool[@name='" + toolName +
                            "' and @version='any']";

                    node = XPathAPI.selectSingleNode(doc, xpath);
                    if (node != null) {
                        printToolInfo(node);
                    } else {
                        String message =
                            "[" + toolName + "] " +
                            (String) m_toolDescs.get(toolName) +
                            " version " + toolver +
                            "\n\n                " +
                            "Information not available.\n\n";
                        log(message, m_logLevel);
                    }
                }
            }

            doc = null;
        } catch (Exception ex) {
            throw new BuildException(ex);
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


    /**
     * Get the description of every tool.
     */
    protected void getToolDescriptions() {
        // Parse the package description file.
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(m_pkgDesc);

            // Set up a DOM tree to query.
            InputSource in = new InputSource(fis);
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            String xpath = "//tool";
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
            Node node;
            while ((node = nodes.nextNode()) != null) {
                NamedNodeMap atts = node.getAttributes();

                String name = atts.getNamedItem("name").getNodeValue();
                String desc = atts.getNamedItem("desc").getNodeValue();

                m_toolDescs.put(name, desc);
            }
        } catch (Exception e) {
            throw new BuildException(e);
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


    /**
     * Print information about the specified tool.
     *
     * @param toolNode The "tool" node.
     */
    protected void printToolInfo(Node toolNode)
        throws Exception {

        String xpath = "@name";
        XObject xobj = XPathAPI.eval(toolNode, xpath);
        String toolName = xobj.str().trim();

        String property =
            "require." + m_osFamily + "." + toolName + ".version";
        String toolver = getProject().getProperty(property);
        if (toolver == null) {
            throw new BuildException(
                "property " + property + " not defined.");
        }

        String message = "[" + toolName + "] " +
            (String) m_toolDescs.get(toolName) + " version " + toolver;
        log(message, m_logLevel);

        xpath = "os[@family='" + m_osFamily +
                "' and @version='" + m_osVer + "']";
        
        Node node = XPathAPI.selectSingleNode(toolNode, xpath);
        if (node != null) {
            printNotesAndHints(node);
        } else {
            xpath = "os[@family='" + m_osFamily + "' and @version='any']";

            node = XPathAPI.selectSingleNode(toolNode, xpath);
            if (node != null) {
                printNotesAndHints(node);
            } else {
                xpath = "os[@family='any']";

                node = XPathAPI.selectSingleNode(toolNode, xpath);
                if (node != null) {
                    printNotesAndHints(node);
                } else {
                    message = "\n\n                " +
                        "Information not available.\n";
                    log(message, m_logLevel);
                }
            }
        }

        // Now proceeed with its dependencies.
        xpath = "@depends";
        xobj = XPathAPI.eval(toolNode, xpath);
        String result = xobj.str().trim();
        if (!result.equals("")) {
            StringTokenizer st = new StringTokenizer(result);

            while (st.hasMoreTokens()) {
                String toolName_ = st.nextToken();

                // Get the required version for this tool.
                property =
                    "require." + m_osFamily + "." + toolName_ + ".version";
                toolver = getProject().getProperty(property);
                if (toolver == null) {
                    throw new BuildException(
                        "property " + property + " not defined.");
                }

                // Information about a specific version of the tool.
                xpath =
                    "/config/tool[@name='" + toolName_ + "' and @version='" +
                    toolver + "']";

                node = XPathAPI.selectSingleNode(toolNode, xpath);
                if (node != null) {
                    printToolInfo(node);
                } else {
                    xpath = "/config/tool[@name='" + toolName_ +
                            "' and @version='any']";

                    node = XPathAPI.selectSingleNode(toolNode, xpath);
                    if (node != null) {
                        printToolInfo(node);
                    } else {
                        message =
                            "[" + toolName_ + "] " +
                            (String) m_toolDescs.get(toolName_) +
                            " version " + toolver +
                            "\n\n                " +
                            "Information not available.\n";
                        log(message, m_logLevel);
                    }
                }
            }
        }
    }


    /**
     * Prints notes and hints about the specified tool.
     *
     * @param osNode The "os" node.
     */
    protected void printNotesAndHints(Node osNode)
        throws Exception {

        String xpath = "notes";
        XObject xobj = XPathAPI.eval(osNode, xpath);
        String result = xobj.str();
        if (!result.equals("")) {
            log(result, m_logLevel);
        }

        xpath = "hints";
        xobj = XPathAPI.eval(osNode, xpath);
        result = xobj.str();
        if (!result.equals("")) {
            log(result, m_logLevel);
        }
    }

    
}

