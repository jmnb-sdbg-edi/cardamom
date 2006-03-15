ser<-scan("OW-0-32000/ABS_S.dat")
cli<-scan("OW-0-32000/ABS_C.dat")

rit_ser<-c(0)
rit_cli<-c(0)

for(i in 1:(dim(as.array(ser))-1)) rit_ser[i]<-(ser[i+1]-ser[i])
for(i in 1:(dim(as.array(cli))-1)) rit_cli[i]<-(cli[i+1]-cli[i])

png("ServerInvABS.png",800,600)
plot(ser,pch=".",col=2,ylim=range(ser,cli),
	main="Absolute Timing for Invocation Benchmark",
	xlab="Invocation Number [#]",ylab="Absolute time [usec]")
points(cli,pch=".",col=4)
legend(1,max(c(ser,cli)),c("ABS_S","ABS_C"),c(2,4))
dev.off()

png("ServerInvRIT.png",800,600)
plot(rit_ser,pch=".",col=2,ylim=range(rit_ser,rit_cli),
	main="Inter-Delay for Invocation Benchmark",
	xlab="Invocation Number [#]",ylab="Inter-Delay [usec]")
points(rit_cli,pch=".",col=4)
legend(1,max(c(rit_ser,rit_cli)),c("ABS_S","ABS_C"),c(2,4))
dev.off()