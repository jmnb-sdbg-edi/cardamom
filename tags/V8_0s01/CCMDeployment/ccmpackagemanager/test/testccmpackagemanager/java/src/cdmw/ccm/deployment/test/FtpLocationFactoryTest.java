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


import cdmw.ccm.deployment.FtpLocationFactory;
import com.thalesgroup.CdmwDeployment.LocationCreationException;


public class FtpLocationFactoryTest 
    extends cdmw.testutils.Testable 
{

    private static String FTP_ROOT_DIR;

    private static final String FTP_LOGIN    = "foo";    
    private static final String FTP_PASSWORD = "fooPass";
    private static final String FTP_HOST     = "myhost";
    
    private static String FILENAME1;

    private static final String FILENAME2 = "/foodir/test/file.txt";
    
    private static final String HOST = cdmw.ossupport.OS.getHostname();
    private static final String LOCATION1 = "ftp://" + HOST + "/test/file.txt";
    private static final String LOCATION2 = "ftp://foo:fooPass@" + HOST + "/test/file.txt";
    private static final String LOCATION3 = "ftp://foo:fooPass@" + FTP_HOST + "/test/file.txt";
    

    public FtpLocationFactoryTest(String ftpDirPath)
    {
       this.out = System.out;
       this.name = "FtpLocationFactory tests";
       this.FTP_ROOT_DIR = ftpDirPath;
       this.FILENAME1 = ftpDirPath + "/test/file.txt";

    }


    public void doTests() {
        
    setNbOfRequestedTestOK(4);

        //
        // Test FtpLocationFactory without host, login and password specified
        //
        FtpLocationFactory factory1 = 
            new FtpLocationFactory(FTP_ROOT_DIR);
        try {
            String location = factory1.filenameToLocation(FILENAME1);
            if (LOCATION1.equals(location)) {
                succeed();
            } else {
                fail("result location is " + location +
                     " instead of " + LOCATION1);
            }
            
        } catch (LocationCreationException e) {
            fail("LocationCreationException:");
            e.printStackTrace();
        }
        
        // test with a filename that is not in FTP_ROOT_DIR
        try {
            String location = factory1.filenameToLocation(FILENAME2);
            fail("filenameToLocation should have raised a LocationCreationException." +
                 "Instead, it returns " + location);
            
        } catch (LocationCreationException e) {
                succeed();
        }
        
        //
        // Test FtpLocationFactory without host specified
        //
        FtpLocationFactory factory2 = 
            new FtpLocationFactory(FTP_ROOT_DIR, FTP_LOGIN, FTP_PASSWORD);
        try {
            String location = factory2.filenameToLocation(FILENAME1);
            if (LOCATION2.equals(location)) {
                succeed();
            } else {
                fail("result location is " + location +
                     " instead of " + LOCATION2);
            }
            
        } catch (LocationCreationException e) {
            fail("LocationCreationException:");
            e.printStackTrace();
        }
        
        //
        // Test FtpLocationFactory with host specified
        //
        FtpLocationFactory factory3 = 
            new FtpLocationFactory(FTP_ROOT_DIR, FTP_LOGIN, FTP_PASSWORD, FTP_HOST);
        try {
            String location = factory3.filenameToLocation(FILENAME1);
            if (LOCATION3.equals(location)) {
                succeed();
            } else {
                fail("result location is " + location +
                     " instead of " + LOCATION3);
            }
            
        } catch (LocationCreationException e) {
            fail("LocationCreationException:");
            e.printStackTrace();
        }




    }


    public static void main(String ftpDirPath) {

        FtpLocationFactoryTest test = new FtpLocationFactoryTest(ftpDirPath);
        test.start();
    }


}



