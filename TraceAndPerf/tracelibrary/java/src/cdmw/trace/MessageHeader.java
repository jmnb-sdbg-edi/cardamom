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


import cdmw.ossupport.Timeval;


/**
 * This class fully describes the origin of a message,
 * and it is use to prefix each message.
 * 
 * Features:
 * none
 *
 */
public class MessageHeader {


    /**
     * The name of the thread which has created the message.
     */
    public String threadId;

    /**
     * The line where the message has been produced.
     */
    public int fileLine;

    /**
     * The file where the message has been produced.
     */
    public String fileName;

    /**
     * The component name.
     */
    // ECR-0123
    public String componentName;

    /**
     * The user specified domain origin of the message.
     */
    public String userDomain;

    /**
     * The user specified level of the message.
     */
    public short userLevel;

    /**
     * The time when the trace has been produced.
     */
    public Timeval timeStamping;



    /**
     * Constructs a new message header.
     *
     * @param tid The name of the thread which has created the message.
     * @param fileLine The line where the message has been produced.
     * @param fileName The file where the message has been produced.
     * @param userDomain The user specified domain origin of the message.
     * @param userLevel The user specified level of the message.
     * @param timestamping The time when the trace has been produced.
     */
    public MessageHeader(String  threadId,
                         int     fileLine,
                         String  fileName,
                         String  userDomain,
                         short   userLevel,
                         Timeval timeStamping)
    {
        this.threadId = threadId;
        this.fileLine = fileLine;
        this.fileName = fileName;
        this.componentName = "";
        this.userDomain = userDomain;
        this.userLevel = userLevel;
        this.timeStamping = timeStamping;
    }


    /**
     * Constructs a new message header.
     *
     * @param tid The name of the thread which has created the message.
     * @param fileLine The line where the message has been produced.
     * @param fileName The file where the message has been produced.
     * @param componentName The component with which the domain is associated
     * @param userDomain The user specified domain origin of the message.
     * @param userLevel The user specified level of the message.
     * @param timestamping The time when the trace has been produced.
     */
    // ECR-0123
    public MessageHeader(String  threadId,
                         int     fileLine,
                         String  fileName,
                         String  componentName,
                         String  userDomain,
                         short   userLevel,
                         Timeval timeStamping)
    {
        this.threadId = threadId;
        this.fileLine = fileLine;
        this.fileName = fileName;
        this.componentName = componentName;
        this.userDomain = userDomain;
        this.userLevel = userLevel;
        this.timeStamping = timeStamping;
    }


    /**
     * Constructs a new message header with no fileLine and no fileName
     *
     * @param tid The name of the thread which has created the message.
     * @param fileLine The line where the message has been produced.
     * @param fileName The file where the message has been produced.
     * @param userDomain The user specified domain origin of the message.
     * @param userLevel The user specified level of the message.
     * @param timestamping The time when the trace has been produced.
     */
    public MessageHeader(String  threadId,
                         String  userDomain,
                         short   userLevel,
                         Timeval timeStamping)
    {
        this(threadId,
             0,
             "",
             userDomain,
             userLevel,
             timeStamping);
    }


    /**
     * Constructs a new message header with no fileLine and no fileName
     *
     * @param tid The name of the thread which has created the message.
     * @param fileLine The line where the message has been produced.
     * @param fileName The file where the message has been produced.
     * @param componentName The component with which the domain is associated
     * @param userDomain The user specified domain origin of the message.
     * @param userLevel The user specified level of the message.
     * @param timestamping The time when the trace has been produced.
     */
    // ECR-0123
    public MessageHeader(String  threadId,
                         String  componentName,
                         String  userDomain,
                         short   userLevel,
                         Timeval timeStamping)
    {
        this(threadId,
             0,
             "",
             componentName,
             userDomain,
             userLevel,
             timeStamping);
    }

}


