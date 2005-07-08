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

### DISPATCHER 1Host

###DEFINE

is2Host<-FALSE

gr1<-"Disp1HObS.png"			#NomeFile grafico Observation (*.png)
gr2<-"Disp1HMean.png"			#NomeFile grafico Mean (*.png)
gr3<-"Disp1HStd.png"			#NomeFile grafico StdDev (*.png)
gr4<-"Disp1HBox.png"			#NomeFile grafico BoxPlot (*.png)
grRO<-"Disp1HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"Disp1HRAM.png"			#NomeFile grafico RAM (*.png)
grCO<-"Disp1HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"Disp1HCPU.png"			#NomeFile grafico CPU (*.png)
som<-"Dispatcher1H.htm"			#NomeFile Sommario (*.htm)

HTML_type<-"Type:Call&Return"
HTML_delay<-"Delay:0-100"
HTML_host<-"Host:1"

x<-c(2,3,4,5,6,7,8,9,10,
	12,14,16,18,20,25,30)		#Ascisse
nomi<-paste(x,"Servants")			#Array nomi
color<-c(1,2,3,4,0) 			#array dei colori
tipi<-c(1,2,3,4)			#array degli stili di grafico 
CD_lwd<-2				#spessore dei grafici
thresh<-.99				#soglia di percentile
dec_num <- 2				#numero decimali per stats
dev_dig <- 3				#digits per deviazione standard
png_width<-800; png_height<-600		#Png Dimentions


DC_DISP_1<-scan("NSRV2/DELTA_C.dat")	###ACQUISIZIONE DELTA_C.dat
DC_DISP_2<-scan("NSRV3/DELTA_C.dat")
DC_DISP_3<-scan("NSRV4/DELTA_C.dat")
DC_DISP_4<-scan("NSRV5/DELTA_C.dat")
DC_DISP_5<-scan("NSRV6/DELTA_C.dat")
DC_DISP_6<-scan("NSRV7/DELTA_C.dat")
DC_DISP_7<-scan("NSRV8/DELTA_C.dat")
DC_DISP_8<-scan("NSRV9/DELTA_C.dat")
DC_DISP_9<-scan("NSRV10/DELTA_C.dat")
DC_DISP_10<-scan("NSRV12/DELTA_C.dat")
DC_DISP_11<-scan("NSRV14/DELTA_C.dat")
DC_DISP_12<-scan("NSRV16/DELTA_C.dat")
DC_DISP_13<-scan("NSRV18/DELTA_C.dat")
DC_DISP_14<-scan("NSRV20/DELTA_C.dat")
DC_DISP_15<-scan("NSRV25/DELTA_C.dat")
DC_DISP_16<-scan("NSRV30/DELTA_C.dat")

RAM_1<-scan("NSRV2/RAMmonitor.mon")	###ACQUISIZIONE RAMmonitor.mon
RAM_2<-scan("NSRV3/RAMmonitor.mon")
RAM_3<-scan("NSRV4/RAMmonitor.mon")
RAM_4<-scan("NSRV5/RAMmonitor.mon")
RAM_5<-scan("NSRV6/RAMmonitor.mon")
RAM_6<-scan("NSRV7/RAMmonitor.mon")
RAM_7<-scan("NSRV8/RAMmonitor.mon")
RAM_8<-scan("NSRV9/RAMmonitor.mon")
RAM_9<-scan("NSRV10/RAMmonitor.mon")
RAM_10<-scan("NSRV12/RAMmonitor.mon")
RAM_11<-scan("NSRV14/RAMmonitor.mon")
RAM_12<-scan("NSRV16/RAMmonitor.mon")
RAM_13<-scan("NSRV18/RAMmonitor.mon")
RAM_14<-scan("NSRV20/RAMmonitor.mon")
RAM_15<-scan("NSRV25/RAMmonitor.mon")
RAM_16<-scan("NSRV30/RAMmonitor.mon")

CPU_1<-scan("NSRV2/CPUmonitor.mon")	###ACQUISIZIONE CPUmonitor.mon
CPU_2<-scan("NSRV3/CPUmonitor.mon")
CPU_3<-scan("NSRV4/CPUmonitor.mon")
CPU_4<-scan("NSRV5/CPUmonitor.mon")
CPU_5<-scan("NSRV6/CPUmonitor.mon")
CPU_6<-scan("NSRV7/CPUmonitor.mon")
CPU_7<-scan("NSRV8/CPUmonitor.mon")
CPU_8<-scan("NSRV9/CPUmonitor.mon")
CPU_9<-scan("NSRV10/CPUmonitor.mon")
CPU_10<-scan("NSRV12/CPUmonitor.mon")
CPU_11<-scan("NSRV14/CPUmonitor.mon")
CPU_12<-scan("NSRV16/CPUmonitor.mon")
CPU_13<-scan("NSRV18/CPUmonitor.mon")
CPU_14<-scan("NSRV20/CPUmonitor.mon")
CPU_15<-scan("NSRV25/CPUmonitor.mon")
CPU_16<-scan("NSRV30/CPUmonitor.mon")

source("DispatcherCore.R")

rm(	CD_lwd,color,dec_num,dev_dig,nomi,png_height,png_width,thresh,tipi,x,
	is2Host,gr1,gr2,gr3,gr4,grR,grRO,grC,grCO,som,HTML_type,HTML_delay,HTML_host,
	DC_DISP_1,DC_DISP_2,DC_DISP_3,DC_DISP_4,
	DC_DISP_5,DC_DISP_6,DC_DISP_7,DC_DISP_8,
	DC_DISP_9,DC_DISP_10,DC_DISP_11,DC_DISP_12,
	DC_DISP_13,DC_DISP_14,DC_DISP_15,DC_DISP_16,
	RAM_1,RAM_2,RAM_3,RAM_4,
	RAM_5,RAM_6,RAM_7,RAM_8,
	RAM_9,RAM_10,RAM_11,RAM_12,
	RAM_13,RAM_14,RAM_15,RAM_16,
	CPU_1,CPU_2,CPU_3,CPU_4,
	CPU_5,CPU_6,CPU_7,CPU_8,
	CPU_9,CPU_10,CPU_11,CPU_12,
	CPU_13,CPU_14,CPU_15,CPU_16)
