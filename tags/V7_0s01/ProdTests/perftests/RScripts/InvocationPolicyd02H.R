### INVOCATION POLICY DELAY 0 2 HOST

###DEFINE
gr1<-"InvPold02HObS.png"		#NomeFile grafico Observation (*.png)
gr2<-"InvPold02HECDF.png"		#NomeFile grafico ECDF (*.png)
gr3<-"InvPold02HBox.png"		#NomeFile grafico BoxPlot (*.png)
grRO<-"InvPold02HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"InvPold02HRAM.png"		#NomeFile grafico RAM (*.png)
grCO<-"InvPold02HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"InvPold02HCPU.png"		#NomeFile grafico CPU (*.png)
grRRO<-"InvPold02HRRAMObs.png"		#NomeFile grafico RRAMObs (*.png)
grRR<-"InvPold02HRRAM.png"		#NomeFile grafico RRAM (*.png)
grRCO<-"InvPold02HRCPUObs.png"		#NomeFile grafico RCPUObs (*.png)
grRC<-"InvPold02HRCPU.png"		#NomeFile grafico RCPU (*.png)
som<-"InvocationPolicyd02H.htm"		#NomeFile Sommario (*.htm)

HTML_type<-c("Type: CR-OW")
HTML_delay<-c("Delay: 0")
HTML_host<-c("Host: 2")

is2Host<-TRUE

nomi<-c("CR",
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
DC_CR<-scan("CR-0/DELTA_C.dat")
DC_OW_NON<-scan("OW-0/DELTA_C.dat")
DC_OW_TRN<-scan("OW_TRANSPORT-0/DELTA_C.dat")
DC_OW_SRV<-scan("OW_SERVER-0/DELTA_C.dat")
DC_OW_TRG<-scan("OW_TARGET-0/DELTA_C.dat")

###ACQUISIZIONE RAMmonitor.mon
RAM_CR<-scan("CR-0/RAMmonitor.mon")
RAM_OW_NON<-scan("OW-0/RAMmonitor.mon")
RAM_OW_TRN<-scan("OW_TRANSPORT-0/RAMmonitor.mon")
RAM_OW_SRV<-scan("OW_SERVER-0/RAMmonitor.mon")
RAM_OW_TRG<-scan("OW_TARGET-0/RAMmonitor.mon")

###ACQUISIZIONE REMOTE_RAMmonitor.mon
RRAM_CR<-scan("CR-0/REMOTE_RAMmonitor.mon")
RRAM_OW_NON<-scan("OW-0/REMOTE_RAMmonitor.mon")
RRAM_OW_TRN<-scan("OW_TRANSPORT-0/REMOTE_RAMmonitor.mon")
RRAM_OW_SRV<-scan("OW_SERVER-0/REMOTE_RAMmonitor.mon")
RRAM_OW_TRG<-scan("OW_TARGET-0/REMOTE_RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_CR<-scan("CR-0/CPUmonitor.mon")
CPU_OW_NON<-scan("OW-0/CPUmonitor.mon")
CPU_OW_TRN<-scan("OW_TRANSPORT-0/CPUmonitor.mon")
CPU_OW_SRV<-scan("OW_SERVER-0/CPUmonitor.mon")
CPU_OW_TRG<-scan("OW_TARGET-0/CPUmonitor.mon")

###ACQUISIZIONE REMOTE_CPUmonitor.mon
RCPU_CR<-scan("CR-0/REMOTE_CPUmonitor.mon")
RCPU_OW_NON<-scan("OW-0/REMOTE_CPUmonitor.mon")
RCPU_OW_TRN<-scan("OW_TRANSPORT-0/REMOTE_CPUmonitor.mon")
RCPU_OW_SRV<-scan("OW_SERVER-0/REMOTE_CPUmonitor.mon")
RCPU_OW_TRG<-scan("OW_TARGET-0/REMOTE_CPUmonitor.mon")

source("InvocationPolicyCore.R")

rm(CD_lwd,color,dec_num,dev_dig,l1,l2,nomi,png_height,png_width,rap,thresh,tipi,
	CPU_CR,CPU_OW_NON,CPU_OW_SRV,CPU_OW_TRG,CPU_OW_TRN,
	RCPU_CR,RCPU_OW_NON,RCPU_OW_SRV,RCPU_OW_TRG,RCPU_OW_TRN,
	DC_CR,DC_OW_NON,DC_OW_SRV,DC_OW_TRG,DC_OW_TRN,
	RAM_CR,RAM_OW_NON,RAM_OW_SRV,RAM_OW_TRG,RAM_OW_TRN,
	RRAM_CR,RRAM_OW_NON,RRAM_OW_SRV,RRAM_OW_TRG,RRAM_OW_TRN,
	gr1,gr2,gr3,grC,grCO,grR,grRO,grRC,grRCO,grRR,grRRO,is2Host,som,
	HTML_type,HTML_delay,HTML_host)
