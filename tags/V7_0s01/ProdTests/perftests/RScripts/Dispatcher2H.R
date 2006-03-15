### DISPATCHER 2Host

###DEFINE

is2Host<-TRUE

gr1<-"Disp2HObS.png"			#NomeFile grafico Observation (*.png)
gr2<-"Disp2HMean.png"			#NomeFile grafico Mean (*.png)
gr3<-"Disp2HStd.png"			#NomeFile grafico StdDev (*.png)
gr4<-"Disp2HBox.png"			#NomeFile grafico BoxPlot (*.png)
grRO<-"Disp2HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"Disp2HRAM.png"			#NomeFile grafico RAM (*.png)
grCO<-"Disp2HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"Disp2HCPU.png"			#NomeFile grafico CPU (*.png)
grRRO<-"Disp2HRRAMObs.png"		#NomeFile grafico RRAMObs (*.png)
grRR<-"Disp2HRRAM.png"			#NomeFile grafico RRAM (*.png)
grRCO<-"Disp2HRCPUObs.png"		#NomeFile grafico RCPUObs (*.png)
grRC<-"Disp2HRCPU.png"			#NomeFile grafico RCPU (*.png)
som<-"Dispatcher2H.htm"			#NomeFile Sommario (*.htm)

HTML_type<-"Type:Call&Return"
HTML_delay<-"Delay:0-100"
HTML_host<-"Host:2"

x<-c(2,3,4,5,6,7,8,9,10,
	12,14,16,18,20,25,30)		#Ascisse
nomi<-paste(x,"Servants")		#Array nomi
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

RRAM_1<-scan("NSRV2/REMOTE_RAMmonitor.mon")	###ACQUISIZIONE REMOTE_RAMmonitor.mon
RRAM_2<-scan("NSRV3/REMOTE_RAMmonitor.mon")
RRAM_3<-scan("NSRV4/REMOTE_RAMmonitor.mon")
RRAM_4<-scan("NSRV5/REMOTE_RAMmonitor.mon")
RRAM_5<-scan("NSRV6/REMOTE_RAMmonitor.mon")
RRAM_6<-scan("NSRV7/REMOTE_RAMmonitor.mon")
RRAM_7<-scan("NSRV8/REMOTE_RAMmonitor.mon")
RRAM_8<-scan("NSRV9/REMOTE_RAMmonitor.mon")
RRAM_9<-scan("NSRV10/REMOTE_RAMmonitor.mon")
RRAM_10<-scan("NSRV12/REMOTE_RAMmonitor.mon")
RRAM_11<-scan("NSRV14/REMOTE_RAMmonitor.mon")
RRAM_12<-scan("NSRV16/REMOTE_RAMmonitor.mon")
RRAM_13<-scan("NSRV18/REMOTE_RAMmonitor.mon")
RRAM_14<-scan("NSRV20/REMOTE_RAMmonitor.mon")
RRAM_15<-scan("NSRV25/REMOTE_RAMmonitor.mon")
RRAM_16<-scan("NSRV30/REMOTE_RAMmonitor.mon")

RCPU_1<-scan("NSRV2/REMOTE_CPUmonitor.mon")	###ACQUISIZIONE REMOTE_CPUmonitor.mon
RCPU_2<-scan("NSRV3/REMOTE_CPUmonitor.mon")
RCPU_3<-scan("NSRV4/REMOTE_CPUmonitor.mon")
RCPU_4<-scan("NSRV5/REMOTE_CPUmonitor.mon")
RCPU_5<-scan("NSRV6/REMOTE_CPUmonitor.mon")
RCPU_6<-scan("NSRV7/REMOTE_CPUmonitor.mon")
RCPU_7<-scan("NSRV8/REMOTE_CPUmonitor.mon")
RCPU_8<-scan("NSRV9/REMOTE_CPUmonitor.mon")
RCPU_9<-scan("NSRV10/REMOTE_CPUmonitor.mon")
RCPU_10<-scan("NSRV12/REMOTE_CPUmonitor.mon")
RCPU_11<-scan("NSRV14/REMOTE_CPUmonitor.mon")
RCPU_12<-scan("NSRV16/REMOTE_CPUmonitor.mon")
RCPU_13<-scan("NSRV18/REMOTE_CPUmonitor.mon")
RCPU_14<-scan("NSRV20/REMOTE_CPUmonitor.mon")
RCPU_15<-scan("NSRV25/REMOTE_CPUmonitor.mon")
RCPU_16<-scan("NSRV30/REMOTE_CPUmonitor.mon")

source("DispatcherCore.R")

rm(	CD_lwd,color,dec_num,dev_dig,nomi,png_height,png_width,thresh,tipi,x,
	is2Host,gr1,gr2,gr3,gr4,grR,grRO,grC,grCO,grRR,grRRO,grRC,grRCO,som,
	HTML_type,HTML_delay,HTML_host,
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
	CPU_13,CPU_14,CPU_15,CPU_16,
	RRAM_1,RRAM_2,RRAM_3,RRAM_4,
	RRAM_5,RRAM_6,RRAM_7,RRAM_8,
	RRAM_9,RRAM_10,RRAM_11,RRAM_12,
	RRAM_13,RRAM_14,RRAM_15,RRAM_16,
	RCPU_1,RCPU_2,RCPU_3,RCPU_4,
	RCPU_5,RCPU_6,RCPU_7,RCPU_8,
	RCPU_9,RCPU_10,RCPU_11,RCPU_12,
	RCPU_13,RCPU_14,RCPU_15,RCPU_16)
