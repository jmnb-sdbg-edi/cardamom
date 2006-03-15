############## function definitions ###############
## filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top(filter_warmup(x))
## return numember x with dec decimal
num_dec <- function(x,dec) x<- round(x*(10^dec))/(10^dec)
###################################################

warmup <- 10      # if warmup=N then the first N samples are filtered out (warm up stage)
threshold <- 1    # if threshold=N then N% of samples is filtered out

## Get data
data_lb<-scan("CR-0/DELTA_C_LB.dat")
data<-scan("CR-0/DELTA_C.dat")

## filtering data 
data_lb_f<-filter(data_lb)
data_f<-filter(data)

## media

media_lb<-mean(data_lb_f)
media<-mean(data_f)

## standard deviation

std_dev<-sd(data_f)
std_dev_lb<-sd(data_lb_f)

## calculate difference in percentage

diff_perc <- ((media_lb-media)/media) * 100

## write results
fname<-"scenario.results"
write("               D-020 result",fname);
write("-------------------------------------------",fname,append=TRUE);
write(ncolumns=3,c(" Results: LB invocation overhead:",
                   num_dec(diff_perc,2),
		   " %"),
                   fname,append=TRUE);
write("-------------------------------------------",fname,append=TRUE);
