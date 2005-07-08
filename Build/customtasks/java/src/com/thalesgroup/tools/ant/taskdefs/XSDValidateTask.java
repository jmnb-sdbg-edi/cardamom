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

import java.net.MalformedURLException;
import java.net.URL;

import java.util.Collection;
import java.util.Vector;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.FileSet;

import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.DefaultHandler;


/**
 * Validate an XML document against an XML schema.
 */
public class XSDValidateTask extends Task {


    protected static final String JAXP_SCHEMA_LANGUAGE =
        "http://java.sun.com/xml/jaxp/properties/schemaLanguage";

    protected static final String W3C_XML_SCHEMA =
        "http://www.w3.org/2001/XMLSchema";

    protected static final String JAXP_SCHEMA_SOURCE =
        "http://java.sun.com/xml/jaxp/properties/schemaSource";

    protected static final String VALIDATION_FEATURE =
        "http://xml.org/sax/features/validation";

    protected static final String INIT_FAILED_MSG = 
        "Could not start XML validation: ";


    // Whether to stop or not if an error occured.
    private boolean m_failOnError = true;

    // Print warning message.
    private boolean m_warn = true;

    // Validate XML documents.
    private boolean m_lenient = false;

    // The file to be validated.
    private File m_file = null;

    // Set of files to be validated.
    private Vector m_filesets = new Vector();

    // The XML schema file.
    private File m_xsd = null;

    // The SAX parser.
    private SAXParser m_parser = null;

    // The error handler to report SAX parsing errors.
    private ValidatorErrorHandler errorHandler = new ValidatorErrorHandler();


    /**
     * Specify how parser errors are to be handled (defaults to
     * <code>true</code>).
     *
     * @param fail If set to <code>true</code> then throw BuildException.
     */
    public void setFailOnError(boolean fail) {
        m_failOnError = fail;
    }


    /**
     * Get fail on error flag value.
     *
     * @return The fail on error flag value.
     */
    public boolean getFailOnError() {
        return m_failOnError;
    }


    /**
     * Specify how parser errors are to be handled (defaults to
     * <code>true</code>).
     *
     * @param warn If set to <code>true</code> then log a warn message for each
     *             SAX warn event.
     */
    public void setWarn(boolean warn) {
        m_warn = warn;
    }


    /**
     * Get warn flag value.
     *
     * @return The warn flag value.
     */
    public boolean getWarn() {
        return m_warn;
    }

    
    /**
     * Specify whether the parser should be validating (defaults to
     * <code>true</code>).
     *
     * @param lenient If set to <code>false<code>, the validation will fail
     *                only if the parsed document is not well formed XML.
     */
    public void setLenient(boolean lenient) {
        m_lenient = lenient;
    }


    /**
     * Get lenient flag value.
     *
     * @return The lenient flag value.
     */
    public boolean getLenient() {
        return m_lenient;
    }


    /**
     * Specify the file to be checked.
     *
     * @param file The file to be checked.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Get the file to be checked.
     *
     * @return The file to be checked.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Specify a set of files to be checked.
     *
     * @param filset The set of files to be checked.
     */
    public void addFileset(FileSet fileset) {
        m_filesets.addElement(fileset);
    }


    /**
     * Get the list of files to be checked.
     *
     * @return The list of files to be checked.
     */
    public Collection getFileset() {
        return m_filesets;
    }


    /**
     * Specify the XML schema to be used for the validation.
     *
     * @param xsd The XML schema file.
     */
    public void setXsd(File xsd) {
        m_xsd = xsd;
    }


    /**
     * Get the XML schema file.
     *
     * @return The XML schema file.
     */
    public File getXsd() {
        return m_xsd;
    }
    

    /**
     * Default constructor.
     */
    public XSDValidateTask() {
        super();
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        // Initialize the parser and set the features.
        int fileProcessed = 0;
        initValidator();

        if (m_file != null) {
            if (m_file.exists() && m_file.canRead() && m_file.isFile())  {
                doValidate(m_file);
                fileProcessed++;
            } else {
                String errorMsg = "file " + m_file + " cannot be read.";
                if (m_failOnError) {
                    throw new BuildException(errorMsg);
                } else {
                    log(errorMsg, Project.MSG_ERR);
                }
            }
        }

        for (int i = 0; i < m_filesets.size(); i++) {
            FileSet fs = (FileSet) m_filesets.elementAt(i);
            DirectoryScanner ds = fs.getDirectoryScanner(getProject());
            String[] files = ds.getIncludedFiles();

            for (int j = 0; j < files.length ; j++)  {
                File srcFile = new File(fs.getDir(getProject()), files[j]);
                doValidate(srcFile);
                fileProcessed++;
            }
        }

        log(fileProcessed + " file(s) have been successfully validated.",
            Project.MSG_VERBOSE);
    }


    /**
     * Initialize the parser. 
     */
    protected void initValidator() {
        SAXParserFactory spf = SAXParserFactory.newInstance();

        spf.setNamespaceAware(true);
        spf.setValidating(m_lenient);

        try {
            m_parser = spf.newSAXParser();

            m_parser.getXMLReader().setFeature(VALIDATION_FEATURE, true);

            // Set the schema language if necessary.
            m_parser.setProperty(JAXP_SCHEMA_LANGUAGE, W3C_XML_SCHEMA);

            if (m_xsd != null) {
                m_parser.setProperty(JAXP_SCHEMA_SOURCE, m_xsd);
            }
        } catch (Exception ex) {
            throw new BuildException(INIT_FAILED_MSG + ex.toString());
        }
    }


    /**
     * Parse the file.
     *
     * @param afile The file to be parsed.
     */
    protected void doValidate(File afile) {
        try {
            log("validating " + afile.getName() + "... ", Project.MSG_VERBOSE);

            errorHandler.init(afile);

            InputSource is = new InputSource(new FileInputStream(afile));

            String uri = "file:" + afile.getAbsolutePath().replace('\\', '/');
            for (int index = uri.indexOf('#');
                 index != -1;
                 index = uri.indexOf('#')) {

                uri = uri.substring(0, index) + "%23" +
                        uri.substring(index + 1);
            }

            is.setSystemId(uri);
            m_parser.parse(is, (DefaultHandler) errorHandler);
        } catch (SAXException ex) {
            if (m_failOnError) {
                throw new BuildException(
                    "could not validate document " + afile);
            }
        } catch (IOException ex) {
            throw new BuildException(
                "could not validate document " + afile, ex);
        }

        if (errorHandler.getFailure()) {
            if (m_failOnError) {
                throw new BuildException(
                    afile + " is not a valid XML document.");
            } else {
                log(afile + " is not a valid XML document", Project.MSG_ERR);
            }
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_file == null) && m_filesets.isEmpty()) {
            throw new BuildException(
                "specify at least one source - a file or a fileset.");
        }
    }


    /**
     * Log SAX parse exceptions and remember if an error occurred.
     */
    protected class ValidatorErrorHandler extends DefaultHandler {


        // The file being parsed.
        private File m_currentFile = null;

        // The last error message.
        private String m_lastErrorMessage = null;

        // Indicate whether an error occurred during last parsing.
        private boolean m_failed = false;


        /**
         * Initialize the error handler.
         *
         * @param file The file being parsed.
         */
        public void init(File file) {
            m_currentFile = file;
            m_failed = false;
        }


        /**
         * Return <code>true</code> if an error occurred during last parsing.
         *
         * @return <code>true</code> if an error occurred during last parsing.
         */
        public boolean getFailure() {
            return m_failed;
        }


        /**
         * Log the fatal error.
         *
         * @param exception The SAX parse exception.
         */
        public void fatalError(SAXParseException exception) {
            m_failed = true;
            log(getMessage(exception), Project.MSG_ERR);
        }


        /**
         * Log the error.
         *
         * @param exception The SAX parse exception.
         */
        public void error(SAXParseException exception) {
            m_failed = true;
            log(getMessage(exception), Project.MSG_ERR);
        }


        /**
         * Log the warning error.
         *
         * @param exception The SAX parse exception.
         */
        public void warning(SAXParseException exception) {
            if (m_warn) {
                log(getMessage(exception), Project.MSG_WARN);
            }
        }


        /**
         * Build the SAX error message.
         *
         * @param exception The SAX parse exception.
         */
        protected String getMessage(SAXParseException exception) {
            String sysID = exception.getSystemId();

            if (sysID != null) {
                try {
                    int line = exception.getLineNumber();
                    int col = exception.getColumnNumber();

                    return new URL(sysID).getFile() +
                        ((line == -1) ? "" : (":" + line +
                                             (col == -1 ? "" : (":" + col)))) +
                                             ": " + exception.getMessage();
                } catch (MalformedURLException mfue) {
                    // Ignore.
                }
            }

            return exception.getMessage();
        }
    }


}

