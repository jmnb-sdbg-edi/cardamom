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

### INVOCATION POLICY CORE

filter<-function(x) x<-x[x<quantile(x,thresh)]
out<-function(x) x<-x[x>quantile(x,thresh)] 
num_dec <- function(x) x<-round(x*(10^dec_num))/(10^dec_num)
dig_dev <- function(x) x<-round(x*(10^dev_dig))/(10^dev_dig)

### DELTA_C FILTERING
DC_CRf<-filter(DC_CR); DC_OW_NONf<-filter(DC_OW_NON)
DC_OW_TRNf<-filter(DC_OW_TRN); DC_OW_SRVf<-filter(DC_OW_SRV)
DC_OW_TRGf<-filter(DC_OW_TRG)

### DELTA_C OUTING
outliers<-data.frame(sort(out(DC_CR)),sort(out(DC_OW_NON)),sort(out(DC_OW_TRN)),
	sort(out(DC_OW_SRV)),sort(out(DC_OW_TRG)))

### Massima occupazione RAM in kBytes
RAM<-c(max(RAM_CR)-min(RAM_CR),max(RAM_OW_NON)-min(RAM_OW_NON),
	max(RAM_OW_TRN)-min(RAM_OW_TRN),max(RAM_OW_SRV)-min(RAM_OW_SRV),
	max(RAM_OW_TRG)-min(RAM_OW_TRG))
RAM<-RAM/1000

### Massima occupazione CPU in %
CPU<-c(mean(CPU_CR),mean(CPU_OW_NON),mean(CPU_OW_TRN),mean(CPU_OW_SRV),mean(CPU_OW_TRG))

if(is2Host){
	### Massima occupazione RAM in kBytes
	RRAM<-c(max(RRAM_CR)-min(RRAM_CR),max(RRAM_OW_NON)-min(RRAM_OW_NON),
		max(RRAM_OW_TRN)-min(RRAM_OW_TRN),max(RRAM_OW_SRV)-min(RRAM_OW_SRV),
		max(RRAM_OW_TRG)-min(RRAM_OW_TRG))
	RAM<-RAM/1000

	### Massima occupazione CPU in %
	RCPU<-c(mean(RCPU_CR),mean(RCPU_OW_NON),mean(RCPU_OW_TRN),
		mean(RCPU_OW_SRV),mean(RCPU_OW_TRG))}


### AUX VARIABLES
BS_CR<-boxplot.stats(DC_CRf)$stats
BS_OW_NON<-boxplot.stats(DC_OW_NONf)$stats
BS_OW_TRN<-boxplot.stats(DC_OW_TRNf)$stats
BS_OW_SRV<-boxplot.stats(DC_OW_SRVf)$stats
BS_OW_TRG<-boxplot.stats(DC_OW_TRGf)$stats
xrange<-range(DC_CRf, DC_OW_NONf, DC_OW_TRNf, DC_OW_SRVf, DC_OW_TRGf)
obs_yrange<-range(DC_CRf,DC_OW_NONf)
bxp_yrange<-range(BS_CR,BS_OW_NON,BS_OW_TRN,BS_OW_SRV,BS_OW_TRG)
bxp_ydom<-(bxp_yrange[2]-bxp_yrange[1])
RAMObs_xrange<-c(0,max(c(dim(as.array(RAM_CR)),dim(as.array(RAM_OW_NON)))))
RAMObs_yrange<-range(RAM_CR,RAM_OW_NON)/1000
RAM_dim_leg<-rap*max(RAM);
CPUObs_xrange<-c(0,max(c(dim(as.array(CPU_CR)),dim(as.array(CPU_OW_NON)))))
CPUObs_yrange<-range(CPU_CR,CPU_OW_NON)
CPU_dim_leg<-rap*max(CPU);
if (is2Host) {
	RRAMObs_xrange<-c(0,max(c(dim(as.array(RRAM_CR)),dim(as.array(RRAM_OW_NON)))))
	RRAMObs_yrange<-range(RRAM_CR,RRAM_OW_NON)/1000
	RRAM_dim_leg<-rap*max(RRAM)
	RCPUObs_xrange<-c(0,max(c(dim(as.array(RCPU_CR)),dim(as.array(RCPU_OW_NON)))))
	RCPUObs_yrange<-range(RCPU_CR,RCPU_OW_NON)
	RCPU_dim_leg<-rap*max(RCPU)}

###OBSERVATIONS graph
png(gr1,width=png_width,height=png_height)
plot(DC_CRf,ylim=obs_yrange,col=color[1],pch="o",
	main="Observations",ylab="Latency [usec]",xlab="Index Number [#]")
points(DC_OW_NONf,col=color[2],pch="o")
legend(0,obs_yrange[2],nomi[1:2],color[1:2])
dev.off()

### ECDFs graph
png(gr2,width=png_width,height=png_height)
plot(ecdf(DC_CRf),xlim=xrange,ylim=c(0,1),col.hor=color[1],do.points=FALSE,lty=tipi[1],lwd=CD_lwd,
	xlab="Latency [usec]",ylab="Sample Distribution [%]",main="Invocation benchmark (ECDF)",)
plot(ecdf(DC_OW_NONf),add=TRUE, col.hor=color[2],do.points=FALSE,lty=tipi[2],lwd=CD_lwd)
plot(ecdf(DC_OW_TRNf),add=TRUE, col.hor=color[3],do.points=FALSE,lty=tipi[3],lwd=CD_lwd)
plot(ecdf(DC_OW_SRVf),add=TRUE, col.hor=color[4],do.points=FALSE,lty=tipi[4],lwd=CD_lwd)
plot(ecdf(DC_OW_TRGf),add=TRUE, col.hor=color[5],do.points=FALSE,lty=tipi[5],lwd=CD_lwd)
legend(xrange[2]*.75,.25,nomi,color)
dev.off()

###BOXPLOT graph
png(gr3,width=png_width,height=png_height)
boxplot(DC_CRf,DC_OW_NONf,DC_OW_TRNf,DC_OW_SRVf,DC_OW_TRGf,
	ylim=bxp_yrange,
	col=color,notch=TRUE,boxwex=.8,pch=".",names=nomi,
	main="Invocation benchmarks (Box & Whisker graph)",
	xlab="",ylab="Latency [usec]")
dev.off()

###RAMObs graph
png(grRO,width=png_width,height=png_height)
plot(RAM_CR/1000,col=color[1],pch="o",
	xlim=RAMObs_xrange,ylim=RAMObs_yrange,
	main="RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
points(RAM_OW_NON/1000,col=color[2],pch="o")
legend(0,RAMObs_yrange[2],nomi[1:2],color[1:2])
dev.off()

###RAM graph
png(grR,width=png_width,height=png_height)
barplot(RAM,xlim=c(0,max(RAM)+1.5*RAM_dim_leg),col=color,space=1,horiz=TRUE,
	main="Max RAM occupacy",xlab="Occupacy [kBytes]")
for(i in 1:dim(as.array(RAM))) {
text(RAM[i]+.6*RAM_dim_leg,i*2-1+l1,num_dec(RAM[i]))
text(RAM[i]+.6*RAM_dim_leg,i*2-1+l2,nomi[i])}
dev.off()

###CPUObs graph
png(grCO,width=png_width,height=png_height)
plot(CPU_CR,col=color[1],pch="o",
	xlim=CPUObs_xrange,ylim=CPUObs_yrange,
	main="CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
points(CPU_OW_NON,col=color[2],pch="o")
legend(0,CPUObs_yrange[2],nomi[1:2],color[1:2])
dev.off()

###CPU graph
png(grC,width=png_width,height=png_height)
barplot(CPU,xlim=c(0,max(CPU)+1.5*CPU_dim_leg),col=color,space=1,horiz=TRUE,
	main="Average CPU occupacy",xlab="Occupacy [%]")
for(i in 1:dim(as.array(CPU))) {
text(CPU[i]+.6*CPU_dim_leg,i*2-1+l1,num_dec(CPU[i]))
text(CPU[i]+.6*CPU_dim_leg,i*2-1+l2,nomi[i])}
dev.off()

if (is2Host) {
	###RRAMObs graph
	png(grRRO,width=png_width,height=png_height)
	plot(RRAM_CR/1000,col=color[1],pch="o",
		xlim=RRAMObs_xrange,ylim=RRAMObs_yrange,
		main="Remote RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
	points(RRAM_OW_NON/1000,col=color[2],pch="o")
	legend(0,RRAMObs_yrange[2],nomi[1:2],color[1:2])
	dev.off()

	###RRAM graph
	png(grRR,width=png_width,height=png_height)
	barplot(RRAM,xlim=c(0,max(RRAM)+1.5*RRAM_dim_leg),col=color,space=1,horiz=TRUE,
	main="Max Remote RAM occupacy",xlab="Occupacy [kBytes]")
	for(i in 1:dim(as.array(RRAM))) {
		text(RRAM[i]+.6*RRAM_dim_leg,i*2-1+l1,num_dec(RRAM[i]))
		text(RRAM[i]+.6*RRAM_dim_leg,i*2-1+l2,nomi[i])}
	dev.off()

	###RCPUObs graph
	png(grRCO,width=png_width,height=png_height)
	plot(RCPU_CR,col=color[1],pch="o",
		xlim=RCPUObs_xrange,ylim=RCPUObs_yrange,
		main="Remote CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
	points(RCPU_OW_NON,col=color[2],pch="o")
	legend(0,RCPUObs_yrange[2],nomi[1:2],color[1:2])
	dev.off()

	###RCPU graph
	png(grRC,width=png_width,height=png_height)
	barplot(RCPU,xlim=c(0,max(RCPU)+1.5*RCPU_dim_leg),col=color,space=1,horiz=TRUE,
	main="Average Remote CPU occupacy",xlab="Occupacy [%]")
	for(i in 1:dim(as.array(RCPU))) {
		text(RCPU[i]+.6*RCPU_dim_leg,i*2-1+l1,num_dec(RCPU[i]))
		text(RCPU[i]+.6*RCPU_dim_leg,i*2-1+l2,nomi[i])}
	dev.off()}

###HTML SUMMARY
write("<html>",som)
write(c("<title> Invocation Performance Test - CARDAMOM </title>",
	"<h1> Invocation Summary</h1>",
	"<h2>", HTML_type, "</h2>",
	"<h2>", HTML_delay, "</h2>",
	"<h2>", HTML_host, "</h2><body><hr>"),som,append=TRUE)

###GRAFICI
write(c("<h3> Summary Graphs </h3>","<p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr1,"\"> Invocation Benchmark (Observations)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr2,"\"> Invocation Benchmark (ECDF)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr3,"\"> Invocation Benchmark (BoxPlot)</a> <p> </p>"),som,append=TRUE)

write(c("<table><tr><td>"),som,append=TRUE)
write(c("<a href=\"",grRO,"\"> RAM Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grR,"\"> Max RAM occupacy </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grCO,"\"> CPU Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grC,"\"> Average CPU occupacy </a><p></p></td>"),som,append=TRUE)

if (is2Host) {
	write(c("<td><a href=\"",grRRO,"\"> REMOTE RAM Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRR,"\"> Max REMOTE RAM occupacy </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRCO,"\"> REMOTE CPU Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRC,"\"> Average REMOTE CPU occupacy </a><p></p></td>"),
	som,append=TRUE)}

write(c("</tr></table><hr>"),som,append=TRUE)

###TABELLA SUMMARY
write(c("<h3> Summary Table </h3>","<p> Statistics vs Scenario </p>","<p> </p>",
	"<body>","<table border=1>"),som,append=TRUE)
write(c("<tr><th> &nbsp; </th><th> Min. </th><th> 1st Qu. </th><th> Median </th>",
	"<th>Mean</th><th>3rd Qu.</th><th>Max.</th><th>Std.Dev.</th></tr>"),som,append=TRUE)

	write(c("<tr><td>",nomi[1],"</td>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_CRf)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_CRf))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><td>",nomi[2],"</td>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_NONf)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_NONf))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><td>",nomi[3],"</td>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_TRNf)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_TRNf))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><td>",nomi[4],"</td>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_SRVf)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_SRVf))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><td>",nomi[5],"</td>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_TRGf)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_TRGf))),"</td></tr>"),som,append=TRUE)
write("</table><p></p><hr>",som,append=TRUE)

###TABELLA OUTLIERS
write("<h3> Outliers Summary Table </h3><p> Values of outstanding points </p><p></p>",
	som,append=TRUE)
write("<table border=1>",som,append=TRUE)
	write(c("<tr><th> Out of ",thresh*100,"% </th>"),som,append=TRUE)
for(i in 1:dim(outliers)[1]) write(c("<th>",i,"' </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)

for (j in 1:dim(as.array(nomi))) {
	write("<tr>",som,append=TRUE)
	write(c("<td>", nomi[j], "</td>"),som,append=TRUE)
	for (i in 1:dim(outliers)[1]) 
		write(c("<td>",num_dec(outliers[i,j]),"</td>"),som,append=TRUE)
	write("</tr>",som,append=TRUE)}
write("</table><p></p><hr>",som,append=TRUE)
write("</body>",som,append=TRUE)
write("</html>",som,append=TRUE)

###CLEANING
