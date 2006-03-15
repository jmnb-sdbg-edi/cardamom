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


#ifndef INCL_DATA_TRANSFER_IMPL_HPP 
#define INCL_DATA_TRANSFER_IMPL_HPP 

#include <iostream>
#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/Codec.hpp>

#include "testcodec_lib/TestCodec.skel.hpp"
#include "testcodec_lib/DataManager.hpp"


namespace Cdmw
{
namespace TestCodec
{

class DataTransfer_impl : 
    virtual public POA_Cdmw::TestCodec::DataTransfer,
    virtual public PortableServer::RefCountServantBase
{
    public:
  
        DataTransfer_impl(CORBA::ORB_ptr orb)
            throw(CORBA::SystemException);
        
        virtual ~DataTransfer_impl()
            throw();
        
        virtual void send_data (const CORBA::OctetSeq& data)
           throw(CORBA::SystemException, Cdmw::TestCodec::DecodingFailure);    


    private:

        DataTransfer_impl(const DataTransfer_impl& rhs)
            throw();
        
        DataTransfer_impl&
        operator=(const DataTransfer_impl& rhs)
            throw();
       

        CORBA::ORB_var m_orb;
        DataManager* m_dataMgr;
        Cdmw::OrbSupport::Codec<Cdmw::TestCodec::TestStruct>* m_codec;

};  // End class DataTransfer_impl


}; // End namespace TestCodec
}; // End namespace Cdmw

#endif // INCL_DATA_TRANSFER_IMPL_HPP

