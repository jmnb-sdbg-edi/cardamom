/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef __PLATFORMADMINUSAGE_HPP__
#define __PLATFORMADMINUSAGE_HPP__

#include <string>

//--------------//
// Declarations //
//--------------//
class CmdUsageException
{
public:
    std::string m_cmdName;

    CmdUsageException( std::string cmdName )
    {
        m_cmdName = cmdName;
    };
};

typedef struct CmdInfo
{
    std::string cmdName;
    std::string cmdInfo;
};


//------------------
// Global variables 
//------------------
class CmdUsage
{
    public:

    static std::string M_cmdUsage;
    static CmdInfo* M_cmdInfoTab;
};


//---------------------//
// Function prototypes //
//---------------------//
std::string usage( std::string cmdName );

#endif //__PLATFORMADMINUSAGE_HPP__

