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
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.util.StringUtils;

import org.apache.xpath.XPathAPI;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Find the CSCs to be built.
 */
public class FindCscs extends Task {


    // The package description file.
    private File m_pkgDesc = null;

    // The property to be created.
    private String m_addProperty = null;

    // The list of selected CSCIs.
    private Vector m_cscis = null;

    // The list of selected interfaces.
    private Vector m_interfaces = null;

    // The list of selected packages.
    private Vector m_packages = null;

    // Flag to determine whether to include CSCs from the validation tests.
    private boolean m_validationTests = false;

    // Flag to determine whether to include CSCs from the integration tests.
    private boolean m_integrationTests = false;

    // Flag to determine whether to include CSCs from the qualification tests.
    private boolean m_qualificationTests = false;

    // Flag to specify the CSC type wanted.
    private String m_cscType = null;
    
    // Selected CSC types.
    private boolean m_builds = false;
    private boolean m_unitTests = false;


    /**
     * Specify the package description file.
     *
     * @param pkgdesc The package description file.
     */
    public void setPkgdesc(File pkgdesc) {
        m_pkgDesc = pkgdesc;
    }


    /**
     * Return the package description file.
     *
     * @return The package description file.
     */
    public File getPkgdesc() {
        return m_pkgDesc;
    }


    /**
     * Specify the property to be created.
     *
     * @param addProperty The property to be created.
     */
    public void setAddProperty(String addProperty) {
        m_addProperty = addProperty;
    }


    /**
     * Return the property name.
     *
     * @return The property name.
     */
    public String getAddProperty() {
        return m_addProperty;
    }


    /**
     * Specify the list of selected CSCIs.
     *
     * @param cscis The list of selected CSCIs.
     */
    public void setCscis(String cscis) {
        m_cscis = StringUtils.split(cscis, ' ');
    }


    /**
     * Get the list of selected CSCIs.
     *
     * @return The list of selected CSCIs.
     */
    public Collection getCscis() {
        return m_cscis;
    }


    /**
     * Specify the list of selected interfaces.
     *
     * @param interfaces The list of selected interfaces.
     */
    public void setInterfaces(String interfaces) {
        m_interfaces = StringUtils.split(interfaces, ' ');
    }


    /**
     * Get the list of selected interfaces.
     *
     * @return The list of selected interfaces.
     */
    public Collection getInterfaces() {
        return m_interfaces;
    }


    /**
     * Specify the list of selected packages.
     *
     * @param packages The list of selected packages.
     */
    public void setPackages(String packages) {
        m_packages = StringUtils.split(packages, ' ');
    }


    /**
     * Get the list of selected packages.
     *
     * @return The list of selected packages.
     */
    public Collection getPackages() {
        return m_packages;
    }


    /**
     * Specify whether or not validation tests CSCs should be selected.
     *
     * @param validationTests The flag.
     */
    public void setValidationTests(boolean validationTests) {
        m_validationTests = validationTests;
    }


    /**
     * Get the validation tests flag.
     *
     * @return The validation tests flag.
     */
    public boolean getValidationTests() {
        return m_validationTests;
    }


    /**
     * Specify whether or not integration tests CSCs should be selected.
     *
     * @param integrationTests The flag.
     */
    public void setIntegrationTests(boolean integrationTests) {
        m_integrationTests = integrationTests;
    }


    /**
     * Get the integration tests flag.
     *
     * @return The integration tests flag.
     */
    public boolean getIntegrationTests() {
        return m_integrationTests;
    }
    

    /**
     * Specify whether or not qualification tests CSCs should be selected.
     *
     * @param qualificationTests The flag.
     */
    public void setQualificationTests(boolean qualificationTests) {
        m_qualificationTests = qualificationTests;
    }


    /**
     * Get the qualification tests flag.
     *
     * @return The qualification tests flag.
     */
    public boolean getQualificationTests() {
        return m_qualificationTests;
    }


    /**
     * Specify the type of CSC to find.
     *
     * @param cscType The type of CSC.
     */
    public void setCscType(String cscType) {
        m_cscType = cscType;
    }


    /**
     * Get the CSC type.
     *
     * @return The CSC type wanted.
     */
    public String getCscType() {
        return m_cscType;
    }
    

    /**
     * Default constructor.
     */
    public FindCscs() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkAttributes();

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

            LinkedHashSet cscs = new LinkedHashSet();

            Iterator cscis = m_cscis.iterator();
            while (cscis.hasNext()) {
                String csci = (String) cscis.next();

                String property = "config." + csci + ".location";
                String csciLocation = getProject().getProperty(property);
                if (csciLocation == null) {
                    throw new BuildException(
                        "property " + property + " not defined.");
                }

                StringBuffer xpath = new StringBuffer();
                xpath.append("//csci[@name='");
                xpath.append(csci);
                xpath.append("']/interface[");

                Iterator itfs = m_interfaces.iterator();
                while (itfs.hasNext()) {
                    xpath.append("@lang='");
                    xpath.append((String) itfs.next());
                    xpath.append("'");
                    if (itfs.hasNext()) {
                        xpath.append(" or ");
                    }
                }

                xpath.append("]/cscs/set/packageid");
                NodeIterator nodes =
                    XPathAPI.selectNodeIterator(doc, xpath.toString());
                Node n;
                while ((n = nodes.nextNode()) != null) {
                    // Get the list of package ids.
                    StringTokenizer st = new StringTokenizer(
                        n.getFirstChild().getNodeValue());
                    Vector ids = new Vector();
                    Vector comboIds = new Vector();

                    while (st.hasMoreTokens()) {
                        String id = st.nextToken();

                        if (id.indexOf("+") != -1) {
                            // Combo IDs, e.g. ID1+ID2+ID3
                            if (!comboIds.contains(id)) {
                                comboIds.add(id);
                            }
                        } else {
                            if (!ids.contains(id)) {
                                ids.add(id);
                            }
                        }
                    }

                    if (ids.isEmpty() && comboIds.isEmpty()) {
                        throw new BuildException(
                            "packageid must contain at least one package id.");
                    }

                    Iterator iter = comboIds.iterator();
                    boolean allComboIdsFound = false;
                    while (!allComboIdsFound && iter.hasNext()) {
                        StringTokenizer st_ =
                            new StringTokenizer((String) iter.next(), "+");
                        Vector v = new Vector();
                        while (st_.hasMoreTokens()) {
                            v.add(st_.nextToken());
                        }

                        int vSize = v.size();
                        v.retainAll(m_packages);

                        allComboIdsFound = (v.size() == vSize);
                    }

                    // If the set of package ids refers to at least one of the
                    // selected packages or contains the keyword"any" then
                    // select this set.
                    Vector clone = (Vector) ids.clone();
                    clone.retainAll(m_packages);

                    if (ids.contains("any") || allComboIdsFound ||
                        !clone.isEmpty()) {

                        // Get the "set" node.
                        Node parent = n.getParentNode();

                        if (parent.hasChildNodes()) {
                            NodeList list = parent.getChildNodes();

                            int length = list.getLength();
                            for (int i = 0; i < length; i++) {
                                Node child = list.item(i);

                                if (child.getNodeType() == Node.ELEMENT_NODE) {
                                    String name = child.getNodeName();

                                    if ((m_builds &&
                                            name.equals("build")) ||
                                        (m_unitTests &&
                                            name.equals("test")) ||
                                        (m_validationTests &&
                                            name.equals("validation")) ||
                                        (m_qualificationTests &&
                                            name.equals("qualification")) ||
                                        (m_integrationTests &&
                                            name.equals("integration"))) {

                                        if (child.getFirstChild() != null) {
                                            st = new StringTokenizer(
                                                        child.getFirstChild().
                                                        getNodeValue());

                                            while (st.hasMoreTokens()) {
                                                cscs.add(csciLocation +
                                                         File.separator +
                                                         st.nextToken());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!cscs.isEmpty()) {
                StringBuffer sb = new StringBuffer();

                Iterator iter = cscs.iterator();
                while (iter.hasNext()) {
                    sb.append((String) iter.next());
                    sb.append(" ");
                }

                getProject().setNewProperty(m_addProperty,
                                            sb.toString().trim());
            }

            doc = null;
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
     * Check attributes.
     */
    protected void checkAttributes() throws BuildException {
        if (m_pkgDesc == null) {
            throw new BuildException("pkgDesc must not be null.");
        }

        if (!m_pkgDesc.exists()) {
            throw new BuildException("pkgDesc does not exist.");
        }

        if (!m_pkgDesc.isFile()) {
            throw new BuildException("pkgDesc is not a file.");
        }

        if ((m_addProperty == null) || m_addProperty.equals("")) {
            throw new BuildException("addProperty must not be null.");
        }

        if ((m_cscis == null) || m_cscis.isEmpty()) {
            throw new BuildException("cscis must not be null.");
        }

        if ((m_interfaces == null) || m_interfaces.isEmpty()) {
            throw new BuildException("interfaces must not be null.");
        }

        if ((m_packages == null) || m_packages.isEmpty()) {
            throw new BuildException("packages must not be null.");
        }

        if ((m_cscType == null) || m_cscType.equals("")) {
            m_builds = true;
            m_unitTests = true;
        } else {
            if (m_cscType.equals("build")) {
                m_builds = true;
            } else if (m_cscType.equals("unit-test")) {
                m_unitTests = true;
            } else if (m_cscType.equals("integration-test")) {
                m_integrationTests = true;
            } else if (m_cscType.equals("validation-test")) {
                m_validationTests = true;
            } else if (m_cscType.equals("qualification-test")) {
                m_qualificationTests = true;
            } else if (m_cscType.equals("all")) {
                m_builds = true;
                m_unitTests = true;
                m_integrationTests = true;
                m_validationTests = true;
                m_qualificationTests = true;
            }
        }
    }


}

