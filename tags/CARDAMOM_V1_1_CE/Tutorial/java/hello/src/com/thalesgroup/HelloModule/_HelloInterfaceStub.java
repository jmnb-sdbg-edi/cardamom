// **********************************************************************
//
// Generated by the ORBacus IDL to Java Translator
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

// Version: 4.1.0

package com.thalesgroup.HelloModule;

//
// IDL:thalesgroup.com/HelloModule/HelloInterface:1.0
//
public class _HelloInterfaceStub extends org.omg.CORBA.portable.ObjectImpl
                                 implements HelloInterface
{
    private static final String[] _ob_ids_ =
    {
        "IDL:thalesgroup.com/HelloModule/HelloInterface:1.0",
    };

    public String[]
    _ids()
    {
        return _ob_ids_;
    }

    final public static java.lang.Class _ob_opsClass = HelloInterfaceOperations.class;

    //
    // IDL:thalesgroup.com/HelloModule/HelloInterface/display_hello:1.0
    //
    public void
    display_hello()
    {
        while(true)
        {
            if(!this._is_local())
            {
                org.omg.CORBA.portable.OutputStream out = null;
                org.omg.CORBA.portable.InputStream in = null;
                try
                {
                    out = _request("display_hello", true);
                    in = _invoke(out);
                    return;
                }
                catch(org.omg.CORBA.portable.RemarshalException _ob_ex)
                {
                    continue;
                }
                catch(org.omg.CORBA.portable.ApplicationException _ob_aex)
                {
                    final String _ob_id = _ob_aex.getId();
                    in = _ob_aex.getInputStream();

                    throw new org.omg.CORBA.UNKNOWN("Unexpected User Exception: " + _ob_id);
                }
                finally
                {
                    _releaseReply(in);
                }
            }
            else
            {
                org.omg.CORBA.portable.ServantObject _ob_so = _servant_preinvoke("display_hello", _ob_opsClass);
                if(_ob_so == null)
                    continue;
                HelloInterfaceOperations _ob_self = (HelloInterfaceOperations)_ob_so.servant;
                try
                {
                    _ob_self.display_hello();
                    return;
                }
                finally
                {
                    _servant_postinvoke(_ob_so);
                }
            }
        }
    }
}