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
import java.io.FileWriter;
import java.io.IOException;

import java.text.DateFormat;

import java.util.Date;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.Map;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.FileSet;


/**
 * Generate stub and skeleton header files associated with a specific ORB.
 * Two files are created:
 *  - idl_filename.stub.hpp
 *  - idl_filename.skel.hpp
 * Additionally, idl_filename.skel_tie.hpp is generated if tie is selected.
 */
public class MakeIdlInclude extends Task {


    // Supported ORBs.
    private Vector m_orbs = new Vector();

    // Whether to stop or not if an error occured.
    private boolean m_failOnError = true;

    // The ORB.
    private String m_orb = null;
    private String m_lcaseOrb = null;

    // The IDL file.
    private File m_file = null;

    // Set of IDL files.
    private Vector m_filesets = new Vector();

    // The output directory.
    private File m_outputDir = null;

    // Suffix for header files.
    private String m_hSuffix = null;

    // Suffix for body files.
    private String m_cSuffix = null;

    // Append this DIR within the include statement.
    private String m_headerInclDir = null;

    // Suffix for client files.
    private String m_clientSuffix = null;

    // Suffix for server files.
    private String m_serverSuffix = null;

    // Generate only client dependencies flag.
    private boolean m_clientOnly = false;

    // Generate only server dependencies flag.
    private boolean m_serverOnly = false;

    // Generate tie header files.
    private boolean m_tie = false;

    // Map of ORB-dependent suffixes.
    private Hashtable m_suffixes = null;


    /**
     * Specify how errors are to be handled.
     *
     * @param fail If set to <code>true</code> then throw BuildException.
     */
    public void setFailOnError(boolean fail) {
        m_failOnError = fail;
    }


    /**
     * Get the fail on error flag.
     *
     * @return The fail on error flag.
     */
    public boolean getFailOnError() {
        return m_failOnError;
    }


    /**
     * Specify the ORB.
     *
     * @param orb The ORB.
     */
    public void setOrb(String orb) {
        m_orb = orb;
    }


    /**
     * Get the ORB.
     *
     * @return The ORB.
     */
    public String getOrb() {
        return m_orb;
    }


    /**
     * Specify the IDL file.
     *
     * @param file The IDL file.
     */
    public void setFile(File file) {
        m_file = file;
    }


    /**
     * Get the IDL file.
     *
     * @return The IDL file.
     */
    public File getFile() {
        return m_file;
    }


    /**
     * Specify a set of IDL files.
     *
     * @param fileset The set of IDL files.
     */
    public void addFileset(FileSet fileset) {
        m_filesets.addElement(fileset);
    }


    /**
     * Specify the output directory.
     *
     * @param outputDir The output directory.
     */
    public void setOutputDir(File outputDir) {
        m_outputDir = outputDir;
    }


    /**
     * Get the output directory.
     *
     * @return The output directory.
     */
    public File getOutputDir() {
        return m_outputDir;
    }


    /**
     * Specify the suffix for header files.
     *
     * @param hSuffix The suffix for header files.
     */
    public void setHSuffix(String hSuffix) {
        m_hSuffix = hSuffix;
    }


    /**
     * Get the suffix for header files.
     *
     * @return The suffix for header files.
     */
    public String getHSuffix() {
        return m_hSuffix;
    }


    /**
     * Specify the directory to append within the include statement.
     *
     * @param headerInclDir The directory to append.
     */
    public void setHeaderInclDir(String headerInclDir) {
        m_headerInclDir = headerInclDir;
    }


    /**
     * Get the directory to append within the include statement.
     *
     * @return The directory to append.
     */
    public String getHeaderInclDir() {
        return m_headerInclDir;
    }


    /**
     * Specify the suffix for body files.
     *
     * @param cSuffix The suffix for body files.
     */
    public void setCSuffix(String cSuffix) {
        m_cSuffix = cSuffix;
    }


    /**
     * Get the suffix for body files.
     *
     * @return The suffix for body files.
     */
    public String getCSuffix() {
        return m_cSuffix;
    }


    /**
     * Specify the suffix for client header files.
     *
     * @param clientSuffix The suffix for client header files.
     */
    public void setClientSuffix(String clientSuffix) {
        m_clientSuffix = clientSuffix;
    }


    /**
     * Get the suffix for client header files.
     *
     * @return The suffix for client header files.
     */
    public String getClientSuffix() {
        return m_clientSuffix;
    }


    /**
     * Specify the suffix for server header files.
     *
     * @param serverSuffix The suffix for server header files.
     */
    public void setServerSuffix(String serverSuffix) {
        m_serverSuffix = serverSuffix;
    }


    /**
     * Get the suffix for server header files.
     *
     * @return The suffix for server header files.
     */
    public String getServerSuffix() {
        return m_serverSuffix;
    }


    /**
     * Specify whether only client dependencies should be generated.
     *
     * @param clientOnly If <code>true</code> then only client dependencies
     *                   will be generated.
     */
    public void setClientOnly(boolean clientOnly) {
        m_clientOnly = clientOnly;
    }


    /**
     * Get the client only flag.
     *
     * @return The client only flag.
     */
    public boolean getClientOnly() {
        return m_clientOnly;
    }


    /**
     * Specify whether only server dependencies should be generated.
     *
     * @param serverOnly If <code>true</code> then only server dependencies
     *                   will be generated.
     */
    public void setServerOnly(boolean serverOnly) {
        m_serverOnly = serverOnly;
    }


    /**
     * Get the server only flag.
     *
     * @return The server only flag.
     */
    public boolean getServerOnly() {
        return m_serverOnly;
    }


    /**
     * Specify whether tie header files should be generated.
     *
     * @param tie If <code>true</code> then tie headers will be generated.
     */
    public void setTie(boolean tie) {
        m_tie = tie;
    }


    /**
     * Get the tie flag.
     *
     * @return The tie flag.
     */
    public boolean getTie() {
        return m_tie;
    }


    /**
     * Default constructor.
     */
    public MakeIdlInclude() {
        super();

        m_orbs.add("ORBacusCPP");
        m_orbs.add("Visibroker");
        m_orbs.add("Orbix");
        m_orbs.add("OmniORB");
        m_orbs.add("Mico");
        m_orbs.add("TAO");
        m_orbs.add("TAOOpenFusion");
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        checkConfiguration();

        m_lcaseOrb = m_orb.toLowerCase();
        if (m_orb.equals("TAOOpenFusion"))
            m_lcaseOrb ="tao";

        m_suffixes = new Hashtable(getSuffixes());

        if (m_file != null) {
            if (m_file.exists() && m_file.canRead() && m_file.isFile())  {
                createHeaderFiles(m_file);
            } else {
                String errorMsg = "file " + m_file + " cannot be read.";

                if (m_failOnError) {
                    throw new BuildException(errorMsg);
                } else {
                    log(errorMsg, Project.MSG_ERR);
                }
            }
        }

        Iterator iter = m_filesets.iterator();
        while (iter.hasNext()) {
            FileSet fs = (FileSet) iter.next();

            DirectoryScanner ds = fs.getDirectoryScanner(getProject());
            String[] files = ds.getIncludedFiles();

            for (int i = 0; i < files.length ; i++)  {
                File srcFile = new File(fs.getDir(getProject()), files[i]);

                createHeaderFiles(srcFile);
            }
        }
    }


    /**
     * Check the attributes.
     */
    protected void checkConfiguration() {
        if ((m_orb == null) || m_orb.equals("")) {
            throw new BuildException("attribute orb must not be null.");
        }

        if (!m_orbs.contains(m_orb)) {
            throw new BuildException("your ORB is not supported.");
        }

        if ((m_file == null) && m_filesets.isEmpty()) {
            throw new BuildException(
                "specify at least one source - a file or a fileset.");
        }

        if (m_outputDir != null) {
            if (!m_outputDir.exists()) {
                throw new BuildException("outputdir does not exist.");
            }

            if (!m_outputDir.isDirectory()) {
                throw new BuildException("outputdir is not a directory.");
            }
        }

        if (m_clientOnly && m_serverOnly) {
            throw new BuildException(
                "clientOnly and serverOnly are exclusive.");
        }
    }


    /**
     * Create the header files for the specified IDL file.
     *
     * @param file The IDL file.
     */
    protected void createHeaderFiles(File file) {
        String basename = file.getName();

        int pos = basename.lastIndexOf('.');
        if (pos != -1) {
            if (basename.substring(pos).equalsIgnoreCase(".idl")) {
                basename = basename.substring(0, pos);
            }
        }

        String ucaseName = basename.toUpperCase();

        String stubH = basename +
            (String) m_suffixes.get("stubSuffix") +
            (String) m_suffixes.get("hSuffix");

        String skelH = basename +
            (String) m_suffixes.get("skelSuffix") +
            (String) m_suffixes.get("hSuffix");

        // Stub headers.
        if (!m_serverOnly) {
            String name = basename + ".stub.hpp." + m_lcaseOrb;

            if ((m_outputDir != null) && !m_outputDir.equals("")) {
                name = m_outputDir + File.separator + name;
            }

            String nameH = ucaseName + "_STUB_H";

            String incH = stubH;

            if ((m_headerInclDir != null) && !m_headerInclDir.equals("")) {
                incH = m_headerInclDir + File.separator + incH;
            }

            generateFile(name, nameH, incH);
        }

        // Skel headers.
        if (!m_clientOnly) {
            String name = basename + ".skel.hpp." + m_lcaseOrb;

            if ((m_outputDir != null) && !m_outputDir.equals("")) {
                name = m_outputDir + File.separator + name;
            }

            String nameH = ucaseName + "_SKEL_H";

            String incH = skelH;

            if ((m_headerInclDir != null) && !m_headerInclDir.equals("")) {
                incH = m_headerInclDir + File.separator + incH;
            }

            generateFile(name, nameH, incH);
        }

        // Tie headers.
        if (m_tie) {
            String skelTieH = skelH;

            if (m_orb.equals("ORBacus") || m_orb.equals("TAO") || m_orb.equals("TAOOpenFusion")) {
                skelTieH = basename +
                    (String) m_suffixes.get("skelTieSuffix") +
                    (String) m_suffixes.get("hSuffix");
            }

            String name = basename + ".skeltie.new";

            if ((m_outputDir != null) && !m_outputDir.equals("")) {
                name = m_outputDir + File.separator + name;
            }

            String nameH = ucaseName + "_SKELTIE_H";
            String incH = skelTieH;
            
            if ((m_headerInclDir != null) && !m_headerInclDir.equals("")) {
                incH = m_headerInclDir + File.separator + incH;
            }

            generateFile(name, nameH, incH);
        }
    }


    /**
     * Set the suffixes accordingly to the ORB.
     *
     * @return A list of suffixes.
     */
    protected Map getSuffixes() {
        Hashtable suffixes = new Hashtable();

        if (m_orb.equals("ORBacus")) {
            suffixes.put("stubSuffix", "");

            suffixes.put("skelSuffix", "_skel");

            suffixes.put("skelTieSuffix", "_skel_tie");

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".h" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".cpp" : m_cSuffix);
        } else if (m_orb.equals("Visibroker")) {
            suffixes.put("stubSuffix",
                         (m_clientSuffix == null) ? "_c" : m_clientSuffix);

            suffixes.put("skelSuffix",
                         (m_serverSuffix == null) ? "_s" : m_serverSuffix);

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".hh" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".cc" : m_cSuffix);
        } else if (m_orb.equals("Orbix")) {
            suffixes.put("stubSuffix",
                         (m_clientSuffix == null) ? "C" : m_clientSuffix);

            suffixes.put("skelSuffix",
                         (m_serverSuffix == null) ? "S" : m_serverSuffix);

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".hh" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".C" : m_cSuffix);
        } else if (m_orb.equals("OmniORB")) {
            suffixes.put("stubSuffix",
                         (m_clientSuffix == null) ? "" : m_clientSuffix);

            suffixes.put("skelSuffix",
                         (m_serverSuffix == null) ? "" : m_serverSuffix);

            String omniOrbDyn = getProject().getProperty("env.OMNIORB_DYN");
            if (omniOrbDyn != null) {
                suffixes.put("skelDynSuffix", "Dyn");
            }

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".hh" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? "SK.cc" : m_cSuffix);
        } else if (m_orb.equals("Mico")) {
            suffixes.put("stubSuffix", "");

            String micoSkel = getProject().getProperty("env.MICO_SKEL");
            suffixes.put("skelSuffix",
                         (micoSkel != null) ? "_skel" : "");

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".h" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".cc" : m_cSuffix);
        } else if (m_orb.equals("TAO")) {
            suffixes.put("stubSuffix", "C");

            suffixes.put("skelSuffix", "S");

            suffixes.put("skelTieSuffix", "S_T");

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".h" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".cpp" : m_cSuffix);
        } else if (m_orb.equals("TAOOpenFusion")) {
            suffixes.put("stubSuffix", "C");

            suffixes.put("skelSuffix", "S");

            suffixes.put("skelTieSuffix", "S_T");

            suffixes.put("hSuffix",
                         (m_hSuffix == null) ? ".h" : m_hSuffix);

            suffixes.put("cSuffix",
                         (m_cSuffix == null) ? ".cpp" : m_cSuffix);
        }

        return suffixes;
    }


    /**
     * Generate header file.
     *
     * @param name  The header file name.
     * @param nameH String to use for #ifndef and #define.
     * @param incH  The include file name.
     */
    protected void generateFile(String name, String nameH, String incH) {
        StringBuffer sb = new StringBuffer();
        
        sb.append("#ifdef ");
        sb.append(m_lcaseOrb);
        sb.append("\n/*\n");
        sb.append("  File generated by mkidlinclude ANT task. DO NOT EDIT!");
        sb.append("\n  Created on: ");
        sb.append(DateFormat.getDateTimeInstance().format(new Date()));
        sb.append(".\n  ORB: ");
        sb.append(m_orb);
        sb.append("\n*/\n\n");
        sb.append("#ifndef ");
        sb.append(nameH);
        sb.append("\n#define ");
        sb.append(nameH);
        sb.append("\n#include <");
        sb.append(incH);
        sb.append(">\n#endif /* ");
        sb.append(nameH);
        sb.append("*/\n#endif /* ");
        sb.append(m_lcaseOrb);
        sb.append(" */\n");

        try {
            FileWriter fw = new FileWriter(name);
            fw.write(sb.toString());
            fw.close();
        } catch (IOException ioe) {
            throw new BuildException(ioe);
        }
    }


}

