results <- "scenario.result"
write("********* PERFORMANCE TEST C-100 ***********", results)
write("*** please note:  unit is (1 invocation) ***", results, append=TRUE)

###ACQUISITION OF COUNTER.dat
COUNTER_THR_10<-scan("THR-10/COUNTER.dat")

write("", results, append=TRUE)
write("THR-10 results", results, append=TRUE)
write("--------------", results, append=TRUE)
write(ncolumns=2, c("NOMINAL =", COUNTER_THR_10[1]), results, append=TRUE)
write(ncolumns=2, c("COUNTER =", COUNTER_THR_10[2]), results, append=TRUE)

rm(COUNTER_THR_10, results)
