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


import com.thalesgroup.tools.ant.util.FindUtils;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import java.util.Iterator;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.util.StringUtils;


/**
 * Given a list of CSCs, determine the order of compilation of each of them
 * and generate a build file to compile the CSCs.
 */
public class CreateBuildFile extends Task {


    private File m_file = null;
    private Vector m_cscs = new Vector();
    private Vector m_compiledCscs = new Vector();
    private boolean m_quiet = false;
    private String m_siteproperties = "";
    private String m_devtokens = "";
    private String m_instokens = "";
    private String m_globaldeps = "";
    private boolean m_globalCompilation = true;
    private boolean m_ignoreccache = false;


    public void setFile(File f) {
        m_file = f;
    }


    public void setCscs(String cscs) {
        m_cscs = StringUtils.split(cscs.trim(), ' ');
    }


    public void setCompiledCscs(String compiledCscs) {
        m_compiledCscs = StringUtils.split(compiledCscs.trim(), ',');
    }


    public void setQuiet(boolean quiet) {
        m_quiet = quiet;
    }


    public void setSiteProperties(String siteproperties) {
        m_siteproperties = siteproperties;
    }


    public void setDevTokens(String devtokens) {
        m_devtokens = devtokens;
    }


    public void setInsTokens(String instokens) {
        m_instokens = instokens;
    }


    public void setGlobalDeps(String globaldeps) {
        m_globaldeps = globaldeps;
    }


    public void setGlobalCompilation(boolean globalCompilation) {
        m_globalCompilation = globalCompilation;
    }

    public void setIgnoreccache(boolean ignoreccache) {
        m_ignoreccache = ignoreccache;
    }


    /**
     * Default constructor.
     */
    public CreateBuildFile() {
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

        if ((m_cscs == null) || m_cscs.isEmpty()) {
            throw new BuildException("cscs must not be null.",
                                     getLocation());
        }
    }


    /**
     * Runs the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        String seqstr = getProject().getProperty("compilation.sequence");
        if (seqstr == null) {
            throw new BuildException(
                "property ${compilation.sequence} does not exist.",
                getLocation());
        }

        Vector seq = StringUtils.split(seqstr, ' ');

        seq.retainAll(m_cscs);

        seq.removeAll(m_compiledCscs);

        try {
            FileWriter writer = new FileWriter(m_file);

            String header =
                "<?xml version=\"1.0\"?>\n" +
                "<project default=\"compile\">\n" +
                "<target name=\"compile\">\n";

            String footer = "</target>\n</project>\n";

            writer.write(header);

            String verbose = (m_quiet ? "-q -emacs" : "");

            String globalCompilation = (m_globalCompilation
                ? "  <arg line=\"-Dglobal.compilation=true\"/>\n" : "");

            Iterator it = seq.iterator();
            while (it.hasNext()) {
                File f = FindUtils.findFile(
                    getProject(), "Build*.xml",
                    new File((String) it.next()),  false);

                String task =
                    "<java fork=\"true\" failonerror=\"true\" " +
                        "classname=\"org.apache.tools.ant.launch.Launcher\" " +
                        "classpath=\"${java.class.path}\">\n" +
                        "  <arg line=\"-f " + f.getAbsolutePath() + "\"/>\n" +
                        "  <arg line=\"" + verbose + "\"/>\n" +
                        "  <arg line=\"-Dbuild.site.properties=" + m_siteproperties + "\"/>\n" +
                        "  <arg line=\"-Dbuild.dev.tokens.properties=" + m_devtokens + "\"/>\n" +
                        "  <arg line=\"-Dbuild.install.tokens.properties=" + m_instokens + "\"/>\n" +
                        "  <arg line=\"-Dcsci.global.dependencies=" + m_globaldeps + "\"/>\n" +
                        "  <arg line=\"-Dignore.ccache=" + (m_ignoreccache ? "true" : "false") + "\"/>\n" +
                        globalCompilation +
                    "</java>\n";

                writer.write(task);
            }

            writer.write(footer);
            writer.close();
        } catch (IOException e) {
            throw new BuildException(e, getLocation());
        }
    }


}
