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


package cdmw.trace;

import cdmw.ossupport.OS;


/**
* This class is used to construct message in one thread.
* When the message has been fully built, a call to do_flush(),
* will transfer its content into another memory to be flushed out
* to each collectors interested in it.
* This class is a singleton for each thread.
* 
* Features:
* Singleton for each Thread.
* 
*/
public class Stream {
        
    /**
     * Line number where the trace begun
     */
    private int line;
    
    /**
     * File name where the trace has been produced
     */            
    private String file;
 
    /**
     * User domain as specified
     */
    private String userDomain;
 
    /**
     * User level as specified
     */    
    private short userLevel;
    
    /**
     * Message to be traced
     */    
    private String message;
    

    /**
     * Singleton instance of stream; local to each threads
     */
    private static final ThreadLocal perThreadInstance =
        new ThreadLocal() {
            // initialize ThreadLocal's object
            public Object initialValue() { return new Stream(); }
        };
    


    /**
     * Return the singleton instance of Stream for current thread
     *
     * @return the singleton instance of Stream for current thread.
     */
    public static Stream getInstance() {
        return (Stream)perThreadInstance.get();
    }
    
    
    

    /**
     * Set the line origin of the trace
     * 
     * @param line the line number
     */ 
    public void setLine(int line) {
        this.line = line;
    }
    
        
    /**
     * Set the file origin of the trace
     * 
     * @param file the file name
     */ 
    public void setFile(String file) {
        this.file = file;
    }

        
    /**
     * Set the user domain from where the trace comes from
     * 
     * @param domain the user domain
     */ 
    public void setUserDomain(String domain) {
        this.userDomain = domain;
    }

        
    /**
     * Set the user level from where the trace comes from
     * 
     * @param level the user level
     */ 
    public void setUserLevel(short level) {
        this.userLevel = level;   
    }


    /**
     * Set the message to be traced
     * 
     * @param message the message
     */ 
    public void setMessage(String message) {
        this.message = message;   
    }


    /**
     *  Transfer the internal message in another memory area
     *  to be flushed out to the Collectors. The stream is
     *  reset to "", so the same stream can be reused for 
     *  other outputs.
     */
    public void doFlush() {
        MessageHeader header = new MessageHeader(Thread.currentThread().getName(),
                                                 line,
                                                 file,
                                                 userDomain,
                                                 userLevel,
                                                 OS.getTime());
                                                 
        FlushAreaMngr.getInstance().addMessage(header, message);
        
        // We can reset the message
        message = "";        
    }    
    
   
}




  

