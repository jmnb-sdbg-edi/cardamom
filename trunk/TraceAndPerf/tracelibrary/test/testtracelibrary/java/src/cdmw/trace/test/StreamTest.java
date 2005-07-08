/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.trace.test;


import cdmw.testutils.Testable;


class StreamTestThread extends Thread {

    private cdmw.trace.Stream myStream;

    public boolean isSameStream(cdmw.trace.Stream stream) {
        return myStream == stream;
    }

    public void run() {
        // We obtain a private stream and keeping its address
        //  to compare later
        myStream = cdmw.trace.Stream.getInstance();
    }


}





public class StreamTest extends Testable {

    public StreamTest() {
        this.out = System.out;
        this.name = "Stream tests";
    }


    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(3);

        println ("Trying to obtain the thread specific stream");
        cdmw.trace.Stream myStream = cdmw.trace.Stream.getInstance();
        succeed();

        println ("Trying to get a second time the stream");
        cdmw.trace.Stream myStream2 = cdmw.trace.Stream.getInstance();
        check (myStream == myStream2);

        println ("Creating a thread asking for another stream");
        StreamTestThread StreamTestThread =
            new StreamTestThread();
        StreamTestThread.start();

        println ("Checking the Stream allocated to the stream is");
        println (" different from our");
        check (!StreamTestThread.isSameStream(myStream));

        try {
            StreamTestThread.join();
        } catch (InterruptedException e) {
            // ignore
        }
    }



    public static void main(String[] args) {

        StreamTest test = new StreamTest();
        test.start();

    }


}


