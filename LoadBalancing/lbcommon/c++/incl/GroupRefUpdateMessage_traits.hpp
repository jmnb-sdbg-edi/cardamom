/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef _CDMW_LB_GROUP_REF_CACHE_MESSAGE_CODEC_HPP_
#define _CDMW_LB_GROUP_REF_CACHE_MESSAGE_CODEC_HPP_

#include "Foundation/orbsupport/TypeTraits.hpp"
#include "LoadBalancing/lbcommon/GroupRefCache.stub.hpp"


namespace Cdmw { 
    namespace OrbSupport { 
        template<> 
        struct type_traits< ::CdmwLB::GroupRefUpdateMessage > 
        { 
            typedef ::CdmwLB::GroupRefUpdateMessage::_var_type            _var_type; 
            typedef const ::CdmwLB::GroupRefUpdateMessage&                _in_type; 
            typedef ::CdmwLB::GroupRefUpdateMessage&                      _inout_type; 
            typedef ::CdmwLB::GroupRefUpdateMessage*&                     _out_type; 
            typedef ::CdmwLB::GroupRefUpdateMessage*                      _retn_type;  
 
            static _retn_type _default_value() { 
                return new ::CdmwLB::GroupRefUpdateMessage(); 
            }; 
            static _in_type _in(const _var_type& v) { 
                return v.in(); 
            }; 
            static _retn_type _retn(_var_type & v) { 
                return v._retn(); 
            }; 
            static _inout_type _inout(_var_type & v) { 
                return v.inout(); 
            }; 
            static _retn_type _copy(_in_type i) { 
                return new ::CdmwLB::GroupRefUpdateMessage(i); 
            }; 
        }; 
    }
}


#endif /* _CDMW_LB_GROUP_REF_CACHE_MESSAGE_CODEC_HPP_ */
