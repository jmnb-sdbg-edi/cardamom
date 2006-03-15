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


package com.thalesgroup.tools.ant.listener;


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;

import org.apache.tools.ant.BuildEvent;
import org.apache.tools.ant.DefaultLogger;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.util.StringUtils;


public class CompilationLogger extends DefaultLogger {


    // Log file.
    private BufferedWriter m_writer = null;


    /**
     * Constructor.
     */
    public CompilationLogger() {
        super();

        try {
            String tmpDir = System.getProperty("java.io.tmpdir");
            String userName = System.getProperty("user.name");
            String FS = System.getProperty("file.separator");
            String logfile = tmpDir + FS + userName + "_compilation_" +
                System.currentTimeMillis() + ".log";

            m_writer = new BufferedWriter(new FileWriter(logfile));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public void targetStarted(BuildEvent event) {
        // Unlike DefaultLogger, CompilationLogger does not print target names.
    }


    public void buildFinished(BuildEvent event) {
        super.buildFinished(event);

        if (m_writer != null) {
            try {
                m_writer.close();
            } catch (IOException e) {
                // Ignore, it should not happen.
            }
        }
    }


    public void messageLogged(BuildEvent event) {
        String evtMsg = event.getMessage();
        int priority = event.getPriority();

        if ((evtMsg == null) ||
            evtMsg.trim().equals("") ||
            evtMsg.startsWith("Override")) {

            return;
        }

        // Filter out messages based on priority.
        if (priority <= Project.MSG_INFO) {
            StringBuffer message = new StringBuffer();

            String name = "";
            if (event.getTask() != null) {
                name = event.getTask().getTaskName();
            }

            if ((priority == Project.MSG_INFO) && !name.equals("java")) {
                return;
            }

            if (event.getTask() != null && !emacsMode) {
                // Print out the name of the task if we're in one
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
                message.append(event.getMessage());
            }

            String msg = message.toString();
            if (priority != Project.MSG_ERR) {
                printMessage(msg, out, priority);
            } else {
                printMessage(msg, err, priority);
            }
            log(msg);
        }
    }


    /**
     * Log the message into a file.
     *
     * @param msg The message to log.
     */
    protected void log(String message) {
        if (m_writer != null) {
            try {
                m_writer.write(message + System.getProperty("line.separator"));
            } catch (IOException e) {
                // Ignore, it should not happen.
            }
        }
    }

}
