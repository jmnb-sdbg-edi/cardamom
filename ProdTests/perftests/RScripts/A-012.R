#* =========================================================================== *
#* This file is part of CARDAMOM (R) which is jointly developed by THALES
#* and SELEX-SI.
#*
#* It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
#* All rights reserved.
#* 
#* CARDAMOM is free software; you can redistribute it and/or modify it under
#* the terms of the GNU Library General Public License as published by the
#* Free Software Foundation; either version 2 of the License, or (at your
#* option) any later version.
#* 
#* CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
#* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
#* License for more details.
#* 
#* You should have received a copy of the GNU Library General
#* Public License along with CARDAMOM; see the file COPYING. If not, write to
#* the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#* =========================================================================== *

results <- "scenario.result"
write("******** PERFORMANCE TEST A-012 ******", results)
write("*** please note:  unit is (1 msec) ***", results, append=TRUE)


###ACQUISITION OF DELTA_C.dat
DC_CR_0<-scan("CR-0/DELTA_C.dat")
DC_CR_10<-scan("CR-10/DELTA_C.dat")
DC_CR_100<-scan("CR-100/DELTA_C.dat")
DC_CR_500<-scan("CR-500/DELTA_C.dat")
DC_COL_CR_0<-scan("COLLOCATED_CR-0/DELTA_C.dat")
DC_COL_CR_10<-scan("COLLOCATED_CR-10/DELTA_C.dat")
DC_COL_CR_100<-scan("COLLOCATED_CR-100/DELTA_C.dat")
DC_COL_CR_500<-scan("COLLOCATED_CR-500/DELTA_C.dat")

warmup <- 10      # if warmup=N then the first N samples are filtered out (warm up stage)
threshold <- 1    # if threshold=N then N% of samples is filtered out (with the "filter_top" function)

#filter: exclude outliers and/or warm up stage samples from the data set
filter_warmup <- function(x) x[warmup:length(x)]
filter_top <- function(x) sort(x)[0:ceiling((length(x)*(100-(threshold%%100)))/100)]
filter <- function(x) filter_top(filter_warmup(x))


### DELTA_C FILTERING
DC_CR_0f<-filter(DC_CR_0/1000);
DC_CR_10f<-filter(DC_CR_10/1000);
DC_CR_100f<-filter(DC_CR_100/1000);
DC_CR_500f<-filter(DC_CR_500/1000);
DC_COL_CR_0f<-filter(DC_COL_CR_0/1000);
DC_COL_CR_10f<-filter(DC_COL_CR_10/1000);
DC_COL_CR_100f<-filter(DC_COL_CR_100/1000);
DC_COL_CR_500f<-filter(DC_COL_CR_500/1000);


write("", results, append=TRUE)
write("CR-0 results", results, append=TRUE)
write("------------", results, append=TRUE)
avg1 <- mean(DC_COL_CR_0f)
avg2 <- mean(DC_CR_0f)
write(ncolumns=2, c("AVG1 (Collocated) =", avg1), results, append=TRUE)
write(ncolumns=2, c("AVG2 (Not-Collocated) =", avg2), results, append=TRUE)
write(ncolumns=2, c("AVG1/AVG2 =", avg1/avg2), results, append=TRUE)
write("", results, append=TRUE)
write("CR-10 results", results, append=TRUE)
write("-------------", results, append=TRUE)
avg1 <- mean(DC_COL_CR_10f)
avg2 <- mean(DC_CR_10f)
write(ncolumns=2, c("AVG1 (Collocated) =", avg1), results, append=TRUE)
write(ncolumns=2, c("AVG2 (Not-Collocated) =", avg2), results, append=TRUE)
write(ncolumns=2, c("AVG1/AVG2 =", avg1/avg2), results, append=TRUE)
write("", results, append=TRUE)
write("CR-100 results", results, append=TRUE)
write("--------------", results, append=TRUE)
avg1 <- mean(DC_COL_CR_100f)
avg2 <- mean(DC_CR_100f)
write(ncolumns=2, c("AVG1 (Collocated) =", avg1), results, append=TRUE)
write(ncolumns=2, c("AVG2 (Not-Collocated) =", avg2), results, append=TRUE)
write(ncolumns=2, c("AVG1/AVG2 =", avg1/avg2), results, append=TRUE)
write("", results, append=TRUE)
write("CR-500 results", results, append=TRUE)
write("--------------", results, append=TRUE)
avg1 <- mean(DC_COL_CR_500f)
avg2 <- mean(DC_CR_500f)
write(ncolumns=2, c("AVG1 (Collocated) =", avg1), results, append=TRUE)
write(ncolumns=2, c("AVG2 (Not-Collocated) =", avg2), results, append=TRUE)
write(ncolumns=2, c("AVG1/AVG2 =", avg1/avg2), results, append=TRUE)
write("", results, append=TRUE)


rm(DC_CR_0,DC_CR_10,DC_CR_100,DC_CR_500,
DC_CR_0f,DC_CR_10f,DC_CR_100f,DC_CR_500f,
DC_COL_CR_0,DC_COL_CR_10,DC_COL_CR_100,DC_COL_CR_500,
DC_COL_CR_0f,DC_COL_CR_10f,DC_COL_CR_100f,DC_COL_CR_500f,
results, avg1, avg2)
