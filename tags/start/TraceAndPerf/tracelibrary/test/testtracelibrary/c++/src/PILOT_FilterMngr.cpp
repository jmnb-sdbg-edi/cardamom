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


#include "testtracelibrary/PILOT_FilterMngr.hpp"



namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


PILOT_FilterMngr::PILOT_FilterMngr()
    throw()
    : Testable("Cdmw::FilterMngr")
{

}


PILOT_FilterMngr::~PILOT_FilterMngr()
    throw()
{

}


void
PILOT_FilterMngr::do_tests()
    throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (692);

    TEST_INFO ("Creating a FilterMngr object");
    FilterMngr filterMngr;
    CdmwTrace::TraceFilterSeq* p_refFilterSeq = new CdmwTrace::TraceFilterSeq;

    TEST_SUCCEED();

    // Test A1
    TEST_INFO ("Check if all domain/level are deactivated by default");
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;  
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);




    // Test B1
    TEST_INFO ("Trying to activate all domain/level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;  
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);






    // Test C1
    TEST_INFO ("Trying to deactivate all_domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test C2
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test C3
    TEST_INFO ("Then trying to activate domain1/level3");
    filterMngr.activate_level ("domain1", 3);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test C4
    TEST_INFO ("Then trying to activate domain2/level1");
    filterMngr.activate_level ("domain2", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test C5
    TEST_INFO ("Then trying to activate domain2/level3");
    filterMngr.activate_level ("domain2", 3);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test C6
    TEST_INFO ("Then trying to activate domain3/level1");
    filterMngr.activate_level ("domain3", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test C7
    TEST_INFO ("Then trying to activate domain3/level3");
    filterMngr.activate_level ("domain3", 3);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
        
    // set sequence length   
    p_refFilterSeq->length(7);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = true;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = 3;
	(*p_refFilterSeq)[2].activation = true; 
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = true;
	(*p_refFilterSeq)[4].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[4].the_value = 3;
	(*p_refFilterSeq)[4].activation = true;
	(*p_refFilterSeq)[5].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[5].the_value = 1;
	(*p_refFilterSeq)[5].activation = true;
	(*p_refFilterSeq)[6].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[6].the_value = 3;
	(*p_refFilterSeq)[6].activation = true;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);





 
    // Test D1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test D2
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test D3
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test D4
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test D5
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
            
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = true;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true; 
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = false;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
 
 
 
 
    // Test E1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test E2
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
  
    // Test E3
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test E4
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test E5
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
            
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = true;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = false; 
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = true;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true); 
    
    
    
    
    
    // Test F1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    
    // Test F2
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false)
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test F3
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test F4
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test F5
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = true; 
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[2].activation = true;
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = false;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true); 
 
 
 
 
    
    // Test G1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test G2
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
  
    // Test G3
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
  
    // Test G4
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test G5
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                    
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = true; 
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[2].activation = false;
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = true;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    // Test H1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    
    // Test H2
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test H3
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test H4
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // Test H5
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
                        
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true; 
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = false;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
 
 
 
 
    // Test I1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test I2
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
  
    // Test I3
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test I4
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // Test I5
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
                        
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = false; 
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = true;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true); 
    
    
    
    
    
    // Test J1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
    
    // Test J2
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true)
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test J3
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true)
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test J4
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true)
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // Test J5
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true)
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
                            
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = false; 
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[2].activation = true;		
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = false;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
 
 
 
 
    
    // Test K1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test K2
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
  
    // Test K3
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
  
    // Test K4
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // Test K5
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
                                
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = false; 
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[2].activation = false;		
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[3].the_value = 1;
	(*p_refFilterSeq)[3].activation = true;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    // Test L1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test L2
    TEST_INFO ("Then trying to deactivate domain2/level3");
    filterMngr.deactivate_level ("domain2", 3);
    
    // Test L3
    TEST_INFO ("Then trying to activate domain4/all_level");
    filterMngr.activate_level ("domain4", CdmwTrace::ALL_VALUES);
    
    // Test L4
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    
    // Test L5
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
                                    
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;		 
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true;		
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[3].the_value = 3;
	(*p_refFilterSeq)[3].activation = false;   
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
  
    
    
    
    
    
    
    // Test M1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test M2
    TEST_INFO ("Then trying to activate domain2/level2");
    filterMngr.activate_level ("domain2", 2);
    
    // Test M3
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    
    // Test M4
    TEST_INFO ("Then trying to deactivate domain3/all_level");
    filterMngr.deactivate_level ("domain3", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                        
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;		 
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = false;		 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
  
    
    
    
    
    
    
    // Test N1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test N2
    TEST_INFO ("Then trying to deactivate domain2/level3");
    filterMngr.deactivate_level ("domain2", 3);
    
    // Test N3
    TEST_INFO ("Then trying to activate domain4/all_level");
    filterMngr.activate_level ("domain4", CdmwTrace::ALL_VALUES);
    
    // Test N4
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    
    // Test N5
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                            
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain4");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = true;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true;		 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
  
    
    
    
    
    
    
    // Test O1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test O2
    TEST_INFO ("Then trying to activate domain2/level3");
    filterMngr.activate_level ("domain2", 3);
       
    // Test O3
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    
    // Test O4
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = true;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[2].the_value = 3;
	(*p_refFilterSeq)[2].activation = true;	 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
  
    
    
    
    
    
    
    // Test P1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test P2
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test P3
    TEST_INFO ("Then trying to activate domain1/level2");
    filterMngr.activate_level ("domain1", 2);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test P4
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
 
    // Test P5
    TEST_INFO ("Then trying to deactivate domain1/level2");
    filterMngr.deactivate_level ("domain1", 2);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 	 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    // Test Q1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test Q2
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test Q3
    TEST_INFO ("Then trying to activate domain2/all_level");
    filterMngr.activate_level ("domain2", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test Q4
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                        
    // set sequence length   
    p_refFilterSeq->length(2);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = true;	 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    // Test R1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test R2
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test R3
    TEST_INFO ("Then trying to activate domain1/level3");
    filterMngr.activate_level ("domain1", 3);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test R4
    TEST_INFO ("Then trying to activate domain2/level2");
    filterMngr.activate_level ("domain2", 2);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test R5
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test R6
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                            
    // set sequence length   
    p_refFilterSeq->length(2);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[1].the_value = 2;
	(*p_refFilterSeq)[1].activation = true;	 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    // Test S1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test S2
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test S3
    TEST_INFO ("Then trying to activate all_domain/level2");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 2);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);

    // Test S4
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                                
    // set sequence length   
    p_refFilterSeq->length(2);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[1].the_value = 2;
	(*p_refFilterSeq)[1].activation = true;	 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    // Test T1
    TEST_INFO ("Trying to deactivate all domain/all_level");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test T2
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    
    // Test T3
    TEST_INFO ("Then trying to activate domain1/level3");
    filterMngr.activate_level ("domain1", 3);

    // Test T4
    TEST_INFO ("Then trying to activate domain2/level2");
    filterMngr.activate_level ("domain2", 2);

    // Test T5
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // Test T6
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
                                                                    
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = false;		 	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = 3;
	(*p_refFilterSeq)[1].activation = true;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[2].the_value = 2;
	(*p_refFilterSeq)[2].activation = true; 
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
    
    
    
    
    
    

    // Test U1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test U2
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);

    // Test U3
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    
    // Test U4
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	// Test V1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test V2
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);
       
    // Test V3
    TEST_INFO ("Then trying to activate domain1/level1");
    filterMngr.activate_level ("domain1", 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	// Test W1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test W2
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    
    // Test W3
    TEST_INFO ("Then trying to deactivate domain2/all_level");
    filterMngr.deactivate_level ("domain2", CdmwTrace::ALL_VALUES);
    
    // Test W4
    TEST_INFO ("Then trying to deactivate domain3/all_level");
    filterMngr.deactivate_level ("domain3", CdmwTrace::ALL_VALUES);

    // Test W5
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[2].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[2].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	// Test X1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test X2
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);

    // Test X3
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	// Test X4
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	// Test X5
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(2);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	// Test X6
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
	
	// set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	
	// Test Y1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test Y2
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);

    // Test Y3
    TEST_INFO ("Then trying to activate all_domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    	
	// Test Y4
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    
	// Test Y5
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);

	// Test Y6
    TEST_INFO ("Then trying to deactivate all_domain/level1");
    filterMngr.deactivate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
	// set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	
	// Test Z1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test Z2
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);

    // Test Z3
    TEST_INFO ("Then trying to deactivate domain2/level1");
    filterMngr.deactivate_level ("domain2", 1);
    
    // Test Z4
    TEST_INFO ("Then trying to deactivate domain3/level1");
    filterMngr.deactivate_level ("domain3", 1);
    
    // Test Z3
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[1].the_value = 1;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	// Test ZA1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test ZA2
    TEST_INFO ("Then trying to deactivate domain1/level1");
    filterMngr.deactivate_level ("domain1", 1);

    // Test ZA3
    TEST_INFO ("Then trying to deactivate domain2/level2");
    filterMngr.deactivate_level ("domain2", 2);
    
    // Test ZA4
    TEST_INFO ("Then trying to deactivate domain3/level3");
    filterMngr.deactivate_level ("domain3", 3);
    
    // Test ZA3
    TEST_INFO ("Then trying to activate all domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == false);
    
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain2");
	(*p_refFilterSeq)[1].the_value = 2;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup("domain3");
	(*p_refFilterSeq)[2].the_value = 3;
	(*p_refFilterSeq)[2].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	// Test ZB1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test ZB2
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
        
    // Test ZB3
    TEST_INFO ("Then trying to activate all domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(3);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	// Test ZB4
    TEST_INFO ("Then trying to activate all domain/level2");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 2);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    // set sequence length   
    p_refFilterSeq->length(4);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;	
	(*p_refFilterSeq)[2].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[2].the_value = 1;
	(*p_refFilterSeq)[2].activation = true;
	(*p_refFilterSeq)[3].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[3].the_value = 2;
	(*p_refFilterSeq)[3].activation = true;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	// Test ZB5
    TEST_INFO ("Then trying to activate domain1/all_level");
    filterMngr.activate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    
    // set sequence length   
    p_refFilterSeq->length(1);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
	
	
	
	
	
	
	
	// Test ZC1
    TEST_INFO ("Trying to activate all domain/all_level");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);

    // Test ZC2
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
        
    // Test ZC3
    TEST_INFO ("Then trying to activate all domain/level1");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 1);
    
	// Test ZC4
    TEST_INFO ("Then trying to activate all domain/level2");
    filterMngr.activate_level (CdmwTrace::ALL_DOMAINS, 2);
	
	// Test ZC5
    TEST_INFO ("Then trying to deactivate domain1/all_level");
    filterMngr.deactivate_level ("domain1", CdmwTrace::ALL_VALUES);
    TEST_CHECK (filterMngr.is_activated("domain1", 1) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 2) == false);
    TEST_CHECK (filterMngr.is_activated("domain1", 3) == false);
    TEST_CHECK (filterMngr.is_activated("domain2", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain2", 3) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 1) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 2) == true);
    TEST_CHECK (filterMngr.is_activated("domain3", 3) == true);
    
    
    // set sequence length   
    p_refFilterSeq->length(2);       
    // set trace filter sequence and check it    
	(*p_refFilterSeq)[0].the_domain = CORBA::string_dup(CdmwTrace::ALL_DOMAINS);
	(*p_refFilterSeq)[0].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[0].activation = true;	
	(*p_refFilterSeq)[1].the_domain = CORBA::string_dup("domain1");
	(*p_refFilterSeq)[1].the_value = CdmwTrace::ALL_VALUES;
	(*p_refFilterSeq)[1].activation = false;
    TEST_CHECK (check_filter_seq (filterMngr, p_refFilterSeq) == true);
}


bool PILOT_FilterMngr::check_filter_seq (FilterMngr& filterMngr,
                                         CdmwTrace::TraceFilterSeq* p_ref_sequence)
{
    CdmwTrace::TraceFilterSeq_var filterSeq;

    // get sequence of trace filters
    filterSeq = filterMngr.get_trace_filters();
  
    // check if sequence has same length    
    if (filterSeq->length() != p_ref_sequence->length())
    {
      return false;
    }
    
    // check sequence content
    bool check_flag = true;
      
    for (unsigned int i=0 ; check_flag && i < filterSeq->length() ; i++)
    { 
      std::string domain_name = (*filterSeq)[i].the_domain.in();
      std::string ref_domain_name = (*p_ref_sequence)[i].the_domain.in();
      
      if (domain_name != ref_domain_name)
      {
        check_flag = false;
      }
    
      int level_value = (*filterSeq)[i].the_value;
      int ref_level_value = (*p_ref_sequence)[i].the_value;
      
      if (level_value != ref_level_value)
      {
        check_flag = false;
      }

      bool activation = (*filterSeq)[i].activation;
      bool ref_activation = (*p_ref_sequence)[i].activation;
      
      if (activation != ref_activation)
      {
        check_flag = false;
      }
    }

    return check_flag;
}

}; // End namespace Trace
}; // End namespace Cdmw

