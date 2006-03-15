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


#ifndef INCL_PLATFORMMNGT_USERCOMMANDCALLBACK_HPP 
#define INCL_PLATFORMMNGT_USERCOMMANDCALLBACK_HPP 

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommandBase.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
* Purpose:
* <p>
* The Base class for the command callback.
*/
class UserCommandCallbackBase
{
  public:
  
    /**
    * Purpose:
    * <p>
    * the callback execute method.
    *
    * @param command_args the arguments of the command
    * @param result_info info about result of command
    *
    * @return failed or ok
    */
    virtual CdmwPlatformMngt::CommandResponse execute (
                          CdmwPlatformMngt::CmdParameterValues command_args,
                          CORBA::String_out *result_info) = 0;
    
  protected:
    
    /**
    * Purpose:
    * <p>
    * the callback constructor.
    *
    */
    UserCommandCallbackBase () {}
};


/**
* Purpose:
* <p>
* The Template class for the command callback.
*  the user callback instantiate this template with its specific class type
*  and with the method pointer to call when callback is executed
*/
template <class ClassType>
class UserCommandCallback : public UserCommandCallbackBase
{

   public:
   
   
    typedef CdmwPlatformMngt::CommandResponse (ClassType::*Method) (
                                               CdmwPlatformMngt::CmdParameterValues,
                                               CORBA::String_out *); 


    /**
    * Purpose:
    * <p>
    * the callback constructor.
    *
    * @param p_class_instance the pointer to user callback class instance.
    * @param p_method the method pointer called by execute calling.
    *                 the method type must be :
    *                 CdmwPlatformMngt::CommandResponse method (
    *                                         CdmwPlatformMngt::CmdParameterValues,
    *                                         CORBA::String_out *);
    *                 
    *
    */    
    UserCommandCallback(ClassType* p_class_instance, Method p_method)
    {
       m_pClass_instance = p_class_instance;
       m_pMethod         = p_method;
    };

    /**
    * Purpose:
    * <p>
    * the callback execute method.
    *
    * @param command_args the arguments of the command
    * @param result_info info about result of command
    *
    * @return failed or ok
    */
    CdmwPlatformMngt::CommandResponse execute (CdmwPlatformMngt::CmdParameterValues command_args,
                                               CORBA::String_out *result_info)
    {
        CdmwPlatformMngt::CommandResponse response = CdmwPlatformMngt::FAILED;
        
        if(m_pClass_instance != 0 && m_pMethod != 0) 
        { 
            try
            {
                response = (m_pClass_instance->*m_pMethod) (command_args, result_info);
            }
            catch(...)
            {
                // ... whatever
            }
        }
        
        return response;
    }


   private:
      
      /**
      * The pointer to user callback class instance.
      */
      ClassType*  m_pClass_instance;
      
      /**
      * The pointer to user callback execute method.
      */
      Method      m_pMethod;

};




} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_USERCOMMANDCALLBACK_HPP
