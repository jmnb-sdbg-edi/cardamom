### PARALLELISM 2 HOST

###DEFINE
gr1<-"Parall2HMean.png"			#NomeFile grafico Mean (*.png)
gr2<-"Parall2HThro.png"			#NomeFile grafico Throughput (*.png)
grRO<-"Parall2HRAMObs.png"		#NomeFile grafico RAMObs (*.png)
grR<-"Parall2HRAM.png"			#NomeFile grafico RAM (*.png)
grCO<-"Parall2HCPUObs.png"		#NomeFile grafico CPUObs (*.png)
grC<-"Parall2HCPU.png"			#NomeFile grafico CPU (*.png)
grRRO<-"Parall2HRRAMObs.png"		#NomeFile grafico RRAMObs (*.png)
grRR<-"Parall2HRRAM.png"		#NomeFile grafico RRAM (*.png)
grRCO<-"Parall2HRCPUObs.png"		#NomeFile grafico RCPUObs (*.png)
grRC<-"Parall2HRCPU.png"		#NomeFile grafico RCPU (*.png)
som<-"Parallelism2H.htm"		#NomeFile Sommario (*.htm)

HTML_type<-c("Type: ?")
HTML_delay<-c("Delay: ?")
HTML_host<-c("Host: 2")

is2Host<-TRUE

x<-c(1,2,3,4,5,6,7,8,9,10,12,14,16,18,20,25,30)
nomi<-paste(as.character(x),c("Thread"))
color<-c(1,2,3,4,6,8)		#array dei colori
tipi<-c(1,2,3,4,5,6)		#array degli stili di grafico 
CD_lwd<-2			#spessore dei grafici
thresh<-.99			#soglia di percentile
dec_num <- 2			#numero decimali per stats
png_width<-800; png_height<-600	#Png Dimentions

###ACQUISIZIONE DELTA_C.dat
PAR<-c(    scan("NTHR1/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR2/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR3/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR4/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR5/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR6/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR7/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR8/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR9/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR10/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR12/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR14/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR16/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR18/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR20/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR25/DELTA_C.dat"))
PAR<-c(PAR,scan("NTHR30/DELTA_C.dat"))

###ACQUISIZIONE RAMmonitor.mon
RAM_1<-scan("NTHR1/RAMmonitor.mon")
RAM_2<-scan("NTHR2/RAMmonitor.mon")
RAM_3<-scan("NTHR3/RAMmonitor.mon")
RAM_4<-scan("NTHR4/RAMmonitor.mon")
RAM_5<-scan("NTHR5/RAMmonitor.mon")
RAM_6<-scan("NTHR6/RAMmonitor.mon")
RAM_7<-scan("NTHR7/RAMmonitor.mon")
RAM_8<-scan("NTHR8/RAMmonitor.mon")
RAM_9<-scan("NTHR9/RAMmonitor.mon")
RAM_10<-scan("NTHR10/RAMmonitor.mon")
RAM_11<-scan("NTHR12/RAMmonitor.mon")
RAM_12<-scan("NTHR14/RAMmonitor.mon")
RAM_13<-scan("NTHR16/RAMmonitor.mon")
RAM_14<-scan("NTHR18/RAMmonitor.mon")
RAM_15<-scan("NTHR20/RAMmonitor.mon")
RAM_16<-scan("NTHR25/RAMmonitor.mon")
RAM_17<-scan("NTHR30/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_1<-scan("NTHR1/CPUmonitor.mon")
CPU_2<-scan("NTHR2/CPUmonitor.mon")
CPU_3<-scan("NTHR3/CPUmonitor.mon")
CPU_4<-scan("NTHR4/CPUmonitor.mon")
CPU_5<-scan("NTHR5/CPUmonitor.mon")
CPU_6<-scan("NTHR6/CPUmonitor.mon")
CPU_7<-scan("NTHR7/CPUmonitor.mon")
CPU_8<-scan("NTHR8/CPUmonitor.mon")
CPU_9<-scan("NTHR9/CPUmonitor.mon")
CPU_10<-scan("NTHR10/CPUmonitor.mon")
CPU_11<-scan("NTHR12/CPUmonitor.mon")
CPU_12<-scan("NTHR14/CPUmonitor.mon")
CPU_13<-scan("NTHR16/CPUmonitor.mon")
CPU_14<-scan("NTHR18/CPUmonitor.mon")
CPU_15<-scan("NTHR20/CPUmonitor.mon")
CPU_16<-scan("NTHR25/CPUmonitor.mon")
CPU_17<-scan("NTHR30/CPUmonitor.mon")

###ACQUISIZIONE REMOTE_RAMmonitor.mon
RRAM_1<-scan("NTHR1/REMOTE_RAMmonitor.mon")
RRAM_2<-scan("NTHR2/REMOTE_RAMmonitor.mon")
RRAM_3<-scan("NTHR3/REMOTE_RAMmonitor.mon")
RRAM_4<-scan("NTHR4/REMOTE_RAMmonitor.mon")
RRAM_5<-scan("NTHR5/REMOTE_RAMmonitor.mon")
RRAM_6<-scan("NTHR6/REMOTE_RAMmonitor.mon")
RRAM_7<-scan("NTHR7/REMOTE_RAMmonitor.mon")
RRAM_8<-scan("NTHR8/REMOTE_RAMmonitor.mon")
RRAM_9<-scan("NTHR9/REMOTE_RAMmonitor.mon")
RRAM_10<-scan("NTHR10/REMOTE_RAMmonitor.mon")
RRAM_11<-scan("NTHR12/REMOTE_RAMmonitor.mon")
RRAM_12<-scan("NTHR14/REMOTE_RAMmonitor.mon")
RRAM_13<-scan("NTHR16/REMOTE_RAMmonitor.mon")
RRAM_14<-scan("NTHR18/REMOTE_RAMmonitor.mon")
RRAM_15<-scan("NTHR20/REMOTE_RAMmonitor.mon")
RRAM_16<-scan("NTHR25/REMOTE_RAMmonitor.mon")
RRAM_17<-scan("NTHR30/REMOTE_RAMmonitor.mon")

###ACQUISIZIONE REMOTE_CPUmonitor.mon
RCPU_1<-scan("NTHR1/REMOTE_CPUmonitor.mon")
RCPU_2<-scan("NTHR2/REMOTE_CPUmonitor.mon")
RCPU_3<-scan("NTHR3/REMOTE_CPUmonitor.mon")
RCPU_4<-scan("NTHR4/REMOTE_CPUmonitor.mon")
RCPU_5<-scan("NTHR5/REMOTE_CPUmonitor.mon")
RCPU_6<-scan("NTHR6/REMOTE_CPUmonitor.mon")
RCPU_7<-scan("NTHR7/REMOTE_CPUmonitor.mon")
RCPU_8<-scan("NTHR8/REMOTE_CPUmonitor.mon")
RCPU_9<-scan("NTHR9/REMOTE_CPUmonitor.mon")
RCPU_10<-scan("NTHR10/REMOTE_CPUmonitor.mon")
RCPU_11<-scan("NTHR12/REMOTE_CPUmonitor.mon")
RCPU_12<-scan("NTHR14/REMOTE_CPUmonitor.mon")
RCPU_13<-scan("NTHR16/REMOTE_CPUmonitor.mon")
RCPU_14<-scan("NTHR18/REMOTE_CPUmonitor.mon")
RCPU_15<-scan("NTHR20/REMOTE_CPUmonitor.mon")
RCPU_16<-scan("NTHR25/REMOTE_CPUmonitor.mon")
RCPU_17<-scan("NTHR30/REMOTE_CPUmonitor.mon")

source("ParallelismCore.R")

rm(CD_lwd,color,dec_num,nomi,png_height,png_width,som,thresh,tipi,x,is2Host,
	HTML_delay,HTML_host,HTML_type,PAR,gr1,gr2,grC,grCO,grR,grRO,grRC,grRCO,grRR,grRRO,
	CPU_1,CPU_10,CPU_11,CPU_12,CPU_13,CPU_14,CPU_15,CPU_16,CPU_17,CPU_2,CPU_3,CPU_4,
	CPU_5,CPU_6,CPU_7,CPU_8,CPU_9,
	RAM_1,RAM_10,RAM_11,RAM_12,RAM_13,RAM_14,RAM_15,RAM_16,RAM_17,RAM_2,RAM_3,RAM_4,
	RAM_5,RAM_6,RAM_7,RAM_8,RAM_9,
	RCPU_1,RCPU_10,RCPU_11,RCPU_12,RCPU_13,RCPU_14,RCPU_15,RCPU_16,RCPU_17,RCPU_2,
	RCPU_3,RCPU_4,RCPU_5,RCPU_6,RCPU_7,RCPU_8,RCPU_9,
	RRAM_1,RRAM_10,RRAM_11,RRAM_12,RRAM_13,RRAM_14,RRAM_15,RRAM_16,RRAM_17,RRAM_2,
	RRAM_3,RRAM_4,RRAM_5,RRAM_6,RRAM_7,RRAM_8,RRAM_9)
