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


package cdmw.ossupport;

public class ThreadedProcess extends Thread {

    /**
     * Unknown Operating System
     */
    private final static int UNKNOWN = 0;
    
    /**
     * Sun Solaris Operating System
     */
    private final static int SUNOS = 1;
    
    /**
     * Linux Operating System
     */
    private final static int LINUX = 2;
    
    /**
     * Windows NT Operating System
     */
    private final static int WINNT = 3;

    /**
     * List of the Operating Systems
     */
    private final static String[] OS = {
        "Unknown",
        "SunOS",
        "Linux",
        "Windows NT"
    };

    /**
     * The encapsulated process object.
     */
    private Process p = null;

    /**
     * Shell command to be executed to run the new process
     */
    private String command = "";
    
    /**
     * List of environment variables to be set when starting the new process
     */
    private String[] envp = null;

    /**
     * Boolean variable indicating if the new process is started or not
     */
    private boolean started = false;
    
    /**
     * Current execution platform
     */
    private int platform;
    
    /**
     * Exception possibly thrown when trying to execute a command line 
     */
    private java.io.IOException ioexception = null;
    
    /**
     * Process creation based on the command parameter.
     *
     * @param command The command line to be executed 
     * in order to have the new process.
     */
    public ThreadedProcess(String command) {
        this.command = command;
        String os = System.getProperty("os.name");
        if ( os.equals(OS[SUNOS]) ) {
            platform = SUNOS;
        } else if ( os.equals(OS[LINUX]) ) {
            platform = LINUX;
        } else if ( os.equals(OS[WINNT]) ) {
            platform = WINNT;
        } else {
            platform = UNKNOWN;
        }

    }
    
    /**
     * Process creation based on the command parameter
     * and specified environment settings.
     *
     * @param command The command line to be executed 
     * in order to have the new process.
     * @param env The environment settings 
     */
    public ThreadedProcess(String command, String[] env) {
        this(command);
        this.envp = env;
    }

    /**
     * Format the command to execute the process in background.
     *
     * @return The formatted command line.
     */
    private String getShellCmd() {
        switch(platform) {
            case UNKNOWN:
                return command;
            case SUNOS :
                return command;
            case LINUX :
                return command;
            case WINNT :
                return "cmd.exe /C \"start " + command + "\"";
            default:
                return command;
        }
    }

    /**
     * Returns the encapsulated process object.
     *
     * @return The encapsulated process object.
     */
    public Process getProcess() {
        return this.p;
    }
    
    public java.io.IOException getIOException() {
        return this.ioexception;
    }

    /**
     * Returns the current execution platform.
     *
     * @return The current execution platform.
     */
    public String getPlatform() {
        return OS[platform];
    }

    /**
     * Set environment variables.
     *
     * @param envp Array of variables to be set.
     */
    public void setEnvp(String[] envp) {
        this.envp = envp;
    }

    /**
     * Check if the process is started.
     *
     * @return The process if started or not.
     */
    public boolean isStarted() {
        return started;
    }

    /**
     * Start the newly created process
     */
    public void run() {

        started = false;
        try {
            if (envp!=null) {
                this.p = Runtime.getRuntime().exec(getShellCmd(), envp);
            } else {
                this.p = Runtime.getRuntime().exec(getShellCmd());
            }
        } catch(java.io.IOException ioe)  {
            this.ioexception = ioe;
            // unable to throw this exception because of overriding run method
            // IOException will be thrown by the createProcess methods
            // in the OS class in case of a null Process
        }
        started = true;

    }

    /**
     * Kill the current process.
     */
    public void killProcess() {
        if (p!=null) {
            getProcess().destroy();
        }
    }

    /**
     * Wait for the process to die.
     *
     * @return Exit status of the process.
     */
    public int waitProcess() throws InterruptedException {

        if (p!=null) {
            // mandatory to avoid deadlock on waitFor
            StreamGobbler errGobbler = new StreamGobbler(getErrorStream());
            StreamGobbler outGobbler = new StreamGobbler(getInputStream());
            // empty the streams simultaneously
            errGobbler.start();
            outGobbler.start();
            // wait for the process termination
            getProcess().waitFor();
            // wait for the threads to die
            errGobbler.join();
            outGobbler.join();
            // return the exit value
            return getProcess().exitValue();
        } else {
            return -1;
        }
        
    }
    
    /**
     * Returns the input stream of the subprocess. The stream obtains 
     * data piped from the standard output stream of the process represented 
     * by this Process object.
     *
     * @return The input stream of the subprocess
     */
    public java.io.InputStream getInputStream() {
        
        if (p != null) {
            return getProcess().getInputStream();
        } else return null;
        
    }
    
    /**
     * Returns the error stream of the subprocess. The stream obtains 
     * data piped from the standard error stream of the process represented 
     * by this Process object.
     *
     * @return The error stream of the subprocess
     */
    public java.io.InputStream getErrorStream() {
        
        if (p != null) {
            return getProcess().getErrorStream();
        } else return null;
        
    }
    
    /**
     * This class allow to empty the streams
     * of a process object to wait for its termination
     *
     */
    private class StreamGobbler extends Thread {

        /**
         * Stream to be emptied
         */
        private java.io.InputStream is;

        /**
         * Constructor of a streamgobbler
         *
         * @param is input stream of bytes from a process object
         */
        public StreamGobbler(java.io.InputStream is) {
            this.is = is;
        }

        /**
         * Empty the stream specified during the creation
         */
        public void run() {
            try {
                while ( is.read() != -1) {
                    // do nothing
                }
            } catch (java.io.IOException ioe) {
                ioe.printStackTrace();
            }
        }
    }

}

