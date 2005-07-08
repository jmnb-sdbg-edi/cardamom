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


package com.thalesgroup.tools.ant.listener;


import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringReader;

import java.util.Vector;

import org.apache.tools.ant.BuildEvent;
import org.apache.tools.ant.DefaultLogger;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.util.DateUtils;
import org.apache.tools.ant.util.StringUtils;


/**
 * Writes build events to a PrintStream.
 */
public class XDefaultLogger extends DefaultLogger {

    // Output stream file.
    protected PrintWriter output = null;

    // Output file.
    protected FileOutputStream fos = null;

    // Store runtest trace.
    private boolean storeRuntestTrace = false;

    // Store task trace.
    private boolean storeTaskTrace = false;

    // Names of the targets to log.
    protected Vector m_targetNames = new Vector();

    // Names of the tasks to log.
    protected Vector m_taskNames = new Vector();


    /**
     * Default constructor.
     */
    public XDefaultLogger() {
        m_targetNames.add("-exec-test");

        m_taskNames.add("echo");
        m_taskNames.add("exec");
        m_taskNames.add("xexec");
        m_taskNames.add("echoproperties");
    }


    /**
     * Log a message to say that the target has started.
     * 
     * @param event An event with any relevant extra information.
     *              Must not be <code>null</code>.
     */
    public void targetStarted(BuildEvent event) {
        if (m_targetNames.contains(event.getTarget().getName())) {
            storeRuntestTrace = true;

            try {
                String outFilename = 
                    event.getProject().getProperty("test.out");

                if (outFilename == null) {
                    outFilename = "runtest.log";
                }
                
                fos = new FileOutputStream(outFilename, false);
                output = new PrintWriter(fos);
            } catch(Exception e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Log a message to say that the target has finished.
     * 
     * @param event An event with any relevant extra information.
     *              Must not be <code>null</code>.
     */
    public void targetFinished(BuildEvent event) {
        if (m_targetNames.contains(event.getTarget().getName())) {
            storeRuntestTrace = false;

            try {
                output.flush();
                fos.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Log a message to say that the task has started.
     *
     * @param event An event with any relevant extra information.
     *              Must not be <code>null</code>.
     */
    public void taskStarted(BuildEvent event) {
        if (m_taskNames.contains(event.getTask().getTaskName())) {
            storeTaskTrace = true;
        }
    }


    /**
     * Log a message to say that the task has finished.
     *
     * @param event An event with any relevant extra information.
     *              Must not be <code>null</code>.
     */
    public void taskFinished(BuildEvent event) {
        if (m_taskNames.contains(event.getTask().getTaskName())) {
            storeTaskTrace = false;
        }
    }


    /**
     * Logs a message, if the priority is suitable.
     * In non-emacs mode, task level messages are prefixed by the
     * task name which is right-justified.
     * 
     * @param event A BuildEvent containing message information.
     *              Must not be <code>null</code>.
     */
    public void messageLogged(BuildEvent event) {
        String evtMsg = event.getMessage();
        int priority = event.getPriority();

        if ((evtMsg == null) ||
            evtMsg.trim().equals("") ||
            evtMsg.startsWith("Override")) {

            return;
        }


        // Filter out messages based on priority
        if ((priority <= msgOutputLevel) && storeTaskTrace) {
            StringBuffer message = new StringBuffer();

            if (event.getTask() != null && !emacsMode) {
                // Print out the name of the task if we're in one
                String name = event.getTask().getTaskName();
                String label = "[" + name + "] ";
                int size = LEFT_COLUMN_SIZE - label.length();
                StringBuffer tmp = new StringBuffer();

                for (int i = 0; i < size; i++) {
                    tmp.append(" ");
                }

                tmp.append(label);
                label = tmp.toString();

                try {
                    BufferedReader r = 
                        new BufferedReader(
                            new StringReader(event.getMessage()));
                    String line = r.readLine();
                    boolean first = true;

                    while (line != null) {
                        if (!first) {
                            message.append(StringUtils.LINE_SEP);
                        }

                        first = false;
                        message.append(label).append(line);
                        line = r.readLine();
                    }
                } catch (IOException e) {
                    // shouldn't be possible
                    message.append(label).append(event.getMessage());
                }
            } else {
                String s = event.getMessage();

                if (s.indexOf("CONDITION_IS") == -1) {
                    message.append(event.getMessage());
                }
            }

            String msg = message.toString();

            if (priority != Project.MSG_ERR) {
                printMessage(msg, out, priority);
            } else {
                printMessage(msg, err, priority);
            }

            log(msg);
            
            if (storeRuntestTrace) {
                output.println(msg);
                output.flush();
            }
        }
    }
}


