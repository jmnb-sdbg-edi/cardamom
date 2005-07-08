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


package cdmw.codegenerator;


import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.StringReader;

import java.util.Collection;
import java.util.Hashtable;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.apache.xpath.XPathAPI;
import org.apache.xpath.objects.XObject;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.traversal.NodeIterator;

import org.xml.sax.InputSource;


public class TemplateManager {


    // List of XSLT templates.
    private HashSet m_tmplFiles = new HashSet();

    // List of XSLT template repositories.
    private HashSet m_includeDirs = new HashSet();

    // The configuration file for the code generator.
    private File m_codegenConf = null;

    // The XMI file.
    private File m_xmiFile = null;

    // The output directory, it defaults to the current directory.
    private File m_outputDir = new File("");

    // The implementation language.
    private String m_lang = "cpp";

    // CSC name to be inserted into some #include (C++ only)
    private String m_cscName = "";
    
    /**
     * Main.
     */
    public static void main(String[] args) {
        TemplateManager tm = new TemplateManager();

        tm.parseArgs(args);
        tm.checkConfiguration();
        tm.checkCodeGenConf();
        tm.runCodeGeneration();
    }


    /**
     * Default constructor.
     */
    public TemplateManager() {
    }


    /**
     * Run the code generation.
     */
    protected void runCodeGeneration() {
        // Generate the main XSLT template.
        String mainTemplate = generateMainTemplate();

        // Apply the templates to the XMI document to generate the source files.

        try {

            // Create a transform factory instance.
            TransformerFactory tfactory = TransformerFactory.newInstance();

            // Create a transformer for the main XSLT template.
            Transformer transformer =
                tfactory.newTransformer(
                    new StreamSource(new StringReader(mainTemplate)));

            // Transform the source XML.
            transformer.transform(new StreamSource(m_xmiFile),
                                  new StreamResult(System.out));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * Parse the command line arguments.
     *
     * @param args The command line arguments.
     */
    protected void parseArgs(String[] args) {
        int i = 0;

        while ((i < args.length) && args[i].startsWith("-")) {
            String arg = args[i++];

            if (arg.equals("-I")) {
                // XSLT template repositories.
                if ((i < args.length) && !args[i].startsWith("-")) {
                    m_includeDirs.add(new File(args[i++]));
                } else {
                    printError("-I requires a directory name");
                }
            } else if (arg.equals("-t")) {
                // Individual XSLT template file.
                if ((i < args.length) && !args[i].startsWith("-")) {
                    File f = new File(args[i++]);

                    if (f.exists() && f.isFile()) {
                        m_tmplFiles.add(f);
                    } else {
                        printError(f.getAbsolutePath() +
                                   " does not exist or is not a file");
                    }
                } else {
                    printError("-t requires a template file name");
                }
            } else if (arg.equals("-c")) {
                // Code generator configuration file.
                if ((i < args.length) && !args[i].startsWith("-")) {
                    File f = new File(args[i++]);

                    if (f.exists() && f.isFile()) {
                        m_codegenConf = f;
                    } else {
                        printError(f.getAbsolutePath() +
                                   " does not exist or is not a file");
                    }
                } else {
                    printError("-c requires a configuration file " +
                               "for the code generator");
                }
            } else if (arg.equals("-o")) {
                // Output directory.
                if ((i < args.length) && !args[i].startsWith("-")) {
                    File f = new File(args[i++]);

                    if (f.exists() && f.isDirectory()) {
                        m_outputDir = f;
                    } else {
                        printError(f.getAbsolutePath() +
                                   " does not exist or is not a directory");
                    }
                } else {
                    printError("-o requires a directory name");
                }
            } else if (arg.equals("-l")) {
                // Implementation language.
                if ((i < args.length) && !args[i].startsWith("-")) {
                    String lang = args[i++];

                    if (lang.equals("cpp") || lang.equals("ada") ||
                        lang.equals("java") || lang.equals("c++")) {

                        m_lang = (lang.equals("c++") ? "cpp" : lang);
                    } else {
                        printError("unknown language, expecting either " +
                                   "cpp, ada or java");
                    }
                } else {
                    printError(
                        "-l requires one of these values [cpp, ada, java]");
                }
            } else if (arg.equals("--csc")) {
                // Name of the CSC
                if ((i < args.length) && !args[i].startsWith("-")) {
                    m_cscName = args[i++];
                } else {
                    printError("--csc requires a CSC name");
                }
            }
        }

        if (i == args.length) {
            printError("usage: TemplateManager [-I dir] [-t tmplfile] " +
                       "[-c cfgfile] [-o dir] [-l lang] [--csc name] xmifile");
        }

        m_xmiFile = new File(args[i]);
    }


    /**
     * Print a message to the standard error stream and exit.
     *
     * @param msg The message to print.
     */
    protected void printError(String msg) {
        System.err.println(msg);
        System.exit(1);
    }


    /**
     * Find XSLT templates from the include directories.
     */
    protected void findTemplates() {
        if ((m_includeDirs != null) && !m_includeDirs.isEmpty()) {
            Iterator iter = m_includeDirs.iterator();
            while (iter.hasNext()) {
                File dir = (File) iter.next();

                if (dir.exists() && dir.isDirectory()) {
                    File[] templates = dir.listFiles(new TemplateFileFilter());
                    for (int i = 0; i < templates.length; i++) {
                        m_tmplFiles.add(templates[i]);
                    }
                }
            }
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        findTemplates();

        if ((m_tmplFiles == null) || m_tmplFiles.isEmpty()) {
            printError("at least one XSLT template file must be specified");
        }

        if (m_codegenConf == null) {
            printError("a configuration file for the code " +
                       "generator must be provided");
        }
    }


    /**
     * Get the named templates from the template files.
     *
     * @return The list of named templates.
     */
    protected Collection getNamedTemplates() {
        DocumentBuilderFactory dfactory = DocumentBuilderFactory.newInstance();
        dfactory.setNamespaceAware(true);

        HashSet namedTemplates = new HashSet();

        Iterator iter = m_tmplFiles.iterator();
        while (iter.hasNext()) {
            FileInputStream fis = null;
            try {
                // Parse each XSLT template file to find the named templates.
                fis = new FileInputStream((File) iter.next());
                InputSource in = new InputSource(fis);
                Document doc = dfactory.newDocumentBuilder().parse(in);

                String xpath = "//xsl:template/@name";
                NodeIterator nodes = XPathAPI.selectNodeIterator(doc, xpath);
                Node n;
                while ((n = nodes.nextNode()) != null) {
                    String name = n.getNodeValue();
                    // Skip all template names that match this regex pattern:
                    // ".*\.content(\.\d+)?"
                    if (!name.endsWith(".content") &&
                        (name.lastIndexOf(".content.") == -1) &&
                        (name.endsWith("hpp") || name.endsWith("cpp") ||
                         name.endsWith("ads") || name.endsWith("adb") ||
                         name.endsWith("java") ||
                         name.equals("corba_component_desc") ||
                         name.equals("config_properties_file"))) {

                        namedTemplates.add(name);
                    }
                }
            } catch (Exception ex) {
                ex.printStackTrace();
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

        return namedTemplates;
    }


    /**
     * Define the global variables for the main XSLT template.
     *
     * @return A map of variables and their respective values. 
     */
    protected Map getMainTmplGlobalVars() {
        Hashtable vars = new Hashtable();

        try {
            vars.put("CODEGEN_CONF", m_codegenConf.getCanonicalPath());
            vars.put("DIR", m_outputDir.getCanonicalPath());
        } catch (IOException e) {
            e.printStackTrace();
        }

        vars.put("LANG", m_lang);
        vars.put("OS", File.pathSeparator.equals(";") ? "dos" : "unix");
        vars.put("CSC_NAME", m_cscName);

        return vars;
    }


    /**
     * Check the code generator configuration file and build up a list
     * of properties.
     *
     * @return A map of properties and their respective values.
     */
    protected Map checkCodeGenConf() {
        DocumentBuilderFactory dfactory = DocumentBuilderFactory.newInstance();
        dfactory.setNamespaceAware(true);

        Hashtable props = new Hashtable();

        FileInputStream fis = null;
        try {
            fis = new FileInputStream(m_codegenConf);
            InputSource in = new InputSource(fis);
            Document doc = dfactory.newDocumentBuilder().parse(in);

            // framework nodes ?
            String xpath = "//programs/program/services/lifecycle/framework";
            XObject xobj = XPathAPI.eval(doc, xpath);
            props.put("hasFrameworkNodes", new Boolean(xobj.bool()));

            // strategies.

            // init strategy values.
            props.put("has_entity", new Boolean(false));
            props.put("has_unmanaged_entity", new Boolean(false));
            props.put("has_session", new Boolean(false));
            props.put("has_service", new Boolean(false));
            props.put("has_process", new Boolean(false));

            NodeIterator frameworkNodes =
                XPathAPI.selectNodeIterator(doc, xpath);

            Node n;
            while ((n = frameworkNodes.nextNode()) != null) {
                xobj = XPathAPI.eval(n, "@idref");
                String idref = xobj.str().trim();

                xpath = "//lifecycle-frameworks/lifecycle-framework" +
                        "[@id = '" + idref + "']/*/@category";
                xobj = XPathAPI.eval(doc, xpath);
                String strategy = xobj.str().trim();
                props.put("has_" + strategy.toLowerCase(), new Boolean(true));
            }

            // lifecycle-framework nodes ?
            xpath = "//lifecycle-frameworks/lifecycle-framework";
            xobj = XPathAPI.eval(doc, xpath);
            props.put("hasLifecycleFrameworkNodes", new Boolean(xobj.bool()));

            // generate lifecycle factories ?
            xpath = "//lifecycle-frameworks/lifecycle-framework/factory-impl";
            xobj = XPathAPI.eval(doc, xpath);
            props.put("mustGenerateFactories", new Boolean(xobj.bool()));

            // generate CCM CIF ?
            xpath = "//lifecycle-frameworks/lifecycle-framework/home-impl";
            xobj = XPathAPI.eval(doc, xpath);
            props.put("mustGenerateCcmCif", new Boolean(xobj.bool()));

            // generate datastore TypeTrait ?
            xpath = "//datastore-types/datastore-type";
            xobj = XPathAPI.eval(doc, xpath);
            props.put("mustGenerateDataStoreTypeTraits", new Boolean(xobj.bool()));
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException ioe) {
                    // Ignore.
                }
            }
        }
        
        return props;
    }


    /**
     * Generate the main XSLT template.
     *
     * @return The main XSLT template.
     */
    protected String generateMainTemplate() {
        StringBuffer mainTemplate = new StringBuffer();

        // Header.
        mainTemplate.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        mainTemplate.append("<xsl:stylesheet version=\"1.1\" xmlns:xsl=");
        mainTemplate.append("\"http://www.w3.org/1999/XSL/Transform\">\n");
        mainTemplate.append("<xsl:output method=\"text\"/>\n");

        // Global variables.
        Map globalVars = getMainTmplGlobalVars();
        Iterator iter = globalVars.keySet().iterator();
        while (iter.hasNext()) {
            String key = (String) iter.next();

            mainTemplate.append("<xsl:variable name=\"");
            mainTemplate.append(key);
            mainTemplate.append("\" select=\"'");
            mainTemplate.append((String) globalVars.get(key));
            mainTemplate.append("'\"/>\n");
        }

        // Includes.
        try {
            iter = m_tmplFiles.iterator();
            while (iter.hasNext()) {
                File f = (File) iter.next();

                mainTemplate.append("<xsl:include href=\"");
                mainTemplate.append(f.getCanonicalPath());
                mainTemplate.append("\"/>\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Main.
        mainTemplate.append("<xsl:template match=\"/\">\n");

        // Start message.
        mainTemplate.append("<xsl:message>\n");
        mainTemplate.append("<xsl:value-of select=\"concat($lf, ");
        mainTemplate.append("'***************************', $lf, ");
        mainTemplate.append("'*   CDMW Code Generator   *', $lf, ");
        mainTemplate.append("'***************************', $lf)\"/>\n");
        mainTemplate.append("</xsl:message>\n");

        // Call named templates.
        String headerSuffix = "";
        String bodySuffix = "";
        if (m_lang.equals("cpp")) {
            headerSuffix = "hpp";
            bodySuffix = "cpp";
        } else if (m_lang.equals("ada")) {
            headerSuffix = "ads";
            bodySuffix = "adb";
        } else if (m_lang.equals("java")) {
            headerSuffix = "java";
            bodySuffix = "java";
        }
        boolean bodyOnly = headerSuffix.equals(bodySuffix);

        Map props = checkCodeGenConf();

        // CDMW interface.
        mainTemplate.append("<xsl:call-template name=\"");
        mainTemplate.append("cdmw_interface_" + bodySuffix + "\"/>\n");

        // DTD.
        mainTemplate.append("<xsl:variable name=\"programNodes\" ");
        mainTemplate.append("select=\"$cdmwNode/programs/program\"/>\n");
        mainTemplate.append("<xsl:for-each select=\"$programNodes\">\n");
        mainTemplate.append("<xsl:call-template name=\"proc_desc\">\n");
        mainTemplate.append("<xsl:with-param name=\"_programNode\" ");
        mainTemplate.append("select=\".\"/>\n");
        mainTemplate.append("</xsl:call-template>\n");
        mainTemplate.append("</xsl:for-each>\n");


        boolean mustGenerateDataStoreTypeTraits =
            ((Boolean) props.get("mustGenerateDataStoreTypeTraits")).booleanValue();

        if(mustGenerateDataStoreTypeTraits) {
            mainTemplate.append("<xsl:call-template name=\"datastore_type_traits\"/>\n");
            mainTemplate.append("<xsl:call-template name=\"global_datastore_type_traits\"/>\n");
        }

        boolean mustGenerateFactories =
            ((Boolean) props.get("mustGenerateFactories")).booleanValue();

        if (mustGenerateFactories) {
            // Factory impl.
            if (m_lang.equals("ada")) {
                mainTemplate.append("<xsl:call-template name=\"factory_");
                mainTemplate.append("impl_" + headerSuffix + "\"/>\n");
                mainTemplate.append("<xsl:call-template name=\"factory_");
                mainTemplate.append("impl_" + bodySuffix + "\"/>\n");
            }

            // Servant provider.
            if (!bodyOnly) {
                mainTemplate.append("<xsl:call-template name=\"servant_");
                mainTemplate.append("provider_" + headerSuffix + "\"/>\n");
            }
            mainTemplate.append("<xsl:call-template name=\"servant_");
            mainTemplate.append("provider_" + bodySuffix + "\"/>\n");
        }

        boolean hasFrameworkNodes =
            ((Boolean) props.get("hasFrameworkNodes")).booleanValue();

        boolean mustGenerateCcmCif =
            ((Boolean) props.get("mustGenerateCcmCif")).booleanValue();

        Collection namedTemplates = getNamedTemplates();

        iter = props.keySet().iterator();
        while (iter.hasNext()) {
            String key = (String) iter.next();
            if (key.startsWith("has_")) {
                String strategy = key.substring(4);
                boolean hasStrategy = ((Boolean) props.get(key)).booleanValue();

                if (hasStrategy || !hasFrameworkNodes) {
                    Iterator i = namedTemplates.iterator();
                    while (i.hasNext()) {
                        String tmpl = (String) i.next();

                        // Lifecycle factories.
                        String factoryRegex = strategy + "_factory_" +
                            "(" + headerSuffix + "|" + bodySuffix + ")";

                        if (Pattern.matches(factoryRegex, tmpl) ||
                            (m_lang.equals("cpp") &&
                                tmpl.equals(strategy + "_factory_allocator_" +
                                            headerSuffix))) {

                            mainTemplate.append("<xsl:call-template name=\"");
                            mainTemplate.append(tmpl);
                            mainTemplate.append("\"/>\n");
                        }

                        // CCM CIF.
                        if (mustGenerateCcmCif) {
                            String ccmcifRegex =
                                strategy + "_(home|component|context)_" +
                                "(" + headerSuffix + "|" + bodySuffix + ")";

                            if (Pattern.matches(ccmcifRegex, tmpl)) {
                                mainTemplate.append("<xsl:call-template ");
                                mainTemplate.append("name=\"");
                                mainTemplate.append(tmpl);
                                mainTemplate.append("\"/>\n");
                            }
                        }
                    }
                }
            }
        }

        // CCM CIF.
        if (mustGenerateCcmCif) {
            iter = namedTemplates.iterator();
            while (iter.hasNext()) {
                String tmpl = (String) iter.next();

                String ccmcifRegex = "ccmcif_(facet|consumer)_" +
                    "(" + headerSuffix + "|" + bodySuffix + ")";

                if (Pattern.matches(ccmcifRegex, tmpl) ||
                    tmpl.equals("corba_component_desc") ||
                    tmpl.equals("config_properties_file")) {

                    mainTemplate.append("<xsl:call-template name=\"");
                    mainTemplate.append(tmpl);
                    mainTemplate.append("\"/>\n");
                }
            }
        }

        // End message.
        mainTemplate.append("<xsl:message>\n");
        mainTemplate.append("<xsl:value-of select=\"concat($lf, ");
        mainTemplate.append("'*********** End ***********', $lf)\"/>\n");
        mainTemplate.append("</xsl:message>\n");

        mainTemplate.append("</xsl:template>\n");

        // Footer.
        mainTemplate.append("</xsl:stylesheet>\n");

        return mainTemplate.toString();
    }


    /**
     * XSLT template file filter.
     */
    private class TemplateFileFilter implements FileFilter {


        /**
         * Test whether or not the specified file must be discarded.
         */
        public boolean accept(File pathname) {
            return pathname.toString().endsWith("Tmpl.xslt") ? true : false;
        }


    }


}


