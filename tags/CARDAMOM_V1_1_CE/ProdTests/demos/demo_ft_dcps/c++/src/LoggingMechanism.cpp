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


#include <LoggingMechanism.hpp>

#include <iostream>
//#include <dcps/c++/dds_string.h>

#include <Foundation/common/Assert.hpp>
    

LoggingMechanism::LoggingMechanism(int argc, char *argv[]) {
	// Create a DomainParticipantFactory.
	std::cout << "LoggingMechanism => create a SpliceDomainParticipantFactory" << std::endl;
/*	m_factory = new DCPS::SpliceDomainParticipantFactory(argc, argv);

	if (!m_factory.in())
	{
		std::cerr << "LoggingMechanism => impossible to create a SpliceDomainParticipantFactory" << std::endl;
		CDMW_ASSERT(false);
	}
	std::cout << "LoggingMechanism => SpliceDomainParticipantFactory created!" << std::endl;
*/    
   // Create a domainParticipant with it.
//    DCPS::DomainParticipantQos dp_qos;
//	m_participant = m_factory->create_participant(0, dp_qos, NULL);
	m_participant = TheParticipantFactory->create_participant(0, PARTICIPANT_QOS_DEFAULT, NULL);


	if (!m_participant.in())
	{
		std::cerr << "LoggingMechanism => impossible to create a DomainParticipant" << std::endl;
		CDMW_ASSERT(false);
	}
	std::cout << "LoggingMechanism => DomainParticipantcreated!" << std::endl;

/*	 // Enable the DomainParticipant.
	 std::cout << "LoggingMechanism => Th_createRegistrar ok!" << std::endl;
	 DCPS::ReturnCode_t status = m_participant->enable();
    if (status) 
	 {
		std::cerr << "LoggingMechanism => impossible to enable the DomainParticipant" << std::endl;
		CDMW_ASSERT(false);		 
	 }
    std::cout << "LoggingMechanism => DomainParticipant enabled." << std::endl; */

	 // Register the 'BasicTypes' type inside my DomainParticipant.
    const char *type_name = "BasicTypes";
    //DemoFTDCPS::BasicTypesDataType myDataType;
    DemoFTDCPS::BasicTypesTypeSupport myDataType;
    DDS::ReturnCode_t status;
    status = myDataType.register_type(m_participant.in(), type_name);
    if (status)
	 {
		std::cerr << "LoggingMechanism => impossible to register the 'BasicTypes' type inside my DomainParticipant" << std::endl;
		CDMW_ASSERT(false);		 
	 }
	 std::cout << "LoggingMechanism => BasicTypesDataType registered." << std::endl;

	 // Create a new Topic for the BasicTypes type.
/*    TopicQos tp_qos;
    tp_qos.durability.kind = VOLATILE_DURABILITY_QOS;
    tp_qos.reliability.kind = BEST_EFFORT_RELIABILITY_QOS;
    tp_qos.delay_laxity.duration.sec = 0;
    tp_qos.delay_laxity.duration.nanosec = 0;*/
    
    m_topic = m_participant->create_topic("myBasicTypes", type_name, TOPIC_QOS_DEFAULT, NULL);
    if (!m_topic.in()) 
	 {
		std::cerr << "LoggingMechanism => impossible to create the topic for the 'BasicTypes' type" << std::endl;
		CDMW_ASSERT(false);		
	 }
	 std::cout << "LoggingMechanism => Topic for BasicTypes created." << std::endl;

	 /*// Enable the Topic.
    status = m_topic->enable();
    if (status) 
	 {
		std::cerr << "LoggingMechanism => impossible to enable the topic for the 'BasicTypes' type" << std::endl;
		CDMW_ASSERT(false);	
	 }
	 std::cout << "LoggingMechanism => Topic enabled." << std::endl;*/

    // Create a Publisher QoS.
    // use a default partition.
	 DDS::PublisherQos pub_qos;

    // Added:
    m_participant->get_default_publisher_qos(pub_qos);
    pub_qos.partition.name.length(1);
    pub_qos.partition.name[0] = "myPartition";
        
    // Create a Publisher in this Partition.
    m_publisher = m_participant->create_publisher(pub_qos, NULL);
	 if (!m_publisher.in()) 
	 {
		std::cerr << "LoggingMechanism => impossible to create a Publisher in partition" << std::endl;
		CDMW_ASSERT(false);	
	 }
	 std::cout << "LoggingMechanism => Publisher created for Partition '" << pub_qos.partition.name[0] << std::endl;

	 /*// Enable the Publisher.
    status = m_publisher->enable();
    if (status) 
	 {
		std::cerr << "LoggingMechanism => impossible to enable the Publisher" << std::endl;
		CDMW_ASSERT(false);	
	 }
	 std::cout << "LoggingMechanism => Publisher enabled." << std::endl;*/

    // Create a BasicTypesDataWriter.
	 //DCPS::DataWriterQos dw_qos;
    
    m_data_writer = m_publisher->create_datawriter(m_topic.in(), DATAWRITER_QOS_DEFAULT, NULL);
	 std::cout << "LoggingMechanism => DataWriter created for type 'BasicTypes'." << std::endl;
    if (!m_data_writer) 
	 {
		std::cerr << "LoggingMechanism => impossible to create a DataWriter for type 'BasicTypes" << std::endl;
		CDMW_ASSERT(false);	
	 }
    
    // Cast it to its original (typed) format.
    
    m_typed_data_writer = dynamic_cast<DemoFTDCPS::BasicTypesDataWriter_ptr>(m_data_writer);
    if (m_typed_data_writer.in() == NULL)
    {
		 std::cerr << "LoggingMechanism => DataWriter handle could not be casted back to its original type!!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    else
    {
		 std::cout << "LoggingMechanism => DataWriter handle successfuly casted to a BasicTypesDataWriter handle." << std::endl;
    }
    
    /*// Enable the DataWriter.
    status = m_typed_data_writer->enable();
    if (status) 
	 {
		  std::cerr << "LoggingMechanism => impossible to enable DataWriter !!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    std::cout << "LoggingMechanism => DataWriter enabled."  << std::endl;*/

}

LoggingMechanism::~LoggingMechanism()
{
	 /* Now start to destroy all entities. */
	DDS::ReturnCode_t status = m_publisher->delete_datawriter(m_typed_data_writer.in());
    if (status) 
	 {
		  std::cerr << "LoggingMechanism destructor => impossible to delete DataWriter !!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    std::cout << "BasicTypesDataWriter deleted!!" << std::endl;

    status = m_participant->delete_publisher(m_publisher.in());
    if (status) 
	 {
		  std::cerr << "LoggingMechanism destructor => impossible to delete DataWriter !!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    std::cout << "Publisher deleted!! " << std::endl;
    
    status = m_participant->delete_topic(m_topic.in());
    if (status) 
	 {
		  std::cerr << "LoggingMechanism destructor => impossible to delete DataWriter !!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    std::cout << "Topic deleted!!" << std::endl;
    
    status = TheParticipantFactory->delete_participant(m_participant.in());
    if (status) 
	 {
		  std::cerr << "LoggingMechanism destructor => impossible to delete DataWriter !!" << std::endl;
		  CDMW_ASSERT(false);	
    }
    std::cout << "DomainParticipant deleted!!" << std::endl;
    
    std::cout << "DomainParticipantFactory deletes itself!! No status available...." << std::endl;
}

void LoggingMechanism::log_state(const FT::State& state) 
{
    
    std::cout << "           . LoggingMechanism::log_state " << (unsigned long) state[0] << std::endl;
    
	 DemoFTDCPS::BasicTypes value;

	 // Fill value from state
	 value.myUlong = state[0];
	 value.myString = CORBA::string_dup("hello");
	 value.myString10 = CORBA::string_dup("thales.com");
	 
    DDS::ReturnCode_t status = m_typed_data_writer->write(value, DDS::HANDLE_NIL);
    if (status)     
	 {
		  std::cerr << "LoggingMechanism log_state => impossible to write value !!" << std::endl;
    }

}


