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


import com.thalesgroup.tools.ant.input.InputHandlerImpl;
import com.thalesgroup.tools.ant.input.InputRequest;
import com.thalesgroup.tools.ant.input.MultipleChoiceInputRequest;
import com.thalesgroup.tools.ant.input.RangeInputRequest;
import com.thalesgroup.tools.ant.util.StringUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import java.util.Collection;
import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.TreeSet;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

import org.apache.xpath.XPathAPI;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Run the CSCI selection.
 */
public class SelectCscis extends Task {


    // The property to be created.
    private String m_property = null;

    // The package description file.
    private File m_pkgDesc = null;

    // The previous selection.
    private Vector m_default = null;

    // The input handler.
    private InputHandlerImpl m_inputHandler = new InputHandlerImpl();


    /**
     * Specify the property to be created.
     *
     * @param property The property to be created.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Return the property name.
     *
     * @return The property name.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Specify the package description file.
     *
     * @param pkgDesc The package description file.
     */
    public void setPkgdesc(File pkgDesc) {
        m_pkgDesc = pkgDesc;
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
     * Specify the previous selection.
     *
     * @param theDefault The previous selection.
     */
    public void setDefault(String theDefault) {
        m_default =
            org.apache.tools.ant.util.StringUtils.split(theDefault, ' ');
        if (m_default.isEmpty())
            m_default = null;
    }


    /**
     * Return the previous selection.
     *
     * @return The previous selection.
     */
    public Vector getDefault() {
        return m_default;
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();
        Collection cscis = getCsciIds();

        if (m_default != null) {
            // if the m_default is incompatible with the new cscis set, 
            // put the m_default to NULL
            Iterator iter = m_default.iterator();
            while (iter.hasNext()) {
                if (!cscis.contains((String) iter.next())) {
                    m_default = null;
                    break;
                }
            }
        }        

        if (discardDefault()) {


            if (!cscis.isEmpty()) {
                makeSelection(cscis);
            }
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_pkgDesc == null) {
            throw new BuildException("attribute pkgDesc must not be null.");
        }

        if (!m_pkgDesc.exists()) {
            throw new BuildException("pkgDesc does not exist.");
        }

        if (!m_pkgDesc.isFile()) {
            throw new BuildException("pkgDesc is not a file.");
        }

        if ((m_property == null) || m_property.equals("")) {
            throw new BuildException("attribute property must not be null.");
        }
    }


    /**
     * Ask the user if he wants to discard the previous selection and make a
     * new selection.
     *
     * @return <code>false<code> if the user wants to keep the previous
     *         selection, <code>true</code> otherwise.
     */
    protected boolean discardDefault() {
        if ((m_default != null) && !m_default.isEmpty()) {
            String msg = "Your previous selection was " + m_default +
                         ".\nDo you want to discard the list and make a new " +
                         "selection ? ";

            Vector yesOrNo = new Vector();
            yesOrNo.add("y");
            yesOrNo.add("n");

            InputRequest requestDiscard =
                new MultipleChoiceInputRequest(msg, yesOrNo, "n");

            // Wait for the answer.
            m_inputHandler.handleInput(requestDiscard);

            if (requestDiscard.getInput().equals("n")) {
                log("Final selection: " + m_default, Project.MSG_WARN);

                // Create property.
                StringBuffer sb = new StringBuffer();
                Iterator iter = m_default.iterator();
                while (iter.hasNext()) {
                    sb.append((String) iter.next());
                    sb.append(" ");
                }
                getProject().setNewProperty(m_property, sb.toString().trim());

                return false;
            }
        }

        return true;
    }


    /**
     * Get all the CSCI identifiers.
     *
     * @return The list of CSCI identifiers.
     */
    protected Collection getCsciIds() {
        TreeSet cscis = new TreeSet();
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(m_pkgDesc);

            // Set up a DOM tree to query.
            InputSource in = new InputSource(fis);
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // Get the CSCI identifiers.
            String xpath = "//csci/@name";
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
            Node n;
            while ((n = nodes.nextNode()) != null) {
                cscis.add(n.getNodeValue());
            }

            doc = null;
        } catch (Exception e) {
            throw new BuildException(e);
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    // Ignore.
                }
            }
        }

        // Store this list into an ANT property.
        if (!cscis.isEmpty()) {
            StringBuffer sb = new StringBuffer();
            Iterator iter = cscis.iterator();
            while (iter.hasNext()) {
                sb.append((String) iter.next());
                sb.append(" ");
            }

            getProject().setNewProperty("config.all.cscis",
                                        sb.toString().trim());
        }

        return cscis;
    }


    /**
     * Run the interactive CSCI selector.
     *
     * @param cscis The CSCI identifiers.
     */
    protected void makeSelection(Collection cscis) {
        Vector selected = new Vector();
        Vector csciList = new Vector(availableForSelection(cscis, selected));

        boolean endSelection =
            ((csciList == null) || (csciList.isEmpty())) ? true : false;

        while (!endSelection) {
            if (!selected.isEmpty() && (csciList.size() == 1)) {
                String msg = "Only one CSCI left in the list: " + csciList +
                             ".\nThis CSCI is automatically selected.";

                log(msg, Project.MSG_WARN);
                selected.add(csciList.get(0));

                break;
            }

            StringBuffer sb = new StringBuffer();
            sb.append("List of CSCI(s): \n");
            int idx = 1;
            Iterator iter = csciList.iterator();
            while (iter.hasNext()) {
                sb.append("    ");
                sb.append(idx++);
                sb.append(") ");
                sb.append(iter.next());
                sb.append("\n");
            }
            sb.append("Enter the number(s) of the CSCI and/or the group of ");
            sb.append("CSCIs separated by semi-colons (ex: 1;3;5-12;4)");

            // Define the regex pattern that will validate entries
            // like " 1; 3;5-12 ;4 ".
            String pattern = "\\s*\\d+(-\\d+)?(\\s*;\\s*\\d+(-\\d+)?)*\\s*";

            // Wait for the answer.
            InputRequest request = new RangeInputRequest(
                sb.toString(), pattern, 1, csciList.size());
            m_inputHandler.handleInput(request);

            // Save the user input.
            selected.addAll(
                StringUtils.mapRangeIntoNames(request.getInput().trim(),
                                              csciList.toArray()));

            // Get the remaining CSCIs.
            csciList = new Vector(availableForSelection(cscis, selected));

            if (!csciList.isEmpty()) {
                Vector yesOrNo = new Vector();
                yesOrNo.add("y");
                yesOrNo.add("n");

                // Ask the user if he wants to add another CSCI to the list.
                InputRequest requestContinue =
                    new MultipleChoiceInputRequest(
                        "Selected CSCI(s): " + selected + "\n" +
                        "Do you want to add more CSCIs ? ", yesOrNo, null);

                // Wait for the answer.
                m_inputHandler.handleInput(requestContinue);

                // Save the answer.
                endSelection =
                    requestContinue.getInput().equals("y") ? false : true;
            } else {
                endSelection = true;
            }
        }

        log("Final selection: " + selected, Project.MSG_WARN);

        // Create the ANT property.
        StringBuffer sb = new StringBuffer();
        Iterator iter = selected.iterator();
        while (iter.hasNext()) {
            sb.append((String) iter.next());
            sb.append(" ");
        }
        getProject().setNewProperty(m_property, sb.toString().trim());
    }


    /**
     * Determine the CSCIs that are still available for selection.
     *
     * @param allIds   The complete list of CSCI identifiers.
     * @param selected The list of CSCIs that have already been selected.
     *
     * @return The list of CSCIs that have not been selected yet.
     */
    protected Collection availableForSelection(Collection allIds,
                                               Collection selected) {

        TreeSet available = new TreeSet(allIds);
        available.removeAll(selected);

        return available;
    }


}

