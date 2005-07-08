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



// This class only calls the other tests for ccmpackagemanager:
//  - FileReferenceTest
//  - FtpLocationFactoryTest

public class MainTest {

    public static void main(String[] args) {

        String ftpDirPath = null;
        // get path of FTP dir if specified
        if (args.length == 1)
            ftpDirPath = args[0];

        String[] noArgs = new String[0];
        
        FileReferenceTest.main(noArgs);
        FtpLocationFactoryTest.main(ftpDirPath);
    }    
}


