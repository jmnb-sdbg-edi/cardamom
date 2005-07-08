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


package cdmw.orbsupport.test;

import cdmw.ossupport.OS;

public class MainTest {

    public static void main(String[] args) {
try{
        // Are we started as the CORBA server ?
        if ( OS.getOptionValue( args, "--server").equals("yes") ) {

            ServerProcess serverp = new ServerProcess();
            try {
                serverp.start( args );
            }
            catch(org.omg.CORBA.UserException ue) {
                ue.printStackTrace();
            }

        // Else, we are the main test driver
        } else {

              ORBSupportTest ost = new ORBSupportTest();
              ost.start();

        }
}catch (Exception ex){
ex.printStackTrace();
}
     }

    public static void writeToFile(String ior,String filename) {

        java.io.File file = new java.io.File(filename);
        String s;

        try {
            java.io.PrintWriter out = new java.io.PrintWriter(
                new java.io.BufferedWriter( new java.io.FileWriter(file) ) );
            out.println(ior);
            out.close();
        } catch(java.io.IOException e) {}

    }

    public static String readFromFile(String file) {

        try {
            java.io.BufferedReader in = new java.io.BufferedReader(
                new java.io.FileReader(file) );
            return in.readLine();
        }
        catch(java.io.IOException ioe) {
            return "";
        }
    }

}

