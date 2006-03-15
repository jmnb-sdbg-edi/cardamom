
CR_100<-scan("CR-100/DELTA_C.dat")
OW_100<-scan("OW-100/DELTA_C.dat")
CR_0<-scan("CR-0/DELTA_C.dat")
OW_0<-scan("OW-0/DELTA_C.dat")

xrange<-c(1,50)
yrange<-c(100,1500)
xdom<-xrange[2]-xrange[1]

png("Transitory.png",800,600)
plot(CR_100,
	ylim=yrange,xlim=xrange,
	type="l",col=2,
	main="Transitory",xlab="Observation number [#]",ylab="Latency [usec]")
points(OW_100,type="l",col=3)
points(CR_0,type="l",col=4)
points(OW_0,type="l",col=6)
legend(xrange[2]-.16*xdom,yrange[2],
	c("CR del:100","OW del:100","CR:del 0","OW:del 0"),
	c(2,3,4,6))
dev.off()