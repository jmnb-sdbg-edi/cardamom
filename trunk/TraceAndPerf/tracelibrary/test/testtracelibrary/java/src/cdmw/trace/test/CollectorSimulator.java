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



class CollectorSimulator extends com.thalesgroup.CdmwTrace.CollectorPOA {
    
       
    private com.thalesgroup.CdmwTrace.MessagesHeader messagesHeader;
        
    private java.util.Vector messages;
    
    
    public CollectorSimulator() {
        messages = new java.util.Vector();
    }
    
    
    public synchronized void 
    put_formatted_messages(com.thalesgroup.CdmwTrace.MessagesHeader the_messages_header,
                           com.thalesgroup.CdmwTrace.FormattedMessage[] the_messages)
    {
        // The header is simply replaced
        messagesHeader = the_messages_header;
                
        // The messages are appended
        if (the_messages != null) {
            for(int i = 0; i < the_messages.length; i++) {
                messages.add(the_messages[i]); 
            }
        }
                
    }    
    
    public void update(com.thalesgroup.CdmwTrace.TraceProducer traceProducer) {
    }
    
    public com.thalesgroup.CdmwTrace.Trace_Filter[] get_trace_filters() {
        return null;
    }
    
    public void activate_level(String componentName, // ECR-0123
                               String domain,
                               short value)
    {
    }
    
    public void deactivate_level(String componentName, // ECR-0123
                                 String domain,
                                 short value)
    {
    }
    
    public com.thalesgroup.CdmwTrace.MessagesHeader getMessagesHeader() {
        return messagesHeader;   
    }
    
    public synchronized com.thalesgroup.CdmwTrace.FormattedMessage[] getFormattedMessageSeq() {
        com.thalesgroup.CdmwTrace.FormattedMessage[] ret = 
            new com.thalesgroup.CdmwTrace.FormattedMessage[messages.size()];

        for (int i = 0; i < messages.size(); i++) {
            ret[i] = (com.thalesgroup.CdmwTrace.FormattedMessage) messages.get(i);
        }
        
        return ret;
    }
    
}  

