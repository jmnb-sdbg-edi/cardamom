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
import java.util.Map;
import java.util.Hashtable;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.Set;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.optional.PropertyFile;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Parse all the CSCProperties.xml files in order to sequence the
 * compilation of the CSCs of each package.
 */
public class FindStaticDeps extends Task {


    // The package description file.
    private File m_pkgDesc = null;

    // The paths of all the CSC properties files.
    private Vector m_paths = new Vector();
    private Vector m_cscs = new Vector();

    // Store the dependencies into this file.
    private File m_file = null;

    // Store the dependencies into an ANT property.
    private String m_property = null;

    // The CSCI names.
    private Vector m_cscis = new Vector();

    // Expand the property.
    private boolean m_expand = false;


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
     * Specify the paths to all the CSC properties files.
     *
     * @param paths The paths to all the CSC properties files.
     */
    public void setPaths(String paths) {
        m_paths = org.apache.tools.ant.util.
                      StringUtils.split(paths.trim(), ' ');
    }


    /**
     * Return the paths to all the CSC properties files.
     *
     * @return A collection of paths.
     */
    public Collection getPaths() {
        return m_paths;
    }


    /**
     * Specify the package dependencies file.
     *
     * @param file The package dependencies file.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Return the package dependencies file.
     *
     * @return The package dependencies file.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Specify the property to create.
     *
     * @param property The property to create.
     */
    public void setProperty(String property) {
        m_property = property;
    }


    /**
     * Get the name of the property.
     *
     * @return The name of the property.
     */
    public String getProperty() {
        return m_property;
    }


    /**
     * Specify the CSC directories.
     *
     * @param cscDirs The CSC directories.
     */
    public void setCscdirs(String cscDirs) {
        String cscDataDir = getProject().getProperty("config.csc.data.dir");
        String cscPropsFile =
            getProject().getProperty("config.csc.properties.file");

        StringTokenizer st = new StringTokenizer(cscDirs);
        while (st.hasMoreTokens()) {
            String s = st.nextToken();

            String file = s + File.separator + cscDataDir +
                          File.separator + cscPropsFile;
            if (!m_paths.contains(file)) {
                m_paths.add(file);
                m_cscs.add(s);
            }
        }
    }


    /**
     * Specify whether the properties must be expanded.
     *
     * @param expand The flag value.
     */
    public void setExpand(boolean expand) {
        m_expand = expand;
    }


    /**
     * Get the expand flag value.
     *
     * @return The expand flag value.
     */
    public boolean getExpand() {
        return m_expand;
    }


    /**
     * Default constructor.
     */
    public FindStaticDeps() {
        super();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_pkgDesc == null) {
            throw new BuildException("attribute pkgdesc must not be null.",
                                     getLocation());
        }

        if ((m_paths == null) || m_paths.isEmpty()) {
            throw new BuildException("attribute paths must not be null.",
                                     getLocation());
        }

        if ((m_file != null) && (m_property != null) && !m_property.equals("")) {
            throw new BuildException(
                "attributes file and property are mutually exclusive.",
                getLocation());
        }

        if ((m_file == null) && (m_property == null)) {
            throw new BuildException(
                "either attribute file or property must be set", getLocation());
        }
    }


    /**
     * Runs the task.
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

            // Get the CSCI names.
            NodeIterator cscis =
                XPathAPI.selectNodeIterator(doc, "//csci/@name");
            Node n;
            while ((n = cscis.nextNode()) != null) {
                String s = (String) n.getNodeValue();

                if (!s.trim().equals("")) {
                    m_cscis.add(s);
                } else {
                    log(m_pkgDesc + ": incorrect CSCI name: " + s,
                        Project.MSG_DEBUG);
                }
            }

            // Get the interfaces.
            NodeIterator nodes =
                XPathAPI.selectNodeIterator(doc, "//interface/@lang");
            Vector langs = new Vector();
            while ((n = nodes.nextNode()) != null) {
                String lang = (String) n.getNodeValue();

                if (!langs.contains(lang)) {
                    langs.add(lang);
                }
            }

            LinkedList cscs    = new LinkedList();
            LinkedList delivs  = new LinkedList();
            Hashtable cscideps = new Hashtable();

            sortByDeps(m_paths, cscs, delivs, cscideps);

            // Expand properties.
            LinkedHashSet lhs = new LinkedHashSet();
            Iterator it = cscs.iterator();
            while (it.hasNext()) {
                String deliv = (String) it.next();
                int pos1 = deliv.indexOf("/");

                if (pos1 != -1) {
                    //int pos2 = deliv.lastIndexOf("/");
                    String cscpath = "";

                    if (m_expand) {
                        cscpath = getProject().getProperty(
                                  "config." + deliv.substring(0, pos1) +
                                  ".location");
                    } else {
                        cscpath = "${config." + deliv.substring(0, pos1) +
                                  ".location}";
                    }
                    //cscpath += deliv.substring(pos1, pos2);
                    cscpath += deliv.substring(pos1);

                    Iterator it2 = langs.iterator();
                    while (it2.hasNext()) {
                        lhs.add(cscpath + "/" + (String) it2.next());
                    }
                } else {
                    log("ignored: " + deliv, Project.MSG_DEBUG);
                }
            }

            it = lhs.iterator();
            StringBuffer sb = new StringBuffer();
            while (it.hasNext()) {
                sb.append(it.next()).append(" ");
            }

            if (m_property != null) {
                // Save the result into a property.
                Vector v = new Vector();
                StringTokenizer st = new StringTokenizer(sb.toString().trim());
                while (st.hasMoreTokens()) {
                    v.add(st.nextToken());
                }

                if (v.isEmpty()) {
                    v.addAll(m_cscs);
                } else {
                    v.retainAll(m_cscs);
                }

                it = v.iterator();
                sb = new StringBuffer();
                while (it.hasNext()) {
                    sb.append(it.next()).append(" ");
                }
                getProject().setNewProperty(m_property, sb.toString().trim());
            } else {
                // Save the result into a file.
                PropertyFile pf = null;

                pf = (PropertyFile) getProject().createTask("propertyfile");
                pf.setFile(m_file);

                PropertyFile.Entry e = pf.createEntry();
                e.setKey("compilation.sequence");
                e.setValue(sb.toString().trim());

                it = cscideps.keySet().iterator();
                while (it.hasNext()) {
                    String csci = (String) it.next();

                    PropertyFile.Entry e2 = pf.createEntry();
                    e2.setKey("config." + csci + ".globaldeps");

                    LinkedHashSet globaldeps =
                        (LinkedHashSet) cscideps.get(csci);
                    Iterator it2 = globaldeps.iterator();
                    sb = new StringBuffer();
                    while (it2.hasNext()) {
                        sb.append(it2.next()).append(" ");
                    }

                    e2.setValue(sb.toString().trim());
                }

                PropertyFile.Entry e3 = pf.createEntry();
                e3.setKey("all.deliveries");

                it = delivs.iterator();
                sb = new StringBuffer();
                while (it.hasNext()) {
                    String deliv = (String) it.next();
                    int pos1 = deliv.indexOf("/");

                    if (pos1 != -1) {
                        //int pos2 = deliv.lastIndexOf("/");
                        String cscpath = "";

                        if (m_expand) {
                            cscpath = getProject().getProperty(
                                      "config." + deliv.substring(0, pos1) +
                                      ".location");
                        } else {
                            cscpath = "${config." + deliv.substring(0, pos1) +
                                      ".location}";
                        }

                        sb.append(cscpath)
                          .append(deliv.substring(pos1)).append(" ");
                    } else {
                        log("ignored: " + deliv, Project.MSG_DEBUG);
                    }
                }
                e3.setValue(sb.toString().trim());

                pf.execute();
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
     * Parse all the CSC properties file and sort the CSCs according to their
     * dependencies.
     *
     * @param paths    The paths to the CSCs.
     * @param list     The sorted list of CSCs.
     * @param delivs   The list of deliveries and their dependencies.
     * @param cscideps The CSCI dependencies.
     */
    protected void sortByDeps(Collection m_paths,
                              LinkedList cscs,
                              LinkedList delivs,
                              Map cscideps) {

        Hashtable map = new Hashtable();

        Iterator it = m_paths.iterator();
        while (it.hasNext()) {
            FileInputStream fis = null;

            String f = null;
            try {
                f = (String) it.next();
                fis = new FileInputStream(f);

                // Set up a DOM tree to query.
                InputSource in = new InputSource(fis);
                DocumentBuilderFactory dfactory =
                    DocumentBuilderFactory.newInstance();
                dfactory.setNamespaceAware(true);
                Document doc = dfactory.newDocumentBuilder().parse(in);

                // CSCI name.
                XObject xobj = XPathAPI.eval(doc, "//csc/@owner");
                String csci = xobj.str().trim();

                if (csci.trim().equals("")) {
                    throw new Exception("bad owner name: " + csci);
                }

                // Path to the CSC.
                int pos1 = f.lastIndexOf(csci + File.separator);

                if (pos1 != -1) {
                    int pos2 = f.lastIndexOf(
                        getProject().getProperty("config.csc.data.dir")
                        + File.separator);

                    String cscpath =
                        f.substring(pos1,
                            f.lastIndexOf(File.separator, pos2 - 2));

                    // Create mappings.
                    NodeIterator nodes =
                        XPathAPI.selectNodeIterator(doc,
                            "//delivery[@visibility='public' " +
                            "and @type!='data']/@basename");
                    Node n;
                    while ((n = nodes.nextNode()) != null) {
                        String nodeValue = n.getNodeValue();

                        map.put(csci + "/" + nodeValue,
                                cscpath + "/" + nodeValue);
                    }
                } else {
                    throw new Exception(
                        "could not find the CSCI name (" + csci + ") " +
                        "in the file name " + f);
                }

                doc = null;
            } catch (Exception e) {
                log(f + ": error: " + e.getMessage(), Project.MSG_WARN);
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

        Hashtable deps = new Hashtable();
        Vector prereqs = new Vector();
        prereqs.add("libraries");
        prereqs.add("jnilibraries");
        prereqs.add("executables");
        prereqs.add("requireidlsfrom");

        it = m_paths.iterator();
        while (it.hasNext()) {
            FileInputStream fis = null;

            String f = null;
            try {
                f = (String) it.next();
                fis = new FileInputStream(f);

                // Set up a DOM tree to query.
                InputSource in = new InputSource(fis);
                DocumentBuilderFactory dfactory =
                    DocumentBuilderFactory.newInstance();
                dfactory.setNamespaceAware(true);
                Document doc = dfactory.newDocumentBuilder().parse(in);

                // CSCI name.
                XObject xobj = XPathAPI.eval(doc, "//csc/@owner");
                String csci = xobj.str().trim();

                if (csci.trim().equals("")) {
                    throw new Exception("bad owner name: " + csci);
                }

                LinkedHashSet globaldeps = new LinkedHashSet();

                // CSC type.
                xobj = XPathAPI.eval(doc, "//csc/@type");
                String cscType = xobj.str().trim();

                // Path to the CSC.
                int pos1 = f.lastIndexOf(csci + File.separator);

                if (pos1 != -1) {
                    int pos2 = f.lastIndexOf(
                        getProject().getProperty("config.csc.data.dir")
                        + File.separator);

                    String cscpath =
                        f.substring(pos1,
                            f.lastIndexOf(File.separator, pos2 - 2));

                    NodeIterator nodes =
                        XPathAPI.selectNodeIterator(
                            doc, "//delivery[@type!='data']");
                    Node n;
                    while ((n = nodes.nextNode()) != null) {
                        xobj = XPathAPI.eval(n, "@basename");
                        String basename = xobj.str().trim();
                        String key = cscpath + "/" + basename;

                        xobj = XPathAPI.eval(n, "@type");
                        String delivType = xobj.str().trim();

                        // not interested in processing data deliveries
                        //if (delivType.equals("data")) continue;

                        key += "#" + delivType;
                        if (!delivs.contains(key)) {
                            delivs.add(key);
                            //deps.put(key, new LinkedHashSet());
                        }

                        if (!deps.containsKey(cscpath)) {
                            deps.put(cscpath, new LinkedHashSet());
                        }

                        Iterator it2 = prereqs.iterator();
                        while (it2.hasNext()) {
                            String prereq = (String) it2.next();

                            String prereqType = "";
                            if (prereq.equals("libraries") ||
                                prereq.equals("requireidlsfrom")) {

                                prereqType = "lib";
                            } else if (prereq.equals("executables")) {
                                prereqType = "exe";
                            } else if (prereq.equals("jnilibraries")) {
                                prereqType = "jnilib";
                            }

                            NodeIterator prereqsNodeIter =
                                XPathAPI.selectNodeIterator(
                                    n, "descendant::" + prereq);
                            Node n2;

                            while ((n2 = prereqsNodeIter.nextNode()) != null) {
                                Node n3 = n2.getFirstChild();

                                if (n3 != null) {
                                    StringTokenizer st =
                                        new StringTokenizer(
                                            n3.getNodeValue());

                                    while (st.hasMoreTokens()) {
                                        String s = st.nextToken();

                                        int pos = s.indexOf("/");
                                        String firstToken = "";
                                        if (pos != -1) {
                                            firstToken = s.substring(0, pos);
                                                //s.substring(0, s.indexOf("/"));
                                        } else {
                                            // case: requireidlsfrom
                                            firstToken = s;
                                        }
                                        String deliv = null;

                                        /*
                                        String prereqType = "";
                                        if (prereq.equals("libraries") ||
                                            prereq.equals("requireidlsfrom")) {

                                            prereqType = "lib";
                                        } else
                                            if (prereq.equals("executables")) {
                                                prereqType = "exe";
                                        } else
                                            if (prereq.equals("jnilibraries")) {
                                                prereqType = "jnilib";
                                        }
                                        */

                                        // the dependency is from other CSCIs
                                        if (m_cscis.contains(firstToken)) {
                                            if (!firstToken.equals(s)) {
                                                deliv = (String) map.get(s);
                                            } else {
                                                // case: requireidlsfrom
                                                deliv = s + "/idllib/x";
                                            }
                                            deliv += "#" + prereqType;
                                            globaldeps.add(firstToken);
                                        }

                                        // the dependency is from the same CSCI
                                        if (deliv == null) {
                                            deliv = csci + "/" + s +
                                                    "#" + prereqType;
                                        }

                                        LinkedHashSet set = (LinkedHashSet)
                                            deps.get(cscpath);
                                            //deps.get(key);

                                        if (set == null) {
                                            set = new LinkedHashSet();
                                        }

                                        /*
                                        set.add(deliv);
                                        deps.put(key, set);
                                        */
                                        if (!deliv.startsWith("null#")) {
                                            set.add(deliv.substring(0,
                                                    deliv.lastIndexOf("/")));
                                        }
                                        deps.put(cscpath, set);
                                    }
                                }
                            }
                        }
                    }

                    if (cscType.equals("")) {
                        LinkedHashSet lhs = (LinkedHashSet) cscideps.get(csci);
                        if (lhs == null) {
                            cscideps.put(csci, globaldeps);
                        } else {
                            lhs.addAll(globaldeps);
                            cscideps.put(csci, lhs);
                        }
                    }
                } else {
                    throw new Exception(
                        "could not find the CSCI name (" + csci + ") " +
                        "in the file name");
                }

                doc = null;
            } catch (Exception e) {
                log(f + ": error: " + e.getMessage(), Project.MSG_WARN);
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

        it = deps.keySet().iterator();
        while (it.hasNext()) {
            String key = (String) it.next();

            // key was already processed, skip
            if (cscs.contains(key)) continue;

            cscs.add(key);
            sort(key, key, deps, cscs);
        }
    }


    /**
     * @param key
     * @param dep
     * @param deps
     * @param cscs
     */
    private void sort(String key, String dep, Map deps, LinkedList cscs) {
        // Sanity check.
        if (deps == null) {
            throw new IllegalArgumentException(
                "argument deps must not be null");
        }

        if (cscs == null) {
            throw new IllegalArgumentException(
                "argument cscs must not be null");
        }

        LinkedHashSet set = (LinkedHashSet) deps.get(dep);

        if ((set == null) || set.isEmpty()) {
            if (!cscs.contains(dep)) {
                if (cscs.size() <= 1) {
                    cscs.addFirst(dep);
                } else {
                    if (!cscs.contains(key)) {
                        cscs.add(dep);
                    } else {
                        cscs.add(cscs.indexOf(key), dep);
                    }
                }
            }
        } else {
            if (!cscs.contains(dep)) {
                if (!cscs.contains(key)) {
                    cscs.add(dep);
                } else {
                    cscs.add(cscs.indexOf(key), dep);
                }
            }

            Iterator it2 = set.iterator();
            while (it2.hasNext()) {
                String dep_ = (String) it2.next();

                // dep_ was already processed, skip
                if (cscs.contains(dep_)) continue;

                sort(dep, dep_, deps, cscs);
            }
        }
    }


}
