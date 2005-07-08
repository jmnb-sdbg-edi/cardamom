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


// CDMW Files
#include "Foundation/common/Assert.hpp"

#include <ServerExecutor_impl.hpp>

#include <DisplayFacetExecutor_impl.hpp>


namespace Hello
{
    
//
// IDL:acme.com/Hello/CCM_Server:1.0
//
ServerExecutor_impl::ServerExecutor_impl()
    : m_splice_port(0L),
      m_argv(0)
{
    m_name = "";
    m_executable_name = "";
    m_display = new DisplayFacetExecutor_impl (this);
}
    
ServerExecutor_impl::~ServerExecutor_impl()
{
    std::cout << "===> ServerExecutor_impl::~ServerExecutor_impl called!" << std::endl;
    delete [] m_argv;
    m_argv = 0;
}


//
// IDL:acme.com/Hello/CCM_Server_Executor/name:1.0
//
char* ServerExecutor_impl::name()
    throw(CORBA::SystemException)
{
    char* name = CORBA::string_dup(m_name.in());
    return name;
}

void ServerExecutor_impl::name(const char* a)
    throw(CORBA::SystemException)
{
    m_name = a;
}

//
// IDL:acme.com/Hello/CCM_Server_Executor/executable_name:1.0
//
char* ServerExecutor_impl::executable_name()
    throw(CORBA::SystemException)
{
    char* executable_name = CORBA::string_dup(m_executable_name.in());
    return executable_name;
}

void ServerExecutor_impl::executable_name(const char* a)
    throw(CORBA::SystemException)
{
    m_executable_name = a;
}

//
// IDL:acme.com/Hello/CCM_Server_Executor/splice_port:1.0
//
CORBA::Long ServerExecutor_impl::splice_port ()
    throw(CORBA::SystemException)
{
    return m_splice_port;
}

void ServerExecutor_impl::splice_port (CORBA::Long splice_port)
    throw(CORBA::SystemException)
{
    m_splice_port = splice_port;
}

//
// IDL:acme.com/Hello/CCM_Server/get_server_facet:1.0
//
CCM_Display_ptr ServerExecutor_impl::get_server_facet()
    throw(CORBA::SystemException)
{
    std::cout << "===> get_server_facet called!" << std::endl;
    
    return CCM_Display::_duplicate(m_display.in());
}


void ServerExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "===> set_session_context called!" << std::endl;
}
void ServerExecutor_impl::ccm_activate()
	throw(CORBA::SystemException)
{
    std::cout << m_name.in() << ":==> ccm_activate called!" << std::endl;
    // Create a DomainParticipantFactory.
    std::cout << m_name.in() << ": create a SpliceDomainParticipantFactory()" 
              << std::endl;
    
    // The following arguments are required by DCPS/Splice2
    int argc = 1;
    if (m_splice_port != 0L)
        ++argc;
    m_argv = new char*[argc+1];
    
    m_argv[0] = new char[strlen(m_executable_name.in()) + 1];
    strcpy(m_argv[0], m_executable_name.in());
    
    if (m_splice_port != 0L) {
        std::ostringstream ostr;
        ostr << "-Splice-port=" << m_splice_port;
        m_argv[1] = new char[ostr.str().length() + 1];
        strcpy(m_argv[1], ostr.str().c_str());
    }
    
    m_argv[argc]=NULL;
    
    m_factory = new DCPS::SpliceDomainParticipantFactory(argc, m_argv);
    
    if (!m_factory.in()) {
        std::cerr << m_name.in() 
                  << ": => impossible to create a SpliceDomainParticipantFactory"
                  << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() 
              << ":  => SpliceDomainParticipantFactory created!" << std::endl;
    
    // Create a domainParticipant with it.
    DCPS::DomainParticipantQos dp_qos;
    m_participant = m_factory->create_participant(0, dp_qos, NULL);
    
    if (!m_participant.in()) {
        std::cerr << m_name.in() 
                  << ": => impossible to create a DomainParticipant" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => DomainParticipantcreated!" << std::endl;
    
    // Enable the DomainParticipant.
    std::cout << m_name.in() 
              << ": => Th_createRegistrar ok!" << std::endl;
    DCPS::ReturnCode_t status = m_participant->enable();
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to enable the DomainParticipant" << std::endl;
        throw CORBA::INTERNAL(); 
    }
    std::cout << m_name.in() << ": => DomainParticipant enabled." << std::endl;
    
    // Register the 'ServerState' type inside my DomainParticipant.
    const char *type_name = "ServerState";
    DemoFTDCPS::ServerStateDataType myDataType;
    status = myDataType.register_type(m_participant.in(), type_name);
    if (status)  {
        std::cerr << m_name.in() 
                  << ": => impossible to register the 'ServerState' type "
                  << "inside my DomainParticipant" << std::endl;
        throw CORBA::INTERNAL(); 
    }
    std::cout << m_name.in() << ": => ServerStateDataType registered." << std::endl;
    
    // Create a new Topic for the BasicTypes type.
    TopicQos tp_qos;
    tp_qos.durability.kind = TRANSIENT_DURABILITY_QOS;
    tp_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;
    tp_qos.delay_laxity.duration.sec = 0;
    tp_qos.delay_laxity.duration.nanosec = 0;
    
    m_topic = m_participant->create_topic("myServerState", type_name, tp_qos, NULL);
    if (!m_topic.in()) {
        std::cerr << m_name.in() 
                  << ": => impossible to create the topic for the 'ServerState' type"
                  << std::endl;
        throw CORBA::INTERNAL(); 
    }
    std::cout << m_name.in() << ": => Topic for BasicTypes created." << std::endl;
    
    // Enable the Topic.
    status = m_topic->enable();
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to enable the topic for the 'ServerState' type" 
                  << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => Topic enabled." << std::endl;
    
    // Create a Publisher QoS.
    // use a default partition.
    DCPS::PublisherQos pub_qos;
    pub_qos.partition.name.length(1);
    pub_qos.partition.name[0] = "myPartition";
    
    // Create a Publisher in this Partition.
    m_publisher = m_participant->create_publisher(pub_qos, NULL);
    if (!m_publisher.in()) {
        std::cerr << m_name.in() << ": => impossible to create a Publisher in partition"
                  << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => Publisher created for Partition '" 
              << pub_qos.partition.name[0] << std::endl;
    
    // Enable the Publisher.
    status = m_publisher->enable();
    if (status) {
        std::cerr << m_name.in() << ": => impossible to enable the Publisher" 
                  << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => Publisher enabled." << std::endl;
    
    // Create a ServerStateDataWriter.
    DCPS::DataWriterQos dw_qos;
    
    m_data_writer = m_publisher->create_datawriter(m_topic, dw_qos, NULL);
    std::cout << m_name.in() << ": => DataWriter created for type 'ServerState'." 
              << std::endl;
    if (!m_data_writer.in()) {
        std::cerr << m_name.in()
                  << ": => impossible to create a DataWriter for type 'ServerState" 
                  << std::endl;
        throw CORBA::INTERNAL();
    }
    
    // Cast it to its original (typed) format.
    m_typed_data_writer = dynamic_cast<ServerStateDataWriter_ptr>(m_data_writer.in());
    if (m_typed_data_writer == NULL) {
        std::cerr << m_name.in()
                  << ": => DataWriter handle could not be casted back to its original"
                  << " type!!" << std::endl;
        throw CORBA::INTERNAL();
    } else {
        std::cout << m_name.in()
                  << ": => DataWriter handle successfuly casted to a "
                  << "ServerStateDataWriter handle." << std::endl;
    }
    
    // Enable the DataWriter.
    status = m_typed_data_writer->enable();
    if (status) {
        std::cerr <<  m_name.in() 
                  << ": => impossible to enable DataWriter !!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => DataWriter enabled."  << std::endl;
    
    // Create a Subscriber QoS.
    DCPS::SubscriberQos sub_qos;
    
    // Use a default Partition.
    sub_qos.partition.name.length(1);    
    sub_qos.partition.name[0] = "myPartition";
    std::cout << m_name.in() 
              << ": => Current Partition: '" << sub_qos.partition.name[0] << "'" 
              << std::endl;
    
    m_subscriber = m_participant->create_subscriber(sub_qos, NULL);
    if (!m_subscriber.in()) {
        std::cerr << m_name.in() << ": => impossible to create Subscriber" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() 
              << ": => Subscriber created for above mentioned Partition." << std::endl;
    
    // Enable the Subscriber.
    status = m_subscriber->enable();
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to enable the Subscriber" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => Subscriber enabled." << std::endl;
    
    // Create a BasicTypesDataReader.
    DCPS::DataReaderQos dr_qos;
    
    m_data_reader = m_subscriber->create_datareader(m_topic, dr_qos, NULL);
    if (!m_data_reader.in()) {
        std::cerr << m_name.in() 
                  << ": => impossible to create DataReader!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() 
              << ": => DataReader created for type 'ServerState'." << std::endl;
    
    // Cast it to its original (typed) format.
    m_typed_data_reader = dynamic_cast<DemoFTDCPS::ServerStateDataReader_ptr>(m_data_reader.in());
    if (m_typed_data_reader == NULL) {
        std::cerr << m_name.in() 
                  << ": => DataReader handle could not be casted back to its "
                  << "original type!!" << std::endl;
        throw CORBA::INTERNAL();
    } else {
        std::cout << m_name.in() 
                  << ": => DataReader handle successfuly casted to a "
                  << "ServerStateDataReader handle." << std::endl;
    }
    
    // Enable the DataReader.
    status = m_typed_data_reader->enable();
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to create DataReader!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => DataReader enabled." << std::endl;
    
}
void ServerExecutor_impl::ccm_passivate()
    throw(CORBA::SystemException)
{
    std::cout << m_name.in() 
              << ": ==> ccm_passivate called!" << std::endl;
    
    /* Now start to destroy all entities. */
    DCPS::ReturnCode_t status = m_subscriber->delete_datareader(m_typed_data_reader);
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to delete ServerStateDataReader!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => ServerStateDataReader deleted!! " << std::endl;
    
    status = m_participant->delete_subscriber(m_subscriber);
    if (status) {
        std::cerr << m_name.in() << ": => impossible to delete Subscriber!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": => Subscriber deleted !! " << status << std::endl;
    
    status = m_publisher->delete_datawriter(m_typed_data_writer);
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to delete DataWriter !!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": ServerStateDataWriter deleted!!" << std::endl;
    
    status = m_participant->delete_publisher(m_publisher);
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to delete DataWriter !!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": Publisher deleted!! " << std::endl;
    
    status = m_participant->delete_topic(m_topic);
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to delete DataWriter !!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": Topic deleted!!" << std::endl;
    
    status = m_factory->delete_participant(m_participant);
    if (status) {
        std::cerr << m_name.in() 
                  << ": => impossible to delete DataWriter !!" << std::endl;
        throw CORBA::INTERNAL();
    }
    std::cout << m_name.in() << ": DomainParticipant deleted!!" << std::endl;
    
    std::cout << m_name.in() 
              << ": DomainParticipantFactory deletes itself!! No status available...." 
              << std::endl;
}

void ServerExecutor_impl::ccm_remove()
    throw(CORBA::SystemException)
{
    std::cout << "===> ccm_remove called!" << std::endl;
}



// State Replication operations
DemoFTDCPS::ServerState ServerExecutor_impl::read_state()
    throw(CORBA::SystemException)
{
    // Allocate holders for read results.
    DemoFTDCPS::ServerStateSeq_var samples;
    DCPS::SampleInfoSeq_var sampleInfo;
    DCPS::SampleStateMask ssm = NOT_READ_SAMPLE_STATE;
    DCPS::LifecycleStateMask lcm = NEW_LIFECYCLE_STATE | MODIFIED_LIFECYCLE_STATE;
    
    std::cout << m_name.in() << ":  => read_state try to read samples" << std::endl;
    m_typed_data_reader->read(samples, sampleInfo, ssm, lcm);
    std::cout << m_name.in() << ": => read_state samples read : samples length = "
              << samples->length() << std::endl;
    
    for (DCPS::ULong k = 0; k < samples->length(); ++k)	{
        std::cout << m_name.in() 
                  << ":  Read ServerState value nr " << k << " :\n" 
                  << "        counter    = " << samples[k].counter << " \n"
                  << "        message    = " << samples[k].message
                  << std::endl;
    }
    DemoFTDCPS::ServerState result;
    
    if (samples->length()) {
        // Only return first state
        result.counter = samples[0L].counter;
        result.message = DDS::string_dup(samples[0L].message);
        std::cout << m_name.in() 
                  << ": Returning first read value " << std::endl;
    } else {
        result.counter = 0L;
        result.message = DDS::string_dup("NO MESSAGE");
        std::cout << m_name.in() << ": no state to update ! " 
                  << " Using default values!" << std::endl;
    }
    return result;
}
void ServerExecutor_impl::write_state(const DemoFTDCPS::ServerState & state)
    throw(CORBA::SystemException)
{
    
    std::cout << m_name.in() 
              << ": write_state (" << state.counter << "," << state.message << ")"
              << std::endl;
    
    DCPS::ReturnCode_t status = m_typed_data_writer->write(state, HANDLE_NIL);
    if (status) {
        std::cerr << m_name.in() 
                  << ": write_state => impossible to write value !!" << std::endl;
    }
}

}; // End of namespace Hello


