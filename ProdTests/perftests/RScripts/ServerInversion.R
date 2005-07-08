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

ser<-scan("OW-0-32000/ABS_S.dat")
cli<-scan("OW-0-32000/ABS_C.dat")

rit_ser<-c(0)
rit_cli<-c(0)

for(i in 1:(dim(as.array(ser))-1)) rit_ser[i]<-(ser[i+1]-ser[i])
for(i in 1:(dim(as.array(cli))-1)) rit_cli[i]<-(cli[i+1]-cli[i])

png("ServerInvABS.png",800,600)
plot(ser,pch=".",col=2,ylim=range(ser,cli),
	main="Absolute Timing for Invocation Benchmark",
	xlab="Invocation Number [#]",ylab="Absolute time [usec]")
points(cli,pch=".",col=4)
legend(1,max(c(ser,cli)),c("ABS_S","ABS_C"),c(2,4))
dev.off()

png("ServerInvRIT.png",800,600)
plot(rit_ser,pch=".",col=2,ylim=range(rit_ser,rit_cli),
	main="Inter-Delay for Invocation Benchmark",
	xlab="Invocation Number [#]",ylab="Inter-Delay [usec]")
points(rit_cli,pch=".",col=4)
legend(1,max(c(rit_ser,rit_cli)),c("ABS_S","ABS_C"),c(2,4))
dev.off()