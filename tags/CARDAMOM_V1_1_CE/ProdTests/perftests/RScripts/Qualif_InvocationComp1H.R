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

###INVOCATION COMPARISON CR-OW 0-100

###DEFINE

is2Host <- FALSE

gr1<-"InvComp1HObs.png"		#NomeFile grafico Observation (*.png)
gr2<-"InvComp1HECDFa.png"	#NomeFile grafico Autoscaled ECDF (*.png)
gr3<-"InvComp1HECDFs.png"	#NomeFile grafico Scaled ECDF (*.png)
gr4<-"InvComp1HBoxa.png"	#NomeFile grafico Autoscaled BoxPlot (*.png)
gr5<-"InvComp1HBoxs.png"	#NomeFile grafico Scaled BoxPlot (*.png)

grRO<-"InvComp1HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"InvComp1HRAM.png"		#NomeFile grafico RAM (*.png)
grCO<-"InvComp1HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"InvComp1HCPU.png"		#NomeFile grafico CPU (*.png)
som<-"InvocationComp1H.htm"		#NomeFile Sommario (*.htm)

HTML_type<-c("Type: CR - OW")
HTML_delay<-c("Delay: 0")
HTML_host<-c("Host:1")

nomi<-c("CR del:0",
	"OW cc del:0")          #array dei nomi 
color<-c(1,2,3,4,6,8)		#array dei colori
tipi<-c(1,2,3,4,5,6)		#array degli stili di grafico 
CD_lwd<-2			#spessore dei grafici
threshold <- 1                  #soglia percentuale outliers
dec_num <- 2			#numero decimali per stats
dev_dig <- 3			#digits per deviazione standard
rap<-.3; l1<-.2; l2<-.8		#RAM LEGENDA: dimensione legenda = rap*max(RAM)
png_width<-800; png_height<-600	#Png Dimentions

###ACQUISIZIONE DELTA_C.dat
DC_CR_0<-scan("CR-0/DELTA_C.dat")
DC_OW_0<-scan("OW-0/DELTA_C.dat")

###ACQUISIZIONE RAMmonitor.mon
RAM_CR_0<-scan("CR-0/RAMmonitor.mon")
RAM_OW_0<-scan("OW-0/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_CR_0<-scan("CR-0/CPUmonitor.mon")
CPU_OW_0<-scan("OW-0/CPUmonitor.mon")

source("Qualif_InvocationCompCore.R")

rm(CD_lwd,color,dec_num,dev_dig,is2Host,l1,l2,nomi,png_height,png_width,rap,tipi,
	HTML_type,HTML_delay,HTML_host,
	CPU_CR_0,CPU_OW_0,
	DC_CR_0,DC_OW_0,
	RAM_CR_0,RAM_OW_0,
	gr1,gr2,gr3,gr4,gr5,grRO,grR,grCO,grC,som)
