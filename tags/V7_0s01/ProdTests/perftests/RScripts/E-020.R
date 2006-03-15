###################################################
# crash time thresholds (from the QTPDR document)
###################################################
CRASH_DET_TIME1=120
CRASH_DET_TIME2=200
###################################################
## percentage thresholds (from the QTPDR document)
###################################################
PERCENT1=95
PERCENT2=99
###################################################

############## function definitions ###############

## return numember x with dec decimal
num_dec <- function(x,dec) x<- round(x*(10^dec))/(10^dec)

###################################################

### read process crash time
prc_time<-scan("time2process.dat",list("",0,"",0));
### extract seconds from the table
prc_sec<-prc_time[[2]];
### extract usec from the table
prc_usec<-prc_time[[4]];

### read observer notificiation time
obs_time<-scan("time2observer.dat",list("",0,"",0));
### extract seconds from the table
obs_sec<-obs_time[[2]];
### extract usec from the table
obs_usec<-obs_time[[4]];

### calculate elapsed seconds
elapsed_sec<-obs_sec-prc_sec;
### calculate elapsed microseconds
elapsed_usec<-obs_usec-prc_usec;
## total elapsed microseconds (10-6 sec)
elapsed_tot=(elapsed_sec*1000000)+(elapsed_usec)
## total number of element
len_tot = length(elapsed_tot)
## number of element with elapsed time > CRASH_DET_TIME1 (in usec)
len_percent1 <- length(elapsed_tot[elapsed_tot > (CRASH_DET_TIME1*1000)])
## number of element with elapsed time > CRASH_DET_TIME1 (in usec)
len_percent2 <- length(elapsed_tot[elapsed_tot > (CRASH_DET_TIME2*1000)])
## percentage of element with elapsed time > CRASH_DET_TIME1
DET_PERCENT1 <- (len_percent1 / len_tot) * 100
## percentage of element with elapsed time > CRASH_DET_TIME1
DET_PERCENT2 <- (len_percent2 / len_tot) * 100

## write results
fname<-"scenario.results"
write("               E-020 result",fname);
write("-------------------------------------------",fname,append=TRUE);
write(ncolumns=7,c("Percentage with notification time >",
                   CRASH_DET_TIME1,
                   "ms:\n",
                   num_dec(DET_PERCENT1,1),
                   "%  (threshold:",
                   PERCENT1,
                   "%)"),
                   fname,append=TRUE);
write(ncolumns=7,c("Percentage with notification time >",
                   CRASH_DET_TIME2,
                   "ms:\n",
                   num_dec(DET_PERCENT2,1),
                   "%  (threshold:",
                   PERCENT2,
                   "%)"),
                   fname,append=TRUE);

write("-------------------------------------------",fname,append=TRUE);

