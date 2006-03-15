### MARSHALLING CallReturn Delay0 1Host (4 to 64k length)

###DEFINE

is2Host<-FALSE

gr1<-"MarshCRd01HObS.png"		#Filename of graph Observation (*.png)
gr2<-"MarshCRd01HMean.png"		#Filename of graph Mean (*.png)
gr3<-"MarshCRd01HStd.png"		#Filename of graph StdDev (*.png)
gr4<-"MarshCRd01HBox.png"		#Filename of graph BoxPlot (*.png)
gr5<-"MarshCRd01HCCMMean.png"		#Filename of graph MeanComparison (*.png)
gr5abs<-"MarshCRd01HCCMMean_abs.png"	#Filename of graph Absolute MeanComparison (*.png)
gr6<-"MarshCRd01HCCMMax.png"		#Filename of graph MaxComparison (*.png)
gr7<-"MarshCRd01HCCMQ3.png"		#Filename of graph Q3Comparison (*.png)
gr8<-"MarshCRd01HCCMMedian.png"		#Filename of graph MedianComp (*.png)
gr9<-"MarshCRd01HCCMQ1.png"		#Filename of graph Q1Comparison (*.png)
gr10<-"MarshCRd01HCCMMin.png"		#Filename of graph MinComparison (*.png)
grRO<-"MarshCRd01HRAMObs.png"		#Filename of graph RAMObs (*.png)
grR<-"MarshCRd01HRAM.png"		#Filename of graph RAM (*.png)
grCO<-"MarshCRd01HCPUObs.png"		#Filename of graph CPUObs (*.png)
grC<-"MarshCRd01HCPU.png"		#Filename of graph CPU (*.png)
som<-"MarshallingCRd01HCCM.htm"		#Filename of report Sommario (*.htm)

HTML_type<-"Type:Call&Return (CCM)"
HTML_delay<-"Delay:0"
HTML_host<-"Host:1"

nomi<-c("Any","Float","Long","Record")	#Array nomi
nomi_leg<-c("Any 8","Any 16","Any 32","Any 64","Any 128","Any 256","Any 512",
	"Any 1k","Any 2k","Any 4k","Any 8k","Any 16k","Any 32k","Any 64k",
	"Float 4","Float 8","Float 16","Float 32","Float 64","Float 128",
	"Float 256","Float 512","Float 1k","Float 2k","Float 4k","Float 8k",
	"Float 16k","Float 32k","Float 64k",
	"Long 4","Long 8","Long 16","Long 32","Long 64","Long 128","Long 256",
	"Long 512","Long 1k","Long 2k","Long 4k","Long 8k","Long 16k",
	"Long 32k","Long 64k",
	"Record 8","Record 16","Record 32","Record 64","Record 128",
	"Record 256","Record 512","Record 1k","Record 2k","Record 4k",
	"Record 8k","Record 16k","Record 32k","Record 64k")
					#Array nomi legenda
color<-c(1,2,3,4,0) 			#array dei colori
tipi<-c(1,2,3,4)			#array degli stili di grafico 
CD_lwd<-2				#spessore dei grafici
thresh<-.99				#soglia di percentile
dec_num <- 2				#numero decimali per stats
dev_dig <- 3				#digits per deviazione standard
png_width<-800; png_height<-600		#Png Dimentions

###ACQUISIZIONE DELTA_C (CCM)
OBS_A_8<-scan("CR-0-ANY-8/DELTA_C.dat")
OBS_A_16<-scan("CR-0-ANY-16/DELTA_C.dat")
OBS_A_32<-scan("CR-0-ANY-32/DELTA_C.dat")
OBS_A_64<-scan("CR-0-ANY-64/DELTA_C.dat")
OBS_A_128<-scan("CR-0-ANY-128/DELTA_C.dat")
OBS_A_256<-scan("CR-0-ANY-256/DELTA_C.dat")
OBS_A_512<-scan("CR-0-ANY-512/DELTA_C.dat")
OBS_A_1024<-scan("CR-0-ANY-1024/DELTA_C.dat")
OBS_A_2048<-scan("CR-0-ANY-2048/DELTA_C.dat")
OBS_A_4096<-scan("CR-0-ANY-4096/DELTA_C.dat")
OBS_A_8192<-scan("CR-0-ANY-8192/DELTA_C.dat")
OBS_A_16384<-scan("CR-0-ANY-16384/DELTA_C.dat")
OBS_A_32768<-scan("CR-0-ANY-32768/DELTA_C.dat")
OBS_A_65536<-scan("CR-0-ANY-65536/DELTA_C.dat")

OBS_F_4<-scan("CR-0-FLOAT-4/DELTA_C.dat")
OBS_F_8<-scan("CR-0-FLOAT-8/DELTA_C.dat")
OBS_F_16<-scan("CR-0-FLOAT-16/DELTA_C.dat")
OBS_F_32<-scan("CR-0-FLOAT-32/DELTA_C.dat")
OBS_F_64<-scan("CR-0-FLOAT-64/DELTA_C.dat")
OBS_F_128<-scan("CR-0-FLOAT-128/DELTA_C.dat")
OBS_F_256<-scan("CR-0-FLOAT-256/DELTA_C.dat")
OBS_F_512<-scan("CR-0-FLOAT-512/DELTA_C.dat")
OBS_F_1024<-scan("CR-0-FLOAT-1024/DELTA_C.dat")
OBS_F_2048<-scan("CR-0-FLOAT-2048/DELTA_C.dat")
OBS_F_4096<-scan("CR-0-FLOAT-4096/DELTA_C.dat")
OBS_F_8192<-scan("CR-0-FLOAT-8192/DELTA_C.dat")
OBS_F_16384<-scan("CR-0-FLOAT-16384/DELTA_C.dat")
OBS_F_32768<-scan("CR-0-FLOAT-32768/DELTA_C.dat")
OBS_F_65536<-scan("CR-0-FLOAT-65536/DELTA_C.dat")

OBS_L_4<-scan("CR-0-LONG-4/DELTA_C.dat")
OBS_L_8<-scan("CR-0-LONG-8/DELTA_C.dat")
OBS_L_16<-scan("CR-0-LONG-16/DELTA_C.dat")
OBS_L_32<-scan("CR-0-LONG-32/DELTA_C.dat")
OBS_L_64<-scan("CR-0-LONG-64/DELTA_C.dat")
OBS_L_128<-scan("CR-0-LONG-128/DELTA_C.dat")
OBS_L_256<-scan("CR-0-LONG-256/DELTA_C.dat")
OBS_L_512<-scan("CR-0-LONG-512/DELTA_C.dat")
OBS_L_1024<-scan("CR-0-LONG-1024/DELTA_C.dat")
OBS_L_2048<-scan("CR-0-LONG-2048/DELTA_C.dat")
OBS_L_4096<-scan("CR-0-LONG-4096/DELTA_C.dat")
OBS_L_8192<-scan("CR-0-LONG-8192/DELTA_C.dat")
OBS_L_16384<-scan("CR-0-LONG-16384/DELTA_C.dat")
OBS_L_32768<-scan("CR-0-LONG-32768/DELTA_C.dat")
OBS_L_65536<-scan("CR-0-LONG-65536/DELTA_C.dat")

OBS_R_8<-scan("CR-0-RECORD-8/DELTA_C.dat")
OBS_R_16<-scan("CR-0-RECORD-16/DELTA_C.dat")
OBS_R_32<-scan("CR-0-RECORD-32/DELTA_C.dat")
OBS_R_64<-scan("CR-0-RECORD-64/DELTA_C.dat")
OBS_R_128<-scan("CR-0-RECORD-128/DELTA_C.dat")
OBS_R_256<-scan("CR-0-RECORD-256/DELTA_C.dat")
OBS_R_512<-scan("CR-0-RECORD-512/DELTA_C.dat")
OBS_R_1024<-scan("CR-0-RECORD-1024/DELTA_C.dat")
OBS_R_2048<-scan("CR-0-RECORD-2048/DELTA_C.dat")
OBS_R_4096<-scan("CR-0-RECORD-4096/DELTA_C.dat")
OBS_R_8192<-scan("CR-0-RECORD-8192/DELTA_C.dat")
OBS_R_16384<-scan("CR-0-RECORD-16384/DELTA_C.dat")
OBS_R_32768<-scan("CR-0-RECORD-32768/DELTA_C.dat")
OBS_R_65536<-scan("CR-0-RECORD-65536/DELTA_C.dat")

###ACQUISIZIONE DELTA_C (Normal)
OBSN_A_8<-scan("../../A-030/runset1/CR-0-ANY-8/DELTA_C.dat")
OBSN_A_16<-scan("../../A-030/runset1/CR-0-ANY-16/DELTA_C.dat")
OBSN_A_32<-scan("../../A-030/runset1/CR-0-ANY-32/DELTA_C.dat")
OBSN_A_64<-scan("../../A-030/runset1/CR-0-ANY-64/DELTA_C.dat")
OBSN_A_128<-scan("../../A-030/runset1/CR-0-ANY-128/DELTA_C.dat")
OBSN_A_256<-scan("../../A-030/runset1/CR-0-ANY-256/DELTA_C.dat")
OBSN_A_512<-scan("../../A-030/runset1/CR-0-ANY-512/DELTA_C.dat")
OBSN_A_1024<-scan("../../A-030/runset1/CR-0-ANY-1024/DELTA_C.dat")
OBSN_A_2048<-scan("../../A-030/runset1/CR-0-ANY-2048/DELTA_C.dat")
OBSN_A_4096<-scan("../../A-030/runset1/CR-0-ANY-4096/DELTA_C.dat")
OBSN_A_8192<-scan("../../A-030/runset1/CR-0-ANY-8192/DELTA_C.dat")
OBSN_A_16384<-scan("../../A-030/runset1/CR-0-ANY-16384/DELTA_C.dat")
OBSN_A_32768<-scan("../../A-030/runset1/CR-0-ANY-32768/DELTA_C.dat")
OBSN_A_65536<-scan("../../A-030/runset1/CR-0-ANY-65536/DELTA_C.dat")

OBSN_F_4<-scan("../../A-030/runset1/CR-0-FLOAT-4/DELTA_C.dat")
OBSN_F_8<-scan("../../A-030/runset1/CR-0-FLOAT-8/DELTA_C.dat")
OBSN_F_16<-scan("../../A-030/runset1/CR-0-FLOAT-16/DELTA_C.dat")
OBSN_F_32<-scan("../../A-030/runset1/CR-0-FLOAT-32/DELTA_C.dat")
OBSN_F_64<-scan("../../A-030/runset1/CR-0-FLOAT-64/DELTA_C.dat")
OBSN_F_128<-scan("../../A-030/runset1/CR-0-FLOAT-128/DELTA_C.dat")
OBSN_F_256<-scan("../../A-030/runset1/CR-0-FLOAT-256/DELTA_C.dat")
OBSN_F_512<-scan("../../A-030/runset1/CR-0-FLOAT-512/DELTA_C.dat")
OBSN_F_1024<-scan("../../A-030/runset1/CR-0-FLOAT-1024/DELTA_C.dat")
OBSN_F_2048<-scan("../../A-030/runset1/CR-0-FLOAT-2048/DELTA_C.dat")
OBSN_F_4096<-scan("../../A-030/runset1/CR-0-FLOAT-4096/DELTA_C.dat")
OBSN_F_8192<-scan("../../A-030/runset1/CR-0-FLOAT-8192/DELTA_C.dat")
OBSN_F_16384<-scan("../../A-030/runset1/CR-0-FLOAT-16384/DELTA_C.dat")
OBSN_F_32768<-scan("../../A-030/runset1/CR-0-FLOAT-32768/DELTA_C.dat")
OBSN_F_65536<-scan("../../A-030/runset1/CR-0-FLOAT-65536/DELTA_C.dat")

OBSN_L_4<-scan("../../A-030/runset1/CR-0-LONG-4/DELTA_C.dat")
OBSN_L_8<-scan("../../A-030/runset1/CR-0-LONG-8/DELTA_C.dat")
OBSN_L_16<-scan("../../A-030/runset1/CR-0-LONG-16/DELTA_C.dat")
OBSN_L_32<-scan("../../A-030/runset1/CR-0-LONG-32/DELTA_C.dat")
OBSN_L_64<-scan("../../A-030/runset1/CR-0-LONG-64/DELTA_C.dat")
OBSN_L_128<-scan("../../A-030/runset1/CR-0-LONG-128/DELTA_C.dat")
OBSN_L_256<-scan("../../A-030/runset1/CR-0-LONG-256/DELTA_C.dat")
OBSN_L_512<-scan("../../A-030/runset1/CR-0-LONG-512/DELTA_C.dat")
OBSN_L_1024<-scan("../../A-030/runset1/CR-0-LONG-1024/DELTA_C.dat")
OBSN_L_2048<-scan("../../A-030/runset1/CR-0-LONG-2048/DELTA_C.dat")
OBSN_L_4096<-scan("../../A-030/runset1/CR-0-LONG-4096/DELTA_C.dat")
OBSN_L_8192<-scan("../../A-030/runset1/CR-0-LONG-8192/DELTA_C.dat")
OBSN_L_16384<-scan("../../A-030/runset1/CR-0-LONG-16384/DELTA_C.dat")
OBSN_L_32768<-scan("../../A-030/runset1/CR-0-LONG-32768/DELTA_C.dat")
OBSN_L_65536<-scan("../../A-030/runset1/CR-0-LONG-65536/DELTA_C.dat")

OBSN_R_8<-scan("../../A-030/runset1/CR-0-RECORD-8/DELTA_C.dat")
OBSN_R_16<-scan("../../A-030/runset1/CR-0-RECORD-16/DELTA_C.dat")
OBSN_R_32<-scan("../../A-030/runset1/CR-0-RECORD-32/DELTA_C.dat")
OBSN_R_64<-scan("../../A-030/runset1/CR-0-RECORD-64/DELTA_C.dat")
OBSN_R_128<-scan("../../A-030/runset1/CR-0-RECORD-128/DELTA_C.dat")
OBSN_R_256<-scan("../../A-030/runset1/CR-0-RECORD-256/DELTA_C.dat")
OBSN_R_512<-scan("../../A-030/runset1/CR-0-RECORD-512/DELTA_C.dat")
OBSN_R_1024<-scan("../../A-030/runset1/CR-0-RECORD-1024/DELTA_C.dat")
OBSN_R_2048<-scan("../../A-030/runset1/CR-0-RECORD-2048/DELTA_C.dat")
OBSN_R_4096<-scan("../../A-030/runset1/CR-0-RECORD-4096/DELTA_C.dat")
OBSN_R_8192<-scan("../../A-030/runset1/CR-0-RECORD-8192/DELTA_C.dat")
OBSN_R_16384<-scan("../../A-030/runset1/CR-0-RECORD-16384/DELTA_C.dat")
OBSN_R_32768<-scan("../../A-030/runset1/CR-0-RECORD-32768/DELTA_C.dat")
OBSN_R_65536<-scan("../../A-030/runset1/CR-0-RECORD-65536/DELTA_C.dat")

###ACQUISIZIONE RAMmonitor
RAM_A_8<-scan("CR-0-ANY-8/RAMmonitor.mon")
RAM_A_16<-scan("CR-0-ANY-16/RAMmonitor.mon")
RAM_A_32<-scan("CR-0-ANY-32/RAMmonitor.mon")
RAM_A_64<-scan("CR-0-ANY-64/RAMmonitor.mon")
RAM_A_128<-scan("CR-0-ANY-128/RAMmonitor.mon")
RAM_A_256<-scan("CR-0-ANY-256/RAMmonitor.mon")
RAM_A_512<-scan("CR-0-ANY-512/RAMmonitor.mon")
RAM_A_1024<-scan("CR-0-ANY-1024/RAMmonitor.mon")
RAM_A_2048<-scan("CR-0-ANY-2048/RAMmonitor.mon")
RAM_A_4096<-scan("CR-0-ANY-4096/RAMmonitor.mon")
RAM_A_8192<-scan("CR-0-ANY-8192/RAMmonitor.mon")
RAM_A_16384<-scan("CR-0-ANY-16384/RAMmonitor.mon")
RAM_A_32768<-scan("CR-0-ANY-32768/RAMmonitor.mon")
RAM_A_65536<-scan("CR-0-ANY-65536/RAMmonitor.mon")

RAM_F_4<-scan("CR-0-FLOAT-4/RAMmonitor.mon")
RAM_F_8<-scan("CR-0-FLOAT-8/RAMmonitor.mon")
RAM_F_16<-scan("CR-0-FLOAT-16/RAMmonitor.mon")
RAM_F_32<-scan("CR-0-FLOAT-32/RAMmonitor.mon")
RAM_F_64<-scan("CR-0-FLOAT-64/RAMmonitor.mon")
RAM_F_128<-scan("CR-0-FLOAT-128/RAMmonitor.mon")
RAM_F_256<-scan("CR-0-FLOAT-256/RAMmonitor.mon")
RAM_F_512<-scan("CR-0-FLOAT-512/RAMmonitor.mon")
RAM_F_1024<-scan("CR-0-FLOAT-1024/RAMmonitor.mon")
RAM_F_2048<-scan("CR-0-FLOAT-2048/RAMmonitor.mon")
RAM_F_4096<-scan("CR-0-FLOAT-4096/RAMmonitor.mon")
RAM_F_8192<-scan("CR-0-FLOAT-8192/RAMmonitor.mon")
RAM_F_16384<-scan("CR-0-FLOAT-16384/RAMmonitor.mon")
RAM_F_32768<-scan("CR-0-FLOAT-32768/RAMmonitor.mon")
RAM_F_65536<-scan("CR-0-FLOAT-65536/RAMmonitor.mon")

RAM_L_4<-scan("CR-0-LONG-4/RAMmonitor.mon")
RAM_L_8<-scan("CR-0-LONG-8/RAMmonitor.mon")
RAM_L_16<-scan("CR-0-LONG-16/RAMmonitor.mon")
RAM_L_32<-scan("CR-0-LONG-32/RAMmonitor.mon")
RAM_L_64<-scan("CR-0-LONG-64/RAMmonitor.mon")
RAM_L_128<-scan("CR-0-LONG-128/RAMmonitor.mon")
RAM_L_256<-scan("CR-0-LONG-256/RAMmonitor.mon")
RAM_L_512<-scan("CR-0-LONG-512/RAMmonitor.mon")
RAM_L_1024<-scan("CR-0-LONG-1024/RAMmonitor.mon")
RAM_L_2048<-scan("CR-0-LONG-2048/RAMmonitor.mon")
RAM_L_4096<-scan("CR-0-LONG-4096/RAMmonitor.mon")
RAM_L_8192<-scan("CR-0-LONG-8192/RAMmonitor.mon")
RAM_L_16384<-scan("CR-0-LONG-16384/RAMmonitor.mon")
RAM_L_32768<-scan("CR-0-LONG-32768/RAMmonitor.mon")
RAM_L_65536<-scan("CR-0-LONG-65536/RAMmonitor.mon")

RAM_R_8<-scan("CR-0-RECORD-8/RAMmonitor.mon")
RAM_R_16<-scan("CR-0-RECORD-16/RAMmonitor.mon")
RAM_R_32<-scan("CR-0-RECORD-32/RAMmonitor.mon")
RAM_R_64<-scan("CR-0-RECORD-64/RAMmonitor.mon")
RAM_R_128<-scan("CR-0-RECORD-128/RAMmonitor.mon")
RAM_R_256<-scan("CR-0-RECORD-256/RAMmonitor.mon")
RAM_R_512<-scan("CR-0-RECORD-512/RAMmonitor.mon")
RAM_R_1024<-scan("CR-0-RECORD-1024/RAMmonitor.mon")
RAM_R_2048<-scan("CR-0-RECORD-2048/RAMmonitor.mon")
RAM_R_4096<-scan("CR-0-RECORD-4096/RAMmonitor.mon")
RAM_R_8192<-scan("CR-0-RECORD-8192/RAMmonitor.mon")
RAM_R_16384<-scan("CR-0-RECORD-16384/RAMmonitor.mon")
RAM_R_32768<-scan("CR-0-RECORD-32768/RAMmonitor.mon")
RAM_R_65536<-scan("CR-0-RECORD-65536/RAMmonitor.mon")

###ACQUISIZIONE CPUmonitor.mon
CPU_A_8<-scan("CR-0-ANY-8/CPUmonitor.mon")
CPU_A_16<-scan("CR-0-ANY-16/CPUmonitor.mon")
CPU_A_32<-scan("CR-0-ANY-32/CPUmonitor.mon")
CPU_A_64<-scan("CR-0-ANY-64/CPUmonitor.mon")
CPU_A_128<-scan("CR-0-ANY-128/CPUmonitor.mon")
CPU_A_256<-scan("CR-0-ANY-256/CPUmonitor.mon")
CPU_A_512<-scan("CR-0-ANY-512/CPUmonitor.mon")
CPU_A_1024<-scan("CR-0-ANY-1024/CPUmonitor.mon")
CPU_A_2048<-scan("CR-0-ANY-2048/CPUmonitor.mon")
CPU_A_4096<-scan("CR-0-ANY-4096/CPUmonitor.mon")
CPU_A_8192<-scan("CR-0-ANY-8192/CPUmonitor.mon")
CPU_A_16384<-scan("CR-0-ANY-16384/CPUmonitor.mon")
CPU_A_32768<-scan("CR-0-ANY-32768/CPUmonitor.mon")
CPU_A_65536<-scan("CR-0-ANY-65536/CPUmonitor.mon")

CPU_F_4<-scan("CR-0-FLOAT-8/CPUmonitor.mon")
CPU_F_8<-scan("CR-0-FLOAT-8/CPUmonitor.mon")
CPU_F_16<-scan("CR-0-FLOAT-16/CPUmonitor.mon")
CPU_F_32<-scan("CR-0-FLOAT-32/CPUmonitor.mon")
CPU_F_64<-scan("CR-0-FLOAT-64/CPUmonitor.mon")
CPU_F_128<-scan("CR-0-FLOAT-128/CPUmonitor.mon")
CPU_F_256<-scan("CR-0-FLOAT-256/CPUmonitor.mon")
CPU_F_512<-scan("CR-0-FLOAT-512/CPUmonitor.mon")
CPU_F_1024<-scan("CR-0-FLOAT-1024/CPUmonitor.mon")
CPU_F_2048<-scan("CR-0-FLOAT-2048/CPUmonitor.mon")
CPU_F_4096<-scan("CR-0-FLOAT-4096/CPUmonitor.mon")
CPU_F_8192<-scan("CR-0-FLOAT-8192/CPUmonitor.mon")
CPU_F_16384<-scan("CR-0-FLOAT-16384/CPUmonitor.mon")
CPU_F_32768<-scan("CR-0-FLOAT-32768/CPUmonitor.mon")
CPU_F_65536<-scan("CR-0-FLOAT-65536/CPUmonitor.mon")

CPU_L_4<-scan("CR-0-LONG-4/CPUmonitor.mon")
CPU_L_8<-scan("CR-0-LONG-8/CPUmonitor.mon")
CPU_L_16<-scan("CR-0-LONG-16/CPUmonitor.mon")
CPU_L_32<-scan("CR-0-LONG-32/CPUmonitor.mon")
CPU_L_64<-scan("CR-0-LONG-64/CPUmonitor.mon")
CPU_L_128<-scan("CR-0-LONG-128/CPUmonitor.mon")
CPU_L_256<-scan("CR-0-LONG-256/CPUmonitor.mon")
CPU_L_512<-scan("CR-0-LONG-512/CPUmonitor.mon")
CPU_L_1024<-scan("CR-0-LONG-1024/CPUmonitor.mon")
CPU_L_2048<-scan("CR-0-LONG-2048/CPUmonitor.mon")
CPU_L_4096<-scan("CR-0-LONG-4096/CPUmonitor.mon")
CPU_L_8192<-scan("CR-0-LONG-8192/CPUmonitor.mon")
CPU_L_16384<-scan("CR-0-LONG-16384/CPUmonitor.mon")
CPU_L_32768<-scan("CR-0-LONG-32768/CPUmonitor.mon")
CPU_L_65536<-scan("CR-0-LONG-65536/CPUmonitor.mon")

CPU_R_8<-scan("CR-0-RECORD-8/CPUmonitor.mon")
CPU_R_16<-scan("CR-0-RECORD-16/CPUmonitor.mon")
CPU_R_32<-scan("CR-0-RECORD-32/CPUmonitor.mon")
CPU_R_64<-scan("CR-0-RECORD-64/CPUmonitor.mon")
CPU_R_128<-scan("CR-0-RECORD-128/CPUmonitor.mon")
CPU_R_256<-scan("CR-0-RECORD-256/CPUmonitor.mon")
CPU_R_512<-scan("CR-0-RECORD-512/CPUmonitor.mon")
CPU_R_1024<-scan("CR-0-RECORD-1024/CPUmonitor.mon")
CPU_R_2048<-scan("CR-0-RECORD-2048/CPUmonitor.mon")
CPU_R_4096<-scan("CR-0-RECORD-4096/CPUmonitor.mon")
CPU_R_8192<-scan("CR-0-RECORD-8192/CPUmonitor.mon")
CPU_R_16384<-scan("CR-0-RECORD-16384/CPUmonitor.mon")
CPU_R_32768<-scan("CR-0-RECORD-32768/CPUmonitor.mon")
CPU_R_65536<-scan("CR-0-RECORD-65536/CPUmonitor.mon")

source("MarshallingCoreCCM.R")

rm( is2Host,CD_lwd,color,dec_num,dev_dig,dig_dev,filter,
gr1,gr2,gr3,gr4,gr5,gr6,gr7,gr8,gr9,gr10,grC,grCO,grR,grRO,
nomi,nomi_leg,num_dec,out,png_height,png_width,som,thresh,tipi,HTML_type,HTML_delay,HTML_host,
CPU_R_1024,CPU_R_128,CPU_R_16,CPU_R_2048,CPU_R_256,CPU_R_32,CPU_R_4096,CPU_R_512,CPU_R_64,CPU_R_8,
CPU_A_1024,CPU_A_128,CPU_A_16,CPU_A_2048,CPU_A_256,CPU_A_32,CPU_A_4096,CPU_A_512,CPU_A_64,CPU_A_8,
CPU_F_1024,CPU_F_128,CPU_F_16,CPU_F_2048,CPU_F_256,CPU_F_32,CPU_F_4,CPU_F_4096,CPU_F_512,CPU_F_64,CPU_F_8,CPU_L_1024,
CPU_L_128,CPU_L_16,CPU_L_2048,CPU_L_256,CPU_L_32,CPU_L_4,CPU_L_4096,CPU_L_512,CPU_L_64,CPU_L_8,
OBS_R_1024,OBS_R_128,OBS_R_16,OBS_R_2048,OBS_R_256,OBS_R_32,OBS_R_4096,OBS_R_512,OBS_R_64,OBS_R_8,
OBS_A_1024,OBS_A_128,OBS_A_16,OBS_A_2048,OBS_A_256,OBS_A_32,OBS_A_4096,OBS_A_512,OBS_A_64,OBS_A_8,
OBS_F_1024,OBS_F_128,OBS_F_16,OBS_F_2048,OBS_F_256,OBS_F_32,OBS_F_4096,OBS_F_512,OBS_F_64,OBS_F_8,
OBS_F_4,
OBS_L_1024,OBS_L_128,OBS_L_16,OBS_L_2048,OBS_L_256,OBS_L_32,OBS_L_4096,OBS_L_512,OBS_L_64,OBS_L_8,OBS_L_4,

OBSN_R_1024,OBSN_R_128,OBSN_R_16,OBSN_R_2048,OBSN_R_256,OBSN_R_32,OBSN_R_4096,OBSN_R_512,OBSN_R_64,OBSN_R_8,
OBSN_A_1024,OBSN_A_128,OBSN_A_16,OBSN_A_2048,OBSN_A_256,OBSN_A_32,OBSN_A_4096,OBSN_A_512,OBSN_A_64,OBSN_A_8,
OBSN_F_1024,OBSN_F_128,OBSN_F_16,OBSN_F_2048,OBSN_F_256,OBSN_F_32,OBSN_F_4096,OBSN_F_512,OBSN_F_64,OBSN_F_8,
OBSN_F_4,
OBSN_L_1024,OBSN_L_128,OBSN_L_16,OBSN_L_2048,OBSN_L_256,OBSN_L_32,OBSN_L_4096,OBSN_L_512,OBSN_L_64,OBSN_L_8,OBSN_L_4,

RAM_R_1024,RAM_R_128,RAM_R_16,RAM_R_2048,RAM_R_256,RAM_R_32,RAM_R_4096,RAM_R_512,RAM_R_64,RAM_R_8,
RAM_A_1024,RAM_A_128,RAM_A_16,RAM_A_2048,RAM_A_256,RAM_A_32,RAM_A_4096,RAM_A_512,RAM_A_64,RAM_A_8,RAM_F_1024,RAM_F_128,RAM_F_16,RAM_F_2048,RAM_F_256,RAM_F_32,RAM_F_4,RAM_F_4096,RAM_F_512,RAM_F_64,RAM_F_8,
RAM_L_1024,RAM_L_128,RAM_L_16,RAM_L_2048,RAM_L_256,RAM_L_32,RAM_L_4,RAM_L_4096,RAM_L_512,RAM_L_64,RAM_L_8)
