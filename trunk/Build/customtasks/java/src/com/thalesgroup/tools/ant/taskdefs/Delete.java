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

import java.util.Iterator;
import java.util.Vector;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.types.DirSet;
import org.apache.tools.ant.types.FileSet;


/**
 * Extend the ANT "delete" task to add support for DirSets.
 */
public class Delete extends org.apache.tools.ant.taskdefs.Delete {


    private static final int DELETE_RETRY_SLEEP_MILLIS = 10;

    // Verbosity flag.
    private int m_verbosity = Project.MSG_VERBOSE;

    // Quiet flag.
    private boolean m_quiet = false;

    // FailOnError flag.
    private boolean m_failOnError = true;

    // The set of directories to be deleted.
    protected Vector m_dirsets = new Vector();


    /**
     * If <code>true</code>, list all names of deleted files or directories.
     *
     * @param verbose "<code>true</code>" or "<code>on</code>"
     */
    public void setVerbose(boolean verbose) {
        if (verbose) {
            m_verbosity = Project.MSG_INFO;
        } else {
            m_verbosity = Project.MSG_VERBOSE;
        }
    }


    /**
     * Return the verbosity flag.
     *
     * @return The verbosity flag.
     */
    public int getVerbose() {
        return m_verbosity;
    }


    /**
     * If <code>true</code> and the file or directory does not exist, do not
     * display a diagnostic message or modify the exit status to reflect an
     * error. This means that if a file or directory cannot be deleted, then no
     * error is reported. This setting emulates the -f option to the Unix
     * &quot;rm&quot; command. Default is false meaning things are
     * &quot;noisy&quot;
     *
     * @param quiet "<code>true</code>" or "<code>on</code>"
     */
    public void setQuiet(boolean quiet) {
        m_quiet = quiet;

        if (m_quiet) {
            m_failOnError = false;
        }
    }


    /**
     * Get the quiet flag.
     *
     * @return The quiet flag.
     */
    public boolean getQuiet() {
        return m_quiet;
    }


    /**
     * If <code>false</code>, note errors but continue.
     *
     * @param failonerror <code>true</code> or <code>false</code>
     */
    public void setFailOnError(boolean failonerror) {
        m_failOnError = failonerror;
    }


    /**
     * Get the failonerror flag.
     *
     * @return The failonerror flag.
     */
    public boolean getFailOnError() {
        return m_failOnError;
    }


    /**
     * Add a set of directories to be deleted.
     */
    public void addDirset(DirSet set) {
        m_dirsets.addElement(set);
    }


    /**
     * Run the task.
     *
     * @throws BuildException The exception raised during task execution.
     */
    public void execute() throws BuildException {
        if (usedMatchingTask) {
            log("DEPRECATED - Use of the implicit FileSet is deprecated.  "
                + "Use a nested fileset element instead.");
        }

        if (file == null && dir == null && (filesets.size() == 0) &&
            (m_dirsets.size() == 0)) {

            throw new BuildException(
                "at least one of the file or dir attributes, or a fileset " +
                "or dirset element, must be set.");
        }

        if (m_quiet && m_failOnError) {
            throw new BuildException("quiet and failonerror cannot both be "
                                     + "set to true", getLocation());
        }

        // Delete the single file.
        if (file != null) {
            if (file.exists()) {
                if (file.isDirectory()) {
                    log("directory " + file.getAbsolutePath() 
                        + " cannot be removed using the file attribute. "
                        + "use dir instead.");
                } else {
                    log("deleting: " + file.getAbsolutePath());

                    if (!delete(file)) {
                        String message =
                            "unable to delete file " + file.getAbsolutePath();
                        if (m_failOnError) {
                            throw new BuildException(message);
                        } else { 
                            log(message, m_quiet ? Project.MSG_VERBOSE 
                                                 : Project.MSG_WARN);
                        }
                    }
                }
            } else {
                log("could not find file " + file.getAbsolutePath() 
                    + " to delete.",
                    Project.MSG_VERBOSE);
            }
        }

        // Delete the directory.
        if (dir != null && dir.exists() && dir.isDirectory() && 
            !usedMatchingTask) {

            // If verbosity is MSG_VERBOSE, that mean we are doing regular 
            // logging (backwards as that sounds).  In that case, we want to
            // print one message about deleting the top of the directory tree.
            // Otherwise, the removeDir method will handle messages for _all_
            // directories.
            if (m_verbosity == Project.MSG_VERBOSE) {
                log("deleting directory " + dir.getAbsolutePath());
            }
            removeDir(dir);
        }

        // Delete the files in the filesets.
        Iterator iter = filesets.iterator();
        while (iter.hasNext()) {
            FileSet fs = (FileSet) iter.next();
            try {
                DirectoryScanner ds = fs.getDirectoryScanner(getProject());
                String[] files = ds.getIncludedFiles();
                String[] dirs = ds.getIncludedDirectories();

                removeFiles(fs.getDir(getProject()), files, dirs);
            } catch (BuildException be) {
                // Directory doesn't exist or is not readable.
                if (m_failOnError) {
                    throw be;
                } else {
                    log(be.getMessage(), 
                        m_quiet ? Project.MSG_VERBOSE : Project.MSG_WARN);
                }
            }
        }

        // Delete the files from the default fileset.
        if (usedMatchingTask && dir != null) {
            try {
                DirectoryScanner ds = super.getDirectoryScanner(dir);
                String[] files = ds.getIncludedFiles();
                String[] dirs = ds.getIncludedDirectories();

                removeFiles(dir, files, dirs);
            } catch (BuildException be) {
                // Directory doesn't exist or is not readable.
                if (m_failOnError) {
                    throw be;
                } else {
                    log(be.getMessage(), 
                        m_quiet ? Project.MSG_VERBOSE : Project.MSG_WARN);
                }
            }
        }

        // Delete the directories in the dirsets.
        iter = m_dirsets.iterator();
        while (iter.hasNext()) {
            DirSet ds = (DirSet) iter.next();
            try {
                File baseDir = ds.getDir(getProject());
                DirectoryScanner dsc = ds.getDirectoryScanner(getProject());
                String[] dirs = dsc.getIncludedDirectories();

                for (int i = 0; i < dirs.length; i++) {
                    File dir = new File(baseDir, dirs[i]);
                    removeDir(dir);
                }
            } catch (BuildException be) {
                 // Directory doesn't exist or is not readable.
                if (m_failOnError) {
                    throw be;
                } else {
                    log(be.getMessage(), 
                        m_quiet ? Project.MSG_VERBOSE : Project.MSG_WARN);
                }
            }
        }
    }


    /**
     * Attempt to fix possible race condition when deleting files on WinXP. If
     * the delete does not work, wait a little and try again.
     *
     * @param f The file to be deleted.
     *
     * @return <code>false</code> if failed to delete,
     *         <code>true</code> otherwise.
     */
    private boolean delete(File f) {
        if (!f.delete()) {
            try {
                Thread.sleep(DELETE_RETRY_SLEEP_MILLIS);
                return f.delete();
            } catch (InterruptedException ex) {
                return f.delete();
            }
        }
        return true;
    }


}

