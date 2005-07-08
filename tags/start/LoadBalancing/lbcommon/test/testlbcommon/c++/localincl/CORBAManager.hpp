/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */
#ifndef CORBAMANAGER_H
#define CORBAMANAGER_H

#include <Foundation/orbsupport/CORBA.hpp>
#include <stdexcept>


/**
 * @brief
 *
 * @author Rafaella Panella <fmorciano@amsjv.it>
 */

/**
 *  @brief Singleton for CORBA initialization management.
 */
class CORBAManager
{
public:
    //! Exception classes
    class InvalidObject   : public std::logic_error
    {
    public:
        InvalidObject()
            :std::logic_error("Not valid object!.")
            {
            }
    };
        CORBAManager();
        ~CORBAManager();
  
        void  init(CORBA::ORB_ptr );
        CORBA::ORB_var    get_orb();
        
        static  CORBAManager* instance();
        
    private:
 
        bool  is_initialized_; 
        
        CORBA::ORB_var    orb_;
    };
#endif // CORBAMANAGER_H
