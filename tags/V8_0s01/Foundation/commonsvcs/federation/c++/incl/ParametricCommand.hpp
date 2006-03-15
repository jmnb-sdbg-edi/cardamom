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
#ifndef _CDMW_COMMONSVCS_PARAMETRIC_COMMAND_HPP_
#define _CDMW_COMMONSVCS_PARAMETRIC_COMMAND_HPP_

#include "Foundation/orbsupport/TypeTraits.hpp"

namespace Cdmw {
    namespace CommonSvcs {
        template <typename DATA_TYPE>
        class ParametricCommand;
    }
}

/**
 * This class defines a command that operates on a parametric data.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
template <typename DATA_TYPE>
class Cdmw::CommonSvcs::ParametricCommand
{
public:
    /**
     * Destroy a <code>ParametricCommand</code> instance.
     */
    virtual ~ParametricCommand() { }
    
public:
    /**
     * Execute a user defined command using the argument as context.
     *
     * @param data the data used by the command as context.
     */
    virtual void
    execute(typename Cdmw::OrbSupport::type_traits<DATA_TYPE>::_in_type data) = 0;
};

#endif /* _CDMW_COMMONSVCS_PARAMETRIC_COMMAND_HPP_ */
