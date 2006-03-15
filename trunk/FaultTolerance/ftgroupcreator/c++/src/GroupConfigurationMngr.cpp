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


#include <cstdlib>
#include <vector>

#include "ftgroupcreator/GroupConfigurationMngr.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Assert.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>


#include <sstream>
#include <dom/DOM.hpp>
#include <parsers/DOMParser.hpp>
#include <util/PlatformUtils.hpp>
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>


#include <ftgroupcreator/ConfigElementParser.hpp>
#include <ftgroupcreator/DatastoreIDsParser.hpp>
#include <ftgroupcreator/GroupsParser.hpp>
#include <ftgroupcreator/LocationsParser.hpp>
#include <ftgroupcreator/DatastoreIDParser.hpp>
#include <ftgroupcreator/LocationParser.hpp>
#include <ftgroupcreator/ObjectGroupParser.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

#include <FaultTolerance/ftcommon/FactoryInfos.hpp>

namespace
{
    std::string to_string(const DOMString &s)
        throw (std::bad_alloc)
    {
        char *c_str = s.transcode();
    
        std::string str(c_str);
    
        delete[] c_str;
        
        return str;
    }
} // end anonymous namespace


namespace Cdmw
{
namespace FT
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

class  ErrorReporter : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    ErrorReporter()
    {
    }

    ~ErrorReporter()
    {
    } 


    // Each error reported by the parsing is translated into an exception to be
    //  catch in the main procedure
    void warning(const SAXParseException& ex) { 
        throw ex; 
    }

    void error(const SAXParseException& ex)
    {
        throw ex;
    }

    void fatalError(const SAXParseException& ex)
    {
        throw ex;

    }

    void resetErrors() { 
    }
};


//
// Ctor
//
GroupConfigurationMngr::GroupConfigurationMngr(CdmwFT::ReplicationManager_ptr replication_manager)
    throw()
   : m_replication_manager(CdmwFT::ReplicationManager::_duplicate(replication_manager))
{
   CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
   CDMW_ASSERT(!CORBA::is_nil(replication_manager));
}

//
// Ctor
//
GroupConfigurationMngr::GroupConfigurationMngr(bool validate)
    throw()
    :m_replication_manager(CdmwFT::ReplicationManager::_nil())
{
}


void GroupConfigurationMngr::validate(const std::string& xml_conf)
    throw(OutOfMemoryException, ConfigurationException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"xml_file=" << xml_conf);
    try
    {
        // Xerces initialisation
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "Error during Xerces-c Initialization.\n"
                   << "  Exception message:"
                   << to_string(toCatch.getMessage()));

        CORBA::INTERNAL ex(Cdmw::OrbSupport::INTERNAL,
                           CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
        
    try 
    {
        try
        {
             // We parse the group configuration file
             std::auto_ptr<DOMParser> parser(new DOMParser);

             parser->setValidationScheme(DOMParser::Val_Auto);
             parser->setDoNamespaces(false);
             
             ErrorReporter errorReporter;
             parser->setErrorHandler( &errorReporter );
    
             parser->parse( xml_conf.c_str() );

            XMLPlatformUtils::Terminate();
        }
        catch (std::bad_alloc &) 
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "bad_alloc exception raised");

            CDMW_THROW(OutOfMemoryException);
        } 
        catch(const SAXParseException& ex1)
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "XMLException raised: "
                      << to_string(ex1.getMessage()));
            CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(ex1.getMessage())), ex1.getLineNumber());
        }
        catch(const XMLException& ex2)
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "XMLException raised: "
                      << to_string(ex2.getMessage()));
            CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(ex2.getMessage())), ex2.getSrcLine());   
        }
        catch (const DOM_DOMException& e) 
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "DOM_DOMException raised: "
                      << to_string(e.msg));
             CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(e.msg)), e.code);   
       }
    }
    catch( CORBA::SystemException& e )
    {
        XMLPlatformUtils::Terminate();
        CDMW_FATAL(FTLogger::GetLogger(),
                   "CORBA System Exception: " << e );
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw;
    }
    catch( CORBA::Exception& e )
    {
        XMLPlatformUtils::Terminate();
        CDMW_FATAL(FTLogger::GetLogger(),
                   "CORBA System Exception: " << e );
        CORBA::INTERNAL ex(Cdmw::OrbSupport::INTERNAL,
                           CORBA::COMPLETED_NO);
        throw ex;
    }
}




void GroupConfigurationMngr::parse(const std::string& xml_conf, bool validate)
    throw(OutOfMemoryException, ConfigurationException, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"xml_file=" << xml_conf 
                      << ", validate=" << validate);
    CDMW_ASSERT(!CORBA::is_nil(m_replication_manager.in()));
    try
    {
        // Xerces initialisation
        XMLPlatformUtils::Initialize();
    }
    catch(const XMLException& toCatch)
    {
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "Error during Xerces-c Initialization.\n"
                   << "  Exception message:"
                   << to_string(toCatch.getMessage()));

        CORBA::INTERNAL ex(Cdmw::OrbSupport::INTERNAL,
                           CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
        
    try 
    {
        try
        {
             // We parse the group configuration file
             std::auto_ptr<DOMParser> parser(new DOMParser);
             if (validate)
             {
                 parser->setValidationScheme(DOMParser::Val_Auto);
                 parser->setDoNamespaces(false);
             }
    
             ErrorReporter errorReporter;
             parser->setErrorHandler( &errorReporter );
    
             parser->parse( xml_conf.c_str() );
    
             DOM_Document doc = parser->getDocument();
    
             // We build a TreeWalker to skip all comment and automatically resolve
             // the entities
             DOM_TreeWalker walker = doc.createTreeWalker(doc,
                 DOM_NodeFilter::SHOW_ELEMENT |
                 DOM_NodeFilter::SHOW_ATTRIBUTE |
                 DOM_NodeFilter::SHOW_TEXT |
                 DOM_NodeFilter::SHOW_DOCUMENT_TYPE,
                 NULL, true);
    
             // The first child is the DTD definition of the XML data, we skip it
             walker.firstChild();
       
            // We obtain the "Groups" node
            walker.nextSibling();
        
            CDMW_DEBUG(FTLogger::GetLogger(),
                       walker.getCurrentNode().getNodeName().transcode());
             
            GroupsParser group(walker.getCurrentNode());
            unsigned int nb_objectgroup = group.get_objectgroup_count();
             
            CDMW_INFO(FTLogger::GetLogger(),
                      "Creation of "<<nb_objectgroup<<" group(s)");
             
            for (unsigned int incog = 0; incog < nb_objectgroup; incog++)
            {
                 bool with_datastore = false;
                 
                 // for each objectgroup
                 ObjectGroupParser objectgroup(group.get_Objectgroup(incog));
                 std::string name = objectgroup.name();
                 std::string type_id = objectgroup.type_id();
                 unsigned int min_num_replicas = objectgroup.min_num_replicas();
                 
        
                 ::FT::Criteria crit;        
                 crit.length(2);
                 
                 ::FT::Name OGDName;
                 OGDName.length(1);
                 OGDName[0].id=CORBA::string_dup(name.c_str());
                 
                 crit[0].nam.length(1);
                 crit[0].nam[0].id="com.thalesgroup.cdmw.ft.Name";
                 crit[0].val <<= OGDName;
        
                 ::FT::Properties prop;
         
                 prop.length(2);
                 prop[0].nam.length(1);
                 prop[0].nam[0].id="org.omg.ft.MinimumNumberReplicas";
                 prop[0].val <<= (CORBA::UShort)min_num_replicas;
        
        
                 LocationsParser locations(objectgroup.get_locations());
                 const CORBA::ULong nb_location = locations.get_location_count();
                 CDMW_INFO(FTLogger::GetLogger(),
                           "Create group :"<<"\n"
                           <<"\t name             : "<<name<<"\n"
                           <<"\t type_id          : "<<type_id<<"\n"
                           <<"\t min_num_replicas : "<<min_num_replicas
                           <<"\t with "<<nb_location<<" location(s)");
        
                 ::FT::FactoryInfos factoryInfos;
                 factoryInfos.length(nb_location);
                 for (CORBA::ULong i = 0; i < nb_location; ++i) 
                 {
                     ::FT::Location loc;
                     LocationParser location(locations.get_location(i));
                     
                     if (location.application().empty())
                     {
                         loc.length(2);
                         loc[1].id = location.process().c_str();
                         loc[1].kind = "processname";
                     }
                     else
                     {
                         loc.length(3);
                         loc[1].id = location.application().c_str();
                         loc[1].kind = "applicationname";
                         loc[2].id = location.process().c_str();
                         loc[2].kind = "processname";
                     }
                     loc[0].id = location.host().c_str();
                     loc[0].kind = "hostname";
    
                     CDMW_INFO(FTLogger::GetLogger(),
                               "\t\t Location "<<i<<":"<<"\n"
                               <<"\t\t\t hostname : "<< location.host()<<"\n"
                               <<"\t\t\t applicationname : "<< location.application() <<"\n"
                               <<"\t\t\t processname : "<< location.process());
                     
                     factoryInfos[i].the_factory = ::FT::GenericFactory::_nil();
                     factoryInfos[i].the_location = loc;
                     ::FT::Criteria factoryCrit;        
                     factoryCrit.length(0);
                     factoryInfos[i].the_criteria = factoryCrit;
                 }
                 
                 prop[1].nam.length(1);
                 prop[1].nam[0].id="org.omg.ft.Factories";
                 prop[1].val <<= factoryInfos;
                 
                 crit[1].nam.length(1);
                 crit[1].nam[0].id="org.omg.ft.FTProperties";
                 crit[1].val <<= prop;
        
                 if (objectgroup.contains_datastoreIDs())
                 {
                     DatastoreIDsParser datastoreids(objectgroup.get_datastoreIDs());
                     unsigned int nb_datastoreID =
                        datastoreids.get_datastoreID_count();
                     ::CdmwFT::StateTransfer::DataStoreIDs dsids;
                     dsids.length(nb_datastoreID);
                     CDMW_INFO(FTLogger::GetLogger(), 
                                "\t with " << nb_datastoreID << " datastore(s)");
                     for (unsigned int incd = 0; incd < nb_datastoreID; incd++)
                     {
                         with_datastore = true;
                         DatastoreIDParser datastoreid(
                               datastoreids.get_datastoreID(incd));
                         unsigned int id = datastoreid.id();
                         CDMW_INFO(FTLogger::GetLogger(), 
                                   "\t\t Datastore ID "<<incd<<":"<<"\n"
                                   <<"\t\t\t id : "<<id);
                         dsids[incd] = id;
                     }
                     
                     if (with_datastore)
                     {
                         crit.length(3);
                         crit[2].nam.length(1);
                         crit[2].nam[0].id="com.thalesgroup.cdmw.ft.DataStoreIdentifiers";
                         crit[2].val <<= dsids;
                         
                     }
                 }
                 
                 ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
                 ::FT::ObjectGroup_var obj_group;
         
                 // create the object group 
                 try
                 {
                     obj_group =  
                        m_replication_manager->create_object(type_id.c_str(), 
                                                             crit, 
                                                             factory_creation_id.out() );
                 }
                 catch( CORBA::SystemException& e )
                 {
                     CDMW_FATAL(FTLogger::GetLogger(),
                                "CORBA System Exception : "<< e);
                     throw;
                 }
                 catch( CORBA::Exception& e )
                 {
                     CDMW_FATAL(FTLogger::GetLogger(),
                               "CORBA User Exception : " << e);
                     throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                                  CORBA::COMPLETED_NO);
                 }
            }

            XMLPlatformUtils::Terminate();
        }
        catch (std::bad_alloc &) 
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "bad_alloc exception raised");

            CDMW_THROW(OutOfMemoryException);
        } 
        catch(const SAXParseException& ex1)
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "XMLException raised: "
                      << to_string(ex1.getMessage()));
            CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(ex1.getMessage())), ex1.getLineNumber());
        }
        catch(const XMLException& ex2)
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "XMLException raised: "
                      << to_string(ex2.getMessage()));
            CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(ex2.getMessage())), ex2.getSrcLine());   
        }
        catch (const DOM_DOMException& e) 
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "DOM_DOMException raised: "
                      << to_string(e.msg));
             CDMW_THROW2(ConfigurationException,
                        to_string(DOMString(e.msg)), e.code);   
       }
    }
    catch( CORBA::SystemException& e )
    {
        XMLPlatformUtils::Terminate();
        CDMW_FATAL(FTLogger::GetLogger(),
                   "CORBA System Exception: " << e );
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw;
    }
    catch( CORBA::Exception& e )
    {
        XMLPlatformUtils::Terminate();
        CDMW_FATAL(FTLogger::GetLogger(),
                   "CORBA System Exception: " << e );
        CORBA::INTERNAL ex(Cdmw::OrbSupport::INTERNAL,
                           CORBA::COMPLETED_NO);
        throw ex;
    }
}


GroupConfigurationMngr::~GroupConfigurationMngr()
    throw()
{
   CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
};


} // End namespace FT
} // End namespace Cdmw


