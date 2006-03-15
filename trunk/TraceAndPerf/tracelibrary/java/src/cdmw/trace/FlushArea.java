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


package cdmw.trace;


/**
 * This class store an array of Message whose size has been
 * specified during initialisation. When full this area
 * return AREA_FULL when an allocation request is done. 
 * The FlushArea is able to transform all internally stored
 * message in CORBA structure to be sent to Collectors.
 * 
 * Features:
 * No Thread safe 
 *
 * @see FlushAreaMngr
 * @see Message
 *
 */
public class FlushArea {
 
    /**
     * Status of the flush area indicating that a message has been added
     * successfully and the area can receive other messages.
     */
    public static final int OK        = 0;

    /**
     * Status of the flush area indicating that a message has been added
     * successfully but the maximum number of messages has been reached.
     */
    public static final int AREA_FULL = 1;

    /**
     * Status of the flush area indicating that the message cannot 
     * be added to the area as the size of the message exceeds 
     * the available size of the area.
     */
    public static final int SIZE_TOO_SMALL = 2;    

    /**
     * Status of the flush area indicating that the message can never 
     * be added to the area as the size of the message exceeds 
     * the total size of the area.
     */
    public static final int MESSAGE_TOO_BIG = 3;
    
 
    /**
     * Container used to stored the created messages.
     */
    private Message[] messages;
    
    /**
     * Contains the index of the next available message in messages.
     */
    private int indexFreeMessage;
    
    /**
     * Current size of the flush area
     * (in number of characters for messages)
     */
    private int currentSize;

    /**
     * Maximum size of flush area 
     */
    private int maxSize;
        
    /**
     * Id of the flush area (usefull for debug)
     */
    private int id;
 
 
 
    /**
     * Constructs a FlushArea with a maximum size, 
     * a maximum number of messages and no id.
     */
    public FlushArea(int nbMessages, int sizeFlushArea) {
        maxSize = sizeFlushArea;
        currentSize = 0;
        
        // A supplementary message is allocated in the message container 
        // for out flush area error message
        messages = new Message[nbMessages+1];
        for (int i = 0; i < messages.length; i++) {
            messages[i] = new Message();    
        }
        
        indexFreeMessage = 0;
        this.id = 0;
    }
 
 
    /**
     * Constructs a FlushArea with a maximum size, 
     * a maximum number of messages and a specified id.
     */
    public FlushArea(int nbMessages, int sizeFlushArea, int id) {
        this(nbMessages, sizeFlushArea);
        this.id = id;
    }
    
    /**
     * Return the id of the FlushArea. 
     * <br>(usefull for debug)
     */
    public int getId() {
        return id;
    }
    

    /**
     * Adds the specified message to the flush area
     * 
     * @param header the header of the message
     * @param body   the body of the message
     *
     * @return the status of the area after the call (OK or AREA_FULL)
     *
     * @see FlushArea#OK
     * @see FlushArea#AREA_FULL
     */     
    public synchronized int addMessage(MessageHeader header, String body) {
        int messageSize = body.length() + header.threadId.length() +
                          header.fileName.length() + header.userDomain.length() +
                          header.componentName.length(); // ECR-0123
        
        // if message is greater than area buffer
        if (messageSize > maxSize) {

            return MESSAGE_TOO_BIG;
        }
        
        // Is there enough room from available area for user?
        if (messageSize + currentSize > maxSize) {
            return SIZE_TOO_SMALL;
        }
        
        messages[indexFreeMessage].setMessage(header, body);
        indexFreeMessage++;
        currentSize += messageSize;
        
        if ((indexFreeMessage >= messages.length-1) ||
            (currentSize == maxSize))
        {
            return AREA_FULL;
        }
        
        return OK;
    }
    

    /**
     * Returns true if the area is empty.
     */     
    public boolean isEmpty() {
        return indexFreeMessage == 0;
    }


    /**
     * Reset the internal set of the FlushArea, each message
     * stored are lost. This methods is intended to be used when 
     * all contained message have been stored elsewhere
     */ 
    public synchronized void reset() {
        indexFreeMessage = 0;
        currentSize = 0;
        
    }    
    
    
    /**
     * Create the header of one specific message. This
     * function is use internally to build the FormattedMessagesSeq
     * 
     * @param message the message whose header representation must be 
     *   transform into CORBA representation
     *
     * @return the CORBA header
     */ 
    private com.thalesgroup.CdmwTrace.MessageHeader 
    createMessageHeader(Message message) 
    {
        // Information about the user defined characteristic
        com.thalesgroup.CdmwTrace.Trace_Filter level = 
            new com.thalesgroup.CdmwTrace.Trace_Filter(
                    message.getHeader().userLevel,
                    message.getHeader().userDomain,
                    message.getHeader().componentName, // ECR-0123
                    true);
                                                 
        // When the message has been produced (stamping)
        com.thalesgroup.CdmwTrace.Timeval timeval = 
            new com.thalesgroup.CdmwTrace.Timeval(
                    message.getHeader().timeStamping.getSeconds(),
                    message.getHeader().timeStamping.getMicroseconds());
                     
        // Create MessageHeader                                 
        com.thalesgroup.CdmwTrace.MessageHeader messageHeader =
            new com.thalesgroup.CdmwTrace.MessageHeader(level,
                                         // TODO ECR 0050 (add threadName in MessageHeader)
                                         0,
                                         //message.getHeader().threadId,
                                         message.getHeader().fileLine,
                                         message.getHeader().fileName,
                                         timeval);
                           
        return messageHeader;
    }
    
    
    /**
     * Create the CORBA message from a cdmw.trace.Message.
     * This function is use internally to build 
     * the FormattedMessagesSeq
     * 
     * @param message the message to be transformed
     *  into CORBA representation
     *
     * @return the CORBA FormattedMessage
     */ 
    public com.thalesgroup.CdmwTrace.FormattedMessage
    createFormattedMessage(Message message) {
        // get header for current message
        com.thalesgroup.CdmwTrace.MessageHeader messageHeader = 
            createMessageHeader(message);
            
        // create FormattedMessage
        return new com.thalesgroup.CdmwTrace.FormattedMessage(
                        messageHeader,
                        message.getBody());
       
    }
    
    
    
    
    /**
     * Build from the internally stored messages, the
     * equivalent CORBA structure.
     * 
     * @return the CORBA sequence of massages
     */     
    public synchronized com.thalesgroup.CdmwTrace.FormattedMessage[] 
    createFormattedMessageSeq() {
        // Create a sequence with the size of number of messages actually stored
        com.thalesgroup.CdmwTrace.FormattedMessage[] messageSeq =
            new com.thalesgroup.CdmwTrace.FormattedMessage[indexFreeMessage];
            
        // fill the sequence            
        for (int i=0; i < messageSeq.length; i++) {
            // create FormattedMessage
            messageSeq[i] = createFormattedMessage(messages[i]);
                    
        }
        
        // return the sequence
        return messageSeq;
    }
    
    
    
    /**
     * Build from the internally stored messages, the
     * equivalent CORBA Header structure .
     * 
     * @return the CORBA MessagesHeader
     */    
    public com.thalesgroup.CdmwTrace.MessagesHeader createMessagesHeader() {
        com.thalesgroup.CdmwTrace.MessagesHeader messagesHeader = 
            new com.thalesgroup.CdmwTrace.MessagesHeader();
        
        // TODO
        messagesHeader.the_host_id = 0;
        
        // set hostname
        messagesHeader.the_host_name = cdmw.ossupport.OS.getHostname();

        // process id not available with Java
        messagesHeader.the_process_id = 0;
        
        // set application name
        messagesHeader.the_application_name = 
            FlushAreaMngr.getInstance().getApplicationName();
        
        // set process name
        messagesHeader.the_process_name = 
            FlushAreaMngr.getInstance().getProcessName();
        
        return messagesHeader;
    }
    
    
    /**
     * Store the specified out flush area error message
     * 
     * @param errmsg the body of the error message
     */     
    public synchronized void storeOutFlushAreaMsg(String errmsg) {
        MessageHeader messageHeader = 
            new MessageHeader(
                    Thread.currentThread().getName(),
                    com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value, // ECR-0123
                    com.thalesgroup.CdmwTrace.ALL_DOMAINS.value,
                    com.thalesgroup.CdmwTrace.ALL_VALUES.value,
                    cdmw.ossupport.OS.getTime());
        
        int messageSize = errmsg.length() + messageHeader.threadId.length() +
                          messageHeader.fileName.length() + 
                          messageHeader.userDomain.length() +
                          messageHeader.componentName.length(); // ECR-0123

        messages[indexFreeMessage].setMessage(messageHeader, errmsg);
        indexFreeMessage++;
        
        // Note: error messages doesn't count as user messages for size checking.
        //       Therefore, we doesn't update currentSize.
        
    }
    
    
    
}  

