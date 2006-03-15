### MARSHALLING CORE CCM (4 to 64k lenght)

filter<-function(x) x<-x[x<quantile(x,thresh)]
out<-function(x) x<-x[x>quantile(x,thresh)]
num_dec <- function(x) x<-round(x*(10^dec_num))/(10^dec_num)
dig_dev <- function(x) x<-round(x*(10^dev_dig))/(10^dev_dig)
Q1<-function(x) quantile(x,.25)
Q3<-function(x) quantile(x,.75)

###DELTA_C (CCM) FILTERING

OBS_A_8f<-filter(OBS_A_8);
OBS_A_16f<-filter(OBS_A_16);
OBS_A_32f<-filter(OBS_A_32);
OBS_A_64f<-filter(OBS_A_64);
OBS_A_128f<-filter(OBS_A_128);
OBS_A_256f<-filter(OBS_A_256);
OBS_A_512f<-filter(OBS_A_512);
OBS_A_1024f<-filter(OBS_A_1024);
OBS_A_2048f<-filter(OBS_A_2048);
OBS_A_4096f<-filter(OBS_A_4096);
OBS_A_8192f<-filter(OBS_A_8192);
OBS_A_16384f<-filter(OBS_A_16384);
OBS_A_32768f<-filter(OBS_A_32768);
OBS_A_65536f<-filter(OBS_A_65536);

OBS_F_4f<-filter(OBS_F_4);
OBS_F_8f<-filter(OBS_F_8);
OBS_F_16f<-filter(OBS_F_16);
OBS_F_32f<-filter(OBS_F_32);
OBS_F_64f<-filter(OBS_F_64);
OBS_F_128f<-filter(OBS_F_128);
OBS_F_256f<-filter(OBS_F_256);
OBS_F_512f<-filter(OBS_F_512);
OBS_F_1024f<-filter(OBS_F_1024);
OBS_F_2048f<-filter(OBS_F_2048);
OBS_F_4096f<-filter(OBS_F_4096);
OBS_F_8192f<-filter(OBS_F_8192);
OBS_F_16384f<-filter(OBS_F_16384);
OBS_F_32768f<-filter(OBS_F_32768);
OBS_F_65536f<-filter(OBS_F_65536);

OBS_L_4f<-filter(OBS_L_4);
OBS_L_8f<-filter(OBS_L_8);
OBS_L_16f<-filter(OBS_L_16);
OBS_L_32f<-filter(OBS_L_32);
OBS_L_64f<-filter(OBS_L_64);
OBS_L_128f<-filter(OBS_L_128);
OBS_L_256f<-filter(OBS_L_256);
OBS_L_512f<-filter(OBS_L_512);
OBS_L_1024f<-filter(OBS_L_1024);
OBS_L_2048f<-filter(OBS_L_2048);
OBS_L_4096f<-filter(OBS_L_4096);
OBS_L_8192f<-filter(OBS_L_8192);
OBS_L_16384f<-filter(OBS_L_16384);
OBS_L_32768f<-filter(OBS_L_32768);
OBS_L_65536f<-filter(OBS_L_65536);

OBS_R_8f<-filter(OBS_R_8);
OBS_R_16f<-filter(OBS_R_16);
OBS_R_32f<-filter(OBS_R_32);
OBS_R_64f<-filter(OBS_R_64);
OBS_R_128f<-filter(OBS_R_128);
OBS_R_256f<-filter(OBS_R_256);
OBS_R_512f<-filter(OBS_R_512);
OBS_R_1024f<-filter(OBS_R_1024);
OBS_R_2048f<-filter(OBS_R_2048);
OBS_R_4096f<-filter(OBS_R_4096);
OBS_R_8192f<-filter(OBS_R_8192);
OBS_R_16384f<-filter(OBS_R_16384);
OBS_R_32768f<-filter(OBS_R_32768);
OBS_R_65536f<-filter(OBS_R_65536);

###DELTA_C (Normal) FILTERING
OBSN_A_8f<-filter(OBSN_A_8);
OBSN_A_16f<-filter(OBSN_A_16);
OBSN_A_32f<-filter(OBSN_A_32);
OBSN_A_64f<-filter(OBSN_A_64);
OBSN_A_128f<-filter(OBSN_A_128);
OBSN_A_256f<-filter(OBSN_A_256);
OBSN_A_512f<-filter(OBSN_A_512);
OBSN_A_1024f<-filter(OBSN_A_1024);
OBSN_A_2048f<-filter(OBSN_A_2048);
OBSN_A_4096f<-filter(OBSN_A_4096);
OBSN_A_8192f<-filter(OBSN_A_8192);
OBSN_A_16384f<-filter(OBSN_A_16384);
OBSN_A_32768f<-filter(OBSN_A_32768);
OBSN_A_65536f<-filter(OBSN_A_65536);

OBSN_F_4f<-filter(OBSN_F_4);
OBSN_F_8f<-filter(OBSN_F_8);
OBSN_F_16f<-filter(OBSN_F_16);
OBSN_F_32f<-filter(OBSN_F_32);
OBSN_F_64f<-filter(OBSN_F_64);
OBSN_F_128f<-filter(OBSN_F_128);
OBSN_F_256f<-filter(OBSN_F_256);
OBSN_F_512f<-filter(OBSN_F_512);
OBSN_F_1024f<-filter(OBSN_F_1024);
OBSN_F_2048f<-filter(OBSN_F_2048);
OBSN_F_4096f<-filter(OBSN_F_4096);
OBSN_F_8192f<-filter(OBSN_F_8192);
OBSN_F_16384f<-filter(OBSN_F_16384);
OBSN_F_32768f<-filter(OBSN_F_32768);
OBSN_F_65536f<-filter(OBSN_F_65536);

OBSN_L_4f<-filter(OBSN_L_4);
OBSN_L_8f<-filter(OBSN_L_8);
OBSN_L_16f<-filter(OBSN_L_16);
OBSN_L_32f<-filter(OBSN_L_32);
OBSN_L_64f<-filter(OBSN_L_64);
OBSN_L_128f<-filter(OBSN_L_128);
OBSN_L_256f<-filter(OBSN_L_256);
OBSN_L_512f<-filter(OBSN_L_512);
OBSN_L_1024f<-filter(OBSN_L_1024);
OBSN_L_2048f<-filter(OBSN_L_2048);
OBSN_L_4096f<-filter(OBSN_L_4096);
OBSN_L_8192f<-filter(OBSN_L_8192);
OBSN_L_16384f<-filter(OBSN_L_16384);
OBSN_L_32768f<-filter(OBSN_L_32768);
OBSN_L_65536f<-filter(OBSN_L_65536);

OBSN_R_8f<-filter(OBSN_R_8);
OBSN_R_16f<-filter(OBSN_R_16);
OBSN_R_32f<-filter(OBSN_R_32);
OBSN_R_64f<-filter(OBSN_R_64);
OBSN_R_128f<-filter(OBSN_R_128);
OBSN_R_256f<-filter(OBSN_R_256);
OBSN_R_512f<-filter(OBSN_R_512);
OBSN_R_1024f<-filter(OBSN_R_1024);
OBSN_R_2048f<-filter(OBSN_R_2048);
OBSN_R_4096f<-filter(OBSN_R_4096);
OBSN_R_8192f<-filter(OBSN_R_8192);
OBSN_R_16384f<-filter(OBSN_R_16384);
OBSN_R_32768f<-filter(OBSN_R_32768);
OBSN_R_65536f<-filter(OBSN_R_65536);

### DELTA_C OUTING
outliers<-data.frame(sort(out(OBS_A_8)),
	sort(out(OBS_A_16)),
	sort(out(OBS_A_32)),
	sort(out(OBS_A_64)),
	sort(out(OBS_A_128)),
	sort(out(OBS_A_256)),
	sort(out(OBS_A_512)),
	sort(out(OBS_A_1024)),
	sort(out(OBS_A_2048)),
	sort(out(OBS_A_4096)),
	sort(out(OBS_A_8192)),
	sort(out(OBS_A_16384)),
	sort(out(OBS_A_32768)),
	sort(out(OBS_A_65536)),
	sort(out(OBS_F_4)),
	sort(out(OBS_F_8)),
	sort(out(OBS_F_16)),
	sort(out(OBS_F_32)),
	sort(out(OBS_F_64)),
	sort(out(OBS_F_128)),
	sort(out(OBS_F_256)),
	sort(out(OBS_F_512)),
	sort(out(OBS_F_1024)),
	sort(out(OBS_F_2048)),
	sort(out(OBS_F_4096)),
	sort(out(OBS_F_8192)),
	sort(out(OBS_F_16384)),
	sort(out(OBS_F_32768)),
	sort(out(OBS_F_65536)),
	sort(out(OBS_L_4)),
	sort(out(OBS_L_8)),
	sort(out(OBS_L_16)),
	sort(out(OBS_L_32)),
	sort(out(OBS_L_64)),
	sort(out(OBS_L_128)),
	sort(out(OBS_L_256)),
	sort(out(OBS_L_512)),
	sort(out(OBS_L_1024)),
	sort(out(OBS_L_2048)),
	sort(out(OBS_L_4096)),
	sort(out(OBS_L_8192)),
	sort(out(OBS_L_16384)),
	sort(out(OBS_L_32768)),
	sort(out(OBS_L_65536)),
	sort(out(OBS_R_8)),
	sort(out(OBS_R_16)),
	sort(out(OBS_R_32)),
	sort(out(OBS_R_64)),
	sort(out(OBS_R_128)),
	sort(out(OBS_R_256)),
	sort(out(OBS_R_512)),
	sort(out(OBS_R_1024)),
	sort(out(OBS_R_2048)),
	sort(out(OBS_R_4096)),
	sort(out(OBS_R_8192)),
	sort(out(OBS_R_16384)),
	sort(out(OBS_R_32768)),
	sort(out(OBS_R_65536)))

### Massima occupazione RAM in kBytes
RAM<-c( max(RAM_A_8)-min(RAM_A_8),max(RAM_A_16)-min(RAM_A_16),max(RAM_A_32)-min(RAM_A_32),
	max(RAM_A_64)-min(RAM_A_64),max(RAM_A_128)-min(RAM_A_128),max(RAM_A_256)-min(RAM_A_256),
	max(RAM_A_512)-min(RAM_A_512),max(RAM_A_1024)-min(RAM_A_1024),
	max(RAM_A_2048)-min(RAM_A_2048),max(RAM_A_4096)-min(RAM_A_4096),
	max(RAM_A_8192)-min(RAM_A_8192),max(RAM_A_16384)-min(RAM_A_16384),
	max(RAM_A_32768)-min(RAM_A_32768),max(RAM_A_65536)-min(RAM_A_65536),

	max(RAM_F_4)-min(RAM_F_4),max(RAM_F_8)-min(RAM_F_8),max(RAM_F_16)-min(RAM_F_16),
	max(RAM_F_32)-min(RAM_F_32),max(RAM_F_64)-min(RAM_F_64),max(RAM_F_128)-min(RAM_F_128),
	max(RAM_F_256)-min(RAM_F_256),max(RAM_F_512)-min(RAM_F_512),
	max(RAM_F_1024)-min(RAM_F_1024),max(RAM_F_2048)-min(RAM_F_2048),
	max(RAM_F_4096)-min(RAM_F_4096),
	max(RAM_F_8192)-min(RAM_F_8192),max(RAM_F_16384)-min(RAM_F_16384),
	max(RAM_F_32768)-min(RAM_F_32768),max(RAM_F_65536)-min(RAM_F_65536),

	max(RAM_L_4)-min(RAM_L_4),max(RAM_L_8)-min(RAM_L_8),max(RAM_L_16)-min(RAM_L_16),
	max(RAM_L_32)-min(RAM_L_32),max(RAM_L_64)-min(RAM_L_64),max(RAM_L_128)-min(RAM_L_128),
	max(RAM_L_256)-min(RAM_L_256),max(RAM_L_512)-min(RAM_L_512),
	max(RAM_L_1024)-min(RAM_L_1024),max(RAM_L_2048)-min(RAM_L_2048),
	max(RAM_L_4096)-min(RAM_L_4096),
	max(RAM_L_8192)-min(RAM_L_8192),max(RAM_L_16384)-min(RAM_L_16384),
	max(RAM_L_32768)-min(RAM_L_32768),max(RAM_L_65536)-min(RAM_L_65536),

	max(RAM_R_8)-min(RAM_R_8),max(RAM_R_16)-min(RAM_R_16),max(RAM_R_32)-min(RAM_R_32),
	max(RAM_R_64)-min(RAM_R_64),max(RAM_R_128)-min(RAM_R_128),max(RAM_R_256)-min(RAM_R_256),
	max(RAM_R_512)-min(RAM_R_512),max(RAM_R_1024)-min(RAM_R_1024),
	max(RAM_R_2048)-min(RAM_R_2048),max(RAM_R_4096)-min(RAM_R_4096),
	max(RAM_R_8192)-min(RAM_R_8192),max(RAM_R_16384)-min(RAM_R_16384),
	max(RAM_R_32768)-min(RAM_R_32768),max(RAM_R_65536)-min(RAM_R_65536))
RAM<-RAM/1000

### Occupazione Media CPU in %
CPU<-c( mean(CPU_A_8),mean(CPU_A_16),mean(CPU_A_32),mean(CPU_A_64),mean(CPU_A_128),
	mean(CPU_A_256),mean(CPU_A_512),mean(CPU_A_1024),mean(CPU_A_2048),mean(CPU_A_4096),
mean(CPU_A_8192),mean(CPU_A_16384),mean(CPU_A_32768),mean(CPU_A_65536),
	mean(CPU_F_4),mean(CPU_F_8),mean(CPU_F_16),mean(CPU_F_32),mean(CPU_F_64),mean(CPU_F_128),
	mean(CPU_F_256),mean(CPU_F_512),mean(CPU_F_1024),mean(CPU_F_2048),mean(CPU_F_4096),
mean(CPU_F_8192),mean(CPU_F_16384),mean(CPU_F_32768),mean(CPU_F_65536),

	mean(CPU_L_4),mean(CPU_L_8),mean(CPU_L_16),mean(CPU_L_32),mean(CPU_L_64),mean(CPU_L_128),
	mean(CPU_L_256),mean(CPU_L_512),mean(CPU_L_1024),mean(CPU_L_2048),mean(CPU_L_4096),
mean(CPU_L_8192),mean(CPU_L_16384),mean(CPU_L_32768),mean(CPU_L_65536),
	mean(CPU_R_8),mean(CPU_R_16),mean(CPU_R_32),mean(CPU_R_64),mean(CPU_R_128),
	mean(CPU_R_256),mean(CPU_R_512),mean(CPU_R_1024),mean(CPU_R_2048),mean(CPU_R_4096),
mean(CPU_R_8192),mean(CPU_R_16384),mean(CPU_R_32768),mean(CPU_R_65536))

if (is2Host) {
### Massima occupazione RRAM in kBytes
RRAM<-c(max(RRAM_A_8)-min(RRAM_A_8),max(RRAM_A_16)-min(RRAM_A_16),max(RRAM_A_32)-min(RRAM_A_32),
	max(RRAM_A_64)-min(RRAM_A_64),max(RRAM_A_128)-min(RRAM_A_128),
	max(RRAM_A_256)-min(RRAM_A_256),max(RRAM_A_512)-min(RRAM_A_512),
	max(RRAM_A_1024)-min(RRAM_A_1024),max(RRAM_A_2048)-min(RRAM_A_2048),
	max(RRAM_A_4096)-min(RRAM_A_4096),
	max(RRAM_A_8192)-min(RRAM_A_8192),max(RRAM_A_16384)-min(RRAM_A_16384),
	max(RRAM_A_32768)-min(RRAM_A_32768),max(RRAM_A_65536)-min(RRAM_A_65536),
	max(RRAM_F_4)-min(RRAM_F_4),max(RRAM_F_8)-min(RRAM_F_8),max(RRAM_F_16)-min(RRAM_F_16),
	max(RRAM_F_32)-min(RRAM_F_32),max(RRAM_F_64)-min(RRAM_F_64),
	max(RRAM_F_128)-min(RRAM_F_128),max(RRAM_F_256)-min(RRAM_F_256),
	max(RRAM_F_512)-min(RRAM_F_512),max(RRAM_F_1024)-min(RRAM_F_1024),
	max(RRAM_F_2048)-min(RRAM_F_2048),max(RRAM_F_4096)-min(RRAM_F_4096),
	max(RRAM_F_8192)-min(RRAM_F_8192),max(RRAM_F_16384)-min(RRAM_F_16384),
	max(RRAM_F_32768)-min(RRAM_F_32768),max(RRAM_F_65536)-min(RRAM_F_65536),

	max(RRAM_L_4)-min(RRAM_L_4),max(RRAM_L_8)-min(RRAM_L_8),max(RRAM_L_16)-min(RRAM_L_16),
	max(RRAM_L_32)-min(RRAM_L_32),max(RRAM_L_64)-min(RRAM_L_64),
	max(RRAM_L_128)-min(RRAM_L_128),max(RRAM_L_256)-min(RRAM_L_256),
	max(RRAM_L_512)-min(RRAM_L_512),max(RRAM_L_1024)-min(RRAM_L_1024),
	max(RRAM_L_2048)-min(RRAM_L_2048),max(RRAM_L_4096)-min(RRAM_L_4096),
	max(RRAM_L_8192)-min(RRAM_L_8192),max(RRAM_L_16384)-min(RRAM_L_16384),
	max(RRAM_L_32768)-min(RRAM_L_32768),max(RRAM_L_65536)-min(RRAM_L_65536),

	max(RRAM_R_8)-min(RRAM_R_8),max(RRAM_R_16)-min(RRAM_R_16),max(RRAM_R_32)-min(RRAM_R_32),
	max(RRAM_R_64)-min(RRAM_R_64),max(RRAM_R_128)-min(RRAM_R_128),
	max(RRAM_R_256)-min(RRAM_R_256),max(RRAM_R_512)-min(RRAM_R_512),
	max(RRAM_R_1024)-min(RRAM_R_1024),max(RRAM_R_2048)-min(RRAM_R_2048),
	max(RRAM_R_4096)-min(RRAM_R_4096),
	max(RRAM_R_8192)-min(RRAM_R_8192),max(RRAM_R_16384)-min(RRAM_R_16384),
	max(RRAM_R_32768)-min(RRAM_R_32768),max(RRAM_R_65536)-min(RRAM_R_65536))
RRAM<-RRAM/1000

### Occupazione Media RCPU in %
RCPU<-c(mean(RCPU_A_8),mean(RCPU_A_16),mean(RCPU_A_32),mean(RCPU_A_64),mean(RCPU_A_128),
	mean(RCPU_A_256),mean(RCPU_A_512),mean(RCPU_A_1024),mean(RCPU_A_2048),mean(RCPU_A_4096),
mean(RCPU_A_8192),mean(RCPU_A_16384),mean(RCPU_A_32768),mean(RCPU_A_65536),

	mean(RCPU_F_4),mean(RCPU_F_8),mean(RCPU_F_16),mean(RCPU_F_32),mean(RCPU_F_64),
	mean(RCPU_F_128),mean(RCPU_F_256),mean(RCPU_F_512),mean(RCPU_F_1024),mean(RCPU_F_2048),
	mean(RCPU_F_4096),
mean(RCPU_F_8192),mean(RCPU_F_16384),mean(RCPU_F_32768),mean(RCPU_F_65536),
	mean(RCPU_L_4),mean(RCPU_L_8),mean(RCPU_L_16),mean(RCPU_L_32),mean(RCPU_L_64),
	mean(RCPU_L_128),mean(RCPU_L_256),mean(RCPU_L_512),mean(RCPU_L_1024),mean(RCPU_L_2048),
	mean(RCPU_L_4096),
mean(RCPU_L_8192),mean(RCPU_L_16384),mean(RCPU_L_32768),mean(RCPU_L_65536),
	mean(RCPU_R_8),mean(RCPU_R_16),mean(RCPU_R_32),mean(RCPU_R_64),mean(RCPU_R_128),
	mean(RCPU_R_256),mean(RCPU_R_512),mean(RCPU_R_1024),mean(RCPU_R_2048),mean(RCPU_R_4096),
mean(RCPU_R_8192),mean(RCPU_R_16384),mean(RCPU_R_32768),mean(RCPU_R_65536))}

### AUX VARIABLES
BS_A_8<-boxplot.stats(OBS_A_8f)$stats;		BS_A_16<-boxplot.stats(OBS_A_16f)$stats
BS_A_32<-boxplot.stats(OBS_A_32f)$stats;	BS_A_64<-boxplot.stats(OBS_A_64f)$stats
BS_A_128<-boxplot.stats(OBS_A_128f)$stats;	BS_A_256<-boxplot.stats(OBS_A_256f)$stats
BS_A_512<-boxplot.stats(OBS_A_512f)$stats;	BS_A_1024<-boxplot.stats(OBS_A_1024f)$stats
BS_A_2048<-boxplot.stats(OBS_A_2048f)$stats;	BS_A_4096<-boxplot.stats(OBS_A_4096f)$stats
BS_A_8192<-boxplot.stats(OBS_A_8192f)$stats;
BS_A_16384<-boxplot.stats(OBS_A_16384f)$stats;
BS_A_32768<-boxplot.stats(OBS_A_32768f)$stats;
BS_A_65536<-boxplot.stats(OBS_A_65536f)$stats;

BS_F_4<-boxplot.stats(OBS_F_4f)$stats;		BS_F_8<-boxplot.stats(OBS_F_8f)$stats;
BS_F_16<-boxplot.stats(OBS_F_16f)$stats;	BS_F_32<-boxplot.stats(OBS_F_32f)$stats;
BS_F_64<-boxplot.stats(OBS_F_64f)$stats;	BS_F_128<-boxplot.stats(OBS_F_128f)$stats;
BS_F_256<-boxplot.stats(OBS_F_256f)$stats;	BS_F_512<-boxplot.stats(OBS_F_512f)$stats;
BS_F_1024<-boxplot.stats(OBS_F_1024f)$stats;	BS_F_2048<-boxplot.stats(OBS_F_2048f)$stats;
BS_F_4096<-boxplot.stats(OBS_F_4096f)$stats;
BS_F_8192<-boxplot.stats(OBS_F_8192f)$stats;
BS_F_16384<-boxplot.stats(OBS_F_16384f)$stats;
BS_F_32768<-boxplot.stats(OBS_F_32768f)$stats;
BS_F_65536<-boxplot.stats(OBS_F_65536f)$stats;

BS_L_4<-boxplot.stats(OBS_L_4f)$stats;		BS_L_8<-boxplot.stats(OBS_L_8f)$stats
BS_L_16<-boxplot.stats(OBS_L_16f)$stats;	BS_L_32<-boxplot.stats(OBS_L_32f)$stats
BS_L_64<-boxplot.stats(OBS_L_64f)$stats;	BS_L_128<-boxplot.stats(OBS_L_128f)$stats
BS_L_256<-boxplot.stats(OBS_L_256f)$stats;	BS_L_512<-boxplot.stats(OBS_L_512f)$stats
BS_L_1024<-boxplot.stats(OBS_L_1024f)$stats;	BS_L_2048<-boxplot.stats(OBS_L_2048f)$stats
BS_L_4096<-boxplot.stats(OBS_L_4096f)$stats;	
BS_L_8192<-boxplot.stats(OBS_L_8192f)$stats;
BS_L_16384<-boxplot.stats(OBS_L_16384f)$stats;
BS_L_32768<-boxplot.stats(OBS_L_32768f)$stats;
BS_L_65536<-boxplot.stats(OBS_L_65536f)$stats;

BS_R_8<-boxplot.stats(OBS_R_8f)$stats;		BS_R_16<-boxplot.stats(OBS_R_16f)$stats;
BS_R_32<-boxplot.stats(OBS_R_32f)$stats;	BS_R_64<-boxplot.stats(OBS_R_64f)$stats;
BS_R_128<-boxplot.stats(OBS_R_128f)$stats;	BS_R_256<-boxplot.stats(OBS_R_256f)$stats;
BS_R_512<-boxplot.stats(OBS_R_512f)$stats;	BS_R_1024<-boxplot.stats(OBS_R_1024f)$stats;
BS_R_2048<-boxplot.stats(OBS_R_2048f)$stats;	BS_R_4096<-boxplot.stats(OBS_R_4096f)$stats;
BS_R_8192<-boxplot.stats(OBS_R_8192f)$stats;
BS_R_16384<-boxplot.stats(OBS_R_16384f)$stats;
BS_R_32768<-boxplot.stats(OBS_R_32768f)$stats;
BS_R_65536<-boxplot.stats(OBS_R_65536f)$stats;

x<-c(4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536)

obs_yrange<-range(OBS_L_4f,OBS_L_65536f)

RAMObs_xrange<-c(0,max(c(dim(as.array(RAM_L_4)),dim(as.array(RAM_L_65536)))))
RAMObs_yrange<-range(RAM_L_4,RAM_L_65536)/1000
CPUObs_xrange<-c(0,max(c(dim(as.array(CPU_L_4)),dim(as.array(CPU_L_65536)))))
CPUObs_yrange<-range(CPU_L_4,CPU_L_65536)

if (is2Host) {
RRAMObs_xrange<-c(0,max(c(dim(as.array(RRAM_L_4)),dim(as.array(RRAM_L_65536)))))
RRAMObs_yrange<-range(RRAM_L_4,RRAM_L_65536)/1000
RCPUObs_xrange<-c(0,max(c(dim(as.array(RCPU_L_4)),dim(as.array(RCPU_L_65536)))))
RCPUObs_yrange<-range(RCPU_L_4,RCPU_L_65536)}

###Calculation of Mean Arrays
Mean_A<-c(mean(OBS_A_8f),mean(OBS_A_16f),mean(OBS_A_32f),mean(OBS_A_64f),
	mean(OBS_A_128f),mean(OBS_A_256f),mean(OBS_A_512f),
	mean(OBS_A_1024f),mean(OBS_A_2048f),mean(OBS_A_4096f),
	mean(OBS_A_8192f),mean(OBS_A_16384f),mean(OBS_A_32768f),
	mean(OBS_A_65536f))
Mean_F<-c(mean(OBS_F_4f),mean(OBS_F_8f),mean(OBS_F_16f),mean(OBS_F_32f),
	mean(OBS_F_64f),mean(OBS_F_128f),mean(OBS_F_256f),mean(OBS_F_512f),
	mean(OBS_F_1024f),mean(OBS_F_2048f),mean(OBS_F_4096f),
	mean(OBS_F_8192f),mean(OBS_F_16384f),mean(OBS_F_32768f),
	mean(OBS_F_65536f))
Mean_L<-c(mean(OBS_L_4f),mean(OBS_L_8f),mean(OBS_L_16f),mean(OBS_L_32f),
	mean(OBS_L_64f),mean(OBS_L_128f),mean(OBS_L_256f),mean(OBS_L_512f),
	mean(OBS_L_1024f),mean(OBS_L_2048f),mean(OBS_L_4096f),
	mean(OBS_L_8192f),mean(OBS_L_16384f),mean(OBS_L_32768f),
	mean(OBS_L_65536f))
Mean_R<-c(mean(OBS_R_8f),mean(OBS_R_16f),mean(OBS_R_32f),mean(OBS_R_64f),
	mean(OBS_R_128f),mean(OBS_R_256f),mean(OBS_R_512f),
	mean(OBS_R_1024f),mean(OBS_R_2048f),mean(OBS_R_4096f),
	mean(OBS_R_8192f),mean(OBS_R_16384f),mean(OBS_R_32768f),
	mean(OBS_R_65536f))
Mean_yrange<-range(Mean_A,Mean_F,Mean_L,Mean_R)

###Calculation of Standard Deviation Arrays
std_A<-c(sqrt(var(OBS_A_8f)),sqrt(var(OBS_A_16f)),sqrt(var(OBS_A_32f)),
	sqrt(var(OBS_A_64f)),sqrt(var(OBS_A_128f)),sqrt(var(OBS_A_256f)),
	sqrt(var(OBS_A_512f)),sqrt(var(OBS_A_1024f)),sqrt(var(OBS_A_2048f)),
	sqrt(var(OBS_A_4096f)),sqrt(var(OBS_A_8192f)),sqrt(var(OBS_A_16384f)),
	sqrt(var(OBS_A_32768f)),sqrt(var(OBS_A_65536f)))
std_F<-c(sqrt(var(OBS_F_4f)),sqrt(var(OBS_F_8f)),sqrt(var(OBS_F_16f)),
	sqrt(var(OBS_F_32f)),sqrt(var(OBS_F_64f)),sqrt(var(OBS_F_128f)),
	sqrt(var(OBS_F_256f)),sqrt(var(OBS_F_512f)),sqrt(var(OBS_F_1024f)),
	sqrt(var(OBS_F_2048f)),sqrt(var(OBS_F_4096f)),sqrt(var(OBS_F_8192f)),
	sqrt(var(OBS_F_16384f)),sqrt(var(OBS_F_32768f)),sqrt(var(OBS_F_65536f)))
std_L<-c(sqrt(var(OBS_L_4f)),sqrt(var(OBS_L_8f)),sqrt(var(OBS_L_16f)),
	sqrt(var(OBS_L_32f)),sqrt(var(OBS_L_64f)),sqrt(var(OBS_L_128f)),
	sqrt(var(OBS_L_256f)),sqrt(var(OBS_L_512f)),sqrt(var(OBS_L_1024f)),
	sqrt(var(OBS_L_2048f)),sqrt(var(OBS_L_4096f)),sqrt(var(OBS_L_8192f)),
	sqrt(var(OBS_L_16384f)),sqrt(var(OBS_L_32768f)),sqrt(var(OBS_L_65536f)))
std_R<-c(sqrt(var(OBS_R_8f)),sqrt(var(OBS_R_16f)),sqrt(var(OBS_R_32f)),
	sqrt(var(OBS_R_64f)),sqrt(var(OBS_R_128f)),sqrt(var(OBS_R_256f)),
	sqrt(var(OBS_R_512f)),sqrt(var(OBS_R_1024f)),sqrt(var(OBS_R_2048f)),
	sqrt(var(OBS_R_4096f)),sqrt(var(OBS_R_8192f)),sqrt(var(OBS_R_16384f)),
	sqrt(var(OBS_R_32768f)),sqrt(var(OBS_R_65536f)))
std_yrange<-range(std_A, std_F, std_L, std_R)

bxp_x<-c("4",NA,NA,		"8",NA,NA,NA,NA,		"16",NA,NA,NA,NA,
	"32",NA,NA,NA,NA,	"64",NA,NA,NA,NA,		"128",NA,NA,NA,NA,
	"256",NA,NA,NA,,NA,	"512",NA,NA,NA,NA,		"1k",NA,NA,NA,NA,
	"2k",NA,NA,NA,NA,	"4k",NA,NA,NA,NA,		"8k",NA,NA,NA,NA,
"16k",NA,NA,NA,NA,	"32k",NA,NA,NA,NA,	"64k",NA,NA,NA,NA)

bxp_yrange<-range(
BS_A_8,BS_A_16,BS_A_32,BS_A_64,BS_A_128,BS_A_256,BS_A_512,BS_A_1024,BS_A_2048,BS_A_4096,BS_A_8192,BS_A_16384,BS_A_32768,BS_A_65536,
BS_F_4,BS_F_8,BS_F_16,BS_F_32,BS_F_64,BS_F_128,BS_F_256,BS_F_512,BS_F_1024,BS_F_2048,BS_F_4096,BS_F_8192,BS_F_16384,BS_F_32768,BS_F_65536,
BS_L_4,BS_L_8,BS_L_16,BS_L_32,BS_L_64,BS_L_128,BS_L_256,BS_L_512,BS_L_1024,BS_L_2048,BS_L_4096,BS_L_8192,BS_L_16384,BS_L_32768,BS_L_65536,
BS_R_8,BS_R_16,BS_R_32,BS_R_64,BS_R_128,BS_R_256,BS_R_512,BS_R_1024,BS_R_2048,BS_R_4096,BS_R_8192,BS_R_16384,BS_R_32768,BS_R_65536)

bxp_ydom<-(bxp_yrange[2]-bxp_yrange[1])

###Calculation of Delta Mean Arrays
DMea_A<-c(mean(OBS_A_8f)/mean(OBSN_A_8f),
	mean(OBS_A_16f)/mean(OBSN_A_16f),
	mean(OBS_A_32f)/mean(OBSN_A_32f),
	mean(OBS_A_64f)/mean(OBSN_A_64f),
	mean(OBS_A_128f)/mean(OBSN_A_128f),
	mean(OBS_A_256f)/mean(OBSN_A_256f),
	mean(OBS_A_512f)/mean(OBSN_A_512f),
	mean(OBS_A_1024f)/mean(OBSN_A_1024f),
	mean(OBS_A_2048f)/mean(OBSN_A_2048f),
	mean(OBS_A_4096f)/mean(OBSN_A_4096f),
	mean(OBS_A_8192f)/mean(OBSN_A_8192f),
	mean(OBS_A_16384f)/mean(OBSN_A_16384f),
	mean(OBS_A_32768f)/mean(OBSN_A_32768f),
	mean(OBS_A_65536f)/mean(OBSN_A_65536f))
DMea_F<-c(mean(OBS_F_4f)/mean(OBSN_F_4f),
	mean(OBS_F_8f)/mean(OBSN_F_8f),
	mean(OBS_F_16f)/mean(OBSN_F_16f),
	mean(OBS_F_32f)/mean(OBSN_F_32f),
	mean(OBS_F_64f)/mean(OBSN_F_64f),
	mean(OBS_F_128f)/mean(OBSN_F_128f),
	mean(OBS_F_256f)/mean(OBSN_F_256f),
	mean(OBS_F_512f)/mean(OBSN_F_512f),
	mean(OBS_F_1024f)/mean(OBSN_F_1024f),
	mean(OBS_F_2048f)/mean(OBSN_F_2048f),
	mean(OBS_F_4096f)/mean(OBSN_F_4096f),
	mean(OBS_F_8192f)/mean(OBSN_F_8192f),
	mean(OBS_F_16384f)/mean(OBSN_F_16384f),
	mean(OBS_F_32768f)/mean(OBSN_F_32768f),
	mean(OBS_F_65536f)/mean(OBSN_F_65536f))
DMea_L<-c(mean(OBS_L_4f)/mean(OBSN_L_4f),
	mean(OBS_L_8f)/mean(OBSN_L_8f),
	mean(OBS_L_16f)/mean(OBSN_L_16f),
	mean(OBS_L_32f)/mean(OBSN_L_32f),
	mean(OBS_L_64f)/mean(OBSN_L_64f),
	mean(OBS_L_128f)/mean(OBSN_L_128f),
	mean(OBS_L_256f)/mean(OBSN_L_256f),
	mean(OBS_L_512f)/mean(OBSN_L_512f),
	mean(OBS_L_1024f)/mean(OBSN_L_1024f),
	mean(OBS_L_2048f)/mean(OBSN_L_2048f),
	mean(OBS_L_4096f)/mean(OBSN_L_4096f),
	mean(OBS_L_8192f)/mean(OBSN_L_8192f),
	mean(OBS_L_16384f)/mean(OBSN_L_16384f),
	mean(OBS_L_32768f)/mean(OBSN_L_32768f),
	mean(OBS_L_65536f)/mean(OBSN_L_65536f))
DMea_R<-c(mean(OBS_R_8f)/mean(OBSN_R_8f),
	mean(OBS_R_16f)/mean(OBSN_R_16f),
	mean(OBS_R_32f)/mean(OBSN_R_32f),
	mean(OBS_R_64f)/mean(OBSN_R_64f),
	mean(OBS_R_128f)/mean(OBSN_R_128f),
	mean(OBS_R_256f)/mean(OBSN_R_256f),
	mean(OBS_R_512f)/mean(OBSN_R_512f),
	mean(OBS_R_1024f)/mean(OBSN_R_1024f),
	mean(OBS_R_2048f)/mean(OBSN_R_2048f),
	mean(OBS_R_4096f)/mean(OBSN_R_4096f),
	mean(OBS_R_8192f)/mean(OBSN_R_8192f),
	mean(OBS_R_16384f)/mean(OBSN_R_16384f),
	mean(OBS_R_32768f)/mean(OBSN_R_32768f),
	mean(OBS_R_65536f)/mean(OBSN_R_65536f))
DMea_A<-DMea_A-1;   DMea_F<-DMea_F-1;   DMea_L<-DMea_L-1;   DMea_R<-DMea_R-1;
DMea_yrange<-range(DMea_A,DMea_F,DMea_L,DMea_R)

###Calculation of Delta Mean Arrays (Absolute values)
DMeaAbs_A<-c(mean(OBS_A_8f)-mean(OBSN_A_8f),
	mean(OBS_A_16f)-mean(OBSN_A_16f),
	mean(OBS_A_32f)-mean(OBSN_A_32f),
	mean(OBS_A_64f)-mean(OBSN_A_64f),
	mean(OBS_A_128f)-mean(OBSN_A_128f),
	mean(OBS_A_256f)-mean(OBSN_A_256f),
	mean(OBS_A_512f)-mean(OBSN_A_512f),
	mean(OBS_A_1024f)-mean(OBSN_A_1024f),
	mean(OBS_A_2048f)-mean(OBSN_A_2048f),
	mean(OBS_A_4096f)-mean(OBSN_A_4096f),
	mean(OBS_A_8192f)-mean(OBSN_A_8192f),
	mean(OBS_A_16384f)-mean(OBSN_A_16384f),
	mean(OBS_A_32768f)-mean(OBSN_A_32768f),
	mean(OBS_A_65536f)-mean(OBSN_A_65536f))
DMeaAbs_F<-c(mean(OBS_F_4f)-mean(OBSN_F_4f),
	mean(OBS_F_8f)-mean(OBSN_F_8f),
	mean(OBS_F_16f)-mean(OBSN_F_16f),
	mean(OBS_F_32f)-mean(OBSN_F_32f),
	mean(OBS_F_64f)-mean(OBSN_F_64f),
	mean(OBS_F_128f)-mean(OBSN_F_128f),
	mean(OBS_F_256f)-mean(OBSN_F_256f),
	mean(OBS_F_512f)-mean(OBSN_F_512f),
	mean(OBS_F_1024f)-mean(OBSN_F_1024f),
	mean(OBS_F_2048f)-mean(OBSN_F_2048f),
	mean(OBS_F_4096f)-mean(OBSN_F_4096f),
	mean(OBS_F_8192f)-mean(OBSN_F_8192f),
	mean(OBS_F_16384f)-mean(OBSN_F_16384f),
	mean(OBS_F_32768f)-mean(OBSN_F_32768f),
	mean(OBS_F_65536f)-mean(OBSN_F_65536f))
DMeaAbs_L<-c(mean(OBS_L_4f)-mean(OBSN_L_4f),
	mean(OBS_L_8f)-mean(OBSN_L_8f),
	mean(OBS_L_16f)-mean(OBSN_L_16f),
	mean(OBS_L_32f)-mean(OBSN_L_32f),
	mean(OBS_L_64f)-mean(OBSN_L_64f),
	mean(OBS_L_128f)-mean(OBSN_L_128f),
	mean(OBS_L_256f)-mean(OBSN_L_256f),
	mean(OBS_L_512f)-mean(OBSN_L_512f),
	mean(OBS_L_1024f)-mean(OBSN_L_1024f),
	mean(OBS_L_2048f)-mean(OBSN_L_2048f),
	mean(OBS_L_4096f)-mean(OBSN_L_4096f),
	mean(OBS_L_8192f)-mean(OBSN_L_8192f),
	mean(OBS_L_16384f)-mean(OBSN_L_16384f),
	mean(OBS_L_32768f)-mean(OBSN_L_32768f),
	mean(OBS_L_65536f)-mean(OBSN_L_65536f))
DMeaAbs_R<-c(mean(OBS_R_8f)-mean(OBSN_R_8f),
	mean(OBS_R_16f)-mean(OBSN_R_16f),
	mean(OBS_R_32f)-mean(OBSN_R_32f),
	mean(OBS_R_64f)-mean(OBSN_R_64f),
	mean(OBS_R_128f)-mean(OBSN_R_128f),
	mean(OBS_R_256f)-mean(OBSN_R_256f),
	mean(OBS_R_512f)-mean(OBSN_R_512f),
	mean(OBS_R_1024f)-mean(OBSN_R_1024f),
	mean(OBS_R_2048f)-mean(OBSN_R_2048f),
	mean(OBS_R_4096f)-mean(OBSN_R_4096f),
	mean(OBS_R_8192f)-mean(OBSN_R_8192f),
	mean(OBS_R_16384f)-mean(OBSN_R_16384f),
	mean(OBS_R_32768f)-mean(OBSN_R_32768f),
	mean(OBS_R_65536f)-mean(OBSN_R_65536f))
DMeaAbs_yrange<-range(DMeaAbs_A,DMeaAbs_F,DMeaAbs_L,DMeaAbs_R)

###Calculation of Delta Max Arrays
DMax_A<-c(max(OBS_A_8f)/max(OBSN_A_8f),
	max(OBS_A_16f)/max(OBSN_A_16f),
	max(OBS_A_32f)/max(OBSN_A_32f),
	max(OBS_A_64f)/max(OBSN_A_64f),
	max(OBS_A_128f)/max(OBSN_A_128f),
	max(OBS_A_256f)/max(OBSN_A_256f),
	max(OBS_A_512f)/max(OBSN_A_512f),
	max(OBS_A_1024f)/max(OBSN_A_1024f),
	max(OBS_A_2048f)/max(OBSN_A_2048f),
	max(OBS_A_4096f)/max(OBSN_A_4096f),
	max(OBS_A_8192f)/max(OBSN_A_8192f),
	max(OBS_A_16384f)/max(OBSN_A_16384f),
	max(OBS_A_32768f)/max(OBSN_A_32768f),
	max(OBS_A_65536f)/max(OBSN_A_65536f))
DMax_F<-c(max(OBS_F_4f)/max(OBSN_F_4f),
	max(OBS_F_8f)/max(OBSN_F_8f),
	max(OBS_F_16f)/max(OBSN_F_16f),
	max(OBS_F_32f)/max(OBSN_F_32f),
	max(OBS_F_64f)/max(OBSN_F_64f),
	max(OBS_F_128f)/max(OBSN_F_128f),
	max(OBS_F_256f)/max(OBSN_F_256f),
	max(OBS_F_512f)/max(OBSN_F_512f),
	max(OBS_F_1024f)/max(OBSN_F_1024f),
	max(OBS_F_2048f)/max(OBSN_F_2048f),
	max(OBS_F_4096f)/max(OBSN_F_4096f),
	max(OBS_F_8192f)/max(OBSN_F_8192f),
	max(OBS_F_16384f)/max(OBSN_F_16384f),
	max(OBS_F_32768f)/max(OBSN_F_32768f),
	max(OBS_F_65536f)/max(OBSN_F_65536f))
DMax_L<-c(max(OBS_L_4f)/max(OBSN_L_4f),
	max(OBS_L_8f)/max(OBSN_L_8f),
	max(OBS_L_16f)/max(OBSN_L_16f),
	max(OBS_L_32f)/max(OBSN_L_32f),
	max(OBS_L_64f)/max(OBSN_L_64f),
	max(OBS_L_128f)/max(OBSN_L_128f),
	max(OBS_L_256f)/max(OBSN_L_256f),
	max(OBS_L_512f)/max(OBSN_L_512f),
	max(OBS_L_1024f)/max(OBSN_L_1024f),
	max(OBS_L_2048f)/max(OBSN_L_2048f),
	max(OBS_L_4096f)/max(OBSN_L_4096f),
	max(OBS_L_8192f)/max(OBSN_L_8192f),
	max(OBS_L_16384f)/max(OBSN_L_16384f),
	max(OBS_L_32768f)/max(OBSN_L_32768f),
	max(OBS_L_65536f)/max(OBSN_L_65536f))
DMax_R<-c(max(OBS_R_8f)/max(OBSN_R_8f),
	max(OBS_R_16f)/max(OBSN_R_16f),
	max(OBS_R_32f)/max(OBSN_R_32f),
	max(OBS_R_64f)/max(OBSN_R_64f),
	max(OBS_R_128f)/max(OBSN_R_128f),
	max(OBS_R_256f)/max(OBSN_R_256f),
	max(OBS_R_512f)/max(OBSN_R_512f),
	max(OBS_R_1024f)/max(OBSN_R_1024f),
	max(OBS_R_2048f)/max(OBSN_R_2048f),
	max(OBS_R_4096f)/max(OBSN_R_4096f),
	max(OBS_R_8192f)/max(OBSN_R_8192f),
	max(OBS_R_16384f)/max(OBSN_R_16384f),
	max(OBS_R_32768f)/max(OBSN_R_32768f),
	max(OBS_R_65536f)/max(OBSN_R_65536f))
DMax_A<-DMax_A-1;   DMax_F<-DMax_F-1;   DMax_L<-DMax_L-1;   DMax_R<-DMax_R-1;
DMax_yrange<-range(DMax_A,DMax_F,DMax_L,DMax_R)

###Calculation of Delta Q3 Arrays
DQ3_A<-c(Q3(OBS_A_8f)/Q3(OBSN_A_8f),
	Q3(OBS_A_16f)/Q3(OBSN_A_16f),
	Q3(OBS_A_32f)/Q3(OBSN_A_32f),
	Q3(OBS_A_64f)/Q3(OBSN_A_64f),
	Q3(OBS_A_128f)/Q3(OBSN_A_128f),
	Q3(OBS_A_256f)/Q3(OBSN_A_256f),
	Q3(OBS_A_512f)/Q3(OBSN_A_512f),
	Q3(OBS_A_1024f)/Q3(OBSN_A_1024f),
	Q3(OBS_A_2048f)/Q3(OBSN_A_2048f),
	Q3(OBS_A_4096f)/Q3(OBSN_A_4096f),
	Q3(OBS_A_8192f)/Q3(OBSN_A_8192f),
	Q3(OBS_A_16384f)/Q3(OBSN_A_16384f),
	Q3(OBS_A_32768f)/Q3(OBSN_A_32768f),
	Q3(OBS_A_65536f)/Q3(OBSN_A_65536f))
DQ3_F<-c(Q3(OBS_F_4f)/Q3(OBSN_F_4f),
	Q3(OBS_F_8f)/Q3(OBSN_F_8f),
	Q3(OBS_F_16f)/Q3(OBSN_F_16f),
	Q3(OBS_F_32f)/Q3(OBSN_F_32f),
	Q3(OBS_F_64f)/Q3(OBSN_F_64f),
	Q3(OBS_F_128f)/Q3(OBSN_F_128f),
	Q3(OBS_F_256f)/Q3(OBSN_F_256f),
	Q3(OBS_F_512f)/Q3(OBSN_F_512f),
	Q3(OBS_F_1024f)/Q3(OBSN_F_1024f),
	Q3(OBS_F_2048f)/Q3(OBSN_F_2048f),
	Q3(OBS_F_4096f)/Q3(OBSN_F_4096f),
	Q3(OBS_F_8192f)/Q3(OBSN_F_8192f),
	Q3(OBS_F_16384f)/Q3(OBSN_F_16384f),
	Q3(OBS_F_32768f)/Q3(OBSN_F_32768f),
	Q3(OBS_F_65536f)/Q3(OBSN_F_65536f))
DQ3_L<-c(Q3(OBS_L_4f)/Q3(OBSN_L_4f),
	Q3(OBS_L_8f)/Q3(OBSN_L_8f),
	Q3(OBS_L_16f)/Q3(OBSN_L_16f),
	Q3(OBS_L_32f)/Q3(OBSN_L_32f),
	Q3(OBS_L_64f)/Q3(OBSN_L_64f),
	Q3(OBS_L_128f)/Q3(OBSN_L_128f),
	Q3(OBS_L_256f)/Q3(OBSN_L_256f),
	Q3(OBS_L_512f)/Q3(OBSN_L_512f),
	Q3(OBS_L_1024f)/Q3(OBSN_L_1024f),
	Q3(OBS_L_2048f)/Q3(OBSN_L_2048f),
	Q3(OBS_L_4096f)/Q3(OBSN_L_4096f),
	Q3(OBS_L_8192f)/Q3(OBSN_L_8192f),
	Q3(OBS_L_16384f)/Q3(OBSN_L_16384f),
	Q3(OBS_L_32768f)/Q3(OBSN_L_32768f),
	Q3(OBS_L_65536f)/Q3(OBSN_L_65536f))
DQ3_R<-c(Q3(OBS_R_8f)/Q3(OBSN_R_8f),
	Q3(OBS_R_16f)/Q3(OBSN_R_16f),
	Q3(OBS_R_32f)/Q3(OBSN_R_32f),
	Q3(OBS_R_64f)/Q3(OBSN_R_64f),
	Q3(OBS_R_128f)/Q3(OBSN_R_128f),
	Q3(OBS_R_256f)/Q3(OBSN_R_256f),
	Q3(OBS_R_512f)/Q3(OBSN_R_512f),
	Q3(OBS_R_1024f)/Q3(OBSN_R_1024f),
	Q3(OBS_R_2048f)/Q3(OBSN_R_2048f),
	Q3(OBS_R_4096f)/Q3(OBSN_R_4096f),
	Q3(OBS_R_8192f)/Q3(OBSN_R_8192f),
	Q3(OBS_R_16384f)/Q3(OBSN_R_16384f),
	Q3(OBS_R_32768f)/Q3(OBSN_R_32768f),
	Q3(OBS_R_65536f)/Q3(OBSN_R_65536f))
DQ3_A<-DQ3_A-1;   DQ3_F<-DQ3_F-1;   DQ3_L<-DQ3_L-1;   DQ3_R<-DQ3_R-1;
DQ3_yrange<-range(DQ3_A,DQ3_F,DQ3_L,DQ3_R)

###Calculation of Delta Median Arrays
DMed_A<-c(median(OBS_A_8f)/median(OBSN_A_8f),
	median(OBS_A_16f)/median(OBSN_A_16f),
	median(OBS_A_32f)/median(OBSN_A_32f),
	median(OBS_A_64f)/median(OBSN_A_64f),
	median(OBS_A_128f)/median(OBSN_A_128f),
	median(OBS_A_256f)/median(OBSN_A_256f),
	median(OBS_A_512f)/median(OBSN_A_512f),
	median(OBS_A_1024f)/median(OBSN_A_1024f),
	median(OBS_A_2048f)/median(OBSN_A_2048f),
	median(OBS_A_4096f)/median(OBSN_A_4096f),
	median(OBS_A_8192f)/median(OBSN_A_8192f),
	median(OBS_A_16384f)/median(OBSN_A_16384f),
	median(OBS_A_32768f)/median(OBSN_A_32768f),
	median(OBS_A_65536f)/median(OBSN_A_65536f))
DMed_F<-c(median(OBS_F_4f)/median(OBSN_F_4f),
	median(OBS_F_8f)/median(OBSN_F_8f),
	median(OBS_F_16f)/median(OBSN_F_16f),
	median(OBS_F_32f)/median(OBSN_F_32f),
	median(OBS_F_64f)/median(OBSN_F_64f),
	median(OBS_F_128f)/median(OBSN_F_128f),
	median(OBS_F_256f)/median(OBSN_F_256f),
	median(OBS_F_512f)/median(OBSN_F_512f),
	median(OBS_F_1024f)/median(OBSN_F_1024f),
	median(OBS_F_2048f)/median(OBSN_F_2048f),
	median(OBS_F_4096f)/median(OBSN_F_4096f),
	median(OBS_F_8192f)/median(OBSN_F_8192f),
	median(OBS_F_16384f)/median(OBSN_F_16384f),
	median(OBS_F_32768f)/median(OBSN_F_32768f),
	median(OBS_F_65536f)/median(OBSN_F_65536f))
DMed_L<-c(median(OBS_L_4f)/median(OBSN_L_4f),
	median(OBS_L_8f)/median(OBSN_L_8f),
	median(OBS_L_16f)/median(OBSN_L_16f),
	median(OBS_L_32f)/median(OBSN_L_32f),
	median(OBS_L_64f)/median(OBSN_L_64f),
	median(OBS_L_128f)/median(OBSN_L_128f),
	median(OBS_L_256f)/median(OBSN_L_256f),
	median(OBS_L_512f)/median(OBSN_L_512f),
	median(OBS_L_1024f)/median(OBSN_L_1024f),
	median(OBS_L_2048f)/median(OBSN_L_2048f),
	median(OBS_L_4096f)/median(OBSN_L_4096f),
	median(OBS_L_8192f)/median(OBSN_L_8192f),
	median(OBS_L_16384f)/median(OBSN_L_16384f),
	median(OBS_L_32768f)/median(OBSN_L_32768f),
	median(OBS_L_65536f)/median(OBSN_L_65536f))
DMed_R<-c(median(OBS_R_8f)/median(OBSN_R_8f),
	median(OBS_R_16f)/median(OBSN_R_16f),
	median(OBS_R_32f)/median(OBSN_R_32f),
	median(OBS_R_64f)/median(OBSN_R_64f),
	median(OBS_R_128f)/median(OBSN_R_128f),
	median(OBS_R_256f)/median(OBSN_R_256f),
	median(OBS_R_512f)/median(OBSN_R_512f),
	median(OBS_R_1024f)/median(OBSN_R_1024f),
	median(OBS_R_2048f)/median(OBSN_R_2048f),
	median(OBS_R_4096f)/median(OBSN_R_4096f),
	median(OBS_R_8192f)/median(OBSN_R_8192f),
	median(OBS_R_16384f)/median(OBSN_R_16384f),
	median(OBS_R_32768f)/median(OBSN_R_32768f),
	median(OBS_R_65536f)/median(OBSN_R_65536f))
DMed_A<-DMed_A-1;   DMed_F<-DMed_F-1;   DMed_L<-DMed_L-1;   DMed_R<-DMed_R-1;
DMed_yrange<-range(DMed_A,DMed_F,DMed_L,DMed_R)

###Calculation of Delta Q1 Arrays
DQ1_A<-c(Q1(OBS_A_8f)/Q1(OBSN_A_8f),
	Q1(OBS_A_16f)/Q1(OBSN_A_16f),
	Q1(OBS_A_32f)/Q1(OBSN_A_32f),
	Q1(OBS_A_64f)/Q1(OBSN_A_64f),
	Q1(OBS_A_128f)/Q1(OBSN_A_128f),
	Q1(OBS_A_256f)/Q1(OBSN_A_256f),
	Q1(OBS_A_512f)/Q1(OBSN_A_512f),
	Q1(OBS_A_1024f)/Q1(OBSN_A_1024f),
	Q1(OBS_A_2048f)/Q1(OBSN_A_2048f),
	Q1(OBS_A_4096f)/Q1(OBSN_A_4096f),
	Q1(OBS_A_8192f)/Q1(OBSN_A_8192f),
	Q1(OBS_A_16384f)/Q1(OBSN_A_16384f),
	Q1(OBS_A_32768f)/Q1(OBSN_A_32768f),
	Q1(OBS_A_65536f)/Q1(OBSN_A_65536f))
DQ1_F<-c(Q1(OBS_F_4f)/Q1(OBSN_F_4f),
	Q1(OBS_F_8f)/Q1(OBSN_F_8f),
	Q1(OBS_F_16f)/Q1(OBSN_F_16f),
	Q1(OBS_F_32f)/Q1(OBSN_F_32f),
	Q1(OBS_F_64f)/Q1(OBSN_F_64f),
	Q1(OBS_F_128f)/Q1(OBSN_F_128f),
	Q1(OBS_F_256f)/Q1(OBSN_F_256f),
	Q1(OBS_F_512f)/Q1(OBSN_F_512f),
	Q1(OBS_F_1024f)/Q1(OBSN_F_1024f),
	Q1(OBS_F_2048f)/Q1(OBSN_F_2048f),
	Q1(OBS_F_4096f)/Q1(OBSN_F_4096f),
	Q1(OBS_F_8192f)/Q1(OBSN_F_8192f),
	Q1(OBS_F_16384f)/Q1(OBSN_F_16384f),
	Q1(OBS_F_32768f)/Q1(OBSN_F_32768f),
	Q1(OBS_F_65536f)/Q1(OBSN_F_65536f))
DQ1_L<-c(Q1(OBS_L_4f)/Q1(OBSN_L_4f),
	Q1(OBS_L_8f)/Q1(OBSN_L_8f),
	Q1(OBS_L_16f)/Q1(OBSN_L_16f),
	Q1(OBS_L_32f)/Q1(OBSN_L_32f),
	Q1(OBS_L_64f)/Q1(OBSN_L_64f),
	Q1(OBS_L_128f)/Q1(OBSN_L_128f),
	Q1(OBS_L_256f)/Q1(OBSN_L_256f),
	Q1(OBS_L_512f)/Q1(OBSN_L_512f),
	Q1(OBS_L_1024f)/Q1(OBSN_L_1024f),
	Q1(OBS_L_2048f)/Q1(OBSN_L_2048f),
	Q1(OBS_L_4096f)/Q1(OBSN_L_4096f),
	Q1(OBS_L_8192f)/Q1(OBSN_L_8192f),
	Q1(OBS_L_16384f)/Q1(OBSN_L_16384f),
	Q1(OBS_L_32768f)/Q1(OBSN_L_32768f),
	Q1(OBS_L_65536f)/Q1(OBSN_L_65536f))
DQ1_R<-c(Q1(OBS_R_8f)/Q1(OBSN_R_8f),
	Q1(OBS_R_16f)/Q1(OBSN_R_16f),
	Q1(OBS_R_32f)/Q1(OBSN_R_32f),
	Q1(OBS_R_64f)/Q1(OBSN_R_64f),
	Q1(OBS_R_128f)/Q1(OBSN_R_128f),
	Q1(OBS_R_256f)/Q1(OBSN_R_256f),
	Q1(OBS_R_512f)/Q1(OBSN_R_512f),
	Q1(OBS_R_1024f)/Q1(OBSN_R_1024f),
	Q1(OBS_R_2048f)/Q1(OBSN_R_2048f),
	Q1(OBS_R_4096f)/Q1(OBSN_R_4096f),
	Q1(OBS_R_8192f)/Q1(OBSN_R_8192f),
	Q1(OBS_R_16384f)/Q1(OBSN_R_16384f),
	Q1(OBS_R_32768f)/Q1(OBSN_R_32768f),
	Q1(OBS_R_65536f)/Q1(OBSN_R_65536f))
DQ1_A<-DQ1_A-1;   DQ1_F<-DQ1_F-1;   DQ1_L<-DQ1_L-1;   DQ1_R<-DQ1_R-1;
DQ1_yrange<-range(DQ1_A,DQ1_F,DQ1_L,DQ1_R)

###Calculation of Delta Min Arrays
DMin_A<-c(min(OBS_A_8f)/min(OBSN_A_8f),
	min(OBS_A_16f)/min(OBSN_A_16f),
	min(OBS_A_32f)/min(OBSN_A_32f),
	min(OBS_A_64f)/min(OBSN_A_64f),
	min(OBS_A_128f)/min(OBSN_A_128f),
	min(OBS_A_256f)/min(OBSN_A_256f),
	min(OBS_A_512f)/min(OBSN_A_512f),
	min(OBS_A_1024f)/min(OBSN_A_1024f),
	min(OBS_A_2048f)/min(OBSN_A_2048f),
	min(OBS_A_4096f)/min(OBSN_A_4096f),
	min(OBS_A_8192f)/min(OBSN_A_8192f),
	min(OBS_A_16384f)/min(OBSN_A_16384f),
	min(OBS_A_32768f)/min(OBSN_A_32768f),
	min(OBS_A_65536f)/min(OBSN_A_65536f))
DMin_F<-c(min(OBS_F_4f)/min(OBSN_F_4f),
	min(OBS_F_8f)/min(OBSN_F_8f),
	min(OBS_F_16f)/min(OBSN_F_16f),
	min(OBS_F_32f)/min(OBSN_F_32f),
	min(OBS_F_64f)/min(OBSN_F_64f),
	min(OBS_F_128f)/min(OBSN_F_128f),
	min(OBS_F_256f)/min(OBSN_F_256f),
	min(OBS_F_512f)/min(OBSN_F_512f),
	min(OBS_F_1024f)/min(OBSN_F_1024f),
	min(OBS_F_2048f)/min(OBSN_F_2048f),
	min(OBS_F_4096f)/min(OBSN_F_4096f),
	min(OBS_F_8192f)/min(OBSN_F_8192f),
	min(OBS_F_16384f)/min(OBSN_F_16384f),
	min(OBS_F_32768f)/min(OBSN_F_32768f),
	min(OBS_F_65536f)/min(OBSN_F_65536f))
DMin_L<-c(min(OBS_L_4f)/min(OBSN_L_4f),
	min(OBS_L_8f)/min(OBSN_L_8f),
	min(OBS_L_16f)/min(OBSN_L_16f),
	min(OBS_L_32f)/min(OBSN_L_32f),
	min(OBS_L_64f)/min(OBSN_L_64f),
	min(OBS_L_128f)/min(OBSN_L_128f),
	min(OBS_L_256f)/min(OBSN_L_256f),
	min(OBS_L_512f)/min(OBSN_L_512f),
	min(OBS_L_1024f)/min(OBSN_L_1024f),
	min(OBS_L_2048f)/min(OBSN_L_2048f),
	min(OBS_L_4096f)/min(OBSN_L_4096f),
	min(OBS_L_8192f)/min(OBSN_L_8192f),
	min(OBS_L_16384f)/min(OBSN_L_16384f),
	min(OBS_L_32768f)/min(OBSN_L_32768f),
	min(OBS_L_65536f)/min(OBSN_L_65536f))
DMin_R<-c(min(OBS_R_8f)/min(OBSN_R_8f),
	min(OBS_R_16f)/min(OBSN_R_16f),
	min(OBS_R_32f)/min(OBSN_R_32f),
	min(OBS_R_64f)/min(OBSN_R_64f),
	min(OBS_R_128f)/min(OBSN_R_128f),
	min(OBS_R_256f)/min(OBSN_R_256f),
	min(OBS_R_512f)/min(OBSN_R_512f),
	min(OBS_R_1024f)/min(OBSN_R_1024f),
	min(OBS_R_2048f)/min(OBSN_R_2048f),
	min(OBS_R_4096f)/min(OBSN_R_4096f),
	min(OBS_R_8192f)/min(OBSN_R_8192f),
	min(OBS_R_16384f)/min(OBSN_R_16384f),
	min(OBS_R_32768f)/min(OBSN_R_32768f),
	min(OBS_R_65536f)/min(OBSN_R_65536f))
DMin_A<-DMin_A-1;   DMin_F<-DMin_F-1;   DMin_L<-DMin_L-1;   DMin_R<-DMin_R-1;
DMin_yrange<-range(DMin_A,DMin_F,DMin_L,DMin_R)

###OBSERVATIONS graph
png(gr1,width=png_width,height=png_height)
plot(OBS_L_4f,col=color[1],pch="o",
	ylim=obs_yrange,
	main="Observations",ylab="Latency [usec]",xlab="Index Number [#]")
points(OBS_L_65536f,col=color[2],pch="o")
legend(0,obs_yrange[2],c(nomi_leg[30],nomi_leg[44]),color[1:2])
dev.off()

### MEAN graph
png(gr2,width=png_width,height=png_height)
plot(Mean_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=Mean_yrange,xlim=range(x),
	main="Marshalling benchmarks (Average)",xlab="DataSize [Bytes]",ylab="Latency [usec]")
points(Mean_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(Mean_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(Mean_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], Mean_yrange[2], nomi, color)
dev.off()

### STD
png(gr3,width=png_width,height=png_height)
plot(std_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,log="x",
	ylim=std_yrange,xlim=range(x),
	xlab="DataSize [Bytes]",ylab="Standard deviation [usec]",
		main="Marshalling benchmarks (Standard deviation)")
points(std_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(std_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(std_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], std_yrange[2], nomi, color)
dev.off()

### BOXPLOTS
png(gr4,width=png_width,height=png_height)
boxplot(OBS_F_4f,OBS_L_4f,NA,
	OBS_A_8f,OBS_F_8f,OBS_L_8f,OBS_R_8f,NA,
	OBS_A_16f,OBS_F_16f,OBS_L_16f,OBS_R_16f,NA,
	OBS_A_32f,OBS_F_32f,OBS_L_32f,OBS_R_32f,NA,
	OBS_A_64f,OBS_F_64f,OBS_L_64f,OBS_R_64f,NA,
	OBS_A_128f,OBS_F_128f,OBS_L_128f,OBS_R_128f,NA,
	OBS_A_256f,OBS_F_256f,OBS_L_256f,OBS_R_256f,NA,
	OBS_A_512f,OBS_F_512f,OBS_L_512f,OBS_R_512f,NA,
	OBS_A_1024f,OBS_F_1024f,OBS_L_1024f,OBS_R_1024f,NA,
	OBS_A_2048f,OBS_F_2048f,OBS_L_2048f,OBS_R_2048f,NA,
	OBS_A_4096f,OBS_F_4096f,OBS_L_4096f,OBS_R_4096f,NA,
	OBS_A_8192f,OBS_F_8192f,OBS_L_8192f,OBS_R_8192f,NA,
	OBS_A_16384f,OBS_F_16384f,OBS_L_16384f,OBS_R_16384f,NA,
	OBS_A_32768f,OBS_F_32768f,OBS_L_32768f,OBS_R_32768f,NA,
	OBS_A_65536f,OBS_F_65536f,OBS_L_65536f,OBS_R_65536f,NA,
	ylim=c(bxp_yrange[1]-(.06*bxp_ydom),bxp_yrange[2]),
	col=c(color[2],color[3],color[5],color,color,color,color,color,color,color,color,color,color,color,color,
		color,color),
	notch=TRUE,boxwex=.8,names=bxp_x,pch=".",
	main="Marshalling benchmarks (Box & Whisker graph)",xlab="DataSize [Bytes]",
		ylab="Samples distributions [usec]")
text(1,BS_F_4[1]-(.03*bxp_ydom),"F");		text(2,BS_L_4[1]-(.03*bxp_ydom),"L")
text(4,BS_A_8[1]-(.03*bxp_ydom),"A");		text(5,BS_F_8[1]-(.03*bxp_ydom),"F")
text(6,BS_L_8[1]-(.03*bxp_ydom),"L");		text(7,BS_R_8[1]-(.03*bxp_ydom),"R")
text(9,BS_A_16[1]-(.03*bxp_ydom),"A");		text(10,BS_F_16[1]-(.03*bxp_ydom),"F")
text(11,BS_L_16[1]-(.03*bxp_ydom),"L");		text(12,BS_R_16[1]-(.03*bxp_ydom),"R")
text(14,BS_A_32[1]-(.03*bxp_ydom),"A");		text(15,BS_F_32[1]-(.03*bxp_ydom),"F")
text(16,BS_L_32[1]-(.03*bxp_ydom),"L");		text(17,BS_R_32[1]-(.03*bxp_ydom),"R")
text(19,BS_A_64[1]-(.03*bxp_ydom),"A");		text(20,BS_F_64[1]-(.03*bxp_ydom),"F")
text(21,BS_L_64[1]-(.03*bxp_ydom),"L");		text(22,BS_R_64[1]-(.03*bxp_ydom),"R")
text(24,BS_A_128[1]-(.03*bxp_ydom),"A");	text(25,BS_F_128[1]-(.03*bxp_ydom),"F")
text(26,BS_L_128[1]-(.03*bxp_ydom),"L");	text(27,BS_R_128[1]-(.03*bxp_ydom),"R")
text(29,BS_A_256[1]-(.03*bxp_ydom),"A");	text(30,BS_F_256[1]-(.03*bxp_ydom),"F")
text(31,BS_L_256[1]-(.03*bxp_ydom),"L");	text(32,BS_R_256[1]-(.03*bxp_ydom),"R")
text(34,BS_A_512[1]-(.03*bxp_ydom),"A");	text(35,BS_F_512[1]-(.03*bxp_ydom),"F")
text(36,BS_L_512[1]-(.03*bxp_ydom),"L");	text(37,BS_R_512[1]-(.03*bxp_ydom),"R")
text(39,BS_A_1024[1]-(.03*bxp_ydom),"A");	text(40,BS_F_1024[1]-(.03*bxp_ydom),"F")
text(41,BS_L_1024[1]-(.03*bxp_ydom),"L");	text(42,BS_R_1024[1]-(.03*bxp_ydom),"R")
text(44,BS_A_2048[1]-(.03*bxp_ydom),"A");	text(45,BS_F_2048[1]-(.03*bxp_ydom),"F")
text(46,BS_L_2048[1]-(.03*bxp_ydom),"L");	text(47,BS_R_2048[1]-(.03*bxp_ydom),"R")
text(49,BS_A_4096[1]-(.03*bxp_ydom),"A");	text(50,BS_F_4096[1]-(.03*bxp_ydom),"F")
text(51,BS_L_4096[1]-(.03*bxp_ydom),"L");	text(52,BS_R_4096[1]-(.03*bxp_ydom),"R")
text(54,BS_A_8192[1]-(.03*bxp_ydom),"A");	text(55,BS_F_8192[1]-(.03*bxp_ydom),"F")
text(56,BS_L_8192[1]-(.03*bxp_ydom),"L");	text(57,BS_R_8192[1]-(.03*bxp_ydom),"R")
text(59,BS_A_16384[1]-(.03*bxp_ydom),"A");	text(60,BS_F_16384[1]-(.03*bxp_ydom),"F")
text(61,BS_L_16384[1]-(.03*bxp_ydom),"L");	text(62,BS_R_16384[1]-(.03*bxp_ydom),"R")
text(64,BS_A_32768[1]-(.03*bxp_ydom),"A");	text(65,BS_F_32768[1]-(.03*bxp_ydom),"F")
text(66,BS_L_32768[1]-(.03*bxp_ydom),"L");	text(67,BS_R_32768[1]-(.03*bxp_ydom),"R")
text(69,BS_A_65536[1]-(.03*bxp_ydom),"A");	text(70,BS_F_65536[1]-(.03*bxp_ydom),"F")
text(71,BS_L_65536[1]-(.03*bxp_ydom),"L");	text(72,BS_R_65536[1]-(.03*bxp_ydom),"R")
legend(1, bxp_yrange[2], nomi, color)
dev.off()

### Delta MEAN Comparison graph
png(gr5,width=png_width,height=png_height)
plot(DMea_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DMea_yrange,xlim=range(x),
	main="Marshalling benchmarks (Average)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DMea_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DMea_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DMea_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DMea_yrange[2], nomi, color)
dev.off()

### Delta MEAN Comparison graph (Absolute values)
png(gr5abs,width=png_width,height=png_height)
plot(DMeaAbs_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DMeaAbs_yrange,xlim=range(x),
	main="Marshalling benchmarks (Average)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (usec)")
points(DMeaAbs_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DMeaAbs_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DMeaAbs_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DMeaAbs_yrange[2], nomi, color)
dev.off()

### Delta MAXIMUM Comparison graph
png(gr6,width=png_width,height=png_height)
plot(DMax_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DMax_yrange,xlim=range(x),
	main="Marshalling benchmarks (Maximum)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DMax_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DMax_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DMax_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DMax_yrange[2], nomi, color)
dev.off()

### Delta Q3 Comparison graph
png(gr7,width=png_width,height=png_height)
plot(DQ3_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DQ3_yrange,xlim=range(x),
	main="Marshalling benchmarks (Q3)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DQ3_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DQ3_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DQ3_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DQ3_yrange[2], nomi, color)
dev.off()

### Delta MEDIAN Comparison graph
png(gr8,width=png_width,height=png_height)
plot(DMed_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DMed_yrange,xlim=range(x),
	main="Marshalling benchmarks (Median)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DMed_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DMed_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DMed_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DMed_yrange[2], nomi, color)
dev.off()

### Delta Q1 Comparison graph
png(gr9,width=png_width,height=png_height)
plot(DQ1_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DQ1_yrange,xlim=range(x),
	main="Marshalling benchmarks (Q1)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DQ1_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DQ1_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DQ1_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DQ1_yrange[2], nomi, color)
dev.off()

### Delta MINIMUM Comparison graph
png(gr10,width=png_width,height=png_height)
plot(DMin_A~x[2:15],type="b",col=color[1],lty=tipi[1],lwd=CD_lwd,
	log="x",
	ylim=DMin_yrange,xlim=range(x),
	main="Marshalling benchmarks (Minimum)",
		xlab="DataSize [Bytes]",ylab="CCM Overhead (ratio)")
points(DMin_F~x,type="b",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(DMin_L~x,type="b",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(DMin_R~x[2:15],type="b",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], DMin_yrange[2], nomi, color)
dev.off()

###RAMObs graph
png(grRO,width=png_width,height=png_height)
plot(RAM_L_4/1000,col=color[1],pch="o",
	xlim=RAMObs_xrange,ylim=RAMObs_yrange,
	main="RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
points(RAM_L_65536/1000,col=color[2],pch="o")
legend(0,RAMObs_yrange[2],c(nomi_leg[30],nomi_leg[44]),color[1:2])
dev.off()

###RAM graphs
png(grR,width=png_width,height=png_height)
plot(RAM[1:14]~x[2:15],type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,log="x",
	ylim=range(RAM),xlim=range(x),
	main="Max RAM occupacy",xlab="DataSize [Bytes]",ylab="Occupacy [kB]")
points(RAM[15:29]~x,type="l",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(RAM[30:44]~x,type="l",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(RAM[45:58]~x[2:15],type="l",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], range(RAM)[2], nomi, color)
dev.off()

###CPUObs graph
png(grCO,width=png_width,height=png_height)
plot(CPU_L_4,col=color[1],pch="o",
	xlim=CPUObs_xrange,ylim=CPUObs_yrange,
	main="CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
points(CPU_L_65536,col=color[2],pch="o")
legend(0,CPUObs_yrange[2],c(nomi_leg[30],nomi_leg[44]),color[1:2])
dev.off()

###CPU graph
png(grC,width=png_width,height=png_height)
plot(CPU[1:14]~x[2:15],type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,log="x",
	ylim=range(CPU),xlim=range(x),
	main="Average CPU occupacy",xlab="DataSize [Bytes]",ylab="Occupacy [%]")
points(CPU[15:29]~x,type="l",col=color[2],lty=tipi[2],lwd=CD_lwd)
points(CPU[30:44]~x,type="l",col=color[3],lty=tipi[3],lwd=CD_lwd)
points(CPU[45:58]~x[2:15],type="l",col=color[4],lty=tipi[4],lwd=CD_lwd)
legend(x[1], range(CPU)[2], nomi, color)
dev.off()

if (is2Host) {
	###RRAMObs graph
	png(grRRO,width=png_width,height=png_height)
	plot(RRAM_L_4/1000,col=color[1],pch="o",
		xlim=RRAMObs_xrange,ylim=RRAMObs_yrange,
		main="Remote RAM Observations",ylab="Used RAM [kBytes]",xlab="Time [sec]")
	points(RRAM_L_65536/1000,col=color[2],pch="o")
	legend(0,RRAMObs_yrange[2],c(nomi_leg[30],nomi_leg[44]),color[1:2])
	dev.off()

	###RRAM graphs
	png(grRR,width=png_width,height=png_height)
	plot(RRAM[1:14]~x[2:15],type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,log="x",
		ylim=range(RRAM),xlim=range(x),
		main="Max Remote RAM occupacy",xlab="DataSize [Bytes]",ylab="Occupacy [kB]")
	points(RRAM[15:29]~x,type="l",col=color[2],lty=tipi[2],lwd=CD_lwd)
	points(RRAM[30:44]~x,type="l",col=color[3],lty=tipi[3],lwd=CD_lwd)
	points(RRAM[45:58]~x[2:15],type="l",col=color[4],lty=tipi[4],lwd=CD_lwd)
	legend(x[1], range(RRAM)[2], nomi, color)
	dev.off()

	###RCPUObs graph
	png(grRCO,width=png_width,height=png_height)
	plot(RCPU_L_4,col=color[1],pch="o",
		xlim=RCPUObs_xrange,ylim=RCPUObs_yrange,
		main="Remote CPU Observations",ylab="Used CPU [%]",xlab="Time [sec]")
	points(RCPU_L_65536,col=color[2],pch="o")
	legend(0,RCPUObs_yrange[2],c(nomi_leg[30],nomi_leg[44]),color[1:2])
	dev.off()

	###RCPU graph
	png(grRC,width=png_width,height=png_height)
	plot(RCPU[1:14]~x[2:15],type="l",col=color[1],lty=tipi[1],lwd=CD_lwd,log="x",
		ylim=range(RCPU),xlim=range(x),
		main="Average Remote CPU occupacy",xlab="DataSize [Bytes]",ylab="Occupacy [%]")
	points(RCPU[15:29]~x,type="l",col=color[2],lty=tipi[2],lwd=CD_lwd)
	points(RCPU[30:44]~x,type="l",col=color[3],lty=tipi[3],lwd=CD_lwd)
	points(RCPU[45:58]~x[2:15],type="l",col=color[4],lty=tipi[4],lwd=CD_lwd)
	legend(x[1], range(RCPU)[2], nomi, color)
	dev.off()}

###HTML SUMMARY
write("<html>",som)
write(c("<title> Marshalling Performance Test - CARDAMOM </title>",
	"<h1> Marshalling Summary </h1>",
	"<h4>",HTML_type,"</h4>",
	"<h4>",HTML_delay,"</h4>",
	"<h4>",HTML_host,"</h4><body><hr>"),som,append=TRUE)

###GRAFICI
write(c("<h3> Summary Graphs </h3>","<p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr1,"\"> Marshalling Benchmark (Observations)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr2,"\"> Marshalling Benchmark (Mean)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr3,"\"> Marshalling Benchmark (Standard Deviation)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr4,"\"> Marshalling Benchmark (BoxPlot)</a> <p> </p>"),som,append=TRUE)

write("<table><tr><td>",som,append=TRUE)
write(c("<a href=\"",grRO,"\"> RAM Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grR,"\"> Max RAM occupacy </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grCO,"\"> CPU Observations </a><p></p>"),som,append=TRUE)
write(c("<a href=\"",grC,"\"> Average CPU occupacy </a><p></p>"),som,append=TRUE)

if(is2Host) {
	write("</td><td>",som,append=TRUE)
	write(c("<a href=\"",grRRO,"\"> Remote RAM Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRR,"\"> Max Remote RAM occupacy </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRCO,"\"> Remote CPU Observations </a><p></p>"),som,append=TRUE)
	write(c("<a href=\"",grRC,"\"> Average CPU occupacy </a><p></p>"),som,append=TRUE)}

write("</td></tr></table>",som,append=TRUE)
write(c("<hr><h4> Comparison CCM vs. no-CCM </h4>","<p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr5,"\"> Marshalling Benchmark (CCM Average)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr6,"\"> Marshalling Benchmark (CCM Maximum)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr7,"\"> Marshalling Benchmark (CCM Q3)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr8,"\"> Marshalling Benchmark (CCM Median)</a> <p> </p>"),
	som,append=TRUE)
write(c("<a href=\"",gr9,"\"> Marshalling Benchmark (CCM Q1)</a> <p> </p>"),som,append=TRUE)
write(c("<a href=\"",gr10,"\"> Marshalling Benchmark (CCM Minimum)</a> <p> </p>"),som,append=TRUE)
write("<hr>",som,append=TRUE)

### SUMMARY TABLE
write(c("<h3> Summary Table </h3>",
	"<p> Statistics vs Data </p><table border=1>"),som,append=TRUE)
write(c("<tr><th> &nbsp; </th><p></p>",
	"<th> Min. </th><th> 1st Qu. </th><th> Median </th><th> Mean </th>",
	"<th> 3rd Qu. </th><th> Max. </th><th> Stnd.Dev </th></tr>"),som,append=TRUE)

write(c("<tr><th>",nomi_leg[1],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_8f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_8f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[2],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_16f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_16))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[3],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_32f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_32f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[4],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_64f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_64f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[5],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_128f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_128f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[6],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_256f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_256f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[7],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_512f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_512f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[8],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_1024f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_1024f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[9],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_2048f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_2048f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[10],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_4096f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_4096f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[11],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_8192f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_8192f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[12],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_16384f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_16384f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[13],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_32768f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_32768f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[14],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_A_65536f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_A_65536f))),"</td></tr>"),som,append=TRUE)

write(c("<tr><th> &nbsp; </th><p></p>",
	"<th> Min. </th><th> 1st Qu. </th><th> Median </th><th> Mean </th>",
	"<th> 3rd Qu. </th><th> Max. </th><th> Stnd.Dev </th></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[15],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_4f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_4f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[16],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_8f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_8f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[17],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_16f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_16f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[18],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_32f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_32f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[19],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_64f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_64f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[20],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_128f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_128f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[21],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_256f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_256f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[22],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_512f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_512f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[23],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_1024f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_1024f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[24],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_2048f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_2048f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[25],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_4096f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_4096f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[26],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_8192f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_8192f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[27],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_16384f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_16384f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[28],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_32768f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_32768f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[29],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_F_65536f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_F_65536f))),"</td></tr>"),som,append=TRUE)


write(c("<tr><th> &nbsp; </th><p></p>",
	"<th> Min. </th><th> 1st Qu. </th><th> Median </th><th> Mean </th>",
	"<th> 3rd Qu. </th><th> Max. </th><th> Stnd.Dev. </th></tr>"),
	som,append=TRUE)
write(c("<tr><th>",nomi_leg[30],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_4f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_4f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[31],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_8f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_8f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[32],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_16f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_16f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[33],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_32f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_32f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[34],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_64f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_64f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[35],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_128f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_128f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[36],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_256f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_256f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[37],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_512f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_512f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[38],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_1024f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_1024f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[39],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_2048f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_2048f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[40],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_4096f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_4096f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[41],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_8192f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_8192f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[42],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_16384f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_16384f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[43],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_32768f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_32768f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[44],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_L_65536f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_L_65536f))),"</td></tr>"),som,append=TRUE)


write(c("<tr><th> &nbsp; </th><p></p>",
	"<th> Min. </th><th> 1st Qu. </th><th> Median </th><th> Mean </th>",
	"<th> 3rd Qu. </th><th> Max. </th><th> Stnd.Dev </th></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[45],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_8f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_8f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[46],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_16f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_16f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[47],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_32f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_32f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[48],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_64f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_64f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[49],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_128f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_128f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[50],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_256f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_256f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[51],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_512f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_512f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[52],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_1024f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_1024f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[53],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_2048f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_2048f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[54],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_4096f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_4096f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[55],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_8192f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_8192f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[56],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_16384f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_16384f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[57],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_32768f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_32768f))),"</td></tr>"),som,append=TRUE)
write(c("<tr><th>",nomi_leg[58],"</th>"),som,append=TRUE)
for (i in 1:6) write(c("<td>",summary(OBS_R_65536f)[i],"</td>"),som,append=TRUE)
write(c("<td>",dig_dev(sqrt(var(OBS_R_65536f))),"</td></tr>"),som,append=TRUE)


write("</table><p></p><hr>",som,append=TRUE)

###TABELLA MEAN
write("<h3> Average Summary Table </h3><p> DataLenght vs DataType </p><p> </p>",som,append=TRUE)
write(c("<table border=1><tr>","<th> &nbsp; </th>"),som,append=TRUE)
for (i in 1:dim(as.array(x)))
	write(c("<th>",as.character(x[i]),"B </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[1], "</th>"), som, append=TRUE)
write("<td> &nbsp; </td>",som,append=TRUE)
for(i in 1:dim(as.array(Mean_A))) 
	write(c("<td>", num_dec(Mean_A[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[2], "</th>"), som, append=TRUE)
for(i in 1:dim(as.array(Mean_F)))
	write(c("<td>", num_dec(Mean_F[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[3], "</th>"), som, append=TRUE)
for(i in 1:dim(as.array(Mean_L)))
	write(c("<td>", num_dec(Mean_L[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[4], "</th>"), som, append=TRUE)
write("<td> &nbsp; </td>",som,append=TRUE)
for(i in 1:dim(as.array(Mean_R)))
	write(c("<td>", num_dec(Mean_R[i]), "</td>"), som, append=TRUE)
write("</tr></table><p></p><hr>",som,append=TRUE)

###TABELLA STANDARD DEVIATION
write("<h3> Standard Deviation Summary Table </h3><p> DataLenght vs DataType </p><p> </p>",
	som,append=TRUE)
write(c("<table border=1><tr>","<th> &nbsp; </th>"),som,append=TRUE)
for (i in 1:dim(as.array(x)))
	write(c("<th>",as.character(x[i]),"B </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[1], "</th>"), som, append=TRUE)
write("<td> &nbsp; </td>",som,append=TRUE)
for(i in 1:dim(as.array(std_A))) write(c("<td>", dig_dev(std_A[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[2], "</th>"), som, append=TRUE)
for(i in 1:dim(as.array(std_F))) write(c("<td>", dig_dev(std_F[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[3], "</th>"), som, append=TRUE)
for(i in 1:dim(as.array(std_L))) write(c("<td>", dig_dev(std_L[i]), "</td>"), som, append=TRUE)
write("</tr>",som,append=TRUE)
write("<tr>", som, append=TRUE)
write(c("<th>", nomi[4], "</th>"), som, append=TRUE)
write("<td> &nbsp; </td>",som,append=TRUE)
for(i in 1:dim(as.array(std_R))) write(c("<td>", dig_dev(std_R[i]), "</td>"), som, append=TRUE)
write("</tr></table><p></p><hr>",som,append=TRUE)


###OUTLIERS
DimOut<-dim(outliers)[1]
write("<h3> Outliers Summary Table </h3><p> Values of outstanding points </p><p></p>",
	som,append=TRUE)
write("<table border=1>",som,append=TRUE)

write(c("<tr><th> Out of ",thresh*100,"% </th>"),som,append=TRUE)
for(i in 1:DimOut) write(c("<th>",i,"' </th>"),som,append=TRUE)
write("</tr>",som,append=TRUE)
for (j in 1:dim(as.array(nomi_leg))) {
write("<tr>",som,append=TRUE)
write(c("<th>", nomi_leg[j], "</th>"),som,append=TRUE)
for (i in 1:DimOut) {
write(c("<td>",num_dec(outliers[i,j]),"</td>"),som,append=TRUE)}
write("</tr>",som,append=TRUE)}
write("</table><p></p><hr>",som,append=TRUE)

write("</body>",som,append=TRUE)
write("</html>",som,append=TRUE)

rm(bxp_x,bxp_ydom,bxp_yrange,i,j,obs_yrange,DimOut,outliers,x,
CPU,CPUObs_xrange,CPUObs_yrange,RAM,RAMObs_xrange,RAMObs_yrange,
Mean_A,Mean_F,Mean_L,Mean_R,Mean_yrange,std_A,std_F,std_L,std_R,std_yrange,
DMea_A,DMea_F,DMea_L,DMea_R,DMea_yrange,DMeaAbs_A,DMeaAbs_F,DMeaAbs_L,DMeaAbs_R,DMeaAbs_yrange,
DMax_A,DMax_F,DMax_L,DMax_R,DMax_yrange,
DQ1_A,DQ1_F,DQ1_L,DQ1_R,DQ1_yrange,DMed_A,Mean_F,DMed_L,DMed_R,DMed_yrange,
DQ3_A,DQ3_F,DQ3_L,DQ3_R,DQ3_yrange,DMin_A,DMin_F,DMin_L,DMin_R,DMin_yrange,
BS_A_1024,BS_A_128,BS_A_16,BS_A_2048,BS_A_256,BS_A_32,BS_A_4096,BS_A_512,BS_A_64,BS_A_8,BS_F_1024,BS_F_128,BS_F_16,BS_F_2048,BS_F_256,BS_F_32,BS_F_4,BS_F_4096,BS_F_512,BS_F_64,BS_F_8,BS_L_1024,BS_L_128,BS_L_16,BS_L_2048,BS_L_256,BS_L_32,BS_L_4,BS_L_4096,BS_L_512,BS_L_64,BS_L_8,BS_R_1024,BS_R_128,BS_R_16,BS_R_2048,BS_R_256,BS_R_32,BS_R_4096,BS_R_512,BS_R_64,BS_R_8,
OBS_A_1024f,OBS_A_128f,OBS_A_16f,OBS_A_2048f,OBS_A_256f,OBS_A_32f,OBS_A_4096f,OBS_A_512f,OBS_A_64f,OBS_A_8f,OBS_F_1024f,OBS_F_128f,OBS_F_16f,OBS_F_2048f,OBS_F_256f,OBS_F_32f,OBS_F_4f,OBS_F_4096f,OBS_F_512f,OBS_F_64f,OBS_F_8f,OBS_L_1024f,OBS_L_128f,OBS_L_16f,OBS_L_2048f,OBS_L_256f,OBS_L_32f,OBS_L_4096f,OBS_L_512f,OBS_L_64f,OBS_L_8f,OBS_L_4f,OBS_R_1024f,OBS_R_128f,OBS_R_16f,OBS_R_2048f,OBS_R_256f,OBS_R_32f,OBS_R_4096f,OBS_R_512f,OBS_R_64f,OBS_R_8f,
OBSN_A_1024f,OBSN_A_128f,OBSN_A_16f,OBSN_A_2048f,OBSN_A_256f,OBSN_A_32f,OBSN_A_4096f,OBSN_A_512f,OBSN_A_64f,OBSN_A_8f,OBSN_F_1024f,OBSN_F_128f,OBSN_F_16f,OBSN_F_2048f,OBSN_F_256f,OBSN_F_32f,OBSN_F_4f,OBSN_F_4096f,OBSN_F_512f,OBSN_F_64f,OBSN_F_8f,OBSN_L_1024f,OBSN_L_128f,OBSN_L_16f,OBSN_L_2048f,OBSN_L_256f,OBSN_L_32f,OBSN_L_4096f,OBSN_L_512f,OBSN_L_64f,OBSN_L_8f,OBSN_L_4f,OBSN_R_1024f,OBSN_R_128f,OBSN_R_16f,OBSN_R_2048f,OBSN_R_256f,OBSN_R_32f,OBSN_R_4096f,OBSN_R_512f,OBSN_R_64f,OBSN_R_8f)
if (is2Host)
	rm(RCPU,RCPUObs_xrange,RCPUObs_yrange,RRAM,RRAMObs_xrange,RRAMObs_yrange)
