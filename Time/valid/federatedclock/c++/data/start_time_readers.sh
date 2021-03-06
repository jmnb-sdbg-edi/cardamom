#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. All rights reserved.
# 
# Copyright (C) SELEX-SI 2004-2005. All rights reserved
# 
# CARDAMOM is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
# License for more details.
# 
# You should have received a copy of the GNU Library General Public
# License along with CARDAMOM; see the file COPYING. If not, write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# =====================================================================

xterm  -geometry +0+0 -sb -sl 5000 -e TimeReader  --CdmwFederatedCtrlClock --CdmwLocalisationService=4883 -ORBInitRef ClockService=corbaloc::localhost:4883/ClockService &
xterm  -geometry -10+0 -sb -sl 5000 -e TimeReader  --CdmwFederatedCtrlClock --CdmwLocalisationService=15555 -ORBInitRef ClockService=corbaloc::localhost:15555/ClockService &
xterm  -geometry +0-10 -sb -sl 5000 -e TimeReader  --CdmwFederatedCtrlClock --CdmwLocalisationService=15554 -ORBInitRef ClockService=corbaloc::localhost:15554/ClockService &

