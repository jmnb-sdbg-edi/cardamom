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


#include "testtracelibrary/PILOT_FlushArea.hpp"
#include "testtracelibrary/PILOT_FlushAreaMngr.hpp"
#include "testtracelibrary/PILOT_StreamMngr.hpp"
#include "testtracelibrary/PILOT_FilterMngr.hpp"
#include "testtracelibrary/PILOT_Trace.hpp"

int main(int argc, char** argv )
{

    Cdmw::Trace::PILOT_StreamMngr pilot_StreamMngr;
    pilot_StreamMngr.start();

    Cdmw::Trace::PILOT_FilterMngr pilot_FilterMngr;
    pilot_FilterMngr.start();
    
    Cdmw::Trace::PILOT_FlushArea pilot_FlushArea;
    pilot_FlushArea.start();

    Cdmw::Trace::PILOT_FlushAreaMngr pilot_FlushAreaMngr;
    pilot_FlushAreaMngr.start();

    Cdmw::Trace::PILOT_Trace pilot_Trace;
    pilot_Trace.start();

}


