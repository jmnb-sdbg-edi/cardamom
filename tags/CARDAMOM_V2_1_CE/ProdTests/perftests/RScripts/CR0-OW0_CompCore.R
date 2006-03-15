###INVOCATION COMPARISON CR-0 OW-0

###ACQUISITION OF DELTA_C.dat
DC_CR_0<-scan("CR-0/DELTA_C.dat")
DC_OW_0<-scan("OW-0/DELTA_C.dat")


#filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top(filter_warmup(x))


### DELTA_C FILTERING (note: while filtering, time unit is also converted from usec to msec)
DC_CR_0f<-filter(DC_CR_0/1000);
DC_OW_0f<-filter(DC_OW_0/1000);

write("", results, append=TRUE)
write("CR-0 results", results, append=TRUE)
write("------------", results, append=TRUE)
avg <- mean(DC_CR_0f)
std <- sqrt(var(DC_CR_0f))
write(ncolumns=2, c("AVG =", avg), results, append=TRUE)
write(ncolumns=2, c("STD =", std), results, append=TRUE)
write(ncolumns=2, c("STD/AVG =", std/avg), results, append=TRUE)
write("", results, append=TRUE)
write("OW-0 results", results, append=TRUE)
write("------------", results, append=TRUE)
avg <- mean(DC_OW_0f)
std <- sqrt(var(DC_OW_0f))
write(ncolumns=2, c("AVG =", avg), results, append=TRUE)
write(ncolumns=2, c("STD =", std), results, append=TRUE)
write(ncolumns=2, c("STD/AVG =", std/avg), results, append=TRUE)


rm(DC_CR_0,DC_OW_0,DC_CR_0f,DC_OW_0f, avg, std)
