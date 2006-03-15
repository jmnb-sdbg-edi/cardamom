###INVOCATION ROUND TRIP 0-100 2 HOST

###DEFINE
is2Host <- TRUE

gr1<-"InvRT2HObs.png"		#NomeFile grafico Observation (*.png)
gr2<-"InvRT2HECDFa.png"		#NomeFile grafico Autoscaled ECDF (*.png)
gr3<-"InvRT2HECDFs.png"		#NomeFile grafico Scaled ECDF (*.png)
gr4<-"InvRT2HBoxa.png"		#NomeFile grafico Autoscaled BoxPlot (*.png)
gr5<-"InvRT2HBoxs.png"		#NomeFile grafico Scaled BoxPlot (*.png)
grRO<-"InvRT2HRAMObs.png"	#NomeFile grafico RAMObs (*.png)
grR<-"InvRT2HRAM.png"		#NomeFile grafico RAM (*.png)
grCO<-"InvRT2HCPUObs.png"	#NomeFile grafico CPUObs (*.png)
grC<-"InvRT2HCPU.png"		#NomeFile grafico CPU (*.png)
grRRO<-"InvRT2HRRAMObs.png"	#NomeFile grafico RRAMObs (*.png)
grRR<-"InvRT2HRRAM.png"		#NomeFile grafico RRAM (*.png)
grRCO<-"InvRT2HRCPUObs.png"	#NomeFile grafico RCPUObs (*.png)
grRC<-"InvRT2HRCPU.png"		#NomeFile grafico RCPU (*.png)
som<-"InvocationRT2H.htm"	#NomeFile Sommario (*.htm)

HTML_type<-c("Type:OW")
HTML_delay<-c("Delay:0-100")
HTML_host<-c("Host:2")

nomi<-c("RT del:0",
	"RT del:100",
	"OW cs del:0",
	"OW cs del:100")	#array dei nomi 
color<-c(1,2,3,4,6,8)		#array dei colori
tipi<-c(1,2,3,4,5,6)		#array degli stili di grafico 
CD_lwd<-2			#spessore dei grafici
thresh<-.99			#soglia di percentile
dec_num <- 2			#numero decimali per stats
dev_dig <- 3			#digits per deviazione standard
rap<-.3; l1<-.2; l2<-.8		#RAM LEGENDA: dimensione legenda = rap*max(RAM)
png_width<-800; png_height<-600	#Png Dimentions

###ACQUISIZIONE DELTA_C.dat
RT_0<-(scan("RT-0/ABS_2.dat")-scan("RT-0/ABS_1.dat"))/2
RT_100<-(scan("RT-100/ABS_2.dat")-scan("RT-100/ABS_1.dat"))/2
OW_0<-(scan("../../A-020/runset1/OW-0/ABS_S.dat")-scan("../../A-020/runset1/OW-0/ABS_C.dat"))
OW_100<-(scan("../../A-020/runset1/OW-100/ABS_S.dat")-scan("../../A-020/runset1/OW-100/ABS_C.dat"))

###ACQUISIZIONE RAMmonitor.mon
RAM_RT_0<-scan("RT-0/RAMmonitor.mon")
RAM_RT_100<-scan("RT-100/RAMmonitor.mon")
RAM_OW_0<-scan("../../A-020/runset1/OW-0/RAMmonitor.mon")
RAM_OW_100<-scan("../../A-020/runset1/OW-100/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_RT_0<-scan("RT-0/CPUmonitor.mon")
CPU_RT_100<-scan("RT-100/CPUmonitor.mon")
CPU_OW_0<-scan("../../A-020/runset1/OW-0/CPUmonitor.mon")
CPU_OW_100<-scan("../../A-020/runset1/OW-100/CPUmonitor.mon")

###ACQUISIZIONE REMOTE_RAMmonitor.mon
RRAM_RT_0<-scan("RT-0/REMOTE_RAMmonitor.mon")
RRAM_RT_100<-scan("RT-100/REMOTE_RAMmonitor.mon")
RRAM_OW_0<-scan("../../A-020/runset1/OW-0/REMOTE_RAMmonitor.mon")
RRAM_OW_100<-scan("../../A-020/runset1/OW-100/REMOTE_RAMmonitor.mon")

###ACQUISIZIONE REMOTE_CPUmonitor.mon
RCPU_RT_0<-scan("RT-0/REMOTE_CPUmonitor.mon")
RCPU_RT_100<-scan("RT-100/REMOTE_CPUmonitor.mon")
RCPU_OW_0<-scan("../../A-020/runset1/OW-0/REMOTE_CPUmonitor.mon")
RCPU_OW_100<-scan("../../A-020/runset1/OW-100/REMOTE_CPUmonitor.mon")

source("InvocationRTCore.R")

rm(CD_lwd,color,dec_num,dev_dig,is2Host,l1,l2,nomi,png_height,png_width,rap,thresh,tipi,
	HTML_type,HTML_delay,HTML_host,	OW_0,OW_100,RT_0,RT_100,
	CPU_RT_0,CPU_RT_100,CPU_OW_0,CPU_OW_100,
	RAM_RT_0,RAM_RT_100,RAM_OW_0,RAM_OW_100,
	RCPU_RT_0,RCPU_RT_100,RCPU_OW_0,RCPU_OW_100,
	RRAM_RT_0,RRAM_RT_100,RRAM_OW_0,RRAM_OW_100,
	gr1,gr2,gr3,gr4,gr5,grRO,grR,grCO,grC,grRRO,grRR,grRCO,grRC,som)
