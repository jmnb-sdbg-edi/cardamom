### TIME INVOCATION COMPARISON CR-0
############## function definitions ###############
## filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top(filter_warmup(x))
## return numember x with dec decimal
num_dec <- function(x,dec) x<- round(x*(10^dec))/(10^dec)
###################################################

###ACQUISITION OF DELTA_C.dat
DC_CR_0<-scan("CR-0/DELTA_C.dat")

## filter data that satisfy the SSS requirements
DC_CR_0f <- DC_CR_0[ DC_CR_0 < TIME_REQ]

## calculate the percentage
PERCENT <- (length(DC_CR_0f)/length(DC_CR_0))*100

write("", results, append=TRUE)
write("------------------------------------------------------------------------", results, append=TRUE)
write(ncolumns=8,c(" Results:\n - Elapsed time is shorter than",
		   TIME_REQ,
		   "usec in",
		   num_dec(PERCENT,2),
		   "% of cases.\n",
		   "- SSS Requirements is:",
		   MOST_CASES,
		   " %"),
                   results,append=TRUE);
write("------------------------------------------------------------------------",results,append=TRUE);

rm(DC_CR_0, DC_CR_0f, TIME_REQ, MOST_CASES, PERCENT)
