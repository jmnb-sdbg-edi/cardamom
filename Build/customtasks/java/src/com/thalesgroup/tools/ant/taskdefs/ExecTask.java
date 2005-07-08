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

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.taskdefs.Execute;
import org.apache.tools.ant.types.Environment;


/**
 * Extend the ANT "exec" task to add support for custom arguments.
 */
public class ExecTask extends org.apache.tools.ant.taskdefs.ExecTask {


    // Show command flag.
    private boolean m_showCmd = false;

    // The directory in which the command should be executed.
    private File m_dir = null;

    // The environment variables.
    private Environment m_env = new Environment();

    // Controls whether the VM (1.3 and above) is used to execute the command.
    private boolean m_vmLauncher = true;

    // The name of the executable program.
    private String m_executable = null;


    /**
     * Specify the show command flag value.
     *
     * @param showCmd The show command flag value.
     */
    public void setShowCmd(boolean showCmd) {
        m_showCmd = showCmd;
    }


    /**
     * Get the show command flag.
     *
     * @return The show command flag.
     */
    public boolean getShowCmd() {
        return m_showCmd;
    }


    /**
     * Specify the working directory of the process.
     *
     * @param dir The working directory.
     */
    public void setDir(File dir) {
        m_dir = dir;
        super.setDir(dir);
    }


    /**
     * Get the working directory.
     *
     * @return The working directory.
     */
    public File getDir() {
        return m_dir;
    }


    /**
     * Add an environment variable to the launched process.
     *
     * @param var An environment variable.
     */
    public void addEnv(Environment.Variable var) {
        m_env.addVariable(var);
    }


    /**
     * Get the environment variables.
     *
     * @return The environment variables.
     */
    public Environment getEnv() {
        return m_env;
    }


    /**
     * If true, launch new process with VM, otherwise use the OS's shell.
     *
     * @param vmLauncher The flag value.
     */
    public void setVMLauncher(boolean vmLauncher) {
        m_vmLauncher = vmLauncher;
    }


    /**
     * Get the VM launcher flag.
     *
     * @return The VM launcher flag.
     */
    public boolean getVMLauncher() {
        return m_vmLauncher;
    }


    /**
     * Set the name of the executable program.
     *
     * @param name The name of the executable program.
     */
    public void setExecutable(String name) {
        m_executable = name;
        cmdl.setExecutable(name);
    }


    /**
     * Get the name of the executable program.
     *
     * @return The name of the executable program.
     */
    public String getExecutable() {
        return m_executable;
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        // m_dir is possibly altered in prepareExec().
        File savedDir = m_dir;

        cmdl.setExecutable(resolveExecutable(m_executable, false));
        checkConfiguration();

        if (isValidOs()) {
            try {
                showCommand();
                runExec(prepareExec());
            } finally {
                m_dir = savedDir;
            }
        }
    }


    /**
     * Show the command.
     */
    protected void showCommand() {
        if (m_showCmd) {
            String[] cmdLine = cmdl.getCommandline();
            StringBuffer sb = new StringBuffer();

            for (int i = 0; i < cmdLine.length; i++) {
                sb.append(cmdLine[i]);
                sb.append(" ");
            }

            log(sb.toString(), Project.MSG_WARN);
        }
    }


    /**
     * Has the user set all necessary attributes ?
     */
    protected void checkConfiguration() throws BuildException {
        if (cmdl.getExecutable() == null) {
            throw new BuildException("no executable specified", getLocation());
        }

        if (m_dir != null && !m_dir.exists()) {
            throw new BuildException(
                "the directory you specified does not exist");
        }

        if (m_dir != null && !m_dir.isDirectory()) {
            throw new BuildException(
                "the directory you specified is not a directory");
        }

        super.checkConfiguration();
    }


    /**
     * Create an Execute instance with the correct working directory set.
     */
    protected Execute prepareExec() throws BuildException {
        // Default directory to the project's base directory
        if (m_dir == null) {
            m_dir = getProject().getBaseDir();
        }

        Execute exe = new Execute(createHandler(), createWatchdog());
        exe.setAntRun(getProject());
        exe.setWorkingDirectory(m_dir);
        exe.setVMLauncher(m_vmLauncher);

        String[] environment = m_env.getVariables();
        if (environment != null) {
            for (int i = 0; i < environment.length; i++) {
                log("setting environment variable: " + environment[i],
                    Project.MSG_VERBOSE);
            }
        }

        exe.setNewenvironment(newEnvironment);
        exe.setEnvironment(environment);

        return exe;
    }


}

