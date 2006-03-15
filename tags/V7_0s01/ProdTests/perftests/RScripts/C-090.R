############## function definitions ###############
## filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top(filter_warmup(x))
## return numember x with dec decimal
num_dec <- function(x,dec) x<- round(x*(10^dec))/(10^dec)
###################################################

warmup <- 2         # if warmup=N then the first N-1 samples are filtered out (warm up stage)

results <- "scenario.result"

## Acuisition of configuration file
client_cfg <- scan("client.cfg")

## Acquisition of DELTA_C.dat
DATASET<-scan("DELTA_C.dat")

## exclude warm up stage samples from the data set
DS_f<-filter_warmup(DATASET);
length(DS_f)

## calculate difference from measures and required interval for timer expiration
diff_int <- DS_f - client_cfg[2]*1000

## calculate how many timer expires in advance
lt_zero <- diff_int[diff_int < 0]
num_lt_zero <- length(lt_zero)

## calculate maximum delay of timer expiration
max_delay <- max(diff_int)

write("******** PERFORMANCE TEST C-090 ******", results)
write("", results, append=TRUE)
write("Results:", results, append=TRUE)
write("------------------------------------------------------------------------", results, append=TRUE)
write(ncolumns=2, c("Number of Timers expirated in advance:",num_lt_zero),results, append=TRUE)
write(ncolumns=3, c("Maximum delay of expiration:",max_delay,"usec"),results, append=TRUE)
write("------------------------------------------------------------------------", results, append=TRUE)

rm(warmup, results, client_cfg, DATASET, DS_f, diff_int, lt_zero, num_lt_zero, max_delay)
