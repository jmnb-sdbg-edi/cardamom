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
 * This class holds one message created by developer
 * using the stream object.
 *
 * @see  FlushArea
 */
public class Message {

    /**
     * The header containing information about the origin of the message.
     */
    protected MessageHeader header;

    /**
     * The body of the message
     */
    protected String body;

    /**
     * Constructs a new empty message. (no header and no body)
     */
    public Message() {
        header = null;
        body = null;
    }

    /**
     * Return the header for this message
     *
     * @return the header
     */
    public MessageHeader getHeader() {
        return header;
    }

    /**
     * Return the body of this message
     *
     * @return the body
     */
    public String getBody() {
        return body;
    }

    /**
     * Fill the message with a new header and a new body.
     * The old values are released.
     *
     * @param header header describing the origin of the message
     * @param str message to be stored internally
     *
     * @exception AssertionFailedException
     */
    public void setMessage(MessageHeader header, String body) {
        this.header = header;
        this.body = body;
    }

    /**
     * Compare 2 MessageHeader.
     * The timestamping attribute is ignored.
     *
     * @param hdr1 the first header.
     * @param hdr2 the second header.
     *
     * @return true or false
     */
    public static boolean headerCmp(MessageHeader hdr1, MessageHeader hdr2) {
        return ((hdr1.threadId == hdr2.threadId) &&
                (hdr1.fileLine == hdr2.fileLine) &&
                (hdr1.fileName == hdr2.fileName) &&
                (hdr1.componentName == hdr2.componentName) && // ECR-0123
                (hdr1.userDomain == hdr2.userDomain) &&
                (hdr1.userLevel == hdr2.userLevel));
    }
}

