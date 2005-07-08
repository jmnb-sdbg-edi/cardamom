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

### INVOCATION POLICY DELAY 100 1 HOST

###DEFINE
gr1<-"InvPold1001HObS.png"		#NomeFile grafico Observation (*.png)
gr2<-"InvPold1001HECDF.png"		#NomeFile grafico ECDF (*.png)
gr3<-"InvPold1001HBox.png"		#NomeFile grafico BoxPlot (*.png)
grRO<-"InvPold1001HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"InvPold1001HRAM.png"		#NomeFile grafico RAM (*.png)
grCO<-"InvPold1001HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"InvPold1001HCPU.png"		#NomeFile grafico CPU (*.png)
som<-"InvocationPolicyd1001H.htm"	#NomeFile Sommario (*.htm)

HTML_type<-c("Type: CR-OW")
HTML_delay<-c("Delay: 100")
HTML_host<-c("Host: 1")

is2Host<-FALSE

nomi<-c("CR ",
	"OW S_NONE",
	"OW S_TRANSPORT",
	"OW S_SERVER",
	"OW S_TARGET")		#array dei nomi 
color<-c(1,2,3,4,6,8)		#array dei colori
tipi<-c(1,2,3,4,5,6)		#array degli stili di grafico 
CD_lwd<-2			#spessore dei grafici
thresh<-.99			#soglia di percentile
dec_num <- 2			#numero decimali per stats
dev_dig <- 3			#digits per deviazione standard
rap<-.3; l1<-.2; l2<-.8		#RAM LEGENDA: dimensione legenda = rap*max(RAM)
png_width<-800; png_height<-600	#Png Dimentions

###ACQUISIZIONE DELTA_C.dat
DC_CR<-scan("CR-100/DELTA_C.dat")
DC_OW_NON<-scan("OW-100/DELTA_C.dat")
DC_OW_TRN<-scan("OW_TRANSPORT-100/DELTA_C.dat")
DC_OW_SRV<-scan("OW_SERVER-100/DELTA_C.dat")
DC_OW_TRG<-scan("OW_TARGET-100/DELTA_C.dat")

###ACQUISIZIONE RAMmonitor.mon
RAM_CR<-scan("CR-100/RAMmonitor.mon")
RAM_OW_NON<-scan("OW-100/RAMmonitor.mon")
RAM_OW_TRN<-scan("OW_TRANSPORT-100/RAMmonitor.mon")
RAM_OW_SRV<-scan("OW_SERVER-100/RAMmonitor.mon")
RAM_OW_TRG<-scan("OW_TARGET-100/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_CR<-scan("CR-100/CPUmonitor.mon")
CPU_OW_NON<-scan("OW-100/CPUmonitor.mon")
CPU_OW_TRN<-scan("OW_TRANSPORT-100/CPUmonitor.mon")
CPU_OW_SRV<-scan("OW_SERVER-100/CPUmonitor.mon")
CPU_OW_TRG<-scan("OW_TARGET-100/CPUmonitor.mon")

source("InvocationPolicyCore.R")

rm(CD_lwd,color,dec_num,dev_dig,l1,l2,nomi,png_height,png_width,rap,thresh,tipi,
	CPU_CR,CPU_OW_NON,CPU_OW_SRV,CPU_OW_TRG,CPU_OW_TRN,
	DC_CR,DC_OW_NON,DC_OW_SRV,DC_OW_TRG,DC_OW_TRN,
	RAM_CR,RAM_OW_NON,RAM_OW_SRV,RAM_OW_TRG,RAM_OW_TRN,
	gr1,gr2,gr3,grC,grCO,grR,grRO,is2Host,som,
	HTML_type,HTML_delay,HTML_host)
