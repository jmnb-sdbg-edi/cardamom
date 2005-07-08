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


package cdmw.ccm.deployment.test;


import cdmw.ccm.deployment.FileUtilities;

public class FileUtilitiesTest extends cdmw.testutils.Testable {

    private static final String TMP_DIR_PREFIX  = "FileUtilitiesTest";
    private static final String TMP_CHILD_DIR_1 = "directory_1";
    private static final String TMP_FILENAME_1  = "tmp1.file";
    private static final String TMP_FILENAME_2  = "tmp2.file";
    
    private static final String DATA_PATH    = ".";
    private static final String ZIP_FILE     = "foo.zip";
    private static final String ZIP_FILE_2   = "foo2.zip";
    private static final String ENTRY_NAME   = "Foo.file";

    private static final String DEFAULT_FTP_DIR = "/home/ftp";
    
    
    private String ftpDirPath;

    public FileUtilitiesTest(String ftpDirPath)
    {
        this.out = System.out;
        this.name = "FileUtilities tests";
        if (ftpDirPath == null)
            this.ftpDirPath = DEFAULT_FTP_DIR;
        else 
            this.ftpDirPath = ftpDirPath;
       
    }


    public void doTests()
    {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(12);
        
        String tmpDirName = System.getProperty("java.io.tmpdir");
        java.io.File tmpDir = new java.io.File(tmpDirName);
        
        
        //
        // Test createTempDir()
        //
        println("Test createTempDir()");
        java.io.File testDir = null;
        try {
            testDir = FileUtilities.createTempDir(TMP_DIR_PREFIX);
            if (testDir.isDirectory() &&  testDir.getParentFile().equals(tmpDir)) {
                succeed();
            } else {
                fail("Creation of tmp dir " + testDir.toString() + " failed.");
            }
        } catch (java.io.IOException e) {
            fail("Creation of tmp dir with prefix " + TMP_DIR_PREFIX + " failed.");
            e.printStackTrace();
        }
        
        
        //
        // Test checkDirectory()
        //
        println("Test checkDirectory()");
        try {
            // check an existing directory
            FileUtilities.checkDirectory(testDir);
            succeed();
        } catch (java.io.IOException e) {
            fail("Checking of " + testDir.toString() + " failed.");
            e.printStackTrace();
        }
        
        java.io.File tmpDir1 = null;
        try {
            // check an unexisting directory
            tmpDir1 = FileUtilities.checkDirectory(testDir.toString() + 
                                                   java.io.File.separator +
                                                   TMP_CHILD_DIR_1);
            if (tmpDir1.isDirectory() && tmpDir1.getParentFile().equals(testDir)) {
                succeed();
            } else {
                fail("Creation of " + tmpDir1.toString() + 
                     " directory while checking failed.");
            }
        } catch (java.io.IOException e) {
            fail("Checking of " + testDir.toString() + 
                 java.io.File.separator + TMP_CHILD_DIR_1 + " failed.");
            e.printStackTrace();
        }
        
        java.io.File tmpFile1 = null;
        try {
            // check an existing directory which is a file
            tmpFile1 = new java.io.File(testDir, TMP_FILENAME_1);
            tmpFile1.createNewFile();
            FileUtilities.checkDirectory(tmpFile1);
            fail("Checking of directory " + tmpFile1 + 
                 " which is a file failed.");
        } catch (java.io.IOException e) {
            succeed();
        }
        
                
        
        //
        // Test moveFileTo()
        //
        println("Test moveFileTo()");
        java.io.File tmpFile2 = new java.io.File(tmpDir1, TMP_FILENAME_2);
        try {
            FileUtilities.moveFileTo(tmpFile1, tmpFile2);
            if (tmpFile2.isFile() && tmpFile2.getParentFile().equals(tmpDir1) &&
                !tmpFile1.exists()) 
            {
                succeed();
            } else {
                fail("Renaming of " + tmpFile1.toString() + " into " +
                     tmpFile2.toString() + " failed.");
            }
        } catch (java.io.IOException e) {
            fail("Renaming of " + tmpFile1.toString() + " into " +
                 tmpDir1.toString() + java.io.File.separator + TMP_FILENAME_2 + " failed.");
            e.printStackTrace();
        }
        
        try {
            // try to rename to an existing directory
            FileUtilities.moveFileTo(tmpFile2, tmpDir1);
            fail("Renaming of " + tmpFile2.toString() + " into " +
                 tmpDir1.toString() + " which is an existing directory succeed.");
        } catch (java.io.IOException e) {
            succeed();
        }
        
        //
        // Test if zip file is accessible
        //
        java.io.File zipFile = new java.io.File(DATA_PATH + java.io.File.separator + 
                                               ZIP_FILE);
        if (!zipFile.exists()) {
            fail("Cannot access to file: " + zipFile.toString() + " .\n" +
                 "Tests of FileUtilities cannot be continued !");
            return;
        }

        //
        // Test isZip()
        //
        println("Test isZip()");
        if (FileUtilities.isZip(zipFile.toString())) {
            succeed();
        } else {
            fail("Testing if " + zipFile.toString() + " is a zip failed.");
        }
        if (FileUtilities.isZip(tmpFile2.toString())) {
            fail("Testing if tmp file " + tmpFile2.toString() + " is a zip succeed.");
        } else {
            succeed();
        }
 

        //
        // Test copyFile()
        //
        println("Test copy of " + zipFile.toString());
        java.io.File zipCopy = new java.io.File(tmpDir1, ZIP_FILE);
        try {
            FileUtilities.copyFile(zipFile.toString(), zipCopy.toString());
            if (zipFile.isFile() && FileUtilities.isZip(zipFile.toString()) &&
                zipCopy.isFile() && FileUtilities.isZip(zipCopy.toString()))
            {
                succeed();
            } else {
                fail("Copy of " + zipFile.toString() + " to " + 
                     zipCopy.toString() + " failed.");
            }
        } catch (java.io.IOException e) {
            fail("Copy of " + zipFile.toString() + " to " + 
                 zipCopy.toString() + " failed.");
            e.printStackTrace();
        }            
    
        
        //
        // Test unzip
        //
        println ("Test unzip of file " + ENTRY_NAME + 
                 " from " + zipCopy.toString());
        java.io.File unzipedFile = new java.io.File(testDir,
                                                    ENTRY_NAME);
        try {
            FileUtilities.unzipFileAs(ENTRY_NAME, 
                                      zipCopy.toString(),
                                      unzipedFile.toString());
            if (unzipedFile.isFile()) {
                succeed();
            } else {
                fail("Unzip of " + ENTRY_NAME + " from " + zipCopy.toString() + 
                     " as " + unzipedFile.toString() + " failed.");
            }

        } catch (java.util.zip.ZipException e) {
            fail("Unzip of " + ENTRY_NAME + " from " + zipCopy.toString() + 
                 " as " + unzipedFile.toString() + " failed.");
            e.printStackTrace();
        } catch (java.io.IOException e) {
            fail("Unzip of " + ENTRY_NAME + " from " + zipCopy.toString() + 
                 " as " + unzipedFile.toString() + " failed.");
            e.printStackTrace();
        }


        //
        // Test if anonymous ftp account exist on localhost
        //
        boolean ftpAccessOK = false;
        try {
            java.net.URL url = new java.net.URL("ftp://localhost/");
            java.net.URLConnection connection = url.openConnection();
            connection.connect(); 
            
            ftpAccessOK = true;
        } catch (java.net.MalformedURLException e) {
            // shouldn't append, url is valid
            cdmw.common.Assert.check(false);
        } catch (java.io.IOException e) {
            fail("WARNING: cannot connect to \"ftp://localhost/\".\n" +
                 "The PackageManager of Cdmw CCM needs a ftp server " +
                 "on its host to run properly. This test needs an anonymous " +
                 "access for succeed.");
        }
        
        //
        // Test if 'ftpDirPath' is writable
        //
        println("Check if " + ftpDirPath + " is writable.");
        boolean ftpWriteOK = false;
        java.io.File ftpDir = new java.io.File(ftpDirPath);
        if (ftpDir.canWrite()) {
            ftpWriteOK = true;
        } else {
            fail("WARNING: cannot write into \"" + ftpDirPath + "\".\n" +
                 "The PackageManager of Cdmw CCM needs to have " +
                 " write access into a ftp account to run properly. " +
                 "This test needs to have write access to \"" + ftpDirPath + "\" to succeed");
        }
        
        // copy foo.zip into ftpDirPath/<tmp_dir>
        boolean copyIntoFtpOk = false;
        java.io.File tmpFtpDir = null;
        if (ftpAccessOK && ftpWriteOK) {
            try {
                // create tmp dir in FTP_DIR
                tmpFtpDir = FileUtilities.createTempDir("FileUtilitiesTest", ftpDir);
                
                // copy ZIP_FILE into tmpFtpDir
                java.io.File zipFtp = new java.io.File(tmpFtpDir, ZIP_FILE);
                FileUtilities.copyFile(zipFile.toString(), zipFtp.toString());
                copyIntoFtpOk = true;
            } catch (java.io.IOException e) {
                // shouldn't append
                fail("Unexpected error:");
                e.printStackTrace();
            }
        }
        
        //
        // Test downloadFile()
        //
        java.io.File zipDownload = new java.io.File(tmpDir1, ZIP_FILE_2);
        if (ftpAccessOK && ftpWriteOK && copyIntoFtpOk) {
            // retrieve zip url
            String zipUrl = "ftp://localhost/" + tmpFtpDir.getName() + "/" + ZIP_FILE;
            println("Test download of " + zipUrl);
            
            // test download
            try {
                FileUtilities.downloadFile(zipUrl, zipDownload.toString());
                if (zipDownload.isFile() && FileUtilities.isZip(zipDownload.toString())) {
                    succeed();
                } else {
                    fail("Download of " + zipUrl + " to " + 
                         zipDownload.toString() + " failed.");
                }
            } catch (cdmw.common.BadParameterException e) {
                fail(e.getMessage());
                e.printStackTrace(); 
            } catch (java.io.IOException e) {
                fail("Download of " + zipUrl + " to " + 
                     zipDownload.toString() + " failed.");
                e.printStackTrace();
            }
            
            // remove tmpFtpDir
            try {
                FileUtilities.removeDirectoryRecursively(tmpFtpDir);
            } catch (java.io.IOException e) {
                fail("Unexpected error:");
                e.printStackTrace();
            }            
        }

        
        //
        // Test removeDirectoryRecursively()
        //
        println("Test recursive remove of " + testDir.toString() + " directory.");
        try {
            FileUtilities.removeDirectoryRecursively(testDir);
            if (testDir.exists() ||
                tmpDir1.exists() ||
                tmpFile1.exists() ||
                tmpFile2.exists() ||
                zipDownload.exists() ||
                zipCopy.exists() ||
                unzipedFile.exists()) 
            {
                fail("Recursive remove of " + testDir.toString() + " directory failed.");
            } else {
                succeed();
            }
        } catch (java.io.IOException e) {
            fail("Recursive remove of " + testDir.toString() + " directory failed.");
            e.printStackTrace();
        }            
    }
    
    
    
    public static void main(String[] args) {        
        String ftpDirPath = null;
        
        // get path of FTP dir if specified
        if (args.length == 1)
            ftpDirPath = args[0];
        
        FileUtilitiesTest test = new FileUtilitiesTest(ftpDirPath);
        test.start();
    }
    
    
}






