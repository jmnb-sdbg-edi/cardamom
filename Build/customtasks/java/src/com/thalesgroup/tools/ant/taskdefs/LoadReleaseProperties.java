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
 * Load ReleaseProperties.xml file and create ANT properties.
 */
public class LoadReleaseProperties extends Task {


    // Source file.
    private File m_file = null;


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
     * Default constructor.
     */
    public LoadReleaseProperties() {
        super();
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

            // Property "config.product.name"
            String xpath = "/config/product/@name";
            XObject xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("config.product.name",
                                        xobj.str().trim());

            // Property "product.version"
            xpath = "/config/product/@version";
            xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("product.version", xobj.str().trim());

            // Property "require.csci.version.checking"
            xpath = "/config/cscis/@version.checking";
            xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("require.csci.version.checking",
                                        xobj.str().trim());

            // Property "require.csci.check.mode"
            xpath = "/config/cscis/@check.mode";
            xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("require.csci.check.mode",
                                        xobj.str().trim());

            // Property "require.tool.version.checking"
            xpath = "/config/tools/@version.checking";
            xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("require.tool.version.checking",
                                        xobj.str().trim());

            // Property "require.tool.check.mode"
            xpath = "/config/tools/@check.mode";
            xobj = XPathAPI.eval(doc, xpath);
            getProject().setNewProperty("require.tool.check.mode",
                                        xobj.str().trim());

            // Property "require.${csci.name}.version"
            xpath = "/config/cscis/csci";
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
            Node node;
            while ((node = nodes.nextNode()) != null) {
                NamedNodeMap atts = node.getAttributes();

                String csciName = atts.getNamedItem("name").getNodeValue();
                String csciVersion =
                    atts.getNamedItem("version").getNodeValue();

                getProject().setNewProperty(
                    "require." + csciName + ".version", csciVersion);
            }

            // Property "require.${osfamily}.${tool.name}.version
            xpath = "/config/tools/osfamily/@name";
            nodes = XPathAPI.selectNodeIterator(doc, xpath);
            while ((node = nodes.nextNode()) != null) {
                String osfamilyName = node.getNodeValue();

                String xpath2 =
                    "/config/tools/osfamily[@name='" + osfamilyName + "']/tool";

                NodeIterator tools = XPathAPI.selectNodeIterator(doc, xpath2);
                Node tool;
                while ((tool = tools.nextNode()) != null) {
                    NamedNodeMap atts = tool.getAttributes();

                    String toolName = atts.getNamedItem("name").getNodeValue();
                    String toolVersion =
                        atts.getNamedItem("version").getNodeValue();

                    getProject().setNewProperty(
                        "require." + osfamilyName + "." + toolName + ".version",
                        toolVersion);
                }
            }

            doc = null;
        } catch (Exception ex) {
            throw new BuildException(ex);
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (java.io.IOException ioe) {
                    // Ignore.
                }
            }
        }
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
    }


}

