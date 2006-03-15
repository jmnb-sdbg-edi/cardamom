#!/bin/sh
# =====================================================================
# This file is part of CARDAMOM (R) which is jointly developed by THALES
# and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
# 2000-2003. All rights reserved.
# 
# Copyright (C) THALES 2004-2005. All rights reserved
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

platform_admin.sh --check-only --sys-define V-SMG-001-err1.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err2.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err3.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err4.xml
platform_admin.sh --check-only --app-add-process APP V-SMG-001-err5.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err6.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err7.xml
platform_admin.sh --check-only --app-add-process APP V-SMG-001-err8.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err9.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err10.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err11.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err12.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err13.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err14.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err15.xml
platform_admin.sh --check-only --sys-add-application V-SMG-001-err16.xml
platform_admin.sh --check-only --sys-define V-SMG-001-err17.xml
platform_admin.sh --check-only --sys-define V-SMG-001.xml

