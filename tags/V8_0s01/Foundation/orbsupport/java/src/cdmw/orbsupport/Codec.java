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


// PCR-0049


package cdmw.orbsupport;


import cdmw.common.Assert;


public class Codec
{
    // reference to the ORB
    private static org.omg.CORBA.ORB M_ORB;


    /**
     * Set the reference of the ORB.
     */
    public static void init(final org.omg.CORBA.ORB orb) {
        Assert.check(orb != null);
        M_ORB = orb;
    }


    /**
     * Constructor.
     */
    public Codec() {
        Assert.check(M_ORB != null);
    }


    /**
     * ValueType to CDR encoder.
     *
     * @param value ValueType to encode
     * @return An Any that holds the OctetSeq.
     */
    public org.omg.CORBA.Any
    encode(final org.omg.CORBA.portable.ValueBase value)
    {
        org.omg.CORBA.Any data = null;

        try {
            org.jacorb.orb.CDROutputStream out =
                new org.jacorb.orb.CDROutputStream(M_ORB);
            // set endian flag
            out.beginEncapsulatedArray();
            // encode
            out.write_value((java.io.Serializable) value);
            data = org.omg.CORBA.ORB.init().create_any();
            org.omg.CORBA.OctetSeqHelper.insert(data, out.getBufferCopy());
        } catch (Exception e) {
            System.err.println(e.getMessage());
            e.printStackTrace(System.err);
        }

        return data;
    }


    /**
     * Extracts an OctetSeq from an Any and returns a CDR input stream.
     *
     * @param data The Any that holds the OctetSeq.
     * @return the CDR input stream.
     */
    public org.omg.CORBA.portable.InputStream
    decode(final org.omg.CORBA.Any data)
    {
        org.jacorb.orb.CDRInputStream in = null;

        try {
            byte[] buf = org.omg.CORBA.OctetSeqHelper.extract(data);
            in = new org.jacorb.orb.CDRInputStream(M_ORB, buf);
            // set endian flag
            in.openEncapsulatedArray();
        } catch (Exception e) {
            System.err.println(e.getMessage());
            e.printStackTrace(System.err);
        }

        return in;
    }
}
