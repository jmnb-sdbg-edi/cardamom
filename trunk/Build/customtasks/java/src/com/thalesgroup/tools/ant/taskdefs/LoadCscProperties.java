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
import com.thalesgroup.tools.ant.util.FileUtils;

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
import java.util.TreeSet;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.Ant;
import org.apache.tools.ant.taskdefs.optional.PropertyFile;
import org.apache.tools.ant.types.FileSet;
import org.apache.tools.ant.types.FilterSet;
import org.apache.tools.ant.types.FilterSetCollection;
import org.apache.tools.ant.util.StringUtils;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Load CSCProperties.xml and create ANT properties.
 */
public class LoadCscProperties extends Task {

    // Source file.
    private File m_file = null;

    // Property prefix.
    private String m_prefix = "";

    // Parse the deliveries element ?
    private boolean m_includeDeliveries = true;

    // Parse the runtest element ?
    private boolean m_includeTests = false;

    // Parse the inspection element ?
    private boolean m_includeInspection = false;

    // The CSC name.
    private String m_cscName = null;

    // The CSCI which contains this CSC.
    private String m_cscOwner = null;

    // Set constraint on dependencies.
    private boolean m_depsConstraints = false;

    // The programming language used in this CSC.
    private String m_cscLang = null;
    private boolean m_isJAVA = false;
    private boolean m_isCPP = false;
    private boolean m_isJNI = false;
    private boolean m_isADA = false;

    // The list of deliveries to be compiled.
    private StringBuffer m_deliveriesToBuild = new StringBuffer();

    // Holds FilterSets.
    private Vector m_filterSets = new Vector();

    // ANT properties.
    private Hashtable m_properties = new Hashtable();

    // Global dependencies.
    private TreeSet m_globalDeps = new TreeSet();

    private Vector m_allDeliveries = new Vector();


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
     * Specify the property prefix.
     *
     * @param prefix The property prefix.
     */
    public void setPrefix(String prefix) {
        m_prefix = prefix;
    }


    /**
     * Get the property prefix.
     *
     * @return The property prefix.
     */
    public String getPrefix() {
        return m_prefix;
    }


    /**
     * Specify whether the deliveries element must be parsed.
     *
     * @param includeDeliveries If true, then parse the deliveries element.
     */
    public void setIncludeDeliveries(boolean includeDeliveries) {
        m_includeDeliveries = includeDeliveries;
    }


    /**
     * Get the include deliveries flag.
     *
     * @return The include deliveries flag value.
     */
    public boolean getIncludeDeliveries() {
        return m_includeDeliveries;
    }


    /**
     * Specify whether the runtest element must be parsed.
     *
     * @param includeTests If true, then parse the runtest element.
     */
    public void setIncludeTests(boolean includeTests) {
        m_includeTests = includeTests;
    }


    /**
     * Get the include tests flag.
     *
     * @return The include tests flag value.
     */
    public boolean getIncludeTests() {
        return m_includeTests;
    }


    /**
     *  Specify whether the inspection element must be parsed.
     *
     *  @param includeInspection If true, then parse the inspection element.
     */
    public void setIncludeInspection(boolean includeInspection) {
        m_includeInspection = includeInspection;
    }


    /**
     * Get the include inspection flag.
     *
     * @return The include inspection flag value.
     */
    public boolean getIncludeInspection() {
        return m_includeInspection;
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
    public LoadCscProperties() {
        super();
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_file == null) {
            throw new BuildException("file must not be null.",
                                     getLocation());
        }

        if (!m_file.exists()) {
            throw new BuildException("file does not exist.",
                                     getLocation());
        }

        if (!m_file.isFile()) {
            throw new BuildException("source file is not a file.",
                                     getLocation());
        }

        if ((m_prefix.length() > 1) && !m_prefix.endsWith(".")) {
            m_prefix += ".";
        }
    }


    /**
     * Runs the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();
        getAntProperties();

        FileInputStream fis = null;

        try {
            InputSource in = null;

            String text = "";

            // If the XML file to be parsed contains tokens prefixed and
            // suffixed by the characte "@" then substitute them.
            boolean filterSetsAvailable =
                (m_filterSets != null) && !m_filterSets.isEmpty();

            if (filterSetsAvailable) {
                text = FileUtils.loadFile(m_file, m_filterSets);
                StringReader stringReader = new StringReader(text);
                in = new InputSource(stringReader);
            } else {
                fis = new FileInputStream(m_file);
                in = new InputSource(fis);
            }

            // Set up a DOM tree to query.
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // Property "csc.name"
            String xpath = "/config/csc/@name";
            XObject xobj = XPathAPI.eval(doc, xpath);
            m_cscName = xobj.str().trim();
            getProject().setNewProperty(m_prefix + "csc.name", m_cscName);

            // Property "csc.owner"
            xpath = "/config/csc/@owner";
            xobj = XPathAPI.eval(doc, xpath);
            m_cscOwner = xobj.str().trim();
            getProject().setNewProperty(m_prefix + "csc.owner", m_cscOwner);

            // Property "csc.lang"
            xpath = "/config/csc/@lang";
            xobj = XPathAPI.eval(doc, xpath);
            m_cscLang = xobj.str().trim();
            getProject().setNewProperty(m_prefix + "csc.lang", m_cscLang);

            if (m_cscLang.equals("c++")) {
                m_isCPP = true;
            } else if (m_cscLang.equals("java")) {
                m_isJAVA = true;
            } else if (m_cscLang.equals("ada")) {
                m_isADA = true;
            }

            // Property "csc.type"
            xpath = "/config/csc/@type";
            xobj = XPathAPI.eval(doc, xpath);
            String cscType = xobj.str().trim();
            if (!cscType.equals("")) {
                getProject().setNewProperty(m_prefix + "csc.type", cscType);

                // Are we about to compile a CSC ? If so, then
                // the "start.compilation" property must have been set.
                String property = "start.compilation";
                String propertyValue = getProject().getProperty(property);
                if (propertyValue != null) {
                    // List of global dependencies.
                    if (cscType.equals("integration-test") ||
                        cscType.equals("unit-test")) {

                        m_depsConstraints = true;
                        createGlobalDepsList();
                    }
                }
            }

            // Parse the deliveries, runtest and inspection nodes.
            if (m_includeDeliveries) {
                parseDeliveries(doc);
            }

            if (m_includeTests) {
                parseRuntest(doc);
            }

            if (m_includeInspection) {
                parseInspection(doc);
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


    /**
     * Get some ANT properties.
     */
    protected void getAntProperties() {
        String[] props = {
            "basedir",
            "sloppy.check",
            "config.csc.data.dir",
            "config.product.name",
            "config.cpp.shared",
            "config.delivery.dir",
            "config.delivery.include.dir",
            "config.delivery.library.dir",
            "config.delivery.idl.dir",
            "config.delivery.executable.dir",
            "config.delivery.data.dir",
            "config.available.cscis",
            "config.csc.idl.dir",
            "config.csci.idl.dir",
            "config.site.csn",
            "config.csc.properties.file",
            "config.csc.cpp.src.dir",
            "config.csc.java.src.dir",
            "config.csc.ada.src.dir",
            "config.global.dependencies"
        };

        for (int i = 0; i < props.length; i++) {
            String property = props[i];
            String propertyValue = getProject().getProperty(property);

            if (propertyValue == null) {
                throw new BuildException(
                    "property " + property + " not defined.",
                    getLocation());
            }
            m_properties.put(property, propertyValue);
        }

        try {
            m_allDeliveries = StringUtils.split(
                getProject().getProperty("all.deliveries"), ' ');
        } catch (NullPointerException npe) {
            throw new BuildException(npe, getLocation());
        }
    }


    /**
     * Build the list of authorized dependencies.
     */
    protected void createGlobalDepsList() {
        m_globalDeps.clear();

        // Authorize this CSCI as dependencies, that's the least we
        // can do :)
        m_globalDeps.add(m_cscOwner);

        // Then add the CSCI dependencies from the global properties.
        String property = "config.global.dependencies";
        String propertyValue = getProject().getProperty(property);
        if (propertyValue == null) {
            throw new BuildException(
                "property " + property + " not defined.", getLocation());
        }

        StringTokenizer st = new StringTokenizer(propertyValue);
        while (st.hasMoreTokens()) {
            String s = st.nextToken();
            if (!m_globalDeps.contains(s)) {
                m_globalDeps.add(s);
            }
        }

        // And now scan the other non-test CSCs from this CSCI and
        // add their dependencies to the list.

        // Check if we have already scanned this CSCI.
        String csciDeps = getProject().getProperty("deps." + m_cscOwner);
        if (csciDeps != null) {
            // Yes, we did it already.
            st = new StringTokenizer(csciDeps);
            while (st.hasMoreTokens()) {
                String s = st.nextToken();
                if (!m_globalDeps.contains(s)) {
                    m_globalDeps.add(s);
                }
            }
        } else {
            /*
            // Scan the CSCI.

            property = "config." + m_cscOwner + ".location";
            String csciLocation = getProject().getProperty(property);
            if (csciLocation == null) {
                throw new BuildException(
                    "property " + property + " not defined.",
                    getLocation());
            }

            property = "config.csc.properties.file";
            String cscPropsFile = getProject().getProperty(property);
            if (cscPropsFile == null) {
                throw new BuildException(
                    "property " + property + " not defined.",
                    getLocation());
            }

            // Locate all the CSCProperties.xml files.
            FileSet fs = new FileSet();
            fs.setProject(getProject());
            fs.setDir(new File(csciLocation));
            fs.setIncludes(
                "**" + File.separator + m_cscLang + File.separator +
                "**" + File.separator + cscPropsFile);

            try {
                DirectoryScanner ds = fs.getDirectoryScanner(getProject());
                String[] files = ds.getIncludedFiles();

                for (int i = 0; i < files.length; i++) {
                    m_globalDeps.addAll(
                        findDeps(new File(csciLocation, files[i])));
                }
            } catch (BuildException be) {
                // Directory doesn't exist or is not readable.
                throw new BuildException(be, getLocation());
            }
            */

            // Ok, let's save our work.
            property = "csci.global.dependencies";
            String csciGlobalDepsFile = getProject().getProperty(property);
            if (csciGlobalDepsFile == null) {
                throw new BuildException(
                    "property " + property + " not defined.",
                    getLocation());
            }

            st = new StringTokenizer(
                getProject().getProperty(
                    "config." + m_cscOwner + ".globaldeps"));
            while (st.hasMoreTokens()) {
                String s = st.nextToken();
                if (!m_globalDeps.contains(s)) {
                    m_globalDeps.add(s);
                }
            }

            PropertyFile pf =
                (PropertyFile) getProject().createTask("propertyfile");
            pf.setFile(new File(csciGlobalDepsFile));

            StringBuffer sb = new StringBuffer();
            Iterator iter = m_globalDeps.iterator();
            while (iter.hasNext()) {
                sb.append(iter.next()).append(" ");
            }

            PropertyFile.Entry e1 = pf.createEntry();
            e1.setKey(m_cscOwner);
            e1.setValue(sb.toString().trim());

            pf.execute();
        }
    }


    /**
     * Find all the dependencies of a given CSC.
     *
     * @param f The build file.
     *
     * @return A collection of CSCI names.
     */
    /*
    protected Collection findDeps(File f) {
        TreeSet ts = new TreeSet();

        FileInputStream fis = null;

        try {
            fis = new FileInputStream(f);

            // Set up a DOM tree to query.
            InputSource in = new InputSource(fis);
            DocumentBuilderFactory dfactory =
                DocumentBuilderFactory.newInstance();
            dfactory.setNamespaceAware(true);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // Do something only if it's a non-test CSC
            // (e.g it does not have the attribute @type set)
            Node n = XPathAPI.selectSingleNode(doc, "/config/csc/@type");
            if (n == null) {
                // The list of available CSCIs.
                Vector csciList =
                    StringUtils.split(
                        (String) m_properties.get(
                            "config.available.cscis"), ' ');

                // Get all the prerequisites nodes.
                NodeIterator nodes =
                    XPathAPI.selectNodeIterator(doc, "//prerequisites");

                String[] childNodes =
                    { "libraries", "jnilibraries", "executables" };

                while ((n = nodes.nextNode()) != null) {
                    for (int i = 0; i < childNodes.length; i++) {
                        XObject xobj = XPathAPI.eval(n, childNodes[i]);
                        String s = xobj.str().trim();

                        if (!s.equals("")) {
                            StringTokenizer st = new StringTokenizer(s);
                            while (st.hasMoreTokens()) {
                                StringTokenizer st2 =
                                    new StringTokenizer(
                                        st.nextToken(), "/");

                                if (st2.hasMoreTokens()) {
                                    String firstToken = st2.nextToken();
                                    if (csciList.contains(firstToken)) {
                                        ts.add(firstToken);
                                    }
                                }
                            }
                        }
                    }
                }
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

        return ts;
    }
    */


    /**
     * Parse the deliveries node.
     *
     * @param doc The XML document.
     */
    protected void parseDeliveries(Document doc)
        throws javax.xml.transform.TransformerException, IOException {
 
        // Find all the deliveries from CSCProperties.
        NodeIterator nodes =
            XPathAPI.selectNodeIterator(doc, "//delivery/@basename");
        Vector deliveries = new Vector();
        Node node;
        while ((node = nodes.nextNode()) != null) {
            deliveries.add(node.getNodeValue());
        }

        // For each delivery, create the following properties:
        //   - csc.delivery_basename.type
        //   - csc.delivery_basename.visibility
        // 
        // If the delivery is an executable or a library, then create also
        // the following properties (or a subset)):
        //   - csc.delivery_basename.sourcefiles
        //   - csc.delivery_basename.requiredlibs
        //   - csc.delivery_basename.requiredjnilibs
        //   - csc.delivery_basename.requiredlibs.ldflags
        //   - csc.delivery_basename.prereqs.to.build
        //   - csc.delivery_basename.incl.flags
        //   - csc.delivery_basename.idl.server
        //   - csc.delivery_basename.idl.client
        //   - csc.delivery_basename.idl.tie
        //   - csc.delivery_basename.idl.incl.dirs
        //   - csc.delivery_basename.idl.stubskel.incl.dirs
        //   - csc.delivery_basename.addcflags
        //   - csc.delivery_basename.addldflags
        //   - csc.delivery_basename.addidlflags
        //   - csc.delivery_basename.fqnclassname
        //   - csc.delivery_basename.CSCI_IDL_PATH
        //   - csc.delivery_basename.codegen.tmpl.incl.dirs
        //
        // If the delivery is a data file or a set of data files, then
        // create the following properties:
        //    - csc.delivery_basename.sourcefiles

        // Get the CSC basedir:
        // getParentFile() is called twice as CSCProperties.xml is stored 
        // in csc_dir/data/CSCProperties.
        File parentDir = m_file.getParentFile().getParentFile();
        String basedir = "";
        try {
            basedir = parentDir.getCanonicalPath();
        } catch (Exception e) {
            throw new BuildException(e, getLocation());
        }

        Iterator iter = deliveries.iterator();
        while (iter.hasNext()) {
            String basename = (String) iter.next();

            m_deliveriesToBuild = new StringBuffer();

            String xpath = "//delivery[@basename='" + basename + "']";
            Node delivNode = XPathAPI.selectSingleNode(doc, xpath);

            // If this delivery has conditions then run the conditions
            // first and make sure that only one condition at most
            // return true.
            boolean hasConditionNodes =
                XPathAPI.eval(delivNode, "count(condition) > 0").bool();

            if (hasConditionNodes) {
                NodeIterator conditions =
                    XPathAPI.selectNodeIterator(delivNode, "condition");

                Node trueCondition = findTrueCondition(basename, conditions);

                if (trueCondition != null) {
                    parseDelivery(basename, trueCondition, basedir);
                } else {
                    iter.remove();
                }
            } else {
                parseDelivery(basename, delivNode, basedir);
            }
        }

        // Property "csc.all.deliveries"
        if (!deliveries.isEmpty()) {
            StringBuffer sb = new StringBuffer();
            iter = deliveries.iterator();
            while (iter.hasNext()) {
                sb.append((String) iter.next()).append(" ");
            }
            getProject().setNewProperty(
                m_prefix + "csc.all.deliveries", sb.toString().trim());
        }
    }


    /**
     * Find among a list of conditions the one that evaluates to true.
     *
     * @param basename   The delivery base name.
     * @param conditions An iterator on the condition nodes.
     *
     * @return The condition node that evaluates to true.
     */
    protected Node findTrueCondition(String basename, NodeIterator conditions)
        throws javax.xml.transform.TransformerException, IOException {

        String cscDataDir = "";
        try {
            cscDataDir = m_file.getParentFile().getCanonicalPath();
        } catch (Exception e) {
            throw new BuildException(e, getLocation());
        }

        int i = 0;
        int lastTrueCond = -1;
        Node lastTrueCondNode = null;

        String outfilename = "condition.output";

        Node cond;
        while ((cond = conditions.nextNode()) != null) {
            i++;

            // Get the ANT build file name in the CSC data directory.
            XObject xobj = XPathAPI.eval(cond, "@antfile");
            File antfile = new File(cscDataDir, xobj.str().trim());

            if (!antfile.exists()) {
                String msg = "the script file " +
                    antfile.getAbsolutePath() + " for the condition " +
                    String.valueOf(i) + " of the delivery \"" + basename +
                    "\" does not exist.";

                throw new BuildException(msg, getLocation());
            }

            // Create an "ant" task to execute the build file.
            Ant anttask = (Ant) getProject().createTask("ant");
            anttask.setAntfile(antfile.getAbsolutePath());

            xobj = XPathAPI.eval(cond, "@target");
            String anttarget = xobj.str().trim();
            if (!anttarget.equals("")) {
                anttask.setTarget(anttarget);
            }

            anttask.setOutput(outfilename);
            anttask.execute();

            // Read the output file and look for either CONDITION_IS_TRUE
            // or CONDITION_IS_FALSE
            FileReader reader_ = null;
            File outfile = new File((String) m_properties.get("basedir"),
                                    outfilename);
            try {
                reader_ = new FileReader(outfile);
                BufferedReader reader = new BufferedReader(reader_);

                String line = reader.readLine();
                while (line != null) {
                    if (line.indexOf("CONDITION_IS_TRUE") != -1) {
                        // At most one condition can evaluate to true,
                        // raise an exception otherwise.
                        if (lastTrueCond == -1) {
                            lastTrueCond = i;
                            lastTrueCondNode = cond;
                        } else {
                            String msg = "the condition " +
                                String.valueOf(lastTrueCond) + " and the " +
                                "condition " + String.valueOf(i) + " of the " +
                                "delivery \"" + basename + "\" both " +
                                "evaluate to TRUE, only one condition must " +
                                "TRUE at most.";

                            throw new BuildException(msg, getLocation());
                        }
                        break;
                    } else if (line.indexOf("CONDITION_IS_FALSE") != -1) {
                        break;
                    }

                    line = reader.readLine();
                }
            } finally {
                try {
                    reader_.close();
                    outfile.delete();
                } catch (Exception e) {
                    // Ignore.
                }
            }
        }

        return (lastTrueCond != -1) ? lastTrueCondNode : null;
    }


    /**
     * Parse a delivery node.
     *
     * @param basename  The delivery base name.
     * @param delivNode The delivery node.
     * @param basedir   The CSC base directory.
     */
    protected void parseDelivery(String basename,
                                 Node delivNode,
                                 String basedir)
        throws javax.xml.transform.TransformerException, IOException {

        // Property "csc.delivery_basename.visibility"
        XObject xobj = XPathAPI.eval(delivNode, "@visibility");
        String result = xobj.str().trim();
        if (result.equals("")) {
            xobj = XPathAPI.eval(delivNode.getParentNode(), "@visibility");
            result = xobj.str().trim();
        }
        getProject().setNewProperty(
            m_prefix + "csc." + basename + ".visibility", result);

        // Property "csc.delivery_basename.type"
        xobj = XPathAPI.eval(delivNode, "@type");
        String type = xobj.str().trim();
        if (type.equals("")) {
            xobj = XPathAPI.eval(delivNode.getParentNode(), "@type");
            type = xobj.str().trim();
        }
        getProject().setNewProperty(
            m_prefix + "csc." + basename + ".type", type);

        boolean isJNI = type.equals("jnilib");

        // Property "csc.delivery_basename.sourcefiles"
        xobj = XPathAPI.eval(delivNode, "sourcefiles");
        result = xobj.str().trim();
        if (!result.equals("")) {
            parseSourcefiles(basename, type, result, basedir, isJNI);
        }

        // Property "csc.delivery_basename.requiredlibs"
        // Property "csc.delivery_basename.requiredjnilibs"
        // Property "csc.delivery_basename.requiredlibs.ldflags"
        // Property "csc.delivery_basename.incl.flags"
        xobj = XPathAPI.eval(delivNode, "prerequisites/libraries");
        result = xobj.str().trim();
        if (!result.equals("")) {
            parsePrerequisites(basename, "lib", result);
        }

        xobj = XPathAPI.eval(delivNode, "prerequisites/jnilibraries");
        result = xobj.str().trim();
        if (!result.equals("")) {
            parsePrerequisites(basename, "jnilib", result);
        }

        xobj = XPathAPI.eval(delivNode, "prerequisites/executables");
        result = xobj.str().trim();
        if (!result.equals("")) {
            parsePrerequisites(basename, "exe", result);
        }

        // Property "csc.delivery_basename.cgen.cfgfile"
        xobj = XPathAPI.eval(delivNode, "prerequisites/generatedfilesfrom");
        result = xobj.str().trim();
        if (!result.equals("")) {
            String dataDir = (String)
                m_properties.get("config.csc.data.dir");

            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".cgen.cfgfile",
                basedir + File.separator + dataDir +
                File.separator + result);
        }

        // Property "csc.delivery_basename.prereqs.to.build"
        if (m_deliveriesToBuild.length() > 0) {
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".prereqs.to.build",
                m_deliveriesToBuild.toString().trim());
        }

        // Property "csc.delivery_basename.idl.server"
        // Property "csc.delivery_basename.idl.client"
        // Property "csc.delivery_basename.idl.tie"
        // Property "csc.delivery_basename.idl.incl.dirs"
        // Property "csc.delivery_basename.idl.stubskel.incl.dirs"
        Node node = XPathAPI.selectSingleNode(delivNode, "idlfiles");
        if (node != null) {
            parseIdlfiles(node, basename);
        }

        // Property "csc.delivery_basename.addcflags"
        xobj = XPathAPI.eval(delivNode, "addcflags");
        result = xobj.str().trim();
        StringBuffer sb;
        if (!result.equals("")) {
            sb = new StringBuffer();

            StringTokenizer st = new StringTokenizer(result);
            while (st.hasMoreTokens()) {
                sb.append(st.nextToken());
                sb.append(" ");
            }

            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".addcflags",
                sb.toString().trim());
        }

        // Property "csc.delivery_basename.addldflags"
        xobj = XPathAPI.eval(delivNode, "addldflags");
        result = xobj.str().trim();
        if (!result.equals("")) {
            sb = new StringBuffer();

            StringTokenizer st = new StringTokenizer(result);
            while (st.hasMoreTokens()) {
                sb.append(st.nextToken());
                sb.append(" ");
            }

            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".addldflags",
                sb.toString().trim());
        }

        // Property "csc.delivery_basename.addidlflags"
        xobj = XPathAPI.eval(delivNode, "addidlflags");
        result = xobj.str().trim();
        if (!result.equals("")) {
            sb = new StringBuffer();

            StringTokenizer st = new StringTokenizer(result);
            while (st.hasMoreTokens()) {
                sb.append(st.nextToken());
                sb.append(" ");
            }

            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".addidlflags",
                sb.toString().trim());
        }

        // Property "csc.delivery_basename.fqnclassname"
        xobj = XPathAPI.eval(delivNode, "fqnclassname");
        result = xobj.str().trim();
        if (!result.equals("")) {
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".fqnclassname", result);
        }
    }


    /**
     * Parse the runtest node.
     *
     * @param doc The XML document.
     */
    protected void parseRuntest(Document doc)
        throws javax.xml.transform.TransformerException, IOException {

        // Find all the tests.
        NodeIterator testNodes =
            XPathAPI.selectNodeIterator(doc, "//runtest/test");
        Vector tests = new Vector();
        StringBuffer sb = new StringBuffer();
        Node n;
        while ((n = testNodes.nextNode()) != null) {
            XObject xobj = XPathAPI.eval(n, "@name");
            String testname = xobj.str().trim();

            sb.append(testname);
            sb.append(" ");

            tests.add(n);
        }

        // Some tests may have a run condition, so we must evaluate the
        // conditions first.
        NodeIterator testWithConds =
            XPathAPI.selectNodeIterator(doc, "//runtest/condition");
        while ((n = testWithConds.nextNode()) != null) {
            if (evalCondition(n)) {
                XObject xobj = XPathAPI.eval(n, "test/@name");
                String testname = xobj.str().trim();

                if (!testname.equals("")) {
                    sb.append(testname);
                    sb.append(" ");

                    tests.add(XPathAPI.selectSingleNode(n, "test"));
                }
            }
        }

        // Property "csc.all.tests"
        if (!tests.isEmpty()) {
            getProject().setNewProperty(
                m_prefix + "csc.all.tests", sb.toString().trim());
        }

        // For each test, create the following properties:
        //  - csc.testname.nbtestable
        //  - csc.testname.maxwait
        //  - csc.testname.neededfiles
        //  - csc.testname.neededjarfiles
        //  - csc.testname.neededprocesses
        //  - csc.testname.testattributes

        Iterator iter = tests.iterator();
        while (iter.hasNext()) {
            Node testNode = (Node) iter.next();
            XObject xobj = XPathAPI.eval(testNode, "@name");
            String testname = xobj.str().trim();

            // Property "csc.testname.nbtestable"
            xobj = XPathAPI.eval(testNode, "@nbtestable");
            String nbTestable = xobj.str().trim();

            getProject().setNewProperty(
                m_prefix + "csc." + testname + ".nbtestable",
                (nbTestable.equals("") ? "1" : nbTestable));

            // Property "csc.testname.maxwait"
            xobj = XPathAPI.eval(testNode, "@maxwait");
            String maxwait = xobj.str().trim();

            getProject().setNewProperty(
                m_prefix + "csc." + testname + ".maxwait",
                (maxwait.equals("") ? "600" : maxwait));

            // Property "csc.testname.neededfiles"
            xobj = XPathAPI.eval(testNode, "neededfiles");
            String result = xobj.str().trim();
            if (!result.equals("")) {
                sb = new StringBuffer();

                StringTokenizer st = new StringTokenizer(result);
                while (st.hasMoreTokens()) {
                    sb.append(st.nextToken());
                    sb.append(" ");
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + testname + ".neededfiles",
                    sb.toString().trim());
            }

            // Property "csc.testname.neededjarfiles"
            xobj = XPathAPI.eval(testNode, "neededjarfiles");
            result = xobj.str().trim();
            if (!result.equals("")) {
                sb = new StringBuffer();

                StringTokenizer st = new StringTokenizer(result);
                while (st.hasMoreTokens()) {
                    sb.append(st.nextToken());
                    sb.append(" ");
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + testname + ".neededjarfiles",
                    sb.toString().trim());
            }

            // Property "csc.testname.neededprocesses"
            xobj = XPathAPI.eval(testNode, "neededprocesses");
            result = xobj.str().trim();
            if (!result.equals("")) {
                sb = new StringBuffer();

                StringTokenizer st = new StringTokenizer(result);
                while (st.hasMoreTokens()) {
                    sb.append(st.nextToken());
                    sb.append(" ");
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + testname + ".neededprocesses",
                    sb.toString().trim());
            }

            // Property "csc.testname.testattributes"
            xobj = XPathAPI.eval(testNode, "testattributes");
            result = xobj.str().trim();
            if (!result.equals("")) {
                sb = new StringBuffer();

                StringTokenizer st = new StringTokenizer(result);
                while (st.hasMoreTokens()) {
                    sb.append(st.nextToken());
                    sb.append(" ");
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + testname + ".testattributes",
                    sb.toString().trim());
            }
        }
    }


    /**
     * Evaluates a condition.
     *
     * @param condition The condition node.
     *
     * @return <code>true</code> or <code>false</code>.
     */
    protected boolean evalCondition(Node condition)
        throws javax.xml.transform.TransformerException, IOException {

        boolean result = false;

        String cscDataDir = "";
        try {
            cscDataDir = m_file.getParentFile().getCanonicalPath();
        } catch (Exception e) {
            throw new BuildException(e, getLocation());
        }

        String outfilename = "condition.output";

        // Get the ANT build file name in the CSC data directory.
        XObject xobj = XPathAPI.eval(condition, "@antfile");
        File antfile = new File(cscDataDir, xobj.str().trim());

        if (!antfile.exists()) {
            String msg = "the script file " + antfile.getAbsolutePath() + 
                         "does not exist.";

            throw new BuildException(msg, getLocation());
        }

        // Create an "ant" task to execute the build file.
        Ant anttask = (Ant) getProject().createTask("ant");
        anttask.setAntfile(antfile.getAbsolutePath());

        xobj = XPathAPI.eval(condition, "@target");
        String anttarget = xobj.str().trim();
        if (!anttarget.equals("")) {
            anttask.setTarget(anttarget);
        }

        anttask.setOutput(outfilename);
        anttask.execute();

        // Read the output file and look for either CONDITION_IS_TRUE
        // or CONDITION_IS_FALSE
        FileReader reader_ = null;
        File outfile = new File((String) m_properties.get("basedir"),
                                outfilename);
        try {
            reader_ = new FileReader(outfile);
            BufferedReader reader = new BufferedReader(reader_);

            String line = reader.readLine();
            while (line != null) {
                if (line.indexOf("CONDITION_IS_TRUE") != -1) {
                    result = true;
                    break;
                } else if (line.indexOf("CONDITION_IS_FALSE") != -1) {
                    result = false;
                    break;
                }

                line = reader.readLine();
            }
        } finally {
            try {
                reader_.close();
                outfile.delete();
            } catch (Exception e) {
                // Ignore.
            }
        }

        return result;
    }


    /**
     * Parse the delivery sourcefiles.
     *
     * @param basename   The delivery base name.
     * @param type       The type of the delivery.
     * @param srcfiles   The source files pattern.
     * @param cscBasedir The CSC base directory.
     * @param isJNI      Whether this delivery is a JNI library.
     */
    protected void parseSourcefiles(String basename,
                                    String type,
                                    String srcfiles,
                                    String cscBasedir,
                                    boolean isJNI) {

        boolean isData = type.equals("data");
        String srcDir = "";
        String cscPropsFile = "";

        if (isData) {
            // data

            srcDir = (String) m_properties.get("config.csc.data.dir");
            cscPropsFile =
                (String) m_properties.get("config.csc.properties.file");
        } else {
            // exe, lib, jnilib

            srcDir = (String) m_properties.get(
                "config.csc." + (m_isCPP ? "cpp" : m_cscLang) + ".src.dir");
        }

        StringBuffer sb = new StringBuffer();
        StringTokenizer st = new StringTokenizer(srcfiles);
        while (st.hasMoreTokens()) {
            String srcFile = st.nextToken();
            Collection files;

            if (srcFile.equals("*")) {
                // Defaults to "*"
                String filePattern = new String(srcFile);

                if (!isData) {
                    // exe, lib, jnilib

                    if (m_isCPP) {
                        filePattern = "*.c *.cpp";
                    } else if (m_isJAVA) {
                        if (isJNI) {
                            filePattern = "*.c *.cpp";
                        } else {
                            filePattern = "*.java";
                        }
                    } else if (m_isADA) {
                        // TODO
                    }
                }

                files = FindUtils.findFiles(
                            getProject(),
                            filePattern,
                            new File(cscBasedir, srcDir),
                            true);
            } else if (srcFile.indexOf("*") != -1) {
                File d = new File(cscBasedir, srcDir);

                files = FindUtils.findFiles(getProject(), srcFile, d, true);

                if (isData) {
                    // The CSC properties file is only useful for the build.
                    File f = new File(d, cscPropsFile);
                    files.remove(f);
                }
            } else {
                files = new Vector();
                File d = new File(cscBasedir, srcDir);
                files.add(new File(d, srcFile));
            }

            if (files != null) {
                Iterator iter = files.iterator();
                while (iter.hasNext()) {
                    File f = (File) iter.next();
                    String s = "";

                    if (m_isJAVA && !isJNI && !isData) {
                        s = "**" + File.separator + f.getName();
                    } else {
                        try {
                            s = f.getCanonicalPath();
                        } catch (Exception e) {
                            // Ignore.
                        }
                    }

                    if (sb.indexOf(s) == -1) {
                        sb.append(s).append(" ");
                    }
                }
            }
        }

        getProject().setNewProperty(
            m_prefix + "csc." + basename + ".sourcefiles",
            sb.toString().trim());
    }


    /**
     * Parse the prerequisites (libs, exes) of a the specified delivery.
     *
     * @param basename The delivery base name.
     * @param reqType  The type of the prerequisites.
     * @param prereqs  The prerequisites.
     */
    protected void parsePrerequisites(String basename,
                                      String reqType,
                                      String prereqs)
        throws BuildException, IOException {

        boolean isExe = reqType.equals("exe");

        if (reqType.equals("jnilib")) {
            m_isCPP = true;
            m_isJAVA = false;
            m_isJNI = true;
        } else {
            m_isJNI = false;
        }

        // Determine the prefix.
        String prefix = "";
        if (!isExe) {
            prefix = ((String)
                m_properties.get("config.product.name")).toLowerCase();
        }

        // Determine the suffix.
        String suffix = "";
        if (m_isJAVA) {
            suffix = ".jar";
        } else {
            if (!isExe) {
                if (m_isCPP) {
                    suffix = m_properties.get("config.cpp.shared").equals("y")
                                ? ".so" : ".a";
                } else if (m_isADA) {
                    // TODO
                }
            }
        }

        // The source directory.
        String srcDir = (String) m_properties.get(
            "config.csc." + (m_isCPP ? "cpp" : m_cscLang) + ".src.dir");

        String deliveryDir = (String) m_properties.get("config.delivery.dir");

        String deliveryInclDir =
            (String) m_properties.get("config.delivery.include.dir");

        String deliveryLibDir =
            (String) m_properties.get("config.delivery.library.dir");

        // The list of available CSCIs.
        Vector csciList =
            StringUtils.split(
                (String) m_properties.get("config.available.cscis"), ' ');

        // Libs and JNI libs specifics.

        // Holds the result of the parsing.
        StringBuffer libs = new StringBuffer();

        Vector csciDelivIdlDirs = new Vector();
        Vector csciDelivDataDirs = new Vector();

        // C++ specifics.
        StringBuffer cppLdFlags = new StringBuffer();
        Vector cppLibDirs = new Vector();
        Vector cppInclDirs = new Vector();

        // Now parse the string.
        // The pattern is: (csci_name/|(csc_name/)+)delivery_basename

        StringTokenizer st = new StringTokenizer(prereqs);
        while (st.hasMoreTokens()) {
            Vector tokens = StringUtils.split(st.nextToken(), '/');

            // The first token is either the name of a CSCI or a CSC.
            String firstToken = (String) tokens.firstElement();

            // So we must check it.
            // Note: A non-existing CSCI will also set isLocal to true.
            // This is why we need further checking later.
            boolean isLocal = !csciList.contains(firstToken);

            String lastToken = (String) tokens.lastElement();

            // Get the location of the CSCI the lib or exe originates from.
            String property =
                "config." + (isLocal ? m_cscOwner : firstToken) + ".location";
            String csciLocation = getProject().getProperty(property);
            if (csciLocation == null) {
                throw new BuildException(
                    "property " + property + " not defined.",
                    getLocation());
            }

            if (isLocal) {
                // Determine whether firstToken refers to a CSC or to an non
                // existing CSCI.
                File d = new File(csciLocation, firstToken);

                if (!(d.exists() && d.isDirectory())) {
                    // This is definitely not a CSC.
                    throw new BuildException(
                        firstToken + " is neither a CSCI nor a CSC.",
                        getLocation());
                }
            } else {
                // Restrict the dependencies.
                if (m_depsConstraints && !m_globalDeps.contains(firstToken)) {
                    String msg =
                        "The " + m_cscName + " CSC has requested " +
                        "a forbidden dependency to the " + firstToken +
                        " CSCI.";
                    throw new BuildException(msg, getLocation());
                }
            }

            // Construct the path to the IDL directory.
            String csciDelivIdlDir =
                csciLocation + File.separator +
                (String) m_properties.get("config.delivery.dir") +
                File.separator +
                (String) m_properties.get("config.delivery.idl.dir");

            if (!csciDelivIdlDirs.contains(csciDelivIdlDir)) {
                csciDelivIdlDirs.add(csciDelivIdlDir);
            }

            // Construct the path to the IDL directory.
            String csciDelivDataDir =
                csciLocation + File.separator +
                (String) m_properties.get("config.delivery.dir") +
                File.separator +
                (String) m_properties.get("config.delivery.data.dir");

            if (!csciDelivDataDirs.contains(csciDelivDataDir)) {
                csciDelivDataDirs.add(csciDelivDataDir);
            }

            // Construct the path to the lib or exe.

            String csciKeyString =
                buildKeyString(isLocal ? m_cscOwner : firstToken);

            String libOrExeLocation = new String(csciLocation);
            String cscLocation = "";

            // C++ lib specifics.
            String inclLocation = "";
            if (!isExe && m_isCPP) {
                inclLocation = new String(csciLocation);
            }

            // Holds the next to last token (only useful for local lib).
            String nextToLastToken = "";

            if (isLocal) {
                if (tokens.size() > 1) {
                    nextToLastToken = (String) tokens.get(tokens.size() - 2);
                }

                // Append the CSCs into the path.
                for (int i = 0; i < tokens.size() - 1; i++) {
                    String csc = (String) tokens.get(i);

                    libOrExeLocation += File.separator + csc;

                    if (!isExe && m_isCPP) {
                        inclLocation += File.separator + csc;
                    }
                }

                // Check if we must insert the programming language in the path.
                File f = new File(libOrExeLocation, srcDir);
                if (!f.exists()) {
                    libOrExeLocation += File.separator + m_cscLang;

                    if (!isExe && m_isCPP) {
                        inclLocation += File.separator + m_cscLang;
                    }
                }

                cscLocation = new String(libOrExeLocation);

                if (!isExe && m_isCPP) {
                    inclLocation +=
                        File.separator + deliveryDir +
                        File.separator + deliveryInclDir;
                }
            } else {
                if (!isExe && m_isCPP) {
                    inclLocation +=
                        File.separator + deliveryDir +
                        File.separator + deliveryInclDir +
                        File.separator + m_cscLang;
                }
            }

            libOrExeLocation +=
                File.separator + deliveryDir +
                File.separator + deliveryLibDir +
                (csciKeyString.equals("") ? csciKeyString
                                          : File.separator + csciKeyString);

            if (reqType.equals("lib") && m_isCPP) {
                cppLdFlags.append("-l");
                cppLdFlags.append(prefix);
                cppLdFlags.append(lastToken);
                cppLdFlags.append(" ");

                if (!cppLibDirs.contains(libOrExeLocation)) {
                    cppLibDirs.add(libOrExeLocation);
                }

                // Local header files.
                if (!cppInclDirs.contains(inclLocation)) {
                    cppInclDirs.add(inclLocation);

                    String s1 = "";
                    if (!nextToLastToken.equals("")) {
                        s1 = inclLocation + File.separator + nextToLastToken;

                        if (!cppInclDirs.contains(s1)) {
                            cppInclDirs.add(s1);
                        }
                    }

                    String orbKey = buildOrbKey(m_cscOwner);
                    if (!orbKey.equals("")) {
                        String s2 = inclLocation +
                                   File.separator + "idllib" +
                                   File.separator + orbKey;

                        if (!cppInclDirs.contains(s2)) {
                            cppInclDirs.add(s2);
                        }

                        String s3 = "";

                        if (!s1.equals("")) {
                            s3 = s1 + File.separator + orbKey;

                            if (!cppInclDirs.contains(s3)) {
                                cppInclDirs.add(s3);
                            }
                        }
                    }
                }

                // Public header files.
                if (!isLocal) {
                    String s = inclLocation + File.separator + firstToken;
                    if (!cppInclDirs.contains(s)) {
                        cppInclDirs.add(s);
                    }

                    String orbKey = buildOrbKey(firstToken);
                    if (!orbKey.equals("")) {
                        s += File.separator + "idllib" +
                             File.separator + orbKey;

                        if (!cppInclDirs.contains(s)) {
                            cppInclDirs.add(s);
                        }
                    }
                }
            }

            // Append the lib or exe file name.
            libOrExeLocation +=
                File.separator + (!isExe ? "lib" : "") +
                prefix + lastToken + suffix;

            // We are done constructing the path to the lib or exe.

            // Save the path.
            File libOrExeFile = new File(libOrExeLocation);
            if (!isExe) {
                libs.append(libOrExeFile.getCanonicalPath());
                if (m_isJAVA) {
                    libs.append(File.pathSeparator);
                } else {
                    libs.append(" ");
                }
            }

            boolean sloppyCheck =
                Boolean.valueOf(
                    (String) m_properties.get("sloppy.check")).booleanValue();

            if (!sloppyCheck || (sloppyCheck && !libOrExeFile.exists())) {
                // Assume that the lib or the exe is missing so that it will
                // be built or rebuilt.

                if (isLocal) {
                    /*
                    m_deliveriesToBuild.append(
                        cscLocation + File.separator + lastToken);
                    */
                    m_deliveriesToBuild.append(cscLocation);
                } else {
                    // We need to find out where the missing lib or exe
                    // originate from.
                    File csc_ = findMissingPrerequisite(csciLocation,
                                                        lastToken,
                                                        reqType);

                    if (csc_ != null) {
                        /*
                        m_deliveriesToBuild.append(
                            csc_.getCanonicalPath() +
                            File.separator + lastToken);
                        */
                        m_deliveriesToBuild.append(csc_.getCanonicalPath());
                    } else {
                        throw new BuildException(
                            "could not determine which CSC from " +
                            firstToken + " CSCI provides the " +
                            (isExe ? "executable " : "library " +
                            libOrExeFile.getName() + "."),
                            getLocation());
                    }
                }
            }

            m_deliveriesToBuild.append(" ");
        }

        if (!isExe) {
            // Property "csc.delivery_basename.requiredlibs"
            String libs_ = libs.toString().trim();
            if (m_isJAVA) {
                if (libs_.endsWith(File.pathSeparator)) {
                    libs_ = libs_.substring(0, libs_.length() - 1);
                }
            }

            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".required" + reqType + "s",
                libs_);

            if (reqType.equals("lib") && m_isCPP) {
                // Property "csc.delivery_basename.incl.flags"
                StringBuffer sb = new StringBuffer();

                Iterator it = cppInclDirs.iterator();
                while (it.hasNext()) {
                    File d = new File ((String) it.next());
                    sb.append(" -I").append(d.getAbsolutePath());
                }

                if (sb.length() > 0) {
                    getProject().setNewProperty(
                        m_prefix + "csc." + basename + ".incl.flags",
                        sb.toString().trim());
                }

                // Property "csc.delivery_basename.requiredlibs.ldflags"
                sb = new StringBuffer();

                it = cppLibDirs.iterator();
                while (it.hasNext()) {
                    File d = new File ((String) it.next());
                    sb.append(" -L").append(d.getAbsolutePath());
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + basename + ".requiredlibs.ldflags",
                    cppLdFlags.toString().trim() +
                    ((sb.length() > 0) ? " " + sb.toString().trim() : ""));
            }
        }

        // Property "csc.delivery_basename.CSCI_IDL_PATH"
        StringBuffer sb = new StringBuffer();
        Iterator it = csciDelivIdlDirs.iterator();
        while (it.hasNext()) {
            sb.append(" -I");
            sb.append((String) it.next());
        }

        if (sb.length() > 0) {
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".CSCI_IDL_PATH",
                sb.toString().trim());
        }

        // Property "csc.delivery_basename.codegen.tmpl.incl.dirs"
        sb = new StringBuffer();
        it = csciDelivDataDirs.iterator();
        while (it.hasNext()) {
            sb.append(" -I ");
            sb.append((String) it.next());
        }

        if (sb.length() > 0) {
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".codegen.tmpl.incl.dirs",
                sb.toString().trim());
        }
    }


    /**
     * Parse the idlfiles node.
     *
     * @param idlfiles The idlfiles node.
     * @param basename The delivery base name.
     */
    protected void parseIdlfiles(Node idlfiles, String basename)
        throws javax.xml.transform.TransformerException {

        // Find the CSC idl directory.
        File cscDir = new File((String) m_properties.get("basedir"));
        if (cscDir.getName().equals(m_cscLang)) {
            cscDir = cscDir.getParentFile();
        }

        String cscIdlDirname = (String) m_properties.get("config.csc.idl.dir");
        File cscIdlDir = new File(cscDir, cscIdlDirname);

        // Find the CSCI idl directory.
        String property = "config." + m_cscOwner + ".location";
        String csciLocation = getProject().getProperty(property);
        if (csciLocation == null) {
            throw new BuildException(
                "property " + property + " not found.",
                getLocation());
        }

        String csciIdlDirname =
            (String) m_properties.get("config.csci.idl.dir");
        File csciIdlDir = new File(csciLocation, csciIdlDirname);

        XObject xobj = XPathAPI.eval(idlfiles.getParentNode(),
                           "boolean(prerequisites/generatedfilesfrom)");
        boolean requireCodegen = xobj.bool();

        // Property "csc.delivery_basename.idl.server"
        xobj = XPathAPI.eval(idlfiles, "server");

        expandIdlFilenames(
            m_prefix + "csc." + basename + ".idl.server",
            xobj.str().trim(), cscIdlDir, csciIdlDir, requireCodegen);

        // Property "csc.delivery_basename.idl.client"
        xobj = XPathAPI.eval(idlfiles, "client");
        expandIdlFilenames(
            m_prefix + "csc." + basename + ".idl.client",
            xobj.str().trim(), cscIdlDir, csciIdlDir, requireCodegen);

        // Property "csc.delivery_basename.idl.tie"
        xobj = XPathAPI.eval(idlfiles, "tie");
        String result = xobj.str().trim();
        if (!result.equals("")) {
            expandIdlFilenames(
                m_prefix + "csc." + basename + ".idl.tie",
                result, cscIdlDir, csciIdlDir, requireCodegen);
        }

        String deliveryDir = (String) m_properties.get("config.delivery.dir");

        String deliveryInclDir =
            (String) m_properties.get("config.delivery.include.dir");

        String deliveryIdlDir =
            (String) m_properties.get("config.delivery.idl.dir");

        // Property "csc.delivery_basename.idl.incl.dirs"
        // Property "csc.delivery_basename.idl.stubskel.incl.dirs"
        xobj = XPathAPI.eval(idlfiles, "requireidlsfrom");
        result = xobj.str().trim();
        if (!result.equals("")) {
            StringBuffer inclDirs = new StringBuffer();
            StringBuffer stubskelInclDirs = new StringBuffer();

            StringTokenizer st = new StringTokenizer(result);
            while (st.hasMoreTokens()) {
                String nextToken = st.nextToken();
                if (nextToken.startsWith("/")) {
                    nextToken = nextToken.substring(1);
                }
                String[] tokens = nextToken.split("/");

                csciLocation = getProject().getProperty(
                    "config." + tokens[0] + ".location");

                String orbKey = buildOrbKey(tokens[0]);

                // Path to the directory which contains the IDL files
                // in the CSCI delivery tree.
                inclDirs.append("-I")
                        .append(csciLocation).append(File.separator)
                        .append(deliveryDir).append(File.separator)
                        .append(deliveryIdlDir).append(" ");

                // Path to the directory which contains the stubs and skels
                // header files.
                if (tokens.length == 1) {
                    stubskelInclDirs.append("-I")
                                    .append(csciLocation).append(File.separator)
                                    .append(deliveryDir).append(File.separator)
                                    .append(deliveryInclDir).append(File.separator)
                                    .append(m_cscLang).append(File.separator)
                                    .append(tokens[0]).append(File.separator)
                                    .append("idllib").append(File.separator)
                                    .append(orbKey).append(" ");
                } else {
                    int m = 1;
                    int n = tokens.length - 1;

                    do {
                        stubskelInclDirs.append("-I")
                                    .append(csciLocation).append(File.separator)
                                    .append(deliveryDir).append(File.separator)
                                    .append(deliveryInclDir).append(File.separator)
                                    .append(m_cscLang).append(File.separator)
                                    .append(tokens[0]).append(File.separator);
                        for (int i = 1; i <= m; i++) {
                            stubskelInclDirs.append(tokens[i]);
                            if (i != m) {
                                stubskelInclDirs.append(File.separator);
                            }
                        }
                        stubskelInclDirs.append(" ");
                        m++;
                    } while (m <= n);

                    stubskelInclDirs.append("-I")
                                    .append(csciLocation).append(File.separator)
                                    .append(deliveryDir).append(File.separator)
                                    .append(deliveryInclDir).append(File.separator)
                                    .append(m_cscLang).append(File.separator)
                                    .append(nextToken).append(File.separator)
                                    .append(orbKey).append(" ");
                }
            }


            // Property "csc.delivery_basename.idl.incl.dirs"
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".idl.incl.dirs",
                inclDirs.toString().trim());

            // Property "csc.delivery_basename.idl.stubskel.incl.dirs"
            getProject().setNewProperty(
                m_prefix + "csc." + basename + ".idl.stubskel.incl.dirs",
                stubskelInclDirs.toString().trim());
        }
    }


    /**
     * Construct a string which identifies a given configuration (OS, tools,
     * etc.).
     *
     * @param csci The CSCI name.
     */
    protected String buildKeyString(String csci) throws BuildException {
        String keyString = "";

        if (m_isCPP || m_isADA) {
            String part1 = buildCompilerKey(csci);
            String part2 = (m_isJNI ? "" : buildOrbKey(csci));

            // Get the canonical system name.
            String siteCSN = (String) m_properties.get("config.site.csn");

            if (!part1.equals("") || !part2.equals("")) {
                keyString = siteCSN +
                    (part1.equals("") ? part1 : "-" + part1) +
                    (part2.equals("") ? part2 : "-" + part2);
            }
        } else if (m_isJAVA) {
            String orbKey = buildOrbKey(csci);

            if (!orbKey.equals("")) {
                keyString = orbKey;
            }
        }

        return keyString;
    }


    /**
     * Return the compound string "compilerName-compilerVersion".
     *
     * @param csci The CSCI name.
     */
    protected String buildCompilerKey(String csci)
        throws BuildException {

        String root =
            "config." + csci + "." + (m_isCPP ? "cpp"
                                              : m_cscLang) + ".";

        String keyString = "";

        // Get the CSCI compiler.
        String csciCompiler = getProject().getProperty(root + "compiler");
        if (csciCompiler != null) {
            // Get the CSCI compiler version.
            String csciCompilerVersion =
                getProject().getProperty(root + "compiler.ver");
            if (csciCompilerVersion == null) {
                throw new BuildException(
                    "property " + root + "compiler.ver not defined.",
                    getLocation());
            }

            keyString = csciCompiler + "-" + csciCompilerVersion;
        }

        return keyString;
    }


    /**
     * Return the compound string "orbName-orbVersion".
     *
     * @param csci The CSCI name.
     */
    protected String buildOrbKey(String csci)
        throws BuildException {

        String root =
            "config." + csci + "." + (m_isCPP ? "cpp"
                                              : m_cscLang) + ".";
        String keyString = "";

        // Get the CSCI orb.
        String csciOrb = getProject().getProperty(root + "orb");
        if (csciOrb != null) {
            // Get the CSCI orb version.
            String csciOrbVersion = getProject().getProperty(root + "orb.ver");
            if (csciOrbVersion == null) {
                throw new BuildException(
                    "property " + root + "orb.ver not defined.",
                    getLocation());
            }

            keyString = csciOrb + "-" + csciOrbVersion;
        }

        return keyString;
    }


    /**
     * Find out which CSC of the specified CSCI the missing library or
     * executable comes from.
     *
     * @param csciLocation The CSCI location.
     * @param basename     The library or executable basename (prefix and suffix
     *                     are stripped off).
     * @param type         The type of prerequisite to look for. 
     */
    protected File findMissingPrerequisite(String csciLocation,
                                           String basename,
                                           String type) {

        Iterator it = m_allDeliveries.iterator();
        while (it.hasNext()) {
            String deliv = (String) it.next();

            if (deliv.startsWith(csciLocation) &&
                deliv.endsWith(basename + "#" + type)) {

                File f = new File(
                    deliv.substring(0, deliv.lastIndexOf("/"))  +
                    File.separator + m_cscLang);

                return f;
            }
        }

        return null;
        /*
        String cscPropsFile =
            (String) m_properties.get("config.csc.properties.file");

        // Get all the build files.
        Collection buildFiles = FindUtils.findFiles(
            getProject(), cscPropsFile, new File(csciLocation), true);

        Iterator iter = buildFiles.iterator();
        while (iter.hasNext()) {
            File buildFile = (File) iter.next();
            FileInputStream fis = null;

            try {
                fis = new FileInputStream(buildFile);

                // Set up a DOM tree to query.
                InputSource in = new InputSource(fis);
                DocumentBuilderFactory dfactory =
                    DocumentBuilderFactory.newInstance();
                dfactory.setNamespaceAware(true);
                Document doc = dfactory.newDocumentBuilder().parse(in);

                // Check if this build file describes how to build the
                // missing library or executable.
                String xpath = "/config/csc[@lang='" + m_cscLang +
                               "']/deliveries/delivery[@basename='" +
                               basename + "' and @type='" + type +
                               "' and @visibility='public']";

                Node node = XPathAPI.selectSingleNode(doc, xpath);
                if (node != null) {
                    // The CSC properties file is located in the data directory.
                    File cscDir = buildFile.getParentFile().getParentFile();

                    return cscDir;
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

        return null;
        */
    }


    /**
     * Expand the IDL file names and save them into the specified property.
     *
     * @param property   The property to be created.
     * @param filelist   The list of IDL files.
     * @param cscIdlDir  The CSC idl directory.
     * @param csciIdlDir The CSCI idl directory.
     * @param reqCodegen If <code>true</code> then IDL files that cannot be
     *                   located are assumed to be generated.
     */
    protected void expandIdlFilenames(String property,
                                      String filelist,
                                      File cscIdlDir,
                                      File csciIdlDir,
                                      boolean reqCodegen) {

        if (cscIdlDir.exists() || csciIdlDir.exists()) {
            StringBuffer sb = new StringBuffer();

            StringTokenizer st = new StringTokenizer(filelist);
            while (st.hasMoreTokens()) {
                String idlfile = st.nextToken();

                File f = null;

                // Try with the CSCI idl directory first.
                if (csciIdlDir.exists()) {
                    f = FindUtils.findFile(
                        getProject(), idlfile, csciIdlDir, false);
                }

                if (f == null) {
                    // The CSCI idl directory does not exist or the IDL file
                    // was not found in that directory, try the CSC idl
                    // directory.
                    f = FindUtils.findFile(
                        getProject(), idlfile, cscIdlDir, false);
                }

                if (f != null) {
                    try {
                        sb.append(f.getCanonicalPath());
                        sb.append(" ");
                    } catch (IOException ioe) {
                        throw new BuildException(ioe, getLocation());
                    }
                } else {
                    if (reqCodegen) {
                        f = new File("@GENERATED@" + File.separator + idlfile);
                        sb.append(f.getPath());
                        sb.append(" ");
                    }
                }
            }

            if (sb.length() > 0) {
                getProject().setNewProperty(property, sb.toString().trim());
            }
        }
    }


    /**
     * Parse the inspection node.
     *
     * @param doc The XML document.
     */
    protected void parseInspection(Document doc)
        throws javax.xml.transform.TransformerException, IOException {

        String[] elems = { "validation", "qualification" };
        File cscDir = m_file.getParentFile().getParentFile().getParentFile();

        for (int i = 0; i < elems.length; i++) {
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc,
                                    "//inspection/" + elems[i]);

            StringBuffer allIds = new StringBuffer();

            Node n;
            while ((n = nodes.nextNode()) != null) {
                XObject xobj = XPathAPI.eval(n, "@id");
                String id = xobj.str().trim();
                allIds.append(id).append(" ");

                xobj = XPathAPI.eval(n, "files");
                String files = xobj.str();

                StringBuffer sb = new StringBuffer();
                StringTokenizer st = new StringTokenizer(files);
                while (st.hasMoreTokens()) {
                    String token = st.nextToken();
                    File f = FindUtils.findFile(
                        getProject(), token, cscDir, true);

                    if (f != null) {
                        sb.append(f.getCanonicalPath()).append(" ");
                    }
                }

                getProject().setNewProperty(
                    m_prefix + "csc." + id + ".files",
                    sb.toString().trim());
            }

            getProject().setNewProperty(
                m_prefix + "csc.inspect." + elems[i] + ".ids",
                allIds.toString().trim());
        }
    }


}
