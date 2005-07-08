#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#*
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *

 rm -rfv generated/ i686-pc-linux-gnu/
 cd data
 ls -1  *.in | cut -d.  --fields 1,2 | xargs rm
 cd ../run
 ls -1  *.in | cut -d.  --fields 1,2 | xargs rm
 cd ../xml
 ls -1  *.in | cut -d.  --fields 1,2 | xargs rm
 cd ..
 rm -rfv /var/ftp/ccmperf
 rm -rfv workDir/
