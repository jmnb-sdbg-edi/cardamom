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

### DISPATCHER CORE

filter<-function(x) x<-x[x<quantile(x,thresh)]
out<-function(x) x<-x[x>quantile(x,thresh)]
num_dec <- function(x) x<-round(x*(10^dec_num))/(10^dec_num)
dig_dev <- function(x) x<-round(x*(10^dev_dig))/(10^dev_dig)

DC_DISP_1f<-filter(DC_DISP_1)		### DELTA_C FILTERING
DC_DISP_2f<-filter(DC_DISP_2)
DC_DISP_3f<-filter(DC_DISP_3)
DC_DISP_4f<-filter(DC_DISP_4)
DC_DISP_5f<-filter(DC_DISP_5)
DC_DISP_6f<-filter(DC_DISP_6)
DC_DISP_7f<-filter(DC_DISP_7)
DC_DISP_8f<-filter(DC_DISP_8)
DC_DISP_9f<-filter(DC_DISP_9)
DC_DISP_10f<-filter(DC_DISP_10)
DC_DISP_11f<-filter(DC_DISP_11)
DC_DISP_12f<-filter(DC_DISP_12)
DC_DISP_13f<-filter(DC_DISP_13)
DC_DISP_14f<-filter(DC_DISP_14)
DC_DISP_15f<-filter(DC_DISP_15)
DC_DISP_16f<-filter(DC_DISP_16)

outliers<-data.frame(sort(out(DC_DISP_1)),		### DELTA_C OUTING
	sort(out(DC_DISP_2)),sort(out(DC_DISP_3)),
	sort(out(DC_DISP_4)),sort(out(DC_DISP_5)),
	sort(out(DC_DISP_6)),sort(out(DC_DISP_6)),
	sort(out(DC_DISP_7)),sort(out(DC_DISP_8)),
	sort(out(DC_DISP_9)),sort(out(DC_DISP_10)),
	sort(out(DC_DISP_11)),sort(out(DC_DISP_12)),
	sort(out(DC_DISP_13)),sort(out(DC_DISP_14)),
	sort(out(DC_DISP_15)),sort(out(DC_DISP_16)))

RAM<-c(max(RAM_1)-min(RAM_1),		### Massima occupazione RAM in kBytes
	max(RAM_2)-min(RAM_2),
	max(RAM_3)-min(RAM_3),
	max(RAM_4)-min(RAM_4),
	max(RAM_5)-min(RAM_5),
	max(RAM_6)-min(RAM_6),
	max(RAM_7)-min(RAM_7),
	max(RAM_8)-min(RAM_8),
	max(RAM_9)-min(RAM_9),
	max(RAM_10)-min(RAM_10),
	max(RAM_11)-min(RAM_11),
	max(RAM_12)-min(RAM_12),
	max(RAM_13)-min(RAM_13),
	max(RAM_14)-min(RAM_14),
	max(RAM_15)-min(RAM_15),
	max(RAM_16)-min(RAM_16))
RAM<-RAM/1000


CPU<-c(mean(CPU_1),mean(CPU_2),mean(CPU_3),mean(CPU_4),		### Massima occupazione CPU
	mean(CPU_5),mean(CPU_6),mean(CPU_7),mean(CPU_8),
	mean(CPU_9),mean(CPU_10),mean(CPU_11),mean(CPU_12),
	mean(CPU_13),mean(CPU_14),mean(CPU_15),mean(CPU_16))

if(is2Host) {
RRAM<-c(max(RRAM_1)-min(RRAM_1),		### Massima occupazione RRAM in kBytes
	max(RRAM_2)-min(RRAM_2),
	max(RRAM_3)-min(RRAM_3),
	max(RRAM_4)-min(RRAM_4),
	max(RRAM_5)-min(RRAM_5),
	max(RRAM_6)-min(RRAM_6),
	max(RRAM_7)-min(RRAM_7),
	max(RRAM_8)-min(RRAM_8),
	max(RRAM_9)-min(RRAM_9),
	max(RRAM_10)-min(RRAM_10),
	max(RRAM_11)-min(RRAM_11),
	max(RRAM_12)-min(RRAM_12),
	max(RRAM_13)-min(RRAM_13),
	max(RRAM_14)-min(RRAM_14),
	max(RRAM_15)-min(RRAM_15),
	max(RRAM_16)-min(RRAM_16))
RRAM<-RRAM/1000

RCPU<-c(mean(RCPU_1),mean(RCPU_2),mean(RCPU_3),mean(RCPU_4),	### Massima occupazione RCPU
	mean(RCPU_5),mean(RCPU_6),mean(RCPU_7),mean(RCPU_8),
	mean(RCPU_9),mean(RCPU_10),mean(RCPU_11),mean(RCPU_12),
	mean(RCPU_13),mean(RCPU_14),mean(RCPU_15),mean(RCPU_16))}

### AUX VARIABLES
BS_DISP_1<-boxplot.stats(DC_DISP_1f)$stats
BS_DISP_2<-boxplot.stats(DC_DISP_2f)$stats
BS_DISP_3<-boxplot.stats(DC_DISP_3f)$stats
BS_DISP_4<-boxplot.stats(DC_DISP_4f)$stats
BS_DISP_5<-boxplot.stats(DC_DISP_5f)$stats
BS_DISP_6<-boxplot.stats(DC_DISP_6f)$stats
BS_DISP_7<-boxplot.stats(DC_DISP_7f)$stats
BS_DISP_8<-boxplot.stats(DC_DISP_8f)$stats
BS_DISP_9<-boxplot.stats(DC_DISP_9f)$stats
BS_DISP_10<-boxplot.stats(DC_DISP_10f)$stats
BS_DISP_11<-boxplot.stats(DC_DISP_11f)$stats
BS_DISP_12<-boxplot.stats(DC_DISP_12f)$stats
BS_DISP_13<-boxplot.stats(DC_DISP_13f)$stats
BS_DISP_14<-boxplot.stats(DC_DISP_14f)$stats
BS_DISP_15<-boxplot.stats(DC_DISP_15f)$stats
BS_DISP_16<-boxplot.stats(DC_DISP_16f)$stats

obs_yrange<- range(DC_DISP_1f,DC_DISP_16f)

RAMObs_xrange<-c(0,max(c(dim(as.array(RAM_1)),dim(as.array(RAM_16)))))
RAMObs_yrange<-range(RAM_1,RAM_16)/1000
CPUObs_xrange<-c(0,max(c(dim(as.array(CPU_1)),dim(as.array(CPU_16)))))
CPUObs_yrange<-range(CPU_1,CPU_16)

if (is2Host) {
RRAMObs_xrange<-c(0,max(c(dim(as.array(RRAM_1)),dim(as.array(RRAM_16)))))
RRAMObs_yrange<-range(RRAM_1,RRAM_16)/1000
RCPUObs_xrange<-c(0,max(c(dim(as.array(RCPU_1)),dim(as.array(RCPU_16)))))
RCPUObs_yrange<-range(RCPU_1,RCPU_16)}

Media<-c(mean(DC_DISP_1f),mean(DC_DISP_2f),mean(DC_DISP_3f),mean(DC_DISP_4f),
	mean(DC_DISP_5f),mean(DC_DISP_6f),mean(DC_DISP_7f),mean(DC_DISP_8f),
	mean(DC_DISP_9f),mean(DC_DISP_10f),mean(DC_DISP_11f),mean(DC_DISP_12f),
	mean(DC_DISP_13f),mean(DC_DISP_14f),mean(DC_DISP_15f),mean(DC_DISP_16f))

stddev<-c(sqrt(var(DC_DISP_1f)),sqrt(var(DC_DISP_2f)),sqrt(var(DC_DISP_3f)),
	sqrt(var(DC_DISP_4f)),sqrt(var(DC_DISP_5f)),sqrt(var(DC_DISP_6f)),
	sqrt(var(DC_DISP_7f)),sqrt(var(DC_DISP_8f)),sqrt(var(DC_DISP_9f)),
	sqrt(var(DC_DISP_10f)),sqrt(var(DC_DISP_11f)),sqrt(var(DC_DISP_12f)),
	sqrt(var(DC_DISP_13f)),sqrt(var(DC_DISP_14f)),sqrt(var(DC_DISP_15f)),
	sqrt(var(DC_DISP_16f)))

bxp_x<-as.character(x)
bxp_yrange<-range(BS_DISP_1,BS_DISP_2,BS_DISP_3,BS_DISP_4,
	BS_DISP_5,BS_DISP_6,BS_DISP_7,BS_DISP_8,
	BS_DISP_9,BS_DISP_10,BS_DISP_11,BS_DISP_12,
	BS_DISP_13,BS_DISP_14,BS_DISP_15,BS_DISP_16)

###OBSERVATIONS graph
png(gr1,width=png_width,height=png_height)
plot(DC_DISP_1f,col=color[1],pch="o",
	ylim=obs_yrange,
	main="Observations",ylab="Latency [usec]",xlab="Index Number [#]")
points(DC_DISP_16f,col=color[2],pch="o")
legend(0,obs_yrange[2],c(nomi[1],nomi[16]),color[1:2])
dev.off()

### MEAN graph
png(gr2,width=png_width,height=png_height)
plot(Media~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
	main="Dispatcher benchmarks (Average)",xlab="Servants [#]",ylab="Latency [usec]")
dev.off()

### STD
png(gr3,width=png_width,height=png_height)
plot(stddev~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
	main="Dispatcher benchmarks (Standard deviation)",xlab="Servants [#]",
		ylab="Standard deviation [usec]")
dev.off()

### BOXPLOTS
png(gr4,width=png_width,height=png_height)
boxplot(DC_DISP_1f,DC_DISP_2f,DC_DISP_3f,DC_DISP_4f,
	DC_DISP_5f,DC_DISP_6f,DC_DISP_7f,DC_DISP_8f,
	DC_DISP_9f,DC_DISP_10f,DC_DISP_11f,DC_DISP_12f,
	DC_DISP_13f,DC_DISP_14f,DC_DISP_15f,DC_DISP_16f,
	ylim=c(bxp_yrange[1]-30,bxp_yrange[2]),
	col=color,notch=TRUE,boxwex=.3,names=bxp_x,pch=".",
	main="Dispatcher benchmarks (Box & Whisker graph)",xlab="Servants [#]",
		ylab="Latency [usec]")
dev.off()

###RAMObs graph
png(grRO,width=png_width,height=png_height)
plot(RAM_1/1000,col=color[1],pch="o",
	xlim=RAMObs_xrange,ylim=RAMObs_yrange,
	main="RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
points(RAM_16/1000,col=color[2],pch="o")
legend(0,RAMObs_yrange[2],c(nomi[1],nomi[16]),color[1:2])
dev.off()

###RAM graphs
png(grR,width=png_width,height=png_height)
plot(RAM~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
	main="Max RAM occupacy",xlab="Servants [#]",ylab="Occupacy [kBytes]")
dev.off()

###CPUObs graph
png(grCO,width=png_width,height=png_height)
plot(CPU_1,col=color[1],pch="o",
	xlim=CPUObs_xrange,ylim=CPUObs_yrange,
	main="CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
points(CPU_16,col=color[2],pch="o")
legend(0,CPUObs_yrange[2],c(nomi[1],nomi[16]),color[1:2])
dev.off()

###CPU graphs
png(grC,width=png_width,height=png_height)
plot(CPU~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
	main="Average CPU occupacy",xlab="Servants [#]",ylab="Occupacy [%]")
dev.off()

if (is2Host) {

	###RRAMObs graph
	png(grRRO,width=png_width,height=png_height)
	plot(RRAM_1/1000,col=color[1],pch="o",
		xlim=RRAMObs_xrange,ylim=RRAMObs_yrange,
		main="Remote RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
	points(RRAM_16/1000,col=color[2],pch="o")
	legend(0,RRAMObs_yrange[2],c(nomi[1],nomi[16]),color[1:2])
	dev.off()

	###RRAM graphs
	png(grRR,width=png_width,height=png_height)
	plot(RRAM~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
		main="Max Remote RAM occupacy",xlab="Servants [#]",ylab="Occupacy [kBytes]")
	dev.off()

	###RCPUObs graph
	png(grRCO,width=png_width,height=png_height)
	plot(RCPU_1,col=color[1],pch="o",
		xlim=CPUObs_xrange,ylim=CPUObs_yrange,
		main="Remote CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
	points(RCPU_16,col=color[2],pch="o")
	legend(0,RCPUObs_yrange[2],c(nomi[1],nomi[16]),color[1:2])
	dev.off()

	###RCPU graphs
	png(grRC,width=png_width,height=png_height)
	plot(RCPU~x,type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,
		main="Average Remote CPU occupacy",xlab="Servants [#]",ylab="Occupacy [%]")
	dev.off()}

###HTML SUMMARY
write("<html>",som)
write(c("<title> Dispatcher Performance Test - CARDAMOM </title>",
	"<h1> Dispatcher Summary </h1>",
	"<h4>",HTML_type,"</h4>",
	"<h4>",HTML_delay,"</h4>",
	"<h4>",HTML_host,"</h4><body><hr>"),som,append=TRUE)

###GRAFICI
write(c("<h3> Summary Graphs </h3>","<p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr1,"\"> Dispatcher Benchmark (Observations)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr2,"\"> Dispatcher Benchmark (Mean)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr3,"\"> Dispatcher Benchmark (Standard Deviation)</a><p></p>"),
	som,append=TRUE)
write(c("<a href=\"",gr4,"\"> Dispatcher Benchmark (BoxPlot)</a> <p> </p>"),som,append=TRUE)

write("<table><tr><td>",som,append=TRUE)
write(c("<a href=\"",grRO,"\"> RAM Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grR,"\"> Max RAM occupacy </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grCO,"\"> CPU Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grC,"\"> Average CPU occupacy </a><p></p>"),som,append=TRUE)

if (is2Host) {
	write("</td><td>",som,append=TRUE)
	write(c("<a href=\"",grRRO,"\"> RemoteRAM Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRR,"\"> Max RemoteRAM occupacy </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRCO,"\"> Remote CPU Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRC,"\"> Average Remote CPU occupacy </a><p></p>"),som,append=TRUE)}

write("</td></tr></table><p></p><hr>",som,append=TRUE)

###SUMMARY TABLE
write(c("<h3> Summary Table </h3>",
	"<p> Statistics vs Servants Number </p><p></p>",
	"<table border=1><tr><th> &nbsp; </th>"),som,append=TRUE)
write(c("<th> Min. </th><th> 1st Qu. </th><th> Median </th><th> Mean </th>",
	"<th> 3rd Qu. </th><th> Max. </th><th> Stnd.Dev. </th></tr>"),som,append=TRUE)

write(c("<tr><th>",as.character(x)[1],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_1f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_1f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[2],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_2f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_2f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[3],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_3f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_3f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[4],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_4f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_4f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[5],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_5f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_5f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[6],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_6f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_6f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[7],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_7f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_7f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[8],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_8f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_8f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[9],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_9f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_9f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[10],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_10f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_10f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[11],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_11f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_11f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[12],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_12f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_12f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[13],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_13f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_13f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[14],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_14f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_14f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[15],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_15f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_15f))),"</td>"),som,append=TRUE)
write(c("</tr><tr><th>",as.character(x)[16],"SRV </th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_DISP_16f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_DISP_16f))),"</td>"),som,append=TRUE)
write("</tr></table><p></p><hr>",som,append=TRUE)

###OUTLIERS
write(c("<h3> Outliers Summary Table </h3>",
	"<p> Values of outstanding points </p><p></p>"),som,append=TRUE)
write(c("<table border=1><tr><th> Out of ",thresh*100,"% </th>"),som,append=TRUE)
for(i in 1:dim(outliers)[1]) 
	write(c("<th>",i,"' </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)

for (j in 1:dim(as.array(x))) {
	write(c("<tr><th>", as.character(x[j]), "SRV</th>"),som,append=TRUE)
	for (i in 1:dim(outliers)[1]) 
		write(c("<td>",num_dec(outliers[i,j]),"</td>"),som,append=TRUE)
	write("</tr>",som,append=TRUE)}

write("</table><p></p><hr></body></html>",som,append=TRUE)

rm(	dig_dev,filter,num_dec,out,
	Media,outliers,stddev,bxp_x,bxp_yrange,i,j,obs_yrange,

	CPU,CPUObs_xrange,CPUObs_yrange,RAM,RAMObs_xrange,RAMObs_yrange,

	BS_DISP_1,BS_DISP_2,BS_DISP_3,BS_DISP_4,
	BS_DISP_5,BS_DISP_6,BS_DISP_7,BS_DISP_8,
	BS_DISP_9,BS_DISP_10,BS_DISP_11,BS_DISP_12,
	BS_DISP_13,BS_DISP_14,BS_DISP_15,BS_DISP_16,

	DC_DISP_1f,DC_DISP_2f,DC_DISP_3f,DC_DISP_4f,
	DC_DISP_5f,DC_DISP_6f,DC_DISP_7f,DC_DISP_8f,
	DC_DISP_9f,DC_DISP_10f,DC_DISP_11f,DC_DISP_12f,
	DC_DISP_13f,DC_DISP_14f,DC_DISP_15f,DC_DISP_16f)

if (is2Host) rm(RCPU,RCPUObs_xrange,RCPUObs_yrange,RRAM,RRAMObs_xrange,RRAMObs_yrange)
