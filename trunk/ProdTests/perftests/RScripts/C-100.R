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

results <- "scenario.result"
write("********* PERFORMANCE TEST C-100 ***********", results)
write("*** please note:  unit is (1 invocation) ***", results, append=TRUE)

###ACQUISITION OF COUNTER.dat
COUNTER_THR_10<-scan("THR-10/COUNTER.dat")

write("", results, append=TRUE)
write("THR-10 results", results, append=TRUE)
write("--------------", results, append=TRUE)
write(ncolumns=2, c("NOMINAL =", COUNTER_THR_10[1]), results, append=TRUE)
write(ncolumns=2, c("COUNTER =", COUNTER_THR_10[2]), results, append=TRUE)

rm(COUNTER_THR_10, results)
