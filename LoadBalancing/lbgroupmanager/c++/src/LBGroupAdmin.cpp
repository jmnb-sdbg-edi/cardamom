/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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

/**
 * @brief A Class Admin for object group management
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include "lbgroupmanager/LBGroupManager_impl.hpp"
#include "lbgroupmanager/LBGroupAdmin.hpp"

#include <stdio.h>



const int Cdmw::LB::LBGroupAdmin::ADD_GROUP_OPT;
const int Cdmw::LB::LBGroupAdmin::ADD_REPLICA_OPT;
const int Cdmw::LB::LBGroupAdmin::REMOVE_GROUP_OPT;
const int Cdmw::LB::LBGroupAdmin::REMOVE_MEMBER_OPT;
const int Cdmw::LB::LBGroupAdmin::SHUT_DOWN_LB_OPT;
const int Cdmw::LB::LBGroupAdmin::EXIT_OPT;
const int Cdmw::LB::LBGroupAdmin::SUCCESS;
const int Cdmw::LB::LBGroupAdmin::ERROR;


namespace Cdmw
{
namespace LB
{
    class OrbThread : public Cdmw::OsSupport::Thread {
        
    public:

        // Constructor
        OrbThread(CORBA::ORB_ptr orb)
            : m_orb(CORBA::ORB::_duplicate(orb))
            {
            }
        
        // Destructor
        ~OrbThread() throw()
            {
            }
        
        void
        shutdown()
            {
                m_orb->shutdown(true);
                m_orb->destroy();
            }
    protected:
        
        void
        run() throw()
            {
                std::cout <<"orb->run."<<std::endl;
                m_orb->run();
            }
        
        
    public:
        
        CORBA::ORB_var  m_orb;
    };
    
    void readKey()
    {
        char ch[16];
        fread(ch, 2, 1, stdin);
    }
    
    LBGroupAdmin::LBGroupAdmin(CdmwLB::LBGroupManager_ptr gm,
                               CosNaming::NamingContext_ptr ns, CORBA::ORB_ptr orb)
        : m_gm(CdmwLB::LBGroupManager::_duplicate(gm)),
          m_ns(CosNaming::NamingContext::_duplicate(ns)),
          m_orb(CORBA::ORB::_duplicate(orb))
    {
        // No-Op
    }
    
    LBGroupAdmin::~LBGroupAdmin()
    {
        // No-Op
    }
    
    
    void
    LBGroupAdmin::run()
    {
        PortableGroup::ObjectGroup_var group;
        PortableGroup::GenericFactory::FactoryCreationId_var fcid;
        int selection = 0;
        bool done = false;
        while (!done)
        {
            system("clear");
            std::cout << ">> Enter one of the following options:\n"
                      << "\t[" << ADD_GROUP_OPT << "] Create Group\n"
                      << "\t[" << ADD_REPLICA_OPT << "] Add Replica to group\n"
                      << "\t[" << REMOVE_MEMBER_OPT << "] Remove Replica from group\n"
                      << "\t[" << REMOVE_GROUP_OPT << "] Remove group\n"
                      << "\t[" << SHUT_DOWN_LB_OPT << "] Shut Down the Group Manager\n"
                      << "\t[" << EXIT_OPT << "] Exit\n"
                      <<">> ";
            std::cin >> selection;
            switch (selection) {
            case ADD_GROUP_OPT:
            {
                CosNaming::Name name (1);
                name.length (1);
                char  name_id[20];
                std::cout << "LBGroupAdmin>> Enter Group Name = ";
                std::cin >> name_id;
                std::cout << std::endl;
                
                name[0].id = CORBA::string_dup(name_id);
                name[0].kind = "Object Group";
                
                
                char repository_id[100];
                
                std::cout << "LBGroupAdmin>> Enter Object Group Type Id= ";
                std::cin >> repository_id;
                std::cout << std::endl;
                
                
                std::string lb_policy;
                std::cout << "LBGroupAdmin>> Enter Load Balancing Policy Type = ";
                std::cin >> lb_policy;
                
                std::cout << std::endl;
                PortableGroup::Criteria criteria (1);
                
                criteria.length (1);
                
                PortableGroup::Property & property = criteria[0];
                property.nam.length (1);
                
                property.nam[0].id =
                    CORBA::string_dup ("cdmw.lb.LBProperties");
                PortableGroup::Properties lb_properties;
                lb_properties.length (1);
                lb_properties[0].nam.length (1);
                lb_properties[0].nam[0].id =
                    CORBA::string_dup ("cdmw.lb.LoadBalancingPolicy");
                if (lb_policy == "")
                    {
                        lb_policy = "default";
                    }
                lb_properties[0].val <<= lb_policy.c_str();
                property.val <<= lb_properties;
                
                try{
                    group = m_gm->create_object(repository_id,
                                                criteria,
                                                fcid.out ());
                    
                    m_ns->rebind (name, group.in ());
                    
                } catch(const PortableGroup::ObjectNotCreated&)
                    {
                        std::cout << "Unexpected Exception: Object Not Created" <<std::endl;
                    }
                catch(const PortableGroup::NoFactory&)
                    {
                        std::cout << "Unexpected Exception: No Factory" <<std::endl;
                    }
                catch(const PortableGroup::InvalidProperty&)
                    {
                        std::cout << "Unexpected Exception: Invalid Property" <<std::endl;
                    }
                catch(const CORBA::SystemException &)
                    {
                        std::cout << "Unexpected Exception: System Exception" <<std::endl;
                    }
                
                
                break; 
            }
            
            
            case ADD_REPLICA_OPT:
            {
                CosNaming::Name name_og (1);
                name_og.length (1);
                
                char name_id[20];
                std::cout << ">> Enter Group Name = ";
                std::cin >> name_id;
                std::cout << std::endl;
                
                name_og[0].id = CORBA::string_dup(name_id);
                name_og[0].kind = "Object Group";
                
                try{
                    group = m_ns->resolve (name_og);
                    char name_rep[20];
                    std::cout << ">> Enter Replica Name = ";
                    
                    std::cin >> name_rep;
                    std::cout << std::endl;
                    
                    
                    
                    CosNaming::Name name_r (1);
                    name_r.length (1);
                    name_r[0].id = CORBA::string_dup(name_rep);
                    name_r[0].kind = "PingableServer";
                    CORBA::Object_var obj = m_ns->resolve(name_r);
                    PortableGroup::Location loc;
                    loc.length(2);
                    loc[0].id="HOST";
                    loc[0].kind=CORBA::string_dup("localhost");
                    loc[1].id="NAME";
                    loc[1].kind = CORBA::string_dup(name_rep);
                    
                    group = m_gm->add_member(group.in (),
                                             loc,
                                             obj.in ());
                    m_ns->rebind(name_og, group.in());
                }catch(const PortableGroup::MemberAlreadyPresent&)
                    {
                        std::cout << "Unexpected Exception: Member Already Present" << std::endl;
                        break;
                    }
                catch(const  PortableGroup::ObjectGroupNotFound&)
                    {
                        std::cout << "Unexpected Exception: Object Group Not Found" << std::endl;
                        break;
                    }
                catch(const PortableGroup::ObjectNotAdded&)
                    {
                        std::cout << "Unexpected Exception: Object Not Added" << std::endl;
                        break;
                    }
                catch(const CosNaming::NamingContext::NotFound&)
                    {
                        std::cout << "Unexpected Exception: Naming Context Not Found" << std::endl;
                        break;
                    }
                catch(const CORBA::SystemException&)
                    {
                        std::cout << "Unexpected Exception: CORBA Exception" << std::endl;
                        break;
                    }
                
                break;
            }
            
            case REMOVE_GROUP_OPT:
            {
                CosNaming::Name name_og (1);
                name_og.length (1);
                char  name_id[20];
                std::cout << "LBGroupAdmin>> Enter Group Name = ";
                std::cin >> name_id;
                std::cout << std::endl;
                
                name_og[0].id = CORBA::string_dup(name_id);
                name_og[0].kind = "Object Group";
                
                
                try{
                    group = m_ns->resolve (name_og);
                    PortableGroup::ObjectGroupId object_group_id = m_gm->get_object_group_id(group.in());
                    
                    CORBA::Any factory_id;
                    factory_id <<= object_group_id;
                    PortableGroup::GenericFactory::FactoryCreationId factory_creation_id(factory_id);
                    
                    m_gm->delete_object(factory_creation_id);
                    m_ns->unbind(name_og);
                }catch(const PortableGroup::ObjectNotFound&)
                    {
                        std::cout << "Unexpected Exception: Object Group Not Found" << std::endl;
                    }
            catch(const CosNaming::NamingContext::NotFound&)
                {
                    std::cout << "Unexpected Exception: Naming Context Not Found" << std::endl;
                    break;
                }
            catch(const CORBA::SystemException&)
                {
                    std::cout << "Unexpected Exception: CORBA Exception" << std::endl;
                }
         
            
                break;
        }
            case REMOVE_MEMBER_OPT:
            {
                CosNaming::Name name_og (1);
                name_og.length (1);
                
                char name_id[20];
                std::cout << ">> Enter Group Name = ";
                std::cin >> name_id;
                std::cout << std::endl;
                
                name_og[0].id = CORBA::string_dup(name_id);
                name_og[0].kind = "Object Group";
                
                try{
                    group = m_ns->resolve (name_og);
                    char name_rep[20];
                    std::cout << ">> Enter Replica Name = ";
                    
                    std::cin >> name_rep;
                    std::cout << std::endl;
                    
                    PortableGroup::Location loc;
                    loc.length(2);
                    loc[0].id="HOST";
                    loc[0].kind=CORBA::string_dup("localhost");
                    loc[1].id="NAME";
                    loc[1].kind = CORBA::string_dup(name_rep);
                    
                    group = m_gm->remove_member(group.in (),
                                                loc);
                    m_ns->rebind(name_og, group.in());
                }catch(const PortableGroup::MemberNotFound&)
                    {
                        std::cout << "Unexpected Exception: Member Not Found" << std::endl;
                    }
                catch(const  PortableGroup::ObjectGroupNotFound&)
                    {
                        std::cout << "Unexpected Exception: Object Group Not Found" << std::endl;
                    }
                catch(const CORBA::SystemException&)
                    {
                        std::cout << "Unexpected Exception: CORBA Exception" << std::endl;
                    }
                
                break;
            }
            
            
            case SHUT_DOWN_LB_OPT:
                //m_gm->shutdown();
                done = true;
                break;
                
            case EXIT_OPT:
                done = true;
                break;
                
            default:
                break;
            }
            
        }
    }
    
};//close namespace LB

};//close namespace Cdmw


void usage()
{
    std::cout << "USAGE:\n\t LBGroupAdmin  [-ORBInitRef NameService=<NS URL>]" << std::endl;
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        usage();
        return Cdmw::LB::LBGroupAdmin::ERROR;
    }
    
    CORBA::ORB_var orb;
    int status =Cdmw::LB::LBGroupAdmin::SUCCESS;
    
    try
    {
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        
        orb =
            Cdmw::OrbSupport::OrbSupport::ORB_init(argc,
                                                   argv,
                                                   orb_strategies); 
        CORBA::Object_var obj=
            orb->resolve_initial_references("RootPOA");
        
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(obj.in());
        
        PortableServer::POAManager_var poa_manager =
            rootPOA->the_POAManager();
        
        obj =
            orb->resolve_initial_references("NameService");
        CosNaming::NamingContext_var ns =
            CosNaming::NamingContext::_narrow(obj.in());
        
        Cdmw::LB::LBGroupManager_impl groupManager(orb.in(), rootPOA.in (), CORBA::string_dup("LB_DOMAIN_ID"), std::cout);
        
        CdmwLB::LBGroupManager_var gm = groupManager._this();
        
        poa_manager->activate();
        
        Cdmw::LB::OrbThread orb_thread(orb.in());
        orb_thread.start();
        
        Cdmw::LB::LBGroupAdmin groupAdmin(gm.in(), ns.in(), orb.in());
        groupAdmin.run();
        orb_thread.shutdown();
        orb_thread.join();
    }
    catch (const CORBA::SystemException& e)
    {
        std::cout << "LBGroupAdmin>> Unhandled CORBA::SystemException..."
                  << e << std::endl;
        return Cdmw::LB::LBGroupAdmin::ERROR;
    }
    
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
    
    if (!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb->destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = Cdmw::LB::LBGroupAdmin::ERROR;
        }
    }        
    
    return status;
}
