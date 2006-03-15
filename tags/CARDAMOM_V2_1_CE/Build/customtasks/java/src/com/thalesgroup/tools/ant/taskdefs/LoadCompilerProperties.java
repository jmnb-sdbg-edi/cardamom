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


import com.thalesgroup.tools.ant.util.StringUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import java.util.Collection;
import java.util.Iterator;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Load CompilerProperties.xml file and create ANT properties.
 */
public class LoadCompilerProperties extends Task {


    // The source file.
    private File m_file = null;

    // The OS family name.
    private String m_osFamily = null;

    // The list of selected tools.
    private Vector m_tools = null;


    /**
     * Set the file to be parsed.
     *
     * @param file The file to be parsed.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Get the source file.
     *
     * @return The source file.
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
        m_tools = org.apache.tools.ant.util.StringUtils.split(tools, ' ');
    }


    /**
     * Get the list of selected tools.
     *
     * @return The list of selected tools.
     */
    public Collection getTools() {
        return m_tools;
    }


    /**
     * Default constructor.
     */
    public LoadCompilerProperties() {
        super();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
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

            // Select only the compilers contained in the list of tools.
            Vector compilers = new Vector();

            // First, get all the compiler from the file.
            StringBuffer xpath = new StringBuffer();

            xpath.append("/config/*/@name");
            NodeIterator nodes =
                XPathAPI.selectNodeIterator(doc, xpath.toString());
            Node n;
            while ((n = nodes.nextNode()) != null) {
                String name = n.getNodeValue();

                if (!compilers.contains(name)) {
                    compilers.add(name);
                }
            }

            // Now keep from the list of compiler names only those that are
            // contained in the list of selected tools.
            compilers.retainAll(m_tools);

            // Get the OS version.
            String osVer = System.getProperty("os.version");

            // Create ANT properties.
            Iterator iter = compilers.iterator();
            while (iter.hasNext()) {
                String name = (String) iter.next();

                // Get the required version for this compiler.
                String property =
                    "require." + m_osFamily + "." + name + ".version";
                String reqVer = getProject().getProperty(property);
                if (reqVer == null) {
                    throw new BuildException(
                        "property " + property + " not defined.");
                }

                // If the file contains properties for this specific version of
                // the compiler, then use it.
                xpath = new StringBuffer();
                xpath.append("/config/*[@name='");
                xpath.append(name);
                xpath.append("' and @version='");
                xpath.append(reqVer);
                xpath.append("']");

                String rootxpath = "";
                n = XPathAPI.selectSingleNode(doc, xpath.toString());
                if (n != null) {
                    rootxpath = xpath.toString();
                } else {
                    xpath = new StringBuffer();
                    xpath.append("/config/*[@name='");
                    xpath.append(name);
                    xpath.append("' and @version='any']");

                    n = XPathAPI.selectSingleNode(doc, xpath.toString());
                    if (n != null) {
                        rootxpath = xpath.toString();
                    }
                }

                String compilerNodeName = n.getNodeName();

                if (!rootxpath.equals("")) {
                    // If the file contains properties for the specific version
                    // of the OS, then use it.
                    xpath = new StringBuffer(rootxpath);
                    xpath.append("/os[@family='");
                    xpath.append(m_osFamily);
                    xpath.append("' and @version='");
                    xpath.append(osVer);
                    xpath.append("']");

                    n = XPathAPI.selectSingleNode(doc, xpath.toString());
                    if (n != null) {
                        rootxpath = xpath.toString();
                    } else {
                        xpath = new StringBuffer(rootxpath);
                        xpath.append("/os[@family='");
                        xpath.append(m_osFamily);
                        xpath.append("' and @version='any']");

                        n = XPathAPI.selectSingleNode(doc, xpath.toString());
                        if (n != null) {
                            rootxpath = xpath.toString();
                        }
                    }
                }

                if (!rootxpath.equals("")) {
                    // Now that we know what node to select, let's proceed
                    // to the creation of the ANT properties.

                    Node compilerNode =
                        XPathAPI.selectSingleNode(doc, rootxpath);

                    if (compilerNodeName.equals("cppcompiler")) {
                        parseCppCompilerNode(compilerNode, name);
                    } else if (compilerNodeName.equals("javacompiler")) {
                        parseJavaCompilerNode(compilerNode, name);
                    } else if (compilerNodeName.equals("adacompiler")) {
                        parseAdaCompilerNode(compilerNode, name);
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
     * Parse a "cppcompiler" node.
     *
     * @param compilerNode The compiler node.
     * @param name         The compiler name.
     */
    protected void parseCppCompilerNode(Node compilerNode, String name)
        throws Exception {

        // Property "config.compilerName.preprocessor"
        XObject xobj = XPathAPI.eval(compilerNode, "preprocessor/@name");
        getProject().setNewProperty(
            "config." + name + ".preprocessor", xobj.str().trim());

        // Property "config.compilerName.cc"
        xobj = XPathAPI.eval(compilerNode, "cc/@name");
        getProject().setNewProperty(
            "config." + name + ".cc", xobj.str().trim());

        // Property "config.compilerName.cxx"
        xobj = XPathAPI.eval(compilerNode, "cxx/@name");
        getProject().setNewProperty(
            "config." + name + ".cxx", xobj.str().trim());

        // Property "config.compilerName.cxx.compilerflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/compilerflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.compilerflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.debugflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/debugflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.debugflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.gcovflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/gcovflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.gcovflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.optimflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/optimflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.optimflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.sharedflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/sharedflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.sharedflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.dependencyflags"
        xobj = XPathAPI.eval(compilerNode, "cxx/dependencyflags");
        getProject().setNewProperty(
            "config." + name + ".cxx.dependencyflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.cxx.depfileext"
        xobj = XPathAPI.eval(compilerNode, "cxx/depfileext");
        getProject().setNewProperty(
            "config." + name + ".cxx.depfileext",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ld.shared"
        xobj = XPathAPI.eval(compilerNode, "ld/shared/@name");
        getProject().setNewProperty(
            "config." + name + ".ld.shared", xobj.str().trim());

        // Property "config.compilerName.ld.shared.flags"
        xobj = XPathAPI.eval(compilerNode, "ld/shared/flags");
        getProject().setNewProperty(
            "config." + name + ".ld.shared.flags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ld.shared.gcovflags"
        xobj = XPathAPI.eval(compilerNode, "ld/shared/gcovflags");
        getProject().setNewProperty(
            "config." + name + ".ld.shared.gcovflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ld.static"
        xobj = XPathAPI.eval(compilerNode, "ld/static/@name");
        getProject().setNewProperty(
            "config." + name + ".ld.static", xobj.str().trim());

        // Property "config.compilerName.ld.static.flags"
        xobj = XPathAPI.eval(compilerNode, "ld/static/flags");
        getProject().setNewProperty(
            "config." + name + ".ld.static.flags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ld.static.gcovflags"
        xobj = XPathAPI.eval(compilerNode, "ld/static/gcovflags");
        getProject().setNewProperty(
            "config." + name + ".ld.static.gcovflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ar.shared"
        xobj = XPathAPI.eval(compilerNode, "ar/shared/@name");
        getProject().setNewProperty(
            "config." + name + ".ar.shared", xobj.str().trim());

        // Property "config.compilerName.ar.shared.flags"
        xobj = XPathAPI.eval(compilerNode, "ar/shared/flags");
        getProject().setNewProperty(
            "config." + name + ".ar.shared.flags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ar.shared.gcovflags"
        xobj = XPathAPI.eval(compilerNode, "ar/shared/gcovflags");
        getProject().setNewProperty(
            "config." + name + ".ar.shared.gcovflags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ar.static"
        xobj = XPathAPI.eval(compilerNode, "ar/static/@name");
        getProject().setNewProperty(
            "config." + name + ".ar.static", xobj.str().trim());

        // Property "config.compilerName.ar.static.flags"
        xobj = XPathAPI.eval(compilerNode, "ar/static/flags");
        getProject().setNewProperty(
            "config." + name + ".ar.static.flags",
            StringUtils.normalizeSpaces(xobj.str()));

        // Property "config.compilerName.ar.static.gcovflags"
        xobj = XPathAPI.eval(compilerNode, "ar/static/gcovflags");
        getProject().setNewProperty(
            "config." + name + ".ar.static.gcovflags",
            StringUtils.normalizeSpaces(xobj.str()));
    }


    /**
     * Parse a "javacompiler" node.
     *
     * @param compilerNode The compiler node.
     * @param name         The compiler name.
     */
    protected void parseJavaCompilerNode(Node compilerNode, String name)
        throws Exception {

        // TODO
    }


    /**
     * Parse a "adacompiler" node.
     *
     * @param compilerNode The compiler node.
     * @param name         The compiler name.
     */
    protected void parseAdaCompilerNode(Node compilerNode, String name)
        throws Exception {

        // TODO
    }


}

