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


#include <testflattransaction3/TestInterceptors.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCohort.skel.hpp>
#include <iostream>

namespace Cdmw
{
    


    class BadCohortTest : public virtual POA_CdmwFT::Transaction::TPCCohort,
                                                                  public virtual PortableServer::RefCountServantBase
    {
    public:   
        
        /**
         * Purpose:
         * <p> Constructor
         */
        BadCohortTest()
        {};
        
        virtual ~BadCohortTest()
        {
        }

        void send_prepare (const CdmwFT::Transaction::TransactionIdentification & id,
                           const CdmwFT::Transaction::StateChanges& change)
            throw(CORBA::SystemException)
        {
            std::cout<< "send_prepare of BadCohortTest answer No for transaction " << id.id <<std::endl;
        };

        void send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
            throw(CORBA::SystemException)
        {
            std::cout<< "send_commit of BadCohortTest don't answer for transaction " << id.id <<std::endl;
        };
    
        void send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
            throw(CORBA::SystemException)
        {
            std::cout<< "send_rollback of BadCohortTest don't answer for transaction " << id.id <<std::endl;
        };

        void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
            throw(CORBA::SystemException)
        {
            std::cout<< "send_get_transaction_status of BadCohortTest answer ABORTED for transaction " << id.id <<std::endl;
        };

        void send_set_transaction_status (const char * sender,
                                          const CdmwFT::Transaction::TransactionIdentification & id,
                                          CdmwFT::Transaction::TransactionStatus status)
            throw(CORBA::SystemException)
        {
            std::cout<< "send_set_transaction_status of BadCohortTest does nothing for transaction " << id.id <<std::endl;
        };


        CORBA::Boolean prepare (const CdmwFT::Transaction::StateChanges& change,
                                CdmwFT::Transaction::TransactionIdentification & id,
                                CORBA::String_out sender)
            throw(CORBA::SystemException)
        {
            sender = CORBA::string_dup("BadCohortTest");
            std::cout<<"---------------> prepare"<<std::endl;
            return false;
        };

        void commit (CdmwFT::Transaction::TransactionIdentification & id,
                     CORBA::String_out sender)
            throw(CORBA::SystemException)
        {
            sender = CORBA::string_dup("");
        };
    

        void rollback (CdmwFT::Transaction::TransactionIdentification & id,
                       CORBA::String_out sender)
            throw(CORBA::SystemException)
        {
            sender = CORBA::string_dup("");
        };
    

        CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                                       CORBA::String_out sender)
            throw(CORBA::SystemException)
        {
            sender = CORBA::string_dup("BadCohortTest");
            return CdmwFT::Transaction::ABORTED;
        };
    
    
        void set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                                     char *& sender,
                                     CdmwFT::Transaction::TransactionIdentification & id)
            throw(CORBA::SystemException)
        {
            sender = CORBA::string_dup("BadCohortTest");
        };


    };

    TestServerInterceptor::TestServerInterceptor (const char* host)
        : m_expected_intercept_count(0)
        , m_actual_intercept_count(0)
        , m_host_name(host)
        , m_method_to_intercept("NONE")
        , m_event_to_intercept(kIntercept_SEND_REPLY)
    {

    }

    void TestServerInterceptor::interception(int expected_event_count, const char* method
                                             , const char* where,const char * what)
    {
        std::cout<<"expected_event_count "<<expected_event_count<<" method "<<method<<" where " <<where<<" what "<<what<<std::endl;        if (strcmp(method, "prepare") == 0) {
            m_method_to_intercept = "prepare";
        }
        else if (strcmp(method, "commit") == 0) {
            m_method_to_intercept = "commit";
        }

        if (strcmp(where, "request") == 0) {
            m_event_to_intercept = kIntercept_RECEIVE_REQUEST;
        }
        else if (strcmp(where, "reply") == 0) {
            m_event_to_intercept = kIntercept_SEND_REPLY;
        }

        if (strcmp(what, "kill") == 0) {
            m_what = "kill";
        }
        else if (strcmp(what, "forward") == 0) {
            m_what = "forward";
        }

        m_expected_intercept_count=expected_event_count;
    }
    



    //
    // IDL to C++ Mapping
    //
    char*
    TestServerInterceptor::name() throw (CORBA::SystemException)
    {
      return CORBA::string_dup("TestServerInterceptor");
    }

    void
    TestServerInterceptor::destroy() throw (CORBA::SystemException)
    {
      
    }

    void
    TestServerInterceptor::receive_request_service_contexts(
        PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {

    }

    void
    TestServerInterceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestServerInterceptor] receive_request(" << actual_method << "):" << std::endl;
        if (m_event_to_intercept == kIntercept_RECEIVE_REQUEST) {
            if (actual_method == m_method_to_intercept) {
                if (m_expected_intercept_count == m_actual_intercept_count) {
                    if (strcmp(m_what.c_str(), "kill") == 0) {
                        std::cout << "  -> count = " << m_actual_intercept_count << " *>>>> EXITING <<<<" << std::endl;
                        ::exit(-1);
                    } else {
                        m_actual_intercept_count++;
                        BadCohortTest* badCohort_obj= new BadCohortTest();
                        CdmwFT::Transaction::TPCCohort_var badCohort = badCohort_obj->_this();
                        std::cout<< "  -> count = " << m_actual_intercept_count << " *>>>> FORWARD <<<<" << std::endl;
                        throw PortableInterceptor::ForwardRequest(badCohort.in(), 0);
                    }    
                }
                else {
                    std::cout << "  -> count = " << m_actual_intercept_count << std::endl;
                    m_actual_intercept_count++;
                }
            }
        }
    }

    void
    TestServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestServerInterceptor] send_reply(" << actual_method << "):" << std::endl;
//         if (m_event_to_intercept == kIntercept_SEND_REPLY) {
//             if (actual_method == m_method_to_intercept) {
//                 if (m_expected_intercept_count == m_actual_intercept_count) {
//                     if (strcmp(m_what.c_str(), "kill") == 0) {
//                         std::cout << "  -> count = " << m_actual_intercept_count << " *>>>> EXITING <<<<" << std::endl;
//                         ::exit(-1);
//                     } else {
//                         m_actual_intercept_count++;
//                         BadCohortTest* badCohort_obj= new BadCohortTest();
//                         CdmwFT::Transaction::TPCCohort_var badCohort = badCohort_obj->_this();
//                         std::cout<< "  -> count = " << m_actual_intercept_count << " *>>>> FORWARD <<<<" << std::endl;
//                         throw PortableInterceptor::ForwardRequest(badCohort.in(), 0);
//                     }    
//                 }
//                 else {
//                     std::cout << "  -> count = " << m_actual_intercept_count << std::endl;
//                     m_actual_intercept_count++;
//                 }
//             }
//         }
      
    }

    void
    TestServerInterceptor::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }

    void
    TestServerInterceptor::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }

    TestClientInterceptor::TestClientInterceptor (const char* host)
        : m_expected_intercept_count(0)
        , m_actual_intercept_count(0)
        , m_host_name(host)
        , m_method_to_intercept("NONE")
        , m_event_to_intercept(kIntercept_RECEIVE_REPLY)
    {
    }


    void TestClientInterceptor::interception(int expected_event_count, const char* method
                                             , const char* where, const char * what)
    {
        std::cout<<"expected_event_count "<<expected_event_count<<" method "<<method<<" where " <<where<<" what "<<what<<std::endl;

        if (strcmp(method, "prepare") == 0) {
            m_method_to_intercept = "prepare";
        }
        else if (strcmp(method, "commit") == 0) {
            m_method_to_intercept = "commit";
        }

        if (strcmp(where, "request") == 0) {
            m_event_to_intercept = kIntercept_SEND_REQUEST;
        }
        else if (strcmp(where, "reply") == 0) {
            m_event_to_intercept = kIntercept_RECEIVE_REPLY;
        }

        if (strcmp(what, "kill") == 0) {
            m_what = "kill";
        }
        else if (strcmp(what, "forward") == 0) {
            m_what = "forward";
        }

        

        m_expected_intercept_count=expected_event_count;
    }
    
    //
    // IDL to C++ Mapping
    //
    char*
    TestClientInterceptor::name() throw (CORBA::SystemException)
    {
      return CORBA::string_dup("TestClientInterceptor");
    }

    void
    TestClientInterceptor::destroy() throw (CORBA::SystemException)
    {
      
    }


    void
    TestClientInterceptor::send_request (PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestClientInterceptor] send_request(" << actual_method << "):" << std::endl;
        if (m_event_to_intercept == kIntercept_SEND_REQUEST) {
            if (actual_method == m_method_to_intercept) {
                if (m_expected_intercept_count == m_actual_intercept_count) {
                    if (strcmp(m_what.c_str(), "kill") == 0) {
                        std::cout << "  -> count = " << m_actual_intercept_count << " *>>>> EXITING <<<<" << std::endl;
                        ::exit(-1);
                    } else {
                        m_actual_intercept_count++;
                        BadCohortTest* badCohort_obj= new BadCohortTest();
                        CdmwFT::Transaction::TPCCohort_var badCohort = badCohort_obj->_this();
                        std::cout<< "  -> count = " << m_actual_intercept_count << " *>>>> FORWARD <<<<" << std::endl;
                        throw PortableInterceptor::ForwardRequest(badCohort.in(), 0);
                    }    
                }
                else {
                    std::cout << "  -> count = " << m_actual_intercept_count << std::endl;
                    m_actual_intercept_count++;
                }
            }
        }
    }


    void
    TestClientInterceptor::send_poll (PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
      
    }


    void
    TestClientInterceptor::receive_reply (PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
        std::string actual_method = ri->operation();
        std::cout << "[++++ " << m_host_name << "::TestClientInterceptor] receive_reply(" << actual_method << "):" << std::endl;
        if (m_event_to_intercept == kIntercept_RECEIVE_REPLY) {
            if (actual_method == m_method_to_intercept) {
                std::cout<<"m_expected_intercept_count "<<m_expected_intercept_count<<"   m_actual_intercept_count "<< m_actual_intercept_count<<std::endl;
                if (m_expected_intercept_count == m_actual_intercept_count) {
                    if (strcmp(m_what.c_str(), "kill") == 0) {
                        std::cout << "  -> count = " << m_actual_intercept_count << " *>>>> EXITING <<<<" << std::endl;
                        ::exit(-1);
                    } else {
                        m_actual_intercept_count++;
                        BadCohortTest* badCohort_obj= new BadCohortTest();
                        CdmwFT::Transaction::TPCCohort_var badCohort = badCohort_obj->_this();
                        std::cout<< "  -> count = " << m_actual_intercept_count << " *>>>> FORWARD <<<<" << std::endl;
                        throw PortableInterceptor::ForwardRequest(badCohort.in(), 1);
                    }    
                }
                else {
                    std::cout << "  -> count = " << m_actual_intercept_count << std::endl;
                    m_actual_intercept_count++;
                }
            }
        }
    }


    void
    TestClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }


    void
    TestClientInterceptor::receive_other(PortableInterceptor::ClientRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      
    }

}; // End namespace Cdmw

