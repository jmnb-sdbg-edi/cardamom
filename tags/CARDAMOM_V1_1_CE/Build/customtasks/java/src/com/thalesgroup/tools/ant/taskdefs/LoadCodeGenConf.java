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


import com.thalesgroup.tools.ant.util.FileUtils;
import com.thalesgroup.tools.ant.util.FindUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;

import java.util.Collection;
import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.FilterSet;
import org.apache.tools.ant.util.StringUtils;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


/**
 * Load the code generator configuration file and create ANT properties.
 */
public class LoadCodeGenConf extends Task {


    // Source file.
    private File m_file = null;

    // Holds FilterSets.
    private Vector m_filterSets = new Vector();

    // The location of the CSC.
    private File m_cscLocation = null;

    // The location of the CSCI.
    private File m_csciLocation = null;

    // The module to export.
    private String m_module_to_export = "Cdmw_dummy";


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
    public Collection getFilterSets() {
        return m_filterSets;
    }


    /**
     * Set the location of the CSC.
     *
     * @param cscLocation The location of the CSC.
     */
    public void setCscLocation(File cscLocation) {
        m_cscLocation = cscLocation;
    }


    /**
     * Get the location of the CSC.
     *
     * @return The location of the CSC.
     */
    public File getCscLocation() {
        return m_cscLocation;
    }


    /**
     * Set the location of the CSCI.
     *
     * @param csciLocation The location of the CSCI.
     */
    public void setCsciLocation(File csciLocation) {
        m_csciLocation = csciLocation;
    }


    /**
     * Get the location of the CSCI.
     *
     * @return The location of the CSCI.
     */
    public File getCsciLocation() {
        return m_csciLocation;
    }


    /**
     * Default constructor.
     */
    public LoadCodeGenConf() {
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

            String xpath = "";
            XObject xobj = null;

            // Property "codegen.module.to.export"
            xpath = "//factories/factory/@idl-interface";
            xobj = XPathAPI.eval(doc, xpath);
            Vector v = null;
            v = StringUtils.split(xobj.str().trim(), '.');

            getProject().setNewProperty("codegen.module.idl2.to.export",
                                   (String) v.firstElement());

            getProject().setNewProperty("codegen.module.to.export",
                                        m_module_to_export);

            // Property "codegen.generate.xmi"
            String s = "//lifecycle-frameworks/lifecycle-framework";
            xpath = "boolean(" + s + "/factory-impl) or " +
                    "boolean(" + s + "/home-impl) or " +
                    "boolean(//datastore-types/datastore-type/@typename)";
            xobj = XPathAPI.eval(doc, xpath);

            if (xobj.bool()) {
                getProject().setNewProperty("codegen.generate.xmi", "true");
            }

            // Property "codegen.generate.idl2.only"
            boolean withLifecycle = true;
            xpath = "count(//factories/factory) > 0 and " +
                    "count(//lifecycle-frameworks/" +
                    "lifecycle-framework) = 0 and " +
                    "count(//programs/program) = 0";
            xobj = XPathAPI.eval(doc, xpath);

            if (xobj.bool()) {
                getProject().setNewProperty(
                    "codegen.generate.idl2.only", "true");
                withLifecycle = false;
            }

            // Property "codegen.idl2.filename"
            boolean withComponents = false;
            xpath = "//factories/factory[@kind = 'HOME']";
            Node node = XPathAPI.selectSingleNode(doc, xpath);
            if (node != null) {
                // Look for a possible file name.
                xobj = XPathAPI.eval(node, "@idl-filename");
                String result = xobj.str().trim();
                if (result.equals("")) {
                    // Couldn't find it so we'll construct one from the IDL
                    // interface name.
                    xobj = XPathAPI.eval(node, "@idl-interface");
                    result = xobj.str().trim();
                    v = StringUtils.split(result, '.');
                    result = (String) v.lastElement();
                } else {
                    // Remove the file extension.
                    v = StringUtils.split(result, '.');
                    result = (String) v.firstElement();
                }

                // Construct the new file name.
                result += "_cif.idl";

                getProject().setNewProperty("codegen.idl2.filename", result);
                withComponents = true;
            }

            // Generate a dummy IDL.
            Vector idlfiles = new Vector();
            Vector idlinterfaces = new Vector();
            xpath = "//factories/factory";
            NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
            while ((node = nodes.nextNode()) != null) {
                // Look an eventual file name.
                xobj = XPathAPI.eval(node, "@idl-filename");
                String result = xobj.str().trim();
                if (result.equals("")) {
                    // Couldn't find it so we'll construct one from the IDL
                    // interface name.
                    xobj = XPathAPI.eval(node, "@idl-interface");
                    result = xobj.str().trim();
                    v = StringUtils.split(result, '.');
                    result = (String) v.lastElement() + ".idl";
                }

                if (!idlfiles.contains(result)) {
                    idlfiles.add(result);
                }


                xobj = XPathAPI.eval(node, "@idl-interface");
                String interfaceResult = xobj.str().trim();
                interfaceResult = interfaceResult.replace('.', ':');
                if (!idlinterfaces.contains(interfaceResult.replaceAll(":", "::" ))) {
                    idlinterfaces.add(interfaceResult.replaceAll(":", "::" ));
                }
            }

            boolean datastoreisused = false;

            xpath = "//datastore-types/datastore-type";
            NodeIterator datastorenodes = XPathAPI.selectNodeIterator(doc, xpath);
            while ((node = datastorenodes.nextNode()) != null) {
                datastoreisused = true;
                // Look an eventual file name.
                XObject dsxobj = XPathAPI.eval(node, "@idl-filename");
                String result = dsxobj.str().trim();
                if (!result.equals("")) {
                    if (!idlfiles.contains(result)) {
                        idlfiles.add(result);
                    }
                }

                dsxobj = XPathAPI.eval(node, "@oid-type");
                result = dsxobj.str().trim();
//                 // if the interface is a CORBA type, remove the "CORBA."
//                 if (result.startsWith("CORBA."))
//                     result = result.replaceAll("CORBA.", "" );
                result = result.replace('.', ':');
                if (!idlinterfaces.contains(result.replaceAll(":", "::" ))) {
                    idlinterfaces.add(result.replaceAll(":", "::" ));
                }

                dsxobj = XPathAPI.eval(node, "@data-type");
                result = dsxobj.str().trim();
//                 // if the interface is a CORBA type, remove the "CORBA."
//                 if (result.startsWith("CORBA."))
//                     result = result.replaceAll("CORBA.", "" );

                result = result.replace('.', ':');
                if (!idlinterfaces.contains(result.replaceAll(":", "::"))) {
                    idlinterfaces.add(result.replaceAll(":", "::"));
                }
            }

            generateDummyIdl(withComponents, withLifecycle, idlfiles, idlinterfaces);
 

            // Property "codegen.idlflags.incl.dirs"
            xpath = "//idl-search-dir";
            nodes = XPathAPI.selectNodeIterator(doc, xpath);
            Vector includes = new Vector();
            while ((node = nodes.nextNode()) != null) {
                String dir = node.getFirstChild().getNodeValue().trim();

                if (!dir.equals("")) {
                    StringTokenizer st = new StringTokenizer(dir);
                    while (st.hasMoreTokens()) {
                        String d = st.nextToken();

                        if (!d.startsWith("-I")) {
                            d = "-I" + d;
                        }

                        if (!includes.contains(d)) {
                            includes.add(d);
                        }
                    }
                }
            }

            StringBuffer sb = new StringBuffer();
            Iterator iter = includes.iterator();
            while (iter.hasNext()) {
                sb.append(" ").append((String) iter.next());
            }

            Collection idlDirs = findIdlDirs(idlfiles);
            iter = idlDirs.iterator();
            while (iter.hasNext()) {
                sb.append(" -I").append((String) iter.next());
            }

            if (withComponents) {
                String property = "config.CCMContainer.location";
                String ccmContainerLoc = getProject().getProperty(property);
                if (ccmContainerLoc == null) {
                    throw new BuildException(
                        "property " + property + " is not defined.");
                }

                File f = FindUtils.findFile(getProject(),
                                            "Components.idl",
                                            new File(ccmContainerLoc),
                                            true);

                if (f != null) {
                    try {
                        sb.append(" -I");
                        sb.append(f.getParentFile().getCanonicalPath());
                    } catch (IOException ioe) {
                        throw new BuildException(ioe);
                    }
                }
            }
            if (sb.length() > 0) {
                getProject().setNewProperty("codegen.idlflags.incl.dirs",
                                            sb.toString().trim());
            }


            if (datastoreisused)
                getProject().setNewProperty("codegen.datastore.is.used", "true");


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
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if (m_file == null) {
            throw new BuildException("file must not be null.");
        }

        if (!m_file.exists()) {
            throw new BuildException(m_file + " does not exist.");
        }

        if (!m_file.isFile()) {
            throw new BuildException("source file is not a file.");
        }

        if (m_cscLocation == null) {
            throw new BuildException("cscLocation must not be null.");
        }

        if (!m_cscLocation.exists()) {
            throw new BuildException(m_cscLocation + " does not exist.");
        }

        if (!m_cscLocation.isDirectory()) {
            throw new BuildException(
                m_cscLocation + " is not a directory.");
        }

        if (m_csciLocation == null) {
            throw new BuildException("csciLocation must not be null.");
        }

        if (!m_csciLocation.exists()) {
            throw new BuildException(m_csciLocation + " does not exist.");
        }

        if (!m_csciLocation.isDirectory()) {
            throw new BuildException(
                m_csciLocation + " is not a directory.");
        }
    }


    /**
     * Create a dummy IDL file. This hack prevents duplicate definitions
     * from different IDL files.
     *
     * @param withComponents If <code>true</code> then include Components.idl.
     * @param withLifecycle  If <code>true</code> then include LifeCycle.idl.
     * @param idlfiles       The list of IDL files to be included. 
     */
    protected void generateDummyIdl(boolean withComponents,
                                    boolean withLifecycle,
                                    Collection idlfiles,
                                    Collection idlinterfaces) {

        String tmpfile = System.getProperty("user.name") + "_codegen_" +
            System.currentTimeMillis() + ".idl";

        File f = new File(System.getProperty("java.io.tmpdir"), tmpfile);

        try {
            getProject().setNewProperty(
                "codegen.dummy.idl", f.getCanonicalPath());

            FileWriter writer = new FileWriter(f);
            
            if (withComponents) {
                writer.write("#include \"Components.idl\"\n");
            }

            if (withLifecycle) {
                writer.write("#include \"CdmwLifeCycle.idl\"\n");
            }

            Iterator iter = idlfiles.iterator();
            while (iter.hasNext()) {
                writer.write("#include \"" + (String) iter.next() + "\"\n");
            }
            writer.write("module " + m_module_to_export +" {\n\n");
            writer.write("interface dummy {\n\n");
            Iterator interfaceiter = idlinterfaces.iterator();
            long inc = 0;
            while (interfaceiter.hasNext()) {
                writer.write("\t" + (String) interfaceiter.next() + " cdmw_dummy_interface" + inc +"();\n\n");
                inc++;
            }
            writer.write("}; //interface dummy\n");
            writer.write("}; //module Cdmw_dummy\n");

            // Flush and close.
            writer.close();
        } catch (IOException ioe) {
            throw new BuildException(ioe);
        }
    }


    /**
     * Find the directories into which the IDL files are contained.
     *
     * @return The list of directories.
     */
    protected Collection findIdlDirs(Collection idlfiles) {
        Vector dirs = new Vector();

        Iterator iter = idlfiles.iterator();
        while (iter.hasNext()) {
            String filename = (String) iter.next();
            File f = FindUtils.findFile(getProject(),
                                        filename,
                                        m_cscLocation.getParentFile(),
                                        true);
            if (f == null) {
                f = FindUtils.findFile(getProject(),
                                       filename,
                                       m_csciLocation,
                                       true);
            }

            if (f != null) {
                try {
                    String path = f.getParentFile().getCanonicalPath();

                    if (!dirs.contains(path)) {
                        dirs.add(path);
                    }
                } catch (IOException ioe) {
                    throw new BuildException(ioe);
                }
            }
        }

        return dirs;
    }


}

