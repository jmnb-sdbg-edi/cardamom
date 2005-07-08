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

#ifndef CORBAMANAGER_HPP
#define CORBAMANAGER_HPP
/**
 * @file
 * @brief 
 * 
 * @author Fabrizio Morciano <fmorciano@amjsv.it>
 * @author Lello Mele <lellomele@yahoo.com>
 */


#include "ace/pre.h"

#include "tao/ORB.h"

#include <stdexcept>
#include <map>
#include <string>
#include <sstream>
#include <vector>

/**
 *  @brief Singleton for CORBA initialization management.
 */
class CORBAManager
{
public:
  //! Exception classes
  class InvalidArgument : public std::logic_error
  {
    public:
      InvalidArgument()
      :std::logic_error("Not valid argument!.")
      {
      }
  };
  class InvalidIOR      : public std::logic_error
  {
    public:
      InvalidIOR()
      :std::logic_error("Not valid IOR!.")
      {
      }
  };
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
  
  void  init(int ac, char ** &av );
  CORBA::Object_ptr get_object( const std::string& name ) 
    throw (InvalidObject);  
  CORBA::ORB_var    get_orb();
  void  add(const std::string& service_alias );  
  void  activate_poa();

  static  CORBAManager* instance();
    
private:
  typedef std::map<std::string,CORBA::Object_ptr> db_type;
  typedef db_type::iterator db_iterator;  
 
  bool  is_initialized_; 
  std::map<std::string,CORBA::Object_ptr>  db_;
  CORBA::ORB_var    orb_;

  int   argc_;
  std::vector<char*> argv_;
};

#include "ace/post.h"

#endif // CORBAMANAGER_HPP
