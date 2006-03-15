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


package cdmw.ccm.deployment.test;


import cdmw.ccm.deployment.FileReference;
import org.omg.Components.Deployment.InstallationFailure;



public class FileReferenceTest 
    extends cdmw.testutils.Testable 
{

    private static final String TMP_DIR_PREFFIX = "FileReferencesTest";
    private static final String FILE_REFERENCE_LOCATION = "./fileReference.txt";
    private static final String FILE_COPY_NAME = "file.test";
    private static final String FOO_LINK = "ftp://fooHost/file.test";
    private static final String TMP_DIR_2 = "testDir";


    public FileReferenceTest()
    {
       this.out = System.out;
       this.name = "FileReferences tests";
    }


    public void doTests() {
        

        setNbOfRequestedTestOK(3);

        java.io.File tmpDir = null;
        java.io.File fileCopy = null;
        try {
            tmpDir = cdmw.ccm.deployment.FileUtilities.createTempDir(TMP_DIR_PREFFIX);
            fileCopy = new java.io.File(tmpDir, FILE_COPY_NAME);
            cdmw.ccm.deployment.FileUtilities.copyFile(FILE_REFERENCE_LOCATION,
                                                           fileCopy.toString());
            
        } catch (java.io.IOException e) {
            fail("Error in tests preparation. Aborting tests for FileReferences.");
            e.printStackTrace();
            return;
        }
    
        //
        // Test FileReference
        //
        println("Test setActualLocation()");
        FileReference fileRef = null;
        try {
            // test FileReference as a Link (FileReference is abstract)
            fileRef = new cdmw.ccm.deployment.Link(FOO_LINK);
            fileRef.setActualLocation(fileCopy);
            succeed();
        } catch (InstallationFailure e) {
            fail("Creation of FileReference for " + fileCopy.toString() + " failed.");
            e.printStackTrace();
        } catch (cdmw.common.BadParameterException e) {
            fail("Creation of Link " + FOO_LINK + " failed.");
            e.printStackTrace();
        }
        
        
        // test getActualLocation()
        println("Test getActualLocation()");
        try {
            String location = fileRef.getActualLocation();
            if (fileCopy.equals(new java.io.File(location))) {
                succeed();
            } else {
                fail("Newly created FileReference has wrong location: " +
                     fileRef.getActualLocation());
            }
        } catch (InstallationFailure e) {
            fail("Unable to have actual location of FileReference.");
            e.printStackTrace();
        }
        
        
        // test moveFileTo() with a directory as destination
        println("Test moveFileTo()");
        try {
            java.io.File tmpDir2 = new java.io.File(tmpDir, TMP_DIR_2);
            tmpDir2.mkdir();
            
            try {
                fileRef.moveFileTo(tmpDir2);

                java.io.File movedFile = new java.io.File(tmpDir2, FILE_COPY_NAME);
                if (movedFile.equals(new java.io.File(fileRef.getActualLocation()))) {
                    succeed();
                } else {
                    fail("Moving FileReference " + fileRef.getActualLocation() +
                        " as " + movedFile.toString() + " failed.");
                }

            } catch (java.io.IOException e) {
                 fail("Moving FileReference " + fileRef.getActualLocation() +
                      " into " + tmpDir.toString() + java.io.File.separator + 
                      FILE_COPY_NAME + " failed.");
                 e.printStackTrace();
            }
            
        } catch (InstallationFailure e) {
            fail("Unable to have actual location of FileReference.");
            e.printStackTrace();
        }
        
        
        //
        // remove tmpDir
        //
        try {
            cdmw.ccm.deployment.FileUtilities.removeDirectoryRecursively(tmpDir);
        } catch (java.io.IOException e) {
            fail("Failed to remove tmp dir:" + tmpDir.toString());
            e.printStackTrace();
        }

    }
    
    
    
    public static void main(String[] args) {
        FileReferenceTest test = new FileReferenceTest();
        test.start();
    }



}

