results <- "scenario.result"

warmup <- 10      # if warmup=N then the first N samples are filtered out (warm up stage)
threshold <- 1    # if threshold=N then N% of samples is filtered out

write("****** PERFORMANCE TEST C-040 ********", results)
write("*** please note:  unit is (1 msec) ***", results, append=TRUE)
source("CR0_CompCore.R")
rm(results)
