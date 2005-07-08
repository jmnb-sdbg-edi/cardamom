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


package cdmw.ossupport.test;

import java.io.PrintStream;
import cdmw.testutils.Testable;
import cdmw.ossupport.OS;

public class ReadWriteLockThread extends Thread {

    private int nbReaders = 0;
    private int nbWriters = 0;
    private PrintStream out;

    public ReadWriteLockThread(PrintStream o) {
        this.out = o;
    }

    public void run() {

        int i = 0;
        // 4 is for manipulate seconds,
        //    and 15 seconds is about the live time of each
        //    thread (NB_LOCKS * sleep( 0..3s)

        try {

            while(i < (4*15) ) {
                synchronized(this) {
                    out.print(".");
                    out.flush();
                    checkInvariant();
                }
                OS.sleep(Testable.getTimescale() * 250);
                i++;
            }
            out.println();

        } catch(InterruptedException ie) {
        } catch(InvariantException e) {
            out.println("Error: " + e.getMessage());
            e.printStackTrace(out);
            System.exit(0);
        }

    }

    public synchronized void incrementReader() {
        nbReaders++;
    }

    public synchronized void decrementReader() {
        nbReaders--;
    }

    public synchronized void incrementWriter() {
        nbWriters++;
    }

    public synchronized void decrementWriter() {
        nbWriters--;
    }

    public synchronized int getNbReaders() {
        return nbReaders;
    }

    public synchronized int getNbWriters() {
        return nbWriters;
    }

    public void checkInvariant()
        throws InvariantException {

        // We can not have reader if writer are inside
        if (nbWriters != 0 && nbReaders != 0) {
            throw new InvariantException(" Readers present with writers");
        }

        if ( nbReaders < 0 ) {
            throw new InvariantException(" Negative count of readers");
        }

        if ( nbWriters < 0 ) {
            throw new InvariantException(" Negative count of writers");
        }

        // We cannot have more than one writer
        if ( nbWriters > 1 ) {
            throw new InvariantException(" More than one writer");
        }

    }

}

class InvariantException extends Exception {

    InvariantException(String s) {
        super(s);
    }

}

