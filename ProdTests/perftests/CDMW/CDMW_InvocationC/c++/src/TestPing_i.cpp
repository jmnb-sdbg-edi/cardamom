/* ========================================================================== *
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
 * ========================================================================= */
 

#include <TestPing_i.h>

#include <time.h>

#include <iostream>
#include <fstream>
#include <string>

#include "ctools/time/HighResTimer.h"
#include "ctools/time/HighResTime.h"
#include "ctools/time/HighResClock.h"

namespace Invocation{

  TestPing_i::TestPing_i(){
    //array is set to null
    m_res = NULL;
  }

  TestPing_i::~TestPing_i(){
    //clening memory
    delete m_res;
  }

  //syncronous test
  void TestPing_i::ping() throw(CORBA::SystemException){}

  //asyncronous test
  void TestPing_i::init_sdump(CORBA::Long dim) 
    throw(CORBA::SystemException){
    //if array already exists
    if(m_res != NULL){
      delete m_res;
    }
    //array and variables inizialization
    m_res = new long double[dim];
    m_dim = dim;
    count = 0;
  }

  void TestPing_i::pos_ping_sdump(CORBA::Long pos) throw(CORBA::SystemException){
    //record of server time
    m_res[pos] = ctools::time::HighResClock::getTime().toMicrosec();
    count++;
  }

  void TestPing_i::save_sdump(const char* name) throw(CORBA::SystemException){
    //wait misures complete
    if(count != m_dim){
      //stop for 1 sec
      sleep(1);
    }
    //write file...

    //FILE * fp = fopen(name, "w");
    ofstream fp (name);
    fp.precision(6);

    for(int i = 0; i < m_dim; i++)
      fp<<m_res[i]<< std::endl;
    fp.close();
  }
}
