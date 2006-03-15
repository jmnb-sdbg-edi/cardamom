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


#include "RecoveryMechanism.hpp"
#include <iostream>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>

using namespace std;
using namespace DDS;
using namespace CORBA;
using namespace Basic;

RecoveryMechanism::RecoveryMechanism(int argc, char *argv[]) 
{
  dataList = new BasicTypeSeq;
  infoList = new SampleInfoSeq;
  
  dpf = DomainParticipantFactory::get_instance();

  //  dp = dpf->create_participant(myDomain, PARTICIPANT_QOS_DEFAULT, NULL);
  dp = dpf->create_participant(NULL, PARTICIPANT_QOS_DEFAULT, NULL);
  cout << " RecoveryMechanism => DomainParticipant created."  << endl;
  
  dt.register_type(dp, "Basic::BasicType");
  cout << " RecoveryMechanism => BasicTypesDataType registered." << endl;
  
  t = dp->create_topic("BasicTypeTopic", "Basic::BasicType", TOPIC_QOS_DEFAULT, NULL);
  cout << " RecoveryMechanism => Topic for BasicTypes created." << endl;
  
  SubscriberQos sQos = SUBSCRIBER_QOS_DEFAULT;
  sQos.partition.name.length(1);
  sQos.partition.name[0] = string_dup("BasicTypeSpace");
  cout << " RecoveryMechanism => Current Partition: '" << sQos.partition.name[0] << "'" << endl;
  
  s = dp->create_subscriber(sQos, NULL);
  cout << " RecoveryMechanism => Subscriber created for above mentioned Partition." << endl;
  
  dr = s->create_datareader(t, DATAREADER_QOS_DEFAULT, NULL);
  cout << " RecoveryMechanism => DataReader created for type 'BasicTypes'." << endl;
  
  fdr = BasicTypeDataReader::_narrow(dr);
  sleep(1); //wait for a message...
}
        
RecoveryMechanism::~RecoveryMechanism() 
{
    s?s->delete_datareader(dr):0;
    CORBA::release(fdr);
    dp?dp->delete_subscriber(s):0;
    dp?dp->delete_topic(t):0;
    dpf?dpf->delete_participant(dp):0;
}


void
RecoveryMechanism::update_state(HelloInterface_impl* hello)
    throw (CORBA::SystemException,
           FT::InvalidState)
{
         // Allocate holders for read results.
         Basic::BasicTypeSeq_var samples = new Basic::BasicTypeSeq();
         DDS::SampleInfoSeq_var sampleInfo = new DDS::SampleInfoSeq();
         DDS::SampleStateMask ssm = DDS::NOT_READ_SAMPLE_STATE;
         DDS::ViewStateMask vsm = DDS::ANY_VIEW_STATE;
         DDS::InstanceStateMask ism = DDS::ALIVE_INSTANCE_STATE;
         
         cout << " RecoveryMechanism => update_state try to read samples" << endl;
         fdr->read(samples, sampleInfo, DDS::LENGTH_UNLIMITED, ssm, vsm, ism);
         cout << " RecoveryMechanism => update_state samples read : samples length = "
                        << samples->length() << endl;
         
         for (CORBA::ULong k = 0; k < samples->length(); ++k)
    {
                  cout << "    Read BasicTypes value nr " << k << " :" << endl;
        cout << "        myUlong     = " << samples[k].myUlong << endl;
                  cout << "        myString    = " << samples[k].myString << endl;
        cout << "        myString10  = " << samples[k].myString10 << endl;
                                                                                                                                                                                                         
    }
                                                                                                                                                                                                         
         if (samples->length())
         {
           size_t state_size = sizeof(samples[samples->length()-1].myUlong) + ::strlen(samples[samples->length()-1].myString) + 1 + 10;
           FT::State state(state_size);
           state.length(state_size); 
           cout << "sizeof ulong = " << sizeof(samples[samples->length()-1].myUlong) << endl;
           state[0] = samples[samples->length()-1].myUlong;
           memcpy(state.get_buffer(), &(samples[samples->length()-1].myUlong), sizeof(samples[samples->length()-1].myUlong));
           memcpy(state.get_buffer() + sizeof(samples[samples->length()-1].myUlong),
                                      samples[samples->length()-1].myString,
                                      ::strlen(samples[samples->length()-1].myString) + 1 );
                                      memcpy(state.get_buffer() + sizeof(samples[samples->length()-1].myUlong) + ::strlen(samples[samples->length()-1].myString) + 1,
                                  samples[samples->length()-1].myString10,
                                  10);
                                                                                                                                                                                                         
        // set current state to Hello servant
        hello->set_state(state);
         }
         else
         {
        cout << "     . . . . RecoveryMechanism: no state to update ! " << endl;
         }
                                                                                                                                                                                                         
        // Release the Samples and SampleInfo
         fdr->return_loan(dataList.inout(), infoList.inout());
}

