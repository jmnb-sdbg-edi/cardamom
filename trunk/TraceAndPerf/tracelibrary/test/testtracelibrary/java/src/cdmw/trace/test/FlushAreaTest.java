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


package cdmw.trace.test;


import cdmw.testutils.Testable;


public class FlushAreaTest extends Testable {
    
     public FlushAreaTest() {
        this.out = System.out;
        this.name = "FlushArea tests";
    }
   

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(11);
    	
        int status;
        int areaSize;
                        
        String message1 = "First Message";
        String message2 = "Second Message";
        String message3 = "Third Message";
                   
        String outFlushAreaMsg = "trace Flush Are Manager is running out of flush areas";

        
        // constructs a message header
        cdmw.trace.MessageHeader header = 
            new cdmw.trace.MessageHeader(Thread.currentThread().getName(),
                                          "", // ECR-0123
                                          "TEST",
                                          (short)1,
                                          cdmw.ossupport.OS.getTime());
        int headerStringSize = header.threadId.length() + 
                               header.fileName.length() +
                               header.userDomain.length() +
                               header.componentName.length(); // ECR-0123
                              
        println ("Testing the number of messages limit for a flush area");
        cdmw.trace.FlushArea flushArea1 = 
            new cdmw.trace.FlushArea(2, 200);
        status = flushArea1.addMessage(header, message1);
        check(status == cdmw.trace.FlushArea.OK);
        status = flushArea1.addMessage (header, message2);
        check(status == cdmw.trace.FlushArea.AREA_FULL);
        flushArea1.storeOutFlushAreaMsg(outFlushAreaMsg);
        succeed();        

        println("Testing the size limit for a flush area");
        areaSize = message1.length() + message2.length();
        areaSize += 2 * headerStringSize;  // Adds 2 header size stored in area
        cdmw.trace.FlushArea flushArea2 = 
            new cdmw.trace.FlushArea(3, areaSize);
        status = flushArea2.addMessage (header, message1);
        check(status == cdmw.trace.FlushArea.OK);
        status = flushArea2.addMessage (header, message2);
        check(status == cdmw.trace.FlushArea.AREA_FULL);
        flushArea2.storeOutFlushAreaMsg (outFlushAreaMsg); 
        succeed();
        
        println("Trying to exceed the size limit for a flush area");
        areaSize = message1.length() + message2.length();
        areaSize += 2 * headerStringSize;  // Adds 2 header size stored in area
        areaSize--;
        cdmw.trace.FlushArea flushArea3 = 
            new cdmw.trace.FlushArea(3, areaSize);
        status = flushArea3.addMessage (header, message1);
        check(status == cdmw.trace.FlushArea.OK);
        status = flushArea3.addMessage (header, message2);
        check(status == cdmw.trace.FlushArea.SIZE_TOO_SMALL);
        flushArea3.storeOutFlushAreaMsg (outFlushAreaMsg); 
        succeed();
        
        println("Trying to add a message too big for a flush area");
        areaSize = message3.length();
        areaSize += headerStringSize;
        areaSize--;
        cdmw.trace.FlushArea flushArea4 = 
            new cdmw.trace.FlushArea(3, areaSize);
        status = flushArea4.addMessage (header, message3);
        check(status == cdmw.trace.FlushArea.MESSAGE_TOO_BIG);
        
        println ("Testing reset of flush area");
        flushArea1.reset();
        check(flushArea1.isEmpty());

    }




    public static void main(String[] args) {
        FlushAreaTest test = new FlushAreaTest();
        test.start();
    }

    
}




  

