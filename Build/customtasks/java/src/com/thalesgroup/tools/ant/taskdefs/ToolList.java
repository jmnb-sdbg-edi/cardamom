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

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import java.util.Collection;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.optional.PropertyFile;
import org.apache.tools.ant.util.StringUtils;
import org.apache.tools.ant.Project;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Run the interactive tool configuration.
 */
public class ToolList extends Task {


    // The package description file.
    private File m_pkgDesc = null;

    // The OS family name.
    private String m_osFamily = null;

    // The list of selected CSCIs.
    private TreeSet m_cscis = null;

    // The list of selected interfaces.
    private TreeSet m_interfaces = null;

    // The list of selected packages.
    private Vector m_packages = null;

    // The input handler.
    private InputHandlerImpl m_inputHandler = new InputHandlerImpl();

    // Map a list of tools to a CSCI.
    private Hashtable m_toolsByCsci = new Hashtable();

    // The list of selected ORBs.
    private Hashtable m_selOrbs = new Hashtable();

    // Constrain all the CSCI to use the same ORB ?
    private boolean m_forceUniqueOrb = false;

    // Has the question been asked ?
    private boolean m_forceUniqueOrbAsked = false;

    // Passive mode flag.
    private boolean m_passiveMode = false;

    
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
     * Specify the list of selected CSCIs.
     *
     * @param cscis The list of selected CSCIs.
     */
    public void setCscis(String cscis) {
        m_cscis = new TreeSet(StringUtils.split(cscis, ' '));
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
        m_interfaces = new TreeSet(StringUtils.split(interfaces, ' '));
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
     * Default constructor.
     */
    public ToolList() {
        super();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_pkgDesc == null) {
            throw new BuildException("pkgDesc must not be null.");
        }

        if (!m_pkgDesc.exists()) {
            throw new BuildException("pkgDesc does not exist.");
        }

        if (!m_pkgDesc.isFile()) {
            throw new BuildException("pkgDesc is not a file.");
        }

        if ((m_osFamily == null) || m_osFamily.equals("")) {
            throw new BuildException("osFamily must not be null.");
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

        String property = "passive.mode";
        String passiveModeValue = getProject().getProperty(property);

        if (passiveModeValue != null) {
            m_passiveMode =
                Boolean.valueOf(passiveModeValue).booleanValue();
        }
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

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

            // Build the minimum list of tools.
            TreeSet selected = new TreeSet();
            Iterator itfs = m_interfaces.iterator();
            while (itfs.hasNext()) {
                String itf = (String) itfs.next();

                Iterator cscis = m_cscis.iterator();
                while (cscis.hasNext()) {
                    String csci = (String) cscis.next();

                    String rootXPath = "//csci[@name='" + csci +
                                       "']/interface[@lang='" + itf + "']";

                    // Deal with the tools that are common to all the CSCs
                    // contained in this CSCI.
                    Node node =
                        XPathAPI.selectSingleNode(doc, rootXPath + "/tools");
                    TreeSet toolsets = new TreeSet();
                    if (node != null) {
                        StringTokenizer st = new StringTokenizer(
                            node.getFirstChild().getNodeValue());
                        while (st.hasMoreTokens()) {
                            toolsets.add(st.nextToken());
                        }
                    }

                    // Now deal with tools that are specific to only a group
                    // of CSCs.
                    NodeIterator nodes =
                        XPathAPI.selectNodeIterator(
                            doc, rootXPath + "/cscs/set/packageid");
                    while ((node = nodes.nextNode()) != null) {
                        // Get the list of package ids.
                        StringTokenizer st = new StringTokenizer(
                            node.getFirstChild().getNodeValue());
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
                                "packageid must contain at least one " +
                                "package id.");
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

                        // If the set of package ids refers to at least one
                        // of the selected packages or contains the keyword
                        // "any" then select this set.
                        Vector clone = (Vector) ids.clone();
                        clone.retainAll(m_packages);

                        if (ids.contains("any") || allComboIdsFound ||
                            !clone.isEmpty()) {

                            // Get the "set" node.
                            Node parent = node.getParentNode();

                            if (parent.hasChildNodes()) {
                                NodeList list = parent.getChildNodes();

                                int length = list.getLength();
                                for (int i = 0; i < length; i++) {
                                    Node child = list.item(i);

                                    if (child.getNodeType()
                                            == Node.ELEMENT_NODE) {
                                        String name = child.getNodeName();

                                        if (name.equals("tools")) {
                                            st = new StringTokenizer(
                                                child.getFirstChild().
                                                            getNodeValue());

                                            while (st.hasMoreTokens()) {
                                                toolsets.add(st.nextToken());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    TreeSet list = (TreeSet)
                        getToolListByCsci(doc, csci, toolsets);
                    selected.addAll(list);
                }
            }

            Vector compilers = new Vector();
            Vector orbs = new Vector();
            Vector otherTools = new Vector();

            Iterator iter = selected.iterator();
            XObject xobj;
            while (iter.hasNext()) {
                String tool = (String) iter.next();

                String xpath = "//toolset[tool/@name='" + tool + "']/@type";
                xobj = XPathAPI.eval(doc, xpath);
                String result = xobj.str().trim();

                if (result.equals("compiler")) {
                    compilers.add(tool);
                } else if (result.equals("orb")) {
                    orbs.add(tool);
                } else {
                    otherTools.add(tool);
                }
            }

            // Property "config.sel.min.compilers"
            StringBuffer sb = new StringBuffer();
            iter = compilers.iterator();
            while (iter.hasNext()) {
                sb.append(iter.next());
                sb.append(" ");
            }

            if (sb.length() > 0) {
                getProject().setNewProperty("config.sel.min.compilers",
                                            sb.toString().trim());
            }

            // Property "config.sel.min.orbs"
            sb = new StringBuffer();
            iter = orbs.iterator();
            while (iter.hasNext()) {
                sb.append(iter.next());
                sb.append(" ");
            }

            if (sb.length() > 0) {
                getProject().setNewProperty("config.sel.min.orbs",
                                            sb.toString().trim());
            }

            // Property "config.sel.min.tools"
            sb = new StringBuffer();
            iter = otherTools.iterator();
            while (iter.hasNext()) {
                sb.append(iter.next());
                sb.append(" ");
            }

            if (sb.length() > 0) {
                getProject().setNewProperty("config.sel.min.tools",
                                            sb.toString().trim());
            }

            // Property "config.${csci.name}.tools"
            iter = m_toolsByCsci.keySet().iterator();
            while (iter.hasNext()) {
                String csci = (String) iter.next();
                TreeSet ts = (TreeSet) m_toolsByCsci.get(csci);

                sb = new StringBuffer();
                Iterator iter2 = ts.iterator();
                while (iter2.hasNext()) {
                    sb.append(iter2.next());
                    sb.append(" ");
                }

                if (sb.length() > 0) {
                    getProject().setNewProperty("config." + csci + ".tools",
                                                sb.toString().trim());
                }
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
     * Given a list of tool sets, this function will return a list of tools.
     *
     * @param doc      The DOM document.
     * @param csci     The CSCI name.
     * @param toolsets The list of tool sets.
     *
     * @return The list of selected tools for the specified CSCI.
     */
    protected Collection getToolListByCsci(Document doc,
                                           String csci,
                                           Collection toolsets)
        throws javax.xml.transform.TransformerException {

        TreeSet selected = new TreeSet();

        Iterator iter = toolsets.iterator();
        while (iter.hasNext()) {
            String id = (String) iter.next();

            // Get the type of this tool set.
            StringBuffer xpath = new StringBuffer();
            xpath.append("//toolset[@id='");
            xpath.append(id);
            xpath.append("']/@type");

            boolean isCompiler = false;
            boolean isORB = false;

            XObject xobj = XPathAPI.eval(doc, xpath.toString());
            String result = xobj.str().trim();
            if (!result.equals("")) {
                if (result.equalsIgnoreCase("compiler")) {
                    isCompiler = true;
                } else if (result.equalsIgnoreCase("orb")) {
                    isORB = true;
                }
            }

            // Get the programming language.
            xpath = new StringBuffer();
            xpath.append("//toolset[@id='");
            xpath.append(id);
            xpath.append("']/@lang");

            xobj = XPathAPI.eval(doc, xpath.toString());
            result = xobj.str().trim();
            String lang = "";
            if (!result.equals("")) {
                lang = new String(result);

                if (lang.equalsIgnoreCase("c++")) {
                    lang = "cpp";
                } else {
                    lang = lang.toLowerCase();
                }
            }

            // Get the list of tools contained in this tool set.
            xpath = new StringBuffer();
            xpath.append("//toolset[@id='");
            xpath.append(id);
            xpath.append("']/tool[contains(@os.list, '");
            xpath.append(m_osFamily);
            xpath.append("')]");

            NodeIterator nodes =
                XPathAPI.selectNodeIterator(doc, xpath.toString());
            Node n;
            TreeMap tools = new TreeMap();
            while ((n = nodes.nextNode()) != null) {
                NamedNodeMap atts = n.getAttributes();

                Node name = atts.getNamedItem("name");
                Node desc = atts.getNamedItem("desc");
                tools.put(name.getNodeValue(),
                          (desc == null) ? "" : desc.getNodeValue());
            }

            // Constrain 1 ORB per programming language.
            if (isORB && m_forceUniqueOrb && !m_selOrbs.isEmpty()) {
                Object obj = m_selOrbs.get(lang);
                Vector selOrbs = null;
                if (obj != null) {
                    selOrbs = (Vector) obj;
                }

                if (selOrbs != null) {
                    Iterator toolsIds = tools.keySet().iterator();
                    while (toolsIds.hasNext()) {
                        if (!selOrbs.contains(toolsIds.next())) {
                            toolsIds.remove();
                        }
                    }
                }
            }

            int numTools = tools.size();

            if (numTools > 0) {
                // Select one tool among the tools of this list.
                String theTool = null;

                if (numTools == 1) {
                    // Automatically selected.
                    theTool = (String) tools.firstKey();
                } else {
                    // Check if this conflict is already resolved.
                    boolean isResolved = false;

                    TreeSet ts = (TreeSet) m_toolsByCsci.get(csci);
                    if (ts != null) {
                        Iterator toolIds = tools.keySet().iterator();
                        while (toolIds.hasNext()) {
                            if (ts.contains((String) toolIds.next())) {
                                isResolved = true;
                                break;
                            }
                        }
                    }

                    if (!isResolved) {
                        theTool = getDefault(csci, tools);

                        if (theTool.equals("")) {
                            theTool = makeToolSelection(csci, tools);
                        }
                    }
                }

                if (theTool != null) {
                    String property =
                        "require." + m_osFamily + "." + theTool + ".version";
                    String version = getProject().getProperty(property);
                    if (version == null) {
                        throw new BuildException(
                            "property " + property + " not defined.");
                    }

                    // Must we constrain the build to use one ORB per
                    // programming language ?
                    if (isORB) {
                        Vector selOrbs = new Vector();
                        Object obj = m_selOrbs.get(lang);
                        if (obj != null) {
                            selOrbs = (Vector) obj;
                        }
                        if (!selOrbs.contains(theTool)) {
                            selOrbs.add(theTool);
                            m_selOrbs.put(lang, selOrbs);
                        }

                        if (m_passiveMode) {
                            m_forceUniqueOrbAsked = true;
                        }

                        if (!m_forceUniqueOrbAsked) {
                            Vector yesOrNo = new Vector();
                            yesOrNo.add("y");
                            yesOrNo.add("n");

                            String s =
                                "It is strongly advised to compile all " +
                                "the CSCIs against the same ORB.\n" +
                                "Would you like to enable this contraint ? ";

                            InputRequest requestForceUnique =
                                new MultipleChoiceInputRequest(
                                    s, yesOrNo, "y");

                            // Wait for the answer.
                            m_inputHandler.handleInput(requestForceUnique);

                            if (requestForceUnique.getInput().equals("y")) {
                                m_forceUniqueOrb = true;
                            }

                            m_forceUniqueOrbAsked = true;
                        }
                    }

                    // Create a properfile ANT task to save some properties
                    // temporarily.
                    boolean noSiteProps =
                        new Boolean(getProject().
                            getProperty("no.site.props")).booleanValue();

                    if (!noSiteProps) {
                        PropertyFile pf = (PropertyFile)
                            getProject().createTask("propertyfile");
                        pf.setFile(new File((String)
                            getProject().getProperty("tmpconfig.cache")));

                        if (isCompiler) {
                            property =
                                "config." + csci + "." + lang + ".compiler";
                            getProject().setNewProperty(property, theTool);
                            getProject().setNewProperty(property + ".ver",
                                                        version);
                        } else if (isORB) {
                            property = "config." + csci + "." + lang + ".orb";
                            getProject().setNewProperty(property, theTool);
                            getProject().setNewProperty(property + ".ver",
                                                        version);
                        }

                        PropertyFile.Entry e1 = pf.createEntry();
                        e1.setKey(property);
                        e1.setValue(theTool);

                        PropertyFile.Entry e2 = pf.createEntry();
                        e2.setKey("config." + csci + ".tools");
                        e2.setValue(" " + theTool);
                        PropertyFile.Entry.Operation op =
                            new PropertyFile.Entry.Operation();
                        op.setValue("+");
                        e2.setOperation(op);

                        pf.execute();
                    }

                    selected.add(theTool);
                }
            }
        }

        if (m_toolsByCsci.containsKey(csci)) {
            TreeSet ts = (TreeSet) m_toolsByCsci.get(csci);
            ts.addAll(selected);
            m_toolsByCsci.put(csci, ts);
        } else {
            m_toolsByCsci.put(csci, selected);
        }

        return selected;
    }


    /**
     * Ask the user if he wants to discard the previous selection and make a
     * new selection.
     *
     * @param csci  The CSCI name.
     * @param tools The list of tools in the tool set.
     *
     * @return The default selection for this tool set or an empty string.
     */
    protected String getDefault(String csci, Map tools) {
        String prefix = getProject().getProperty("mb.prev");

        String previous = getProject().getProperty(
            prefix + ".config." + csci + ".tools");

        if ((previous != null) && !previous.equals("")) {
            Vector oldList = StringUtils.split(previous, ' ');

            // We are now going to find out what tool the user has previously
            // selected among those in the "tools" map.
            TreeSet previousChoice = new TreeSet(tools.keySet());
            previousChoice.retainAll(oldList);

            // previousChoice should now contain only one element. If not, it
            // means that the tool set has been modified in the package
            // description file since the last tool selection and therefore
            // the user will be asked to make a new selection.
            if (previousChoice.size() == 1) {
                String toolname = (String) previousChoice.first();

                // check if the toolname is present into the list.
                // if not, return ""
                if (!tools.containsKey(toolname))
                    {
                         log(toolname + "not present", Project.MSG_WARN);
                        return "";
                    }


                if (m_passiveMode) {
                    return toolname;
                } else {
                    // We found his previous choice, ask him if he wants
                    // to let it as is.
                    StringBuffer sb = new StringBuffer();
                    sb.append("The configurator cannot determine which ");
                    sb.append("of these tools ");
                    sb.append(tools);
                    sb.append(" to use for the ");
                    sb.append(csci);
                    sb.append(" CSCI, however you have previously selected [");
                    sb.append(toolname);
                    sb.append("].\nDo you want to discard [");
                    sb.append(toolname);
                    sb.append("] and make a new selection ? ");

                    Vector yesOrNo = new Vector();
                    yesOrNo.add("y");
                    yesOrNo.add("n");

                    InputRequest requestDiscard =
                        new MultipleChoiceInputRequest(sb.toString(),
                                                       yesOrNo, "n");

                    // Wait for the answer.
                    m_inputHandler.handleInput(requestDiscard);

                    if (requestDiscard.getInput().equals("n")) {
                        return toolname;
                    }
                }
            }
        }

        return "";
    }


    /**
     * Run the interactive tool selection.
     *
     * @param csci  The CSCI name.
     * @param tools The list of tools in the tool set.
     *
     * @return The selected tool for this tool set.
     */
    protected String makeToolSelection(String csci, Map tools) {
        // Build the list of choices.
        Vector accept = new Vector();
        for (int i = 1; i <= tools.size(); i++) {
            accept.add(String.valueOf(i));
        }

        StringBuffer sb = new StringBuffer();
        sb.append("List of conflicting tools for the ");
        sb.append(csci).append(" CSCI:\n");

        int idx = 1;
        Iterator iter = tools.keySet().iterator();
        while (iter.hasNext()) {
            String toolname = (String) iter.next();
            sb.append("   ").append(idx++).append(") ").append(toolname);
            sb.append(" - ").append(tools.get(toolname)).append("\n");
        }
        sb.append("\nWhich of them do you want to use ? ");

        // Wait for the user input.
        InputRequest request = new MultipleChoiceInputRequest(
            sb.toString(), accept, null);
        m_inputHandler.handleInput(request);

        int inputNumber =
            new Integer(request.getInput()).intValue() - 1;

        return (String) tools.keySet().toArray()[inputNumber];
    }


}
