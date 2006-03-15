/* =========================================================================== *
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
 * =========================================================================== */


#include "LoggingMechanism.hpp"
#include <iostream>
#include <Foundation/common/Assert.hpp>
    
LoggingMechanism::LoggingMechanism(int argc, char *argv[]) 
{
  // Create a DomainParticipantFactory.
  BasicTypeSeq_var dataList = new BasicTypeSeq;
  SampleInfoSeq_var infoList = new SampleInfoSeq;
  ReturnCode_t status;
  
  dpf = DomainParticipantFactory::get_instance();
  std::cout << "LoggingMechanism => create a SpliceDomainParticipantFactory" << std::endl;
  dp = dpf->create_participant(myDomain, PARTICIPANT_QOS_DEFAULT, NULL);

  if (dp == 0)
    std::cout << "LoggingMechanism => DomainParticipant is null "  << std::endl;
  else 
    std::cout << "LoggingMechanism => DomainParticipantcreated!"  << std::endl;

  status = dt.register_type(dp, "Basic::BasicType");
  std::cout << "LoggingMechanism => The Return code of register is " << status << std::endl;
  
  if (status)
  {
    std::cerr << "LoggingMechanism => impossible to register the 'BasicTypes' type inside my DomainParticipant" << std::endl;
    CDMW_ASSERT(false);
  }   
  std::cout << "LoggingMechanism => BasicTypesDataType registered." << std::endl;

  t = dp->create_topic("BasicTypeTopic", "Basic::BasicType", TOPIC_QOS_DEFAULT, NULL);
  std::cout << "LoggingMechanism => Topic for BasicTypes created." << std::endl;

  PublisherQos pQos = PUBLISHER_QOS_DEFAULT;
  pQos.partition.name.length(1);
  pQos.partition.name[0] = string_dup("BasicTypeSpace");
  p = dp->create_publisher(pQos, NULL);
  std::cout << "LoggingMechanism => Publisher created for Partition '" << pQos.partition.name[0] << std::endl;

  dw = p->create_datawriter(t, DATAWRITER_QOS_DEFAULT, NULL);
  
  std::cout << "LoggingMechanism => DataWriter created for type 'BasicTypes'." << std::endl;
  if (!dw) 
  {
    std::cerr << "LoggingMechanism => impossible to create a DataWriter for type 'BasicTypes" << std::endl;
    CDMW_ASSERT(false);	
  }

  fdw = BasicTypeDataWriter::_narrow(dw);
                                                                               
}


LoggingMechanism::~LoggingMechanism()
{
  /* Now start to destroy all entities. */
  p?p->delete_datawriter(dw):0;
  CORBA::release(fdw);
  dp?dp->delete_publisher(p):0;
  dp?dp->delete_topic(t):0;
  dpf?dpf->delete_participant(dp):0;
}


void LoggingMechanism::log_state(const FT::State& state) 
{
  std::cout << "           . LoggingMechanism::log_state " << (unsigned long) state[0] << std::endl;
  BasicType value;
  value.myUlong = state[0];
  value.myString = CORBA::string_dup("hello");
  value.myString10 = CORBA::string_dup("thales.com");
  DDS::ReturnCode_t status = fdw->write(value, DDS::HANDLE_NIL);
  if (status)
  {
    std::cerr << "LoggingMechanism log_state => impossible to write value !!" << std::endl;
  }
}    




