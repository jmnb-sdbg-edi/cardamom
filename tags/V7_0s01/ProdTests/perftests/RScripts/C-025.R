results <- "scenario.result"

warmup <- 10      # if warmup=N then the first N samples are filtered out (warm up stage)
threshold <- 1    # if threshold=N then N% of samples is filtered out

TIME_REQ <- 100    # usec, SSS requirements
MOST_CASES <- 95   # %, SSS requirements

write("****** PERFORMANCE TEST C-025 ********", results)
source("CR0_CompCore.R")
rm(results)
