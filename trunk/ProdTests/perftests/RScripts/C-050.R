results <- "scenario.result"

write("******** PERFORMANCE TEST C-050 ******", results)
write("*** please note:  unit is (1 msec) ***", results, append=TRUE)


###ACQUISITION OF DELTA_C.dat
DC_CR_10<-scan("CR-10/DELTA_C.dat")
DC_CR_50<-scan("CR-50/DELTA_C.dat")
DC_CR_100<-scan("CR-100/DELTA_C.dat")
DC_CR_500<-scan("CR-500/DELTA_C.dat")

warmup <- 10        # if warmup=N then the first N samples are filtered out (warm up stage)
threshold <- 0.5    # if threshold=N then 2N% of samples is filtered out (with the "filter_top_and_down" function)

#filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter_down <- function(x) sort(x)[ceiling((length(x)*(threshold%%100))/100):length(x)]
filter_top_and_down <- function(x) sort(x)[ceiling((length(x)*(threshold%%100))/100):floor((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top_and_down(filter_warmup(x))

### DELTA_C FILTERING
DC_CR_10f<-filter(DC_CR_10/1000);
DC_CR_50f<-filter(DC_CR_50/1000);
DC_CR_100f<-filter(DC_CR_100/1000);
DC_CR_500f<-filter(DC_CR_500/1000);


write("", results, append=TRUE)
write("CR-10 results", results, append=TRUE)
write("-------------", results, append=TRUE)
T <- 10
write(ncolumns=2, c("max|Ts-T| =", max(abs(DC_CR_10f-T))), results, append=TRUE)
write("", results, append=TRUE)
write("CR-50 results", results, append=TRUE)
write("-------------", results, append=TRUE)
T <- 50
write(ncolumns=2, c("max|Ts-T| =", max(abs(DC_CR_50f-T))), results, append=TRUE)
write("", results, append=TRUE)
write("CR-100 results", results, append=TRUE)
write("--------------", results, append=TRUE)
T <- 100
write(ncolumns=2, c("max|Ts-T| =", max(abs(DC_CR_100f-T))), results, append=TRUE)
write("", results, append=TRUE)
write("CR-500 results", results, append=TRUE)
write("--------------", results, append=TRUE)
T <- 500
write(ncolumns=2, c("max|Ts-T| =", max(abs(DC_CR_500f-T))), results, append=TRUE)
write("", results, append=TRUE)


rm(DC_CR_10,DC_CR_50,DC_CR_100,DC_CR_500,DC_CR_10f,DC_CR_50f,DC_CR_100f,DC_CR_500f, results,T)
