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


#ifndef INCL_TEST_CDMW_CCM_MESSAGES_IMPL_HPP
#define INCL_TEST_CDMW_CCM_MESSAGES_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include "testccmcontainer/hello.skel.hpp"

namespace TestCdmwCCM
{
    
    /**
    * Purpose:
    * <p> Implementation of a Message valuetype
    *
    */
    class Message_impl : public OBV_HelloModule::Message,
                         public virtual CORBA::DefaultValueRefCountBase
    {
    public:
        /**
        * Purpose:
        * <p> Constructor
        */
        Message_impl();
        
        /**
        * Purpose:
        * <p> Constructor
        *
        *@param the_name Name of the ConfigValue
        *@param the_value Value of the ConfigValue
        */
        Message_impl(const char* msg);
        
        /**
        * Purpose:
        * <p> Destructor
        *
        */
        ~Message_impl() throw();

        
        /**
        * Purpose:
        * <p> Clone current ConfigValue
        *
        *@exception A pointer to a clone of self
        */
        virtual CORBA::ValueBase* _copy_value();

    };

    /**
     * Implementation of a Message valuetype factory.
     */
    class MessageFactory : virtual public HelloModule::Message_init
    {
    public:
        CORBA::ValueBase * create_for_unmarshal ();
    };

     
    /**
    * Purpose:
    * <p> Implementation of a Message valuetype
    *
    */
    class TraceMessage_impl : public OBV_HelloModule::TraceMessage,
                              public virtual CORBA::DefaultValueRefCountBase
    {
    public:
        /**
        * Purpose:
        * <p> Constructor
        */
        TraceMessage_impl();
        
        /**
        * Purpose:
        * <p> Constructor
        *
        *@param the_name Name of the ConfigValue
        *@param the_value Value of the ConfigValue
        */
        TraceMessage_impl(const char* msg, CORBA::Long level);
        
        /**
        * Purpose:
        * <p> Destructor
        *
        */
        ~TraceMessage_impl() throw();

        
        /**
        * Purpose:
        * <p> Clone current ConfigValue
        *
        *@exception A pointer to a clone of self
        */
        virtual CORBA::ValueBase* _copy_value();

    };
    

    /**
     * Implementation of a Message valuetype factory.
     */
    class TraceMessageFactory : virtual public HelloModule::TraceMessage_init
    {
    public:
        CORBA::ValueBase * create_for_unmarshal ();
    };
    
}; // End namespace TestCdmwCCM

#endif // INCL_TEST_CDMW_CCM_MESSAGES_IMPL_HPP

