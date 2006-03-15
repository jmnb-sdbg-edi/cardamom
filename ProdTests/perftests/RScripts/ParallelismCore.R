### PARALLELISM CORE

num_dec <- function(x) x<-round(x*(10^dec_num))/(10^dec_num)

### Massima occupazione RAM in kBytes
RAM<-c(max(RAM_1)-min(RAM_1),max(RAM_2)-min(RAM_2),max(RAM_3)-min(RAM_3),max(RAM_4)-min(RAM_4),
	max(RAM_5)-min(RAM_5),max(RAM_6)-min(RAM_6),max(RAM_7)-min(RAM_7),max(RAM_8)-min(RAM_8),
	max(RAM_9)-min(RAM_9),max(RAM_10)-min(RAM_10),max(RAM_11)-min(RAM_11),
	max(RAM_12)-min(RAM_12),max(RAM_13)-min(RAM_13),max(RAM_14)-min(RAM_14),
	max(RAM_15)-min(RAM_15),max(RAM_16)-min(RAM_16),max(RAM_17)-min(RAM_17))
RAM<-RAM/1000

### Massima occupazione CPU in %
CPU<-c(mean(CPU_1),mean(CPU_2),mean(CPU_3),mean(CPU_4),mean(CPU_5),mean(CPU_6),mean(CPU_7),
	mean(CPU_8),mean(CPU_9),mean(CPU_10),mean(CPU_11),mean(CPU_12),mean(CPU_13),
	mean(CPU_14),mean(CPU_15),mean(CPU_16),mean(CPU_17))

if(is2Host){
	### Massima occupazione RRAM in kBytes
	RRAM<-c(max(RRAM_1)-min(RRAM_1),max(RRAM_2)-min(RRAM_2),max(RRAM_3)-min(RRAM_3),
		max(RRAM_4)-min(RRAM_4),max(RRAM_5)-min(RRAM_5),max(RRAM_6)-min(RRAM_6),
		max(RRAM_7)-min(RRAM_7),max(RRAM_8)-min(RRAM_8),max(RRAM_9)-min(RRAM_9),
		max(RRAM_10)-min(RRAM_10),max(RRAM_11)-min(RRAM_11),max(RRAM_12)-min(RRAM_12),
		max(RRAM_13)-min(RRAM_13),max(RRAM_14)-min(RRAM_14),max(RRAM_15)-min(RRAM_15),
		max(RRAM_16)-min(RRAM_16),max(RRAM_17)-min(RRAM_17))
	RRAM<-RRAM/1000

	### Massima occupazione RCPU in %
	RCPU<-c(mean(RCPU_1),mean(RCPU_2),mean(RCPU_3),mean(RCPU_4),mean(RCPU_5),mean(RCPU_6),
		mean(RCPU_7),mean(RCPU_8),mean(RCPU_9),mean(RCPU_10),mean(RCPU_11),mean(RCPU_12),
		mean(RCPU_13),mean(RCPU_14),mean(RCPU_15),mean(RCPU_16),mean(RCPU_17))}

### AUX VARIABLES
RAMObs_xrange<-c(0,max(c(dim(as.array(RAM_1)),dim(as.array(RAM_17)))))
RAMObs_yrange<-range(RAM_1,RAM_17)/1000
CPUObs_xrange<-c(0,max(c(dim(as.array(CPU_1)),dim(as.array(CPU_17)))))
CPUObs_yrange<-range(CPU_1,CPU_17)
if (is2Host) {
	RRAMObs_xrange<-c(0,max(c(dim(as.array(RRAM_1)),dim(as.array(RRAM_17)))))
	RRAMObs_yrange<-range(RRAM_1,RRAM_17)/1000
	RCPUObs_xrange<-c(0,max(c(dim(as.array(RCPU_1)),dim(as.array(RCPU_17)))))
	RCPUObs_yrange<-range(RCPU_1,RCPU_17)}

###MEAN graph
png(gr1,width=png_width,height=png_height)
plot(PAR~x,col=color[1],type="l",
	main="Parallelism benchmark (Average)",ylab="Latency [usec]",xlab="Thread Number [#]")
dev.off()

###THROUGHPUT graph
png(gr2,width=png_width,height=png_height)
plot((1000/PAR)~x,col=color[1],type="l",
	main="Parallelism benchmark (Average)",
	ylab="Throughput [#/msec]",xlab="Thread Number [#]")
dev.off()

###RAMObs graph
png(grRO,width=png_width,height=png_height)
plot(RAM_1/1000,col=color[1],pch="o",
	xlim=RAMObs_xrange,ylim=RAMObs_yrange,
	main="RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
points(RAM_17/1000,col=color[2],pch="o")
legend(0,RAMObs_yrange[2],c(nomi[1],nomi[17]),color[1:2])
dev.off()

###RAM graph
png(grR,width=png_width,height=png_height)
plot(RAM~x,type="l",lwd=CD_lwd,
	main="Max RAM occupacy",xlab="Thread Number [#]",ylab="Occupacy [kBytes]")
dev.off()

###CPUObs graph
png(grCO,width=png_width,height=png_height)
plot(CPU_1,col=color[1],pch="o",
	xlim=CPUObs_xrange,ylim=CPUObs_yrange,
	main="CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
points(CPU_17,col=color[2],pch="o")
legend(0,CPUObs_yrange[2],c(nomi[1],nomi[17]),color[1:2])
dev.off()

###CPU graph
png(grC,width=png_width,height=png_height)
plot(CPU~x,type="l",lwd=CD_lwd,
	main="Average CPU occupacy",xlab="Thread Number [#]",ylab="Occupacy [%]")
dev.off()

if (is2Host) {
	###RRAMObs graph
	png(grRRO,width=png_width,height=png_height)
	plot(RRAM_1/1000,col=color[1],pch="o",
		xlim=RRAMObs_xrange,ylim=RRAMObs_yrange,
		main="Remote RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
	points(RRAM_17/1000,col=color[2],pch="o")
	legend(0,RRAMObs_yrange[2],c(nomi[1],nomi[17]),color[1:2])
	dev.off()

	###RRAM graph
	png(grRR,width=png_width,height=png_height)
	plot(RRAM~x,type="l",lwd=CD_lwd,
		main="Max Remote RAM occupacy",xlab="Thread Number [#]",ylab="Occupacy [kBytes]")
	dev.off()

	###RCPUObs graph
	png(grRCO,width=png_width,height=png_height)
	plot(RCPU_1,col=color[1],pch="o",
		xlim=RCPUObs_xrange,ylim=RCPUObs_yrange,
		main="Remote CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
	points(RCPU_17,col=color[2],pch="o")
	legend(0,RCPUObs_yrange[2],c(nomi[1],nomi[17]),color[1:2])
	dev.off()

	###RCPU graph
	png(grRC,width=png_width,height=png_height)
	plot(RCPU~x,type="l",lwd=CD_lwd,
		main="Average Remote CPU occupacy",xlab="Thread Number [#]",ylab="Occupacy [%]")
	dev.off()}

###HTML SUMMARY
write("<html>",som)
write(c("<title> Parallelism Performance Test - CARDAMOM </title>",
	"<h1> Parallelism Summary</h1>",
	"<h2>", HTML_type, "</h2>",
	"<h2>", HTML_delay, "</h2>",
	"<h2>", HTML_host, "</h2><body><hr>"),som,append=TRUE)

###GRAFICI
write(c("<h3> Summary Graphs </h3>","<p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr1,"\"> Parallelism Benchmark (Latency)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr2,"\"> Parallelism Benchmark (Throughput)</a> <p> </p>"),
	som,append=TRUE)
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
write(c("<h3> Latency Table </h3>",
	"<p> Average Latency vs Thread Number </p><p></p><body><table border=1>"),
	som,append=TRUE)

write(c("<tr><th> Threads </th>"),som,append=TRUE)
for(i in 1:dim(as.array(nomi))) 
	write(c("<th>",as.character(x)[i]," </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)

write("<tr><td> Average </td>",som,append=TRUE)
for(i in 1:dim(as.array(PAR)))
	write(c("<td>", num_dec(PAR[i]), "</td>"),som,append=TRUE)
write("</tr></table><p></p><hr>",som,append=TRUE)

write(c("<h3> Throughput Table </h3>",
	"<p> Average Throughput vs Thread Number </p><p></p><table border=1>"),
	som,append=TRUE)

write("<tr><th> Threads </th>",som,append=TRUE)
for(i in 1:dim(as.array(nomi))) 
	write(c("<th>",as.character(x)[i],"</th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)

write("<tr><td> Average </td>",som,append=TRUE)

for(i in 1:dim(as.array(PAR))) 
	write(c("<td>", num_dec(1000/PAR[i]),"</td>"),som,append=TRUE)
write("</tr></table><p></p><hr></body></html>",som,append=TRUE)

rm(CPU,CPUObs_xrange,CPUObs_yrange,RAM,RAMObs_xrange,RAMObs_yrange,i,num_dec)

if (is2Host) rm(RCPU,RCPUObs_xrange,RCPUObs_yrange,RRAM,RRAMObs_xrange,RRAMObs_yrange)
