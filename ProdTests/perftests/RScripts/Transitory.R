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


CR_100<-scan("CR-100/DELTA_C.dat")
OW_100<-scan("OW-100/DELTA_C.dat")
CR_0<-scan("CR-0/DELTA_C.dat")
OW_0<-scan("OW-0/DELTA_C.dat")

xrange<-c(1,50)
yrange<-c(100,1500)
xdom<-xrange[2]-xrange[1]

png("Transitory.png",800,600)
plot(CR_100,
	ylim=yrange,xlim=xrange,
	type="l",col=2,
	main="Transitory",xlab="Observation number [#]",ylab="Latency [usec]")
points(OW_100,type="l",col=3)
points(CR_0,type="l",col=4)
points(OW_0,type="l",col=6)
legend(xrange[2]-.16*xdom,yrange[2],
	c("CR del:100","OW del:100","CR:del 0","OW:del 0"),
	c(2,3,4,6))
dev.off()