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

#include <iostream>

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <testfaultdetector/SIM_consumer.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
namespace Cdmw {

namespace FT {
        

    SIM_consumer::SIM_consumer()
        :m_a_new_event_is_arrived(false)
    {
    }
    
    
    SIM_consumer::~SIM_consumer()
    {
    }
    
    
    void SIM_consumer::push_structured_event(const CosNotification::StructuredEvent & notification)
        throw (CosEventComm::Disconnected,
               CORBA::SystemException)
    {
        std::cout<<"NEW EVENT"<<std::endl;  
        m_a_new_event_is_arrived = true;        

        const ::FT::Location * location;
        notification.filterable_data[1].value >>= location;
        const std::string s_location = 
        Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*location);
        std::cout<< "push_structured_event(): Location is '" << s_location << "'." << std::endl;
        
        // Handle host failure, application failure, and process failure
        std::string hostname(""), applicationname(""), processname("");
        const CORBA::ULong lg = location->length();
        
        // there are two type of message 
        // 1 : the "ObjectCrashFault" message to notify if an object is crashed
        // 2 : the "ObjectResponding" message to notify if an object responding (for the moment, 
        //     it's just used for daemon)
        std::string type_name_message = CORBA::string_dup(notification.header.fixed_header.event_type.type_name);
        if ( type_name_message == "ObjectCrashFault")
        {
            switch (lg) {
                case 1:
                    {
                        const std::string hostname((*location)[0].id);
                        std::cout<<"ObjectCrashFault hostname : "<<hostname<<std::endl;   
                        m_hostStatus[hostname] = false;                        
                    }
                    break;
                default:
                    // Do nothing for the moment
                    break;
            }

        } else {
                
            if (type_name_message == "ObjectResponding")
            {
                switch (lg) {                        
                    case 1:
                        {
                            const std::string hostname((*location)[0].id);
                            std::cout<<"ObjectResponding hostname : "<<hostname<<std::endl;
                            m_hostStatus[hostname] = true;
                        }
                        break;
                    default:
                        // Do nothing for the moment
                        break;
                }                
            }                
        }        
    }

    void SIM_consumer::add_location(std::string location)
    {
        std::cout<<"SIM_consumer::add_location :"<<location<<std::endl;    
        m_hostStatus[location] = true;        
    }
    
    bool SIM_consumer::get_host_status(std::string location)
    {
        std::cout<<"SIM_consumer::get_host_status :"<<m_hostStatus[location]<<std::endl;        
        return m_hostStatus[location];        
    }
    
    bool SIM_consumer::a_new_event_is_arrived()
    {
        bool temp = m_a_new_event_is_arrived;
        m_a_new_event_is_arrived = false;            
        return temp;        
    }
    
    
    
}; // namespace FT
}; // namespace Cdmw
