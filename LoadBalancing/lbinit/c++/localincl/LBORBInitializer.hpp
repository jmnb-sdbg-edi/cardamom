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
 * @brief Implementation for ORB Initializer class.
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 * @author Antonello Cedrone <a.cedrone@vitrociset.it>
 */

#ifndef INCL_CDMW_LB_LBINIT_ORB_INITIALIZER_HPP
#define INCL_CDMW_LB_LBINIT_ORB_INIIALIZER_HPP

// Cdmw Files
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

namespace Cdmw
{
    
namespace LB
{
    
namespace LBInit
{

class ORBInitializer : public PortableInterceptor::ORBInitializer,
                       public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    //constructor
    ORBInitializer(void);
    
    //destructor
    ~ORBInitializer();

    //constructor     
    ORBInitializer(std::string interceptor_side);
    
    //
    // IDL to C++ Mapping
    //
    virtual void pre_init (PortableInterceptor::ORBInitInfo_ptr info)
        throw (CORBA::SystemException);
    
    virtual void post_init (PortableInterceptor::ORBInitInfo_ptr info)
        throw (CORBA::SystemException);

private:
    std::string m_interceptor_side;
};
    
};//End of namespace LBInit

};//End of namespace LB

};//End of namespace Cdmw

#endif // INCL_CDMW_LB_LBINIT_ORB_INITIALIZER_HPP
