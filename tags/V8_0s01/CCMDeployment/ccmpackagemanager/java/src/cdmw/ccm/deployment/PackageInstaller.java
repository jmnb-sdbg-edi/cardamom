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


package cdmw.ccm.deployment;

import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.ERROR_IN_ARCHIVE;

import cdmw.ccm.deployment.FileUtilities;

import com.thalesgroup.CdmwXML.XMLParser;


abstract class PackageInstaller {

    private static final String META_INF_DIR  = "meta-inf";


    protected java.io.File tmpDir;
    protected XMLParser parser;

    protected PackageImpl packImpl;
    protected boolean isAnalyzeDone;



    protected PackageInstaller(java.io.File tmpDir,
                               XMLParser parser)
        throws InstallationFailure
    {
        try {
            cdmw.ccm.deployment.FileUtilities.checkDirectory(tmpDir);
        } catch (java.io.IOException e) {
            throw new InstallationFailure(e.getMessage(), READ_WRITE_ERROR.value);
        }

        this.tmpDir = tmpDir;
        this.parser = parser;
        this.packImpl = null;
        this.isAnalyzeDone = false;
    }


    public abstract void analyze()
        throws InstallationFailure;



    protected void install(java.io.File installDir,
                           java.io.File codebaseDir)
        throws InstallationFailure
    {
        // check if package has been analyzed
        cdmw.common.Assert.check(isAnalyzeDone);

        try {
            FileUtilities.checkDirectory(installDir);
            FileUtilities.checkDirectory(codebaseDir);
            FileUtilities.checkDirectory(tmpDir);
        } catch (java.io.IOException e) {
            throw new InstallationFailure(e.getMessage(),
                                          READ_WRITE_ERROR.value);
        }

        // installation of all archives into tmp dir
        java.util.Iterator it = packImpl.getArchiveIterator();
        while (it.hasNext()) {
            Archive a = (Archive) it.next();

            if (!a.isInstalled()) {
                // if not installed, install it
                System.out.println("   download archive " + a.getSourceLink() +
                                   " ...");
                a.installInto(tmpDir);
            }
/*             else {
                // if installed but not in installDir, move it
                java.io.File parentDir =
                    (new java.io.File(a.getActualLocation())).getParentFile();
                if (!parentDir.equals(installDir)) {
                    try {
                        a.moveFileTo(installDir)
                    } catch (java.io.IOException e) {
                        throw new IntallationFailure(
                            "Unable to move " + f.getActualLocation() +
                            " to " + installDir + " :\n" + e.getMessage(),
                            READ_WRITE_ERROR.value);
                    }
                }
            }
*/
        }

        // installation of all filesinarchive into installDir
        it = packImpl.getFileInArchiveIterator();
        while (it.hasNext()) {
            FileInArchive f = (FileInArchive) it.next();

            if (!f.isInstalled()) {
                // if not installed, install it
                System.out.println("   install fileinarchive " + f.getArchiveEntry() +
                                   " ...");
                f.installInto(installDir);
            } else {
                // if installed but not in installDir, move it
                java.io.File parentDir =
                    (new java.io.File(f.getActualLocation())).getParentFile();
                if (!parentDir.equals(installDir)) {
                    System.out.println("   install fileinarchive " + f.getArchiveEntry() +
                                       " ...");
                    try {
                        f.moveFileTo(installDir);
                    } catch (java.io.IOException e) {
                        throw new InstallationFailure(
                            "Unable to move " + f.getActualLocation() +
                            " to " + installDir + " :\n" + e.getMessage(),
                            READ_WRITE_ERROR.value);
                    }

                }
            }
        }

        // installation of all links into installDir
        it = packImpl.getLinkIterator();
        while (it.hasNext()) {
            Link l = (Link) it.next();

            if (!l.isInstalled()) {
                // if not installed, install it
                System.out.println("   install link " + l.getSourceLink() +
                                   " ...");
                l.installInto(installDir);
            } else {
                // if installed but not in installDir, move it
                java.io.File parentDir =
                    (new java.io.File(l.getActualLocation())).getParentFile();
                if (!parentDir.equals(installDir)) {
                    System.out.println("   install link " + l.getSourceLink() +
                                       " ...");
                    try {
                        l.moveFileTo(installDir);
                    } catch (java.io.IOException e) {
                        throw new InstallationFailure(
                            "Unable to move " + l.getActualLocation() +
                            " to " + installDir + " :\n" + e.getMessage(),
                            READ_WRITE_ERROR.value);
                    }
                }
            }
        }

        // installation of all codebases
        it = packImpl.getUsedCodebaseIterator();
        while (it.hasNext()) {
            Codebase c = (Codebase) it.next();

            if (!c.isInstalled()) {
                // if not installed, install it
                System.out.println("   install codebase " + c.getSourceLink() +
                                   " ...");
                c.installInto(codebaseDir);
            } else {
                // if installed but not in codebaseDir, move it
                java.io.File parentDir =
                    (new java.io.File(c.getActualLocation())).getParentFile();
                if (!parentDir.equals(codebaseDir)) {
                    System.out.println("   install codebase " + c.getSourceLink() +
                                       " ...");
                    try {
                        c.moveFileTo(codebaseDir);
                    } catch (java.io.IOException e) {
                        throw new InstallationFailure(
                            "Unable to move " + c.getActualLocation() +
                            " to " + installDir + " :\n" + e.getMessage(),
                            READ_WRITE_ERROR.value);
                    }
                }
            }
        }

        // others files should have already been installed
    }




    public void remove() {
        // delete all archives which have been installed
        java.util.Iterator it = packImpl.getArchiveIterator();
        while (it.hasNext()) {
            Archive a = (Archive) it.next();
            a.delete();
        }
    }



    // extract from 'meta-inf' directory in archiveFile
    // a descriptor with the specified suffix
    public static FileInArchive extractDescriptorWithSuffix(String suffix,
                                                            Archive archive,
                                                            java.io.File destinationDir)
        throws InstallationFailure
    {
        cdmw.common.Assert.check(destinationDir.isDirectory());

        java.util.zip.ZipFile zip = null;
        try {
            zip = new java.util.zip.ZipFile(archive.getActualLocation());
        } catch (java.io.IOException e) {
            // shouldn't happen (archive already tested)
            cdmw.common.Assert.check(false);
        }

        // list archive
        java.util.Enumeration files = zip.entries();

        // find a descriptor file in meta-inf directory
        java.util.zip.ZipEntry descriptorEntry = null;
        int founded = 0;
        while (files.hasMoreElements()) {
            java.util.zip.ZipEntry entry = (java.util.zip.ZipEntry) files.nextElement();
            String entryName = entry.getName().toLowerCase();
            if (   (entryName.endsWith(suffix))
                && (entryName.startsWith(META_INF_DIR))
                && (entryName.lastIndexOf('/') == META_INF_DIR.length())
               )
            {
                founded++;
                descriptorEntry = entry;
            }
        }

        // no descriptor found
        if (founded == 0) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                "No " + suffix + " file in archive " +
                archive.getActualLocation() + ".",
                ERROR_IN_ARCHIVE.value);
        }

        // more than one descriptor found
        if (founded > 1) {
            throw new org.omg.Components.Deployment.InstallationFailure(
                "Several " + suffix + " file in archive " +
                archive.getActualLocation() + ".",
                ERROR_IN_ARCHIVE.value);
        }

        // create FileInArchive descriptor
        FileInArchive descriptor = new FileInArchive(descriptorEntry.getName(),
                                                     archive);

        // install descriptor into tmpDir
        descriptor.installInto(destinationDir);

        // close zipFile
        try {
            zip.close();
        } catch (java.io.IOException e) {
            // shouldn't happen (archive already tested)
            cdmw.common.Assert.check(false);
        }

        return descriptor;
    }




}


