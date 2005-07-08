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

###INVOCATION COMPARISON CORE

filter<-function(x) x<-x[x<quantile(x,thresh)]
out<-function(x) x<-x[x>quantile(x,thresh)] 
num_dec <- function(x) x<-round(x*(10^dec_num))/(10^dec_num)
dig_dev <- function(x) x<-round(x*(10^dev_dig))/(10^dev_dig)

### DELTA_C FILTERING
DC_CR_0f<-filter(DC_CR_0); DC_CR_100f<-filter(DC_CR_100)
DC_OW_0f<-filter(DC_OW_0); DC_OW_100f<-filter(DC_OW_100)
DA_OW_0f<-filter(DA_OW_0); DA_OW_100f<-filter(DA_OW_100)

### DELTA_C OUTING
outliers<-data.frame(sort(out(DC_CR_0)),sort(out(DC_CR_100)),sort(out(DC_OW_0)),
	sort(out(DC_OW_100)),sort(out(DA_OW_0)),sort(out(DA_OW_100)))

### Massima occupazione RAM in kBytes
RAM<-c(max(RAM_CR_0)-min(RAM_CR_0),max(RAM_CR_100)-min(RAM_CR_100),
	max(RAM_OW_0)-min(RAM_OW_0),max(RAM_OW_100)-min(RAM_OW_100))
RAM<-RAM/1000

### Massima occupazione CPU in %
CPU<-c(mean(CPU_CR_0),mean(CPU_CR_100),mean(CPU_OW_0),mean(CPU_OW_100))

if (is2Host) {
	### Massima occupazione RAM in kBytes
	RRAM<-c(max(RRAM_CR_0)-min(RRAM_CR_0),max(RRAM_CR_100)-min(RRAM_CR_100),	
		max(RRAM_OW_0)-min(RRAM_OW_0),max(RRAM_OW_100)-min(RRAM_OW_100))
	RRAM<-RRAM/1000	
	
	### Massima occupazione CPU in %
	RCPU<-c(mean(RCPU_CR_0),mean(RCPU_CR_100),mean(RCPU_OW_0),mean(RCPU_OW_100))}

### AUX VARIABLES
BS_CR_0<-boxplot.stats(DC_CR_0f)$stats
BS_CR_100<-boxplot.stats(DC_CR_100f)$stats
BS_OW_0<-boxplot.stats(DC_OW_0f)$stats
BS_OW_100<-boxplot.stats(DC_OW_100f)$stats
BSA_OW_0<-boxplot.stats(DA_OW_0f)$stats
BSA_OW_100<-boxplot.stats(DA_OW_100f)$stats
xrange<-range(DC_CR_0f,DC_CR_100f,DC_OW_0f,DC_OW_100f,DA_OW_0f,DA_OW_100f)
obs_yrange<-range(DC_CR_0f,DC_CR_100f)
bxp_yrange<-range(BS_CR_0,BS_CR_100,BS_OW_0,BS_OW_100,BSA_OW_0,BSA_OW_100)
bxp_ydom<-(bxp_yrange[2]-bxp_yrange[1])
RAMObs_xrange<-c(0,max(c(dim(as.array(RAM_CR_0)),dim(as.array(RAM_CR_100)))))
RAMObs_yrange<-range(RAM_CR_0,RAM_CR_100)/1000
RAM_dim_leg<-rap*max(RAM);
CPUObs_yrange<-range(CPU_CR_0,CPU_CR_100)
CPUObs_xrange<-c(0,max(c(dim(as.array(CPU_CR_0)),dim(as.array(CPU_CR_100)))))
CPU_dim_leg<-rap*max(CPU);
if (is2Host) {
	RRAMObs_xrange<-c(0,max(c(dim(as.array(RRAM_CR_0)),dim(as.array(RRAM_CR_100)))))
	RRAMObs_yrange<-range(RRAM_CR_0,RRAM_CR_100)/1000
	RRAM_dim_leg<-rap*max(RRAM);

	RCPUObs_yrange<-range(RCPU_CR_0,RCPU_CR_100)
	RCPUObs_xrange<-c(0,max(c(dim(as.array(RCPU_CR_0)),dim(as.array(RCPU_CR_100)))))
	RCPU_dim_leg<-rap*max(RCPU);}

###OBSERVATION graph
png(gr1,width=png_width,height=png_height)
plot(DC_CR_0f,ylim=obs_yrange,col=color[1],pch="o",
	main="Observations",ylab="Latency [usec]",xlab="Index Number [#]")
points(DC_CR_100f,col=color[2],pch="o")
legend(0,obs_yrange[2],nomi[1:2],color[1:2])
dev.off()

### ECDFs graph (Autoscale)
png(gr2,width=png_width,height=png_height)
plot(ecdf(DC_CR_0f),ylim=c(0,1),xlim=xrange,col.hor=color[1],do.points=FALSE,lty=tipi[5],lwd=CD_lwd,
	xlab="Latency [usec]",ylab="Sample Distributions [%]",
	main="Invocation benchmark (AutoScaled ECDF)")
plot(ecdf(DC_CR_100f),add=TRUE, col.hor=color[2],do.points=FALSE,lty=tipi[6],lwd=CD_lwd)
plot(ecdf(DC_OW_0f),add=TRUE,col.hor=color[3],do.points=FALSE,lty=tipi[1],lwd=CD_lwd)
plot(ecdf(DC_OW_100f),add=TRUE, col.hor=color[4],do.points=FALSE,lty=tipi[2],lwd=CD_lwd)
plot(ecdf(DA_OW_0f),add=TRUE, col.hor=color[5],do.points=FALSE,lty=tipi[3],lwd=CD_lwd)
plot(ecdf(DA_OW_100f),add=TRUE, col.hor=color[6],do.points=FALSE,lty=tipi[4],lwd=CD_lwd)
legend(xrange[2]*3/4,.25,nomi,color)
dev.off()

### ECDFs graph (0..1000)
png(gr3,width=png_width,height=png_height)
plot(ecdf(DC_CR_0f),ylim=c(0,1),xlim=c(0,1000),col.hor=color[1],do.points=FALSE,lty=tipi[5],lwd=CD_lwd,
	xlab="Latency [usec]",ylab="Sample Distributions [%]",
	main="Invocation benchmark (Scaled ECDF)")
plot(ecdf(DC_CR_100f),add=TRUE, col.hor=color[2],do.points=FALSE,lty=tipi[6],lwd=CD_lwd)
plot(ecdf(DC_OW_0f),add=TRUE,col.hor=color[3],do.points=FALSE,lty=tipi[1],lwd=CD_lwd)
plot(ecdf(DC_OW_100f),add=TRUE, col.hor=color[4],do.points=FALSE,lty=tipi[2],lwd=CD_lwd)
plot(ecdf(DA_OW_0f),add=TRUE, col.hor=color[5],do.points=FALSE,lty=tipi[3],lwd=CD_lwd)
plot(ecdf(DA_OW_100f),add=TRUE, col.hor=color[6],do.points=FALSE,lty=tipi[4],lwd=CD_lwd)
legend(1000*3/4,.25,nomi,color)
dev.off()

###BOXPLOT graph (Autoscale)
png(gr4,width=png_width,height=png_height)
boxplot(DC_CR_0f,DC_CR_100f,DC_OW_0f,DC_OW_100f,DA_OW_0f,DA_OW_100f,
	ylim=c(bxp_yrange[1]-(.05*bxp_ydom),bxp_yrange[2]),
	col=color,notch=TRUE,boxwex=.8,pch=".",names=nomi,
	main="Invocation benchmarks (Box & Whisker AutoScaled graph)",
	xlab="",ylab="Latency [usec]")
dev.off()

###BOXPLOT graph (0..1000)
png(gr5,width=png_width,height=png_height)
boxplot(DC_CR_0f,DC_CR_100f,DC_OW_0f,DC_OW_100f,DA_OW_0f,DA_OW_100f,
	ylim=c(0,1000),
	col=color,notch=TRUE,boxwex=.8,pch=".",names=nomi,
	main="Invocation benchmarks (Box & Whisker Scaled graph)",
	xlab="",ylab="Latency [usec]")
dev.off()

###RAMObs graph
png(grRO,width=png_width,height=png_height)
plot(RAM_CR_0/1000,col=color[1],pch="o",
	xlim=RAMObs_xrange,ylim=RAMObs_yrange,
	main="RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
points(RAM_CR_100/1000,col=color[2],pch="o")
legend(0,RAMObs_yrange[2],nomi [1:2],color[1:2])
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
plot(CPU_CR_0,col=color[1],pch="o",
	xlim=CPUObs_xrange,ylim=CPUObs_yrange,
	main="CPU Observations",ylab="Usec CPU [%]",xlab="Time [sec]")
points(CPU_CR_100,col=color[2],pch="o")
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
	plot(RRAM_CR_0/1000,col=color[1],pch="o",
	xlim=RRAMObs_xrange,ylim=RRAMObs_yrange,
	main="Remote RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
	points(RRAM_CR_100/1000,col=color[2],pch="o")
	legend(0,RRAMObs_yrange[2],c("RAM CR Delay:0","RAM CR delay:100"),color[1:2])
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
	plot(RCPU_CR_0,col=color[1],pch="o",
		xlim=RCPUObs_xrange,ylim=RCPUObs_yrange,
		main="Remote CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
	points(RCPU_CR_100,col=color[2],pch="o")
	legend(0,RCPUObs_yrange[2],nomi[1:2],color[1:2])
	dev.off()

	###RCPU graph
	png(grRC,width=png_width,height=png_height)
	barplot(RCPU,xlim=c(0,max(RCPU)+1.5*RCPU_dim_leg),col=color,space=1,horiz=TRUE,
		main="Average CPU occupacy",xlab="Occupacy [%]")
	for(i in 1:dim(as.array(RCPU))) {
		text(RCPU[i]+.6*RCPU_dim_leg,i*2-1+l1,num_dec(RCPU[i]))
		text(RCPU[i]+.6*RCPU_dim_leg,i*2-1+l2,nomi[i])}
	dev.off()}

###HTML SUMMARY
write("<html>",som)
write(c("<title> Invocation Performance Test - CARDAMOM </title>",
	"<h1> Invocation Summary - Comparison </h1>",
	"<h4>",HTML_type,"</h4>",
	"<h4>",HTML_delay,"</h4>",
	"<h4>",HTML_host,"</h4><body><hr>"),som,append=TRUE)

###GRAFICI
write("<h3> Summary Graphs </h3><p></p>",som,append=TRUE)
write(c("<a href=\"",gr1,"\">     Invocation Benchmark (Observations)</a><p></p>"),som,append=TRUE)

write(c("<table><tr><td>"),som,append=TRUE)
write(c("<a href=\"",gr2,"\"> Invocation Benchmark (ECDF) </a><p></p>"),
	som,append=TRUE)
write(c("<a href=\"",gr4,"\"> Invocation Benchmark (BoxPlot) </a><p></p>"),
	som,append=TRUE)
write("</td><td>",som,append=TRUE)
write(c("<a href=\"",gr3,"\"> Invocation Benchmark (1 msec scaled ECDF) </a><p></p>"),
	som,append=TRUE)
write(c("<a href=\"",gr5,"\"> Invocation Benchmark (1 msec scaled BoxPlot) </a><p></p>"),
	som,append=TRUE)
write(c("</td></tr></table>"),som,append=TRUE)

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

write(c("</tr></table><p></p><hr>"),som,append=TRUE)

###TABELLA SUMMARY
write(c("<h3> Summary Table </h3>","<p> Statistics vs Scenario </p>","<p> </p>"),som,append=TRUE)
write("<body>",som,append=TRUE)
write("<table border=1>",som,append=TRUE)
write(c("<tr><th> &nbsp; </th><th> Min. </th><th> 1st Qu. </th><th> Median </th>",
	"<th>Mean</th><th>3rd Qu.</th><th>Max.</th><th>Std.Dev.</th></tr>"),som,append=TRUE)

### WRITE LABEL (e.g."CR del:0")
      	write(c("<tr><th>",nomi[1],"</th>"),som,append=TRUE)
### WRITE 6 values (Min, 1st Qu., Median, Mean, 3rd Qu., Max)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_CR_0f)[i]),"</td>"),som,append=TRUE)
### WRITE Standard Deviation
write(c("<td>",dig_dev(sqrt(var(DC_CR_0f))),"</td></tr>"),som,append=TRUE)
###
	write(c("<tr><th>",nomi[2],"</th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_CR_100f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_CR_100f))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><th>",nomi[3],"</th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_0f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_0f))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><th>",nomi[4],"</th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DC_OW_100f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DC_OW_100f))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><th>",nomi[5],"</th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DA_OW_0f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DA_OW_0f))),"</td></tr>"),som,append=TRUE)
	write(c("<tr><th>",nomi[6],"</th>"),som,append=TRUE)
for(i in 1:6) write(c("<td>",num_dec(summary(DA_OW_100f)[i]),"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(DA_OW_100f))),"</td></tr>"),som,append=TRUE)
write("</table><hr>",som,append=TRUE)

###TABELLA OUTLIERS
write("<h3> Outliers Summary Table </h3><p> Values of outstanding points </p><p></p>",
	som,append=TRUE)
write("<table border=1>",som,append=TRUE)
	write(c("<tr><th> Out of ",thresh*100,"% </th>"),som,append=TRUE)
for(i in 1:dim(outliers)[1]) write(c("<th>",i,"' </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)

for (j in 1:dim(as.array(nomi))) {
	write("<tr>",som,append=TRUE)
	write(c("<th>", nomi[j], "</th>"),som,append=TRUE)
	for (i in 1:dim(outliers)[1]) 
		write(c("<td>",num_dec(outliers[i,j]),"</td>"),som,append=TRUE)
	write("</tr>",som,append=TRUE)}
write("</table><p></p><hr>",som,append=TRUE)
write("</body>",som,append=TRUE)
write("</html>",som,append=TRUE)

rm(dig_dev,filter,i,j,num_dec,out,
	BSA_OW_0,BSA_OW_100,BS_CR_0,BS_CR_100,BS_OW_0,BS_OW_100,
	DA_OW_0f,DA_OW_100f,DC_CR_0f,DC_CR_100f,DC_OW_0f,DC_OW_100f,
	CPU,CPUObs_xrange,CPUObs_yrange,CPU_dim_leg,
	RAM,RAMObs_xrange,RAMObs_yrange,RAM_dim_leg,
	outliers,bxp_ydom,bxp_yrange,obs_yrange,xrange)

if (is2Host) rm(RCPU,RCPUObs_xrange,RCPUObs_yrange,RCPU_dim_leg,
		RRAM,RRAMObs_xrange,RRAMObs_yrange,RRAM_dim_leg)
