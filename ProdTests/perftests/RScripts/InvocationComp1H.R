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
HTML_delay<-c("Delay: 0 - 100")
HTML_host<-c("Host:1")

nomi<-c("CR del:0",
	"CR del:100",
	"OW cc del:0",
	"OW cc del:100",
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
DC_CR_0<-scan("CR-0/DELTA_C.dat")
DC_CR_100<-scan("CR-100/DELTA_C.dat")
DC_OW_0<-scan("OW-0/DELTA_C.dat")
DC_OW_100<-scan("OW-100/DELTA_C.dat")
DA_OW_0<-(scan("OW-0/ABS_S.dat")-scan("OW-0/ABS_C.dat"))
DA_OW_100<-(scan("OW-100/ABS_S.dat")-scan("OW-100/ABS_C.dat"))

###ACQUISIZIONE RAMmonitor.mon
RAM_CR_0<-scan("CR-0/RAMmonitor.mon")
RAM_CR_100<-scan("CR-100/RAMmonitor.mon")
RAM_OW_0<-scan("OW-0/RAMmonitor.mon")
RAM_OW_100<-scan("OW-100/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_CR_0<-scan("CR-0/CPUmonitor.mon")
CPU_CR_100<-scan("CR-100/CPUmonitor.mon")
CPU_OW_0<-scan("OW-0/CPUmonitor.mon")
CPU_OW_100<-scan("OW-100/CPUmonitor.mon")

source("InvocationCompCore.R")

rm(CD_lwd,color,dec_num,dev_dig,is2Host,l1,l2,nomi,png_height,png_width,rap,thresh,tipi,
	HTML_type,HTML_delay,HTML_host,
	CPU_CR_0,CPU_CR_100,CPU_OW_0,CPU_OW_100,
	DA_OW_0,DA_OW_100,DC_CR_0,DC_CR_100,DC_OW_0,DC_OW_100,
	RAM_CR_0,RAM_CR_100,RAM_OW_0,RAM_OW_100,
	gr1,gr2,gr3,gr4,gr5,grRO,grR,grCO,grC,som)
