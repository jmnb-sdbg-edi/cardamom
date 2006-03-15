scan("CR-0-2000-1/DELTA_C.dat") -> cd2000a
scan("CR-0-2000-2/DELTA_C.dat") -> cd2000b
scan("CR-0-2000-3/DELTA_C.dat") -> cd2000c
scan("CR-0-2000-4/DELTA_C.dat") -> cd2000d
scan("CR-0-2000-5/DELTA_C.dat") -> cd2000e

scan("CR-0-4000-1/DELTA_C.dat") -> cd4000a
scan("CR-0-4000-2/DELTA_C.dat") -> cd4000b
scan("CR-0-4000-3/DELTA_C.dat") -> cd4000c
scan("CR-0-4000-4/DELTA_C.dat") -> cd4000d
scan("CR-0-4000-5/DELTA_C.dat") -> cd4000e

scan("CR-0-8000-1/DELTA_C.dat") -> cd8000a
scan("CR-0-8000-2/DELTA_C.dat") -> cd8000b
scan("CR-0-8000-3/DELTA_C.dat") -> cd8000c
scan("CR-0-8000-4/DELTA_C.dat") -> cd8000d
scan("CR-0-8000-5/DELTA_C.dat") -> cd8000e

scan("CR-0-16000-1/DELTA_C.dat") -> cd16000a
scan("CR-0-16000-2/DELTA_C.dat") -> cd16000b
scan("CR-0-16000-3/DELTA_C.dat") -> cd16000c
scan("CR-0-16000-4/DELTA_C.dat") -> cd16000d
scan("CR-0-16000-5/DELTA_C.dat") -> cd16000e

scan("CR-0-32000-1/DELTA_C.dat") -> cd32000a
scan("CR-0-32000-2/DELTA_C.dat") -> cd32000b
scan("CR-0-32000-3/DELTA_C.dat") -> cd32000c
scan("CR-0-32000-4/DELTA_C.dat") -> cd32000d
scan("CR-0-32000-5/DELTA_C.dat") -> cd32000e

filter<-function(x) x<-x[x<quantile(x,0.90)]
dec_num<-function(x) x<-(round(x*100)/100)

print(c("2000 samples"))
print(summary(filter(cd2000a)))
print(summary(filter(cd2000b)))
print(summary(filter(cd2000c)))
print(summary(filter(cd2000d)))
print(summary(filter(cd2000e)))

massimo<-c(0); minimo<-c(0)
for(i in 1:6){
	massimo[i]<-max(summary(filter(cd2000a))[i],summary(filter(cd2000b))[i],
		summary(filter(cd2000c))[i],summary(filter(cd2000d))[i],
		summary(filter(cd2000e))[i])
	minimo[i]<-min(summary(filter(cd2000a))[i],summary(filter(cd2000b))[i],
		summary(filter(cd2000c))[i],summary(filter(cd2000d))[i],
		summary(filter(cd2000e))[i])}
err<-c(0)
for(i in 1:6)
	err[i]<-dec_num((massimo[i]-minimo[i])/minimo[i])
print(err)

print(c("4000 samples"))
print(summary(filter(cd4000a)))
print(summary(filter(cd4000b)))
print(summary(filter(cd4000c)))
print(summary(filter(cd4000d)))
print(summary(filter(cd4000e)))

print(c("8000 samples"))
print(summary(filter(cd8000a)))
print(summary(filter(cd8000b)))
print(summary(filter(cd8000c)))
print(summary(filter(cd8000d)))
print(summary(filter(cd8000e)))

print(c("16000 samples"))
print(summary(filter(cd16000a)))
print(summary(filter(cd16000b)))
print(summary(filter(cd16000c)))
print(summary(filter(cd16000d)))
print(summary(filter(cd16000e)))

print(c("32000 samples"))
print(summary(filter(cd32000a)))
print(summary(filter(cd32000b)))
print(summary(filter(cd32000c)))
print(summary(filter(cd32000d)))
print(summary(filter(cd32000e)))

mean(filter(cd2000a)) -> cd2000af
mean(filter(cd2000b)) -> cd2000bf
mean(filter(cd2000c)) -> cd2000cf
mean(filter(cd2000d)) -> cd2000df
mean(filter(cd2000e)) -> cd2000ef

mean(filter(cd4000a)) -> cd4000af
mean(filter(cd4000b)) -> cd4000bf
mean(filter(cd4000c)) -> cd4000cf
mean(filter(cd4000d)) -> cd4000df
mean(filter(cd4000e)) -> cd4000ef

mean(filter(cd8000a)) -> cd8000af
mean(filter(cd8000b)) -> cd8000bf
mean(filter(cd8000c)) -> cd8000cf
mean(filter(cd8000d)) -> cd8000df
mean(filter(cd8000e)) -> cd8000ef

mean(filter(cd16000a)) -> cd16000af
mean(filter(cd16000b)) -> cd16000bf
mean(filter(cd16000c)) -> cd16000cf
mean(filter(cd16000d)) -> cd16000df
mean(filter(cd16000e)) -> cd16000ef

mean(filter(cd32000a)) -> cd32000af
mean(filter(cd32000b)) -> cd32000bf
mean(filter(cd32000c)) -> cd32000cf
mean(filter(cd32000d)) -> cd32000df
mean(filter(cd32000e)) -> cd32000ef


c(cd2000af,cd2000bf,cd2000cf,cd2000df,cd2000ef) -> m2000
c(cd4000af,cd4000bf,cd4000cf,cd4000df,cd4000ef) -> m4000
c(cd8000af,cd8000bf,cd8000cf,cd8000df,cd8000ef) -> m8000
c(cd16000af,cd16000bf,cd16000cf,cd16000df,cd16000ef) -> m16000
c(cd32000af,cd32000bf,cd32000cf,cd32000df,cd32000ef) -> m32000

print(c("Distribution"))
print(summary(m2000))
print(summary(m4000))
print(summary(m8000))
print(summary(m16000))
print(summary(m32000))

png("ObsNumBxp.png",800,600)
boxplot(cd2000a,cd2000b,cd2000c,cd2000d,cd2000e,NA,
	cd4000a,cd4000b,cd4000c,cd4000d,cd4000e,NA,
	cd8000a,cd8000b,cd8000c,cd8000d,cd8000e,NA,
	cd16000a,cd16000b,cd16000c,cd16000d,cd16000e,NA,
	cd32000a,cd32000b,cd32000c,cd32000d,cd32000e,
	pch=" ",ylim=c(150,400),
	names=c("2k",NA,NA,NA,NA,NA,"4k",NA,NA,NA,NA,NA,"8k",NA,NA,NA,NA,NA,
		"16k",NA,NA,NA,NA,NA,"32k",NA,NA,NA,NA),
	col=c(2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6),
	main="Sample Distributions VS Observation Number")
dev.off()

png("MeanNumBxp.png",800,600)
boxplot(m2000,m4000,m8000,m16000,m32000,
	names=c("2k","4k","8k","16k","32k"),
	col=c(2,3,4,5,6),
	main="Average Latency VS Observation Number")
dev.off()

media<-c(mean(m2000),mean(m4000),mean(m8000),mean(m16000),mean(m32000))
x<-c(2,4,8,16,32)

png("MeanNum.png",800,600)
plot(media~x,type="l",col=2,
	main="Average Latency VS Observation Number")
dev.off()

stddev<-c(sqrt(var(m2000)),sqrt(var(m4000)),sqrt(var(m8000)),sqrt(var(m16000)),sqrt(var(m32000)))

png("StdNum.png",800,600)
plot(stddev~x,type="l",col=2,
	main="Standard Deviation VS Observation Number")
dev.off()
