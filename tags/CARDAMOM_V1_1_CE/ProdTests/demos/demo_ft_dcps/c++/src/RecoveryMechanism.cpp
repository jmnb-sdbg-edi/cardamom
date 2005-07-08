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

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>

RecoveryMechanism::RecoveryMechanism(int argc, char *argv[]) {

	     // Create a DomainParticipantFactory.
//    m_factory = new SpliceDomainParticipantFactory(argc, argv);
    
    // Create a domainParticipant with it.
//	DCPS::DomainParticipantQos dp_qos;
    m_participant = TheParticipantFactory->create_participant(NULL, PARTICIPANT_QOS_DEFAULT, NULL);
    if (!m_participant.in()) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to create a DomainParticipant" << std::endl;
		CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => DomainParticipant created."  << std::endl;

    /*// Enable the DomainParticipant.
	 DCPS::ReturnCode_t status = m_participant->enable();
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to enable the DomainParticipant" << std::endl;
		CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => DomainParticipant enabled." << std::endl;*/
    
    // Register the 'BasicTypes' type inside my DomainParticipant.
    const char *type_name = "BasicTypes";
	 DemoFTDCPS::BasicTypesTypeSupport myDataType;
    
    DDS::ReturnCode_t status;
  

    status = myDataType.register_type(m_participant.in(), type_name);
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to register the 'BasicTypes' type inside the DomainParticipant" << std::endl;
		CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => BasicTypesDataType registered." << std::endl;
    
    /*// Create a new Topic for the BasicTypes type.
	 DCPS::TopicQos tp_qos;
    tp_qos.durability.kind = VOLATILE_DURABILITY_QOS;
    tp_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    tp_qos.delay_laxity.duration.sec = 0;
    tp_qos.delay_laxity.duration.nanosec = 0;*/
    
    m_topic = m_participant->create_topic("myBasicTypes", type_name, TOPIC_QOS_DEFAULT, NULL);
    if (!m_topic.in()) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to create a new Topic for the BasicTypes type." << std::endl;
		 CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => Topic for BasicTypes created." << std::endl;
    
    /*// Enable the Topic.
    status = m_topic->enable();
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to enable Topic" << std::endl;
		 CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => Topic enabled." << std::endl;*/

    // Create a Subscriber QoS.
	 DDS::SubscriberQos sub_qos;

    // Use a default Partition.
    m_participant->get_default_subscriber_qos(sub_qos);
    sub_qos.partition.name.length(1);    
    sub_qos.partition.name[0] = "myPartition";
    std::cout << " RecoveryMechanism => Current Partition: '" << sub_qos.partition.name[0] << "'" << std::endl;
    
    m_subscriber = m_participant->create_subscriber(sub_qos, NULL);
    if (!m_subscriber.in()) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to create Subscriber" << std::endl;
	    CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => Subscriber created for above mentioned Partition." << std::endl;
    
    /*// Enable the Subscriber.
    status = m_subscriber->enable();
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to enable the Subscriber" << std::endl;
		 CDMW_ASSERT(false);		 
	 }
	 std::cout << " RecoveryMechanism => Subscriber enabled." << std::endl;*/

    // Create a BasicTypesDataReader.
	 //DCPS::DataReaderQos dr_qos;
    
    m_data_reader = m_subscriber->create_datareader(m_topic.in(), DATAREADER_QOS_DEFAULT, NULL);
    if (!m_data_reader.in()) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to create DataReader!" << std::endl;
	    CDMW_ASSERT(false);
	 }
	 std::cout << " RecoveryMechanism => DataReader created for type 'BasicTypes'." << std::endl;
    
    // Cast it to its original (typed) format.
    m_typed_data_reader = dynamic_cast<DemoFTDCPS::BasicTypesDataReader_ptr>(m_data_reader.in());
    if (m_typed_data_reader == NULL)
    {
		 std::cerr << "RecoveryMechanism => DataReader handle could not be casted back to its original type!!" << std::endl;
	    CDMW_ASSERT(false);
    }
    else
    {
		 std::cout << "RecoveryMechanism => DataReader handle successfuly casted to a BasicTypesDataReader handle." << std::endl;
    }
    
    /*// Enable the DataReader.
    status = m_typed_data_reader->enable();
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism => impossible to create DataReader!" << std::endl;
	    CDMW_ASSERT(false);
	 }
	 std::cout << " RecoveryMechanism => DataReader enabled." << std::endl;*/
}
        
RecoveryMechanism::~RecoveryMechanism() {

    DDS::ReturnCode_t status = m_subscriber->delete_datareader(m_typed_data_reader);
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism destructor => impossible to delete BasicTypesDataReader!" << std::endl;
	    CDMW_ASSERT(false);
	 }
	 std::cout << " RecoveryMechanism destructor => BasicTypesDataReader deleted!! " << std::endl;
    
    status = m_participant->delete_subscriber(m_subscriber.in());
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism destructor => impossible to delete Subscriber!" << std::endl;
	    CDMW_ASSERT(false);
	 }
    std::cout << " RecoveryMechanism destructor => Subscriber deleted !! " << status << std::endl;
    
    status = m_participant->delete_topic(m_topic.in());
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism destructor => impossible to delete Topic!" << std::endl;
	    CDMW_ASSERT(false);
	 }
    std::cout << "Topic deleted !!" << std::endl;
    
    status = TheParticipantFactory->delete_participant(m_participant.in());
    std::cout << "DomainParticipant deleted !!" << std::endl;
    if (status) 
	 {
		 std::cerr << " RecoveryMechanism destructor => impossible to delete Participant!" << std::endl;
	    CDMW_ASSERT(false);
	 }
    
    std::cout << " RecoveryMechanism destructor => DomainParticipantFactory deletes itself!!" << std::endl;
   
}

void 
RecoveryMechanism::update_state(HelloInterface_impl* hello) 
    throw (CORBA::SystemException,
           FT::InvalidState)
{
	 // Allocate holders for read results.
	 DemoFTDCPS::BasicTypesSeq_var samples = new DemoFTDCPS::BasicTypesSeq();
	 DDS::SampleInfoSeq_var sampleInfo = new DDS::SampleInfoSeq();
	 DDS::SampleStateMask ssm = DDS::NOT_READ_SAMPLE_STATE;
	 DDS::ViewStateMask vsm = DDS::ANY_VIEW_STATE;
	 DDS::InstanceStateMask ism = DDS::ALIVE_INSTANCE_STATE;

	 std::cout << " RecoveryMechanism => update_state try to read samples" << std::endl;
	 m_typed_data_reader->read(samples, sampleInfo, DDS::LENGTH_UNLIMITED, ssm, vsm, ism);
	 std::cout << " RecoveryMechanism => update_state samples read : samples length = "
		        << samples->length() << std::endl;

	 for (CORBA::ULong k = 0; k < samples->length(); ++k)
    {
		  std::cout << "    Read BasicTypes value nr " << k << " :" << std::endl;
        std::cout << "        myUlong     = " << samples[k].myUlong << std::endl;
		  std::cout << "        myString    = " << samples[k].myString << std::endl;
        std::cout << "        myString10  = " << samples[k].myString10 << std::endl;

    }

	 if (samples->length())
	 {
		  size_t state_size = sizeof(samples[samples->length()-1].myUlong) + ::strlen(samples[samples->length()-1].myString) + 1 + 10;
	     FT::State state(state_size);
        state.length(state_size);
		  std::cout << "sizeof ulong = " << sizeof(samples[samples->length()-1].myUlong) << std::endl;
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
        std::cout << "     . . . . RecoveryMechanism: no state to update ! " << std::endl;
	 }

	// Release the Samples and SampleInfo
	m_typed_data_reader->return_loan(samples, sampleInfo);
};



