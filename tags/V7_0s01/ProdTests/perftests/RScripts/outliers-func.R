### FUNCTION FOR MANAGE OUTLIERS
### (C) 2004 Lello Mele

thresh <- 1.5

#filter: eliminina gli outlier

filter <- function(x) x[0:ceiling((length(x)*(100-(thresh%%100)))/100)]

#out: prende solo gli outlier

out <- function(x) x[(floor((length(x)*(100-(thresh%%100)))/100)+1):length(x)]
