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


#include <ClientThread.hpp>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>


void ClientThread::run() throw()
{

    std::cout << "------------------------------" << std::endl; 
    std::cout << "Type return to start client..." << std::endl;
    std::string foo;
    std::cin >> foo;
    std::cout << "Client is running..." << std::endl;
    
    // identifier of the Data
    Demo::Ident id = 666;
    
    // counter which will be used for data modifications
    unsigned int count = 0;
    
    
    // create a first data
    Demo::Data firstData;
    firstData.id = id;
    firstData.part1.version = count;           // part1.version will always be equal to the counter
    firstData.part1.sh = 2 * count + 1;        // part1.sh will always be equal to 2*count+1
    firstData.part2.version = count + 10;      // part2.version will always be equal to count+10
    firstData.part2.ul = 5 * count + 18;       // part2.version will always be equal to 5*count+18
    firstData.part3.version = count;           // part3.version will always be equal to the counter
    firstData.part3.sh = 2 * count + 1;        // part3.sh will always be equal to 2*count+1
    
 
    // set the first data
    try {
        std::cout << "   =======> set first data " << count << std::endl;
        m_database->set_data(firstData);
        std::cout << "   <======= first data set OK\n" << std::endl;
    } catch (const CORBA::SystemException& e) {
        std::cerr << "Unexpected CORBA Exception during first set_data:\n"   
                  << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                  << std::endl;
        return;
    }
    
    
    // call get_data() and set_data() operations 20 times with an interval of 3 seconds
    for (count = 1; count<=20 && !m_stopped; ++count) {
        try {
            std::cout << "   =======> data modification number " << count << std::endl;
        
            std::cout << "       ===> get data " << std::endl;
            Demo::Data_var data;
            try {
                data = m_database->get_data(id);
            } catch (Demo::DataNotFound& e) {
                std::cout << "       ===> data not found !! (previous set_data failed ?)" << std::endl;
                return;
            }
        
            // check that data is correct
            int old_count = count-1;
            if (data->part1.version != old_count) {
                std::cout << "       ===> data part1 has unexpected version !! : " << data->part1.version << std::endl;
                std::cout << "            expected version was : " << old_count << std::endl;
            }
            if (data->part1.sh != 2*old_count+1) {
                std::cout << "       ===> data part1 has unexpected value !! : " << data->part1.sh << std::endl;
                std::cout << "            expected value was : " << (2*old_count+1) << std::endl;
            }
            if (data->part2.version != old_count+10) {
                std::cout << "       ===> data part2 has unexpected version !! : " << data->part2.version << std::endl;
                std::cout << "            expected version was : " << (old_count+10) << std::endl;
            }
            if (data->part2.ul != (unsigned long) 5*old_count+18) {
                std::cout << "       ===> data part2 has unexpected value !! : " << data->part2.ul << std::endl;
                std::cout << "            expected value was : " << (5*old_count+18) << std::endl;
            }
            if (data->part3.version != old_count) {
                std::cout << "       ===> data part3 has unexpected version !! : " << data->part3.version << std::endl;
                std::cout << "            expected version was : " << old_count << std::endl;
            }
            if (data->part3.sh != 2*old_count+1) {
                std::cout << "       ===> data part3 has unexpected value !! : " << data->part3.sh << std::endl;
                std::cout << "            expected value was : " << (2*old_count+1) << std::endl;
            }
        
            std::cout << "       ===> modify data " << std::endl;
            data->part1.version = count;           // part1.version is always be equal to the counter
            data->part1.sh = 2 * count + 1;        // part1.sh is always be equal to 2*count+1
            data->part2.version = count + 10;      // part2.version is always be equal to count+10
            data->part2.ul = 5 * count + 18;       // part2.version is always be equal to 5*count+18
            data->part3.version = count;           // part1.version is always be equal to the counter
            data->part3.sh = 2 * count + 1;        // part1.sh is always be equal to 2*count+1

            std::cout << "       ===> set data " << std::endl;
            m_database->set_data(data);
        
            std::cout << "   <======= data modification " << count << " OK\n" << std::endl;

            // wait 3 seconds    
            Cdmw::OsSupport::OS::sleep(3000);
            
        } catch (const CORBA::Exception& e) {
            std::cerr << "UNEXPECTED CORBA EXCEPTION !! :\n" 
                      << Cdmw::OrbSupport::OrbSupport::exception_to_string(e) 
                      << std::endl;
        }
        
    }
    
}




