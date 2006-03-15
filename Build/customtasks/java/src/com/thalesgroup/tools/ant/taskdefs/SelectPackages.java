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
import java.util.Map;
import java.util.StringTokenizer;
import java.util.Hashtable;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Run the interactive package selection.
 */
public class SelectPackages extends Task {


    // The package description file.
    private File m_pkgDesc = null;

    // The OS family name.
    private String m_osFamily = null;

    // The list of CSCIs available on the system.
    private Vector m_availableCscis = null;

    // The previous package selection.
    private Vector m_defaultPkgs = null;

    // The previous interface selection.
    private Vector m_defaultItfs = null;

    // The previous selection of CSCIs.
    private Vector m_defaultCscis = null;

    // The input handler.
    private InputHandlerImpl m_inputHandler = new InputHandlerImpl();

    // The list of packages.
    private Map m_packages;


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
     * Specify the list of CSCIs available on the system.
     *
     * @param cscis The list of available CSCIs.
     */
    public void setAvailableCscis(String cscis) {
        m_availableCscis =
            org.apache.tools.ant.util.StringUtils.split(cscis, ' ');
    }


    /**
     * Get the list of CSCIs available on the system.
     *
     * @return The list of available CSCIs.
     */
    public Collection getAvailableCscis() {
        return m_availableCscis;
    }


    /**
     * Specify the default list of selected packages.
     *
     * @param defaultPkgs The default list of selected packages.
     */
    public void setDefaultPkgs(String defaultPkgs) {
        m_defaultPkgs =
            org.apache.tools.ant.util.StringUtils.split(defaultPkgs, ' ');
        if (m_defaultPkgs.isEmpty())
            m_defaultPkgs = null;
    }


    /**
     * Get the default list of selected packages.
     *
     * @return The default list of selected packages.
     */
    public Collection getDefaultPkgs() {
        return m_defaultPkgs;
    }


    /**
     * Specify the default list of selected interfaces.
     *
     * @param defaultItfs The default list of selected interfaces.
     */
    public void setDefaultItfs(String defaultItfs) {
        m_defaultItfs =
            org.apache.tools.ant.util.StringUtils.split(defaultItfs, ' ');
    }


    /**
     * Get the default list of selected interfaces.
     *
     * @return The default list of selected interfaces.
     */
    public Collection getDefaultItfs() {
        return m_defaultItfs;
    }


    /**
     * Specify the default list of selected CSCIs.
     *
     * @param defaultCscis The default list of selected CSCIs.
     */
    public void setDefaultCscis(String defaultCscis) {
        m_defaultCscis =
            org.apache.tools.ant.util.StringUtils.split(defaultCscis, ' ');
    }


    /**
     * Get the default list of selected CSCIs.
     *
     * @return The default list of selected CSCIs.
     */
    public Collection getDefaultCscis() {
        return m_defaultCscis;
    }


    /**
     * Default constructor.
     */
    public SelectPackages() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();


        m_packages = getPackages();

        if (m_defaultPkgs != null) {
            // if the m_defaultPkgs is incompatible with the new package set, 
            // put the m_defaultPkgs to NULL
            Iterator iter = m_defaultPkgs.iterator();
            while (iter.hasNext()) {
                String s = new String((String) iter.next());
                if (!m_packages.containsKey(s)) {
                    m_defaultPkgs = null;
                    break;
                }
            }
        }
        
        if (discardDefault()) {
            if (!m_packages.isEmpty()) {
                makePackageSelection();
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

        if ((m_osFamily == null) || m_osFamily.equals("")) {
            throw new BuildException("attribute osFamily must not be null.");
        }

        if ((m_availableCscis == null) || m_availableCscis.isEmpty()) {
            throw new BuildException(
                "attribute availableCscis must not be null.");
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
        if ((m_defaultPkgs != null) && !m_defaultPkgs.isEmpty() &&
            (m_defaultItfs != null) && !m_defaultItfs.isEmpty() &&
            (m_defaultCscis != null) && !m_defaultCscis.isEmpty()) {

            String msg = "Your previous selection was " + m_defaultPkgs +
                         " with the " + m_defaultItfs + " interfaces.\n" +
                         "Do you want to discard the list and make a " +
                         "new selection ? ";

            Vector yesOrNo = new Vector();
            yesOrNo.add("y");
            yesOrNo.add("n");

            InputRequest requestDiscard =
                new MultipleChoiceInputRequest(msg, yesOrNo, "n");

            // Wait for the answer.
            m_inputHandler.handleInput(requestDiscard);

            if (requestDiscard.getInput().equals("n")) {
                log("Final selection: " + m_defaultPkgs + "/" + m_defaultItfs,
                    Project.MSG_WARN);

                // Property "config.sel.packages"
                StringBuffer sb = new StringBuffer();
                Iterator iter = m_defaultPkgs.iterator();
                while (iter.hasNext()) {
                    sb.append((String) iter.next());
                    sb.append(" ");
                }
                getProject().setNewProperty(
                    "config.sel.packages", sb.toString().trim());

                // Property "config.sel.interfaces"
                sb = new StringBuffer();
                iter = m_defaultItfs.iterator();
                while (iter.hasNext()) {
                    sb.append((String) iter.next());
                    sb.append(" ");
                }
                getProject().setNewProperty(
                    "config.sel.interfaces", sb.toString().trim());

                // Property "config.sel.min.cscis"
                sb = new StringBuffer();
                iter = m_defaultCscis.iterator();
                while (iter.hasNext()) {
                    sb.append((String) iter.next());
                    sb.append(" ");
                }
                getProject().setNewProperty(
                    "config.sel.min.cscis", sb.toString().trim());

                return false;
            }
        }

        return true;
    }


    /**
     * Get all the package descriptions.
     *
     * @return The list of packages.
     */
    protected Map getPackages() {
        TreeMap packages = new TreeMap();
        FileInputStream fis = null;

        try {
            fis = new FileInputStream(m_pkgDesc);

            // Set up a DOM tree to query.
            InputSource in = new InputSource(fis);
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // Get all the packages that run on the specified OS.
            StringBuffer xpath = new StringBuffer();
            xpath.append("//package[contains(os/text(), '");
            xpath.append(m_osFamily);
            xpath.append("')]");

            TreeMap uncompletePkgs = new TreeMap();

            NodeIterator nodes =
                XPathAPI.selectNodeIterator(doc, xpath.toString());
            Node n;
            while ((n = nodes.nextNode()) != null) {
                PackageNode pkg = new PackageNode(n);

                // Check if the package can be built, i.e. it has no missing
                // dependencies.
                Collection missingDeps = pkg.getMissingDependencies();

                if (missingDeps.isEmpty()) {
                    packages.put(pkg.getId(), pkg);
                } else {
                    uncompletePkgs.put(pkg.getId(), pkg);
                }

                // And now proceed with its options.
                Iterator iter = pkg.getOptions().iterator();
                while (iter.hasNext()) {
                    PackageNode opt = (PackageNode) iter.next();
                    missingDeps = opt.getMissingDependencies();

                    if (missingDeps.isEmpty()) {
                        packages.put(opt.getId(), opt);
                    } else {
                        uncompletePkgs.put(opt.getId(), opt);
                    }
                }
            }
 
            if (packages.isEmpty()) {
                StringBuffer sb = new StringBuffer();
                sb.append("You do not have enough CSCIs to build any ");
                sb.append("packages.\nHere are the missing CSCIs for ");
                sb.append("each package:\n");
                
                Iterator iter = uncompletePkgs.keySet().iterator();
                while (iter.hasNext()) {
                    String id = (String) iter.next();
                    PackageNode pkg = (PackageNode) uncompletePkgs.get(id);

                    sb.append("    ").append(id).append(": ");
                    sb.append(pkg.getMissingDependencies()).append("\n");
                }

                log(sb.toString(), Project.MSG_WARN);
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

        return packages;
    }


    /**
     * Run the interactive package selection.
     */
    protected void makePackageSelection() {
        TreeSet selected = new TreeSet();
        boolean endSelection = false;

        Vector packageList =
            new Vector(availableForSelection(m_packages.keySet(), selected));

        endSelection =
            ((packageList == null) || (packageList.isEmpty())) ? true : false;

        while (!endSelection) {
            if (!selected.isEmpty() && (packageList.size() == 1)) {
                StringBuffer sb = new StringBuffer();
                sb.append("Only one package left in the list: ");
                sb.append(packageList);
                sb.append(".\nThis package is automatically selected.");

                log(sb.toString(), Project.MSG_WARN);
                selected.add(packageList.get(0));

                break;
            }

            StringBuffer sb = new StringBuffer();
            sb.append("List of package(s): \n");
            int idx = 1;
            Iterator iter = packageList.iterator();
            while (iter.hasNext()) {
                PackageNode pkg =
                    (PackageNode) m_packages.get(iter.next());

                sb.append("    ").append(idx++);
                sb.append(") " + (pkg.isOption() ? "    " : ""));
                sb.append(pkg.getId()).append(" - ");

                if (pkg.isOption()) {
                    PackageNode parent =
                        (PackageNode) m_packages.get(pkg.getParentId());
                    sb.append(parent.getDesc()).append(" + ");
                }

                sb.append(pkg.getDesc()).append("\n");
            }
            sb.append("Enter the number of the package and/or the group of ");
            sb.append("packages separated by semi-colons (ex: 1;3;5-12;4)");

            // Define the regex pattern that will validate entries
            // like " 1; 3;5-12 ;4 ".
            String pattern = "\\s*\\d+(-\\d+)?(\\s*;\\s*\\d+(-\\d+)?)*\\s*";

            // Wait for the answer.
            InputRequest request = new RangeInputRequest(
                sb.toString(), pattern, 1, packageList.size());
            m_inputHandler.handleInput(request);

            // Save the user input.
            selected.addAll(
                StringUtils.mapRangeIntoNames(request.getInput().trim(),
                                              packageList.toArray()));

            // Get the remaining packages.
            packageList = new Vector(availableForSelection(m_packages.keySet(),
                                                           selected));

            if (!packageList.isEmpty()) {
                Vector yesOrNo = new Vector();
                yesOrNo.add("y");
                yesOrNo.add("n");

                // Ask the user if he wants to add another package to the list.
                InputRequest requestContinue =
                    new MultipleChoiceInputRequest(
                        "Selected package(s): " + selected + "\n" +
                        "Do you want to add more packages ? ", yesOrNo, null);

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

        // Property "config.sel.packages"
        StringBuffer sb = new StringBuffer();
        TreeSet minDeps = new TreeSet();
        Iterator iter = selected.iterator();
        while (iter.hasNext()) {
            PackageNode pkg = (PackageNode) m_packages.get(iter.next());

            sb.append(pkg.getId());
            sb.append(" ");

            // Build the list of minimum CSCI dependencies.
            Iterator iter2 = pkg.getCscis().iterator();
            while (iter2.hasNext()) {
                minDeps.add((String) iter2.next());
            }
        }
        getProject().setNewProperty("config.sel.packages",
                                    sb.toString().trim());

        // Property "config.sel.min.cscis"
        sb = new StringBuffer();
        iter = minDeps.iterator();
        while (iter.hasNext()) {
            sb.append((String) iter.next());
            sb.append(" ");

        }
        getProject().setNewProperty("config.sel.min.cscis",
                                    sb.toString().trim());

        makeInterfaceSelection(selected);
    }


    /**
     * Run the interactive interface selection.
     *
     * @param selected The list of selected packages.
     */
    protected void makeInterfaceSelection(Collection selected) {
        TreeSet interfaces = new TreeSet();

        Iterator iter = selected.iterator();
        while (iter.hasNext()) {
            PackageNode pkg = (PackageNode) m_packages.get(iter.next());
            Collection supportedItfs = pkg.getInterfaces();

            if (supportedItfs != null) {
                Iterator iter2 = supportedItfs.iterator();
                while (iter2.hasNext()) {
                    interfaces.add((String) iter2.next());
                }
            }
        }

        StringBuffer sb = new StringBuffer();
        boolean endSelection = false;

        while (!endSelection) {
            switch (interfaces.size()) {
                case 0:
                    throw new BuildException(
                        "No common interfaces available for the packages: " +
                        selected);

                case 1:
                    sb.append((String) interfaces.first());
                    endSelection = true;
                    break;

                default:
                    Vector yesOrNo = new Vector();
                    yesOrNo.add("y");
                    yesOrNo.add("n");

                    iter = interfaces.iterator();
                    while (iter.hasNext()) {
                        String itf = (String) iter.next();

                        // Ask the user if he wants to keep use this interface.
                        InputRequest requestKeep = 
                            new MultipleChoiceInputRequest(
                                "Do you want to enable the " +
                                itf.toUpperCase() + " interface ? ",
                                yesOrNo, "n");
                        m_inputHandler.handleInput(requestKeep);

                        if (requestKeep.getInput().equals("y")) {
                            sb.append(itf);
                            sb.append(" ");
                        }
                    }

                    if (sb.length() == 0) {
                        log("You must select at leat one interface.",
                            Project.MSG_WARN);
                        endSelection = false;
                    } else {
                        endSelection = true;
                    }
            }
        }

        getProject().setNewProperty("config.sel.interfaces",
                                    sb.toString().trim());
    }


    /**
     * Determine the CSCIs that are still available for selection.
     *
     * @param allIds   The complete list of package identifiers.
     * @param selected The list of packages that have already been selected.
     *
     * @return The list of packages that have not been selected yet.
     */
    protected Collection availableForSelection(Collection allIds,
                                               Collection selected) {

        TreeSet available = new TreeSet(allIds);
        available.removeAll(selected);

        // Ensure that if an option was selected then its parent must be
        // selected as well.
        TreeSet selectedParents = new TreeSet();
        Iterator iter = selected.iterator();
        while (iter.hasNext()) {
            PackageNode pkg =
                (PackageNode) m_packages.get((String) iter.next());

            if (pkg.isOption()) {
                String parentId = pkg.getParentId();

                available.remove(parentId);
                selectedParents.add(parentId);
            }
        }

        selected.addAll(selectedParents);

        return available;
    }


    /**
     * A "package" node.
     */
    protected class PackageNode {


        // The packade id.
        private String m_id = null;

        // The description of the package.
        private String m_desc = null;

        // The list of dependencies.
        private TreeSet m_cscis = new TreeSet();

        // The list of supported interfaces.
        private TreeSet m_interfaces = new TreeSet();

        // The list of options.
        private Vector m_options = new Vector();

        // Indicate whether or not this package is an option.
        private boolean m_isOption = false;

        // The parent id.
        private String m_parentId = null;


        /**
         * Constructor.
         *
         * @param pkgNode The package node.
         */
        public PackageNode(Node pkgNode)
            throws javax.xml.transform.TransformerException {

            if (pkgNode == null) {
                throw new IllegalArgumentException("pkgNode must not be null.");
            }

            setId(pkgNode);
            setDesc(pkgNode);
            setCscis(pkgNode);
            setInterfaces(pkgNode);
            setOptions(pkgNode);
        }


        /**
         * Specify the package id.
         *
         * @param pkgNode The package node.
         */
        public void setId(Node pkgNode)
            throws javax.xml.transform.TransformerException {

            XObject xobj = XPathAPI.eval(pkgNode, "@id");
            m_id = xobj.str().trim();
        }


        /**
         * Get the package id.
         *
         * @return The package id.
         */
        public String getId() {
            return m_id;
        }


        /**
         * Specify the description of the package.
         *
         * @param pkgNode The package node.
         */
        public void setDesc(Node pkgNode)
            throws javax.xml.transform.TransformerException {

            XObject xobj = XPathAPI.eval(pkgNode, "@desc");
            m_desc = xobj.str().trim();
        }


        /**
         * Get the description of this package.
         *
         * @return The description of this package.
         */
        public String getDesc() {
            return m_desc;
        }


        /**
         * Specify the dependencies.
         *
         * @param pkgNode The package node.
         */
        public void setCscis(Node pkgNode)
            throws javax.xml.transform.TransformerException {

            XObject xobj = XPathAPI.eval(pkgNode, "cscis");
            StringTokenizer st = new StringTokenizer(xobj.str());
            while (st.hasMoreTokens()) {
                m_cscis.add(st.nextToken());
            }
        }


        /**
         * Get the dependencies.
         *
         * @return The list dependencies.
         */
        public Collection getCscis() {
            return m_cscis;
        }


        /**
         * Specify the supported interfaces.
         *
         * @param pkgNode The package node.
         */
        public void setInterfaces(Node pkgNode)
            throws javax.xml.transform.TransformerException {

            Hashtable itfcounters = new Hashtable();

            Iterator iter = m_cscis.iterator();
            while (iter.hasNext()) {
                String xpath = "//csci[@name='" + iter.next() +
                               "']/interface/cscs/set/packageid";

                // Get all the interfaces which contain at least one set of
                // CSCs where this package id is listed in.
                NodeIterator nodes =
                    XPathAPI.selectNodeIterator(pkgNode.getOwnerDocument(),
                                                xpath);
                Node node;
                Vector itfs = new Vector();
                while ((node = nodes.nextNode()) != null) {
                    Vector pkgids =
                        org.apache.tools.ant.util.StringUtils.split(
                            StringUtils.normalizeSpaces(
                                node.getFirstChild().getNodeValue()), ' ');

                    if (pkgids.contains(m_id) || pkgids.contains("any")) {
                        XObject xobj =
                            XPathAPI.eval(node, "ancestor::interface/@lang");
                        String lang = xobj.str().trim();

                        if (!itfs.contains(lang)) {
                            if (itfcounters.containsKey(lang)) {
                                Integer n = (Integer) itfcounters.get(lang);
                                itfcounters.put(lang,
                                                new Integer(n.intValue() + 1));
                            } else {
                                itfcounters.put(lang, new Integer(1));
                            }

                            itfs.add(lang);
                        }
                    }
                }
            }

            // Keep only the interfaces that are common to all the CSCIs listed
            // in m_cscis.
            int nbCscis = m_cscis.size();
            iter = itfcounters.keySet().iterator();
            while (iter.hasNext()) {
                String lang = (String) iter.next();
                Integer count = (Integer) itfcounters.get(lang);

                if (count.intValue() == nbCscis) {
                    m_interfaces.add(lang);
                }
            }
        }


        /**
         * Get the supported interfaces.
         *
         * @return The supported interfaces.
         */
        public Collection getInterfaces() {
            return m_interfaces;
        }


        /**
         * Specify the options.
         *
         * @param pkgNode The package node.
         */
        public void setOptions(Node pkgNode)
            throws javax.xml.transform.TransformerException {
            
            if (pkgNode.getNodeName().equals("option")) {
                m_isOption = true;
            } else {
                m_isOption = false;

                String xpath = "option[contains(os/text(), '" +
                               m_osFamily + "')]";

                NodeIterator nodes =
                    XPathAPI.selectNodeIterator(pkgNode, xpath);
                Node node;
                while ((node = nodes.nextNode()) != null) {
                    PackageNode pkg = new PackageNode(node);

                    TreeSet cscis = (TreeSet) pkg.getCscis();
                    cscis.addAll(m_cscis);
                    
                    pkg.setParentId(m_id);
                    m_options.add(pkg);
                }
            }
        }


        /**
         * Get the list of options.
         *
         * @return The list of options.
         */
        public Collection getOptions() {
            return m_options;
        }


        /**
         * Determine whether this is package is an option.
         *
         * @return <code>true<code> if this package is an option.
         */
        public boolean isOption() {
            return m_isOption;
        }


        /**
         * Specify the parent id.
         *
         * @param parentId The parent id.
         */
        public void setParentId(String parentId) {
            m_parentId = parentId;
        }


        /**
         * Get the parent id.
         *
         * @return The parent id.
         */
        public String getParentId() {
            return m_parentId;
        }


        /**
         * Get the missing dependencies.
         *
         * @return The list of missing dependencies.
         */
        public Collection getMissingDependencies() {
            TreeSet missingDeps = (TreeSet) m_cscis.clone();
            missingDeps.removeAll(m_availableCscis);

            return missingDeps;
        }


    }


}

