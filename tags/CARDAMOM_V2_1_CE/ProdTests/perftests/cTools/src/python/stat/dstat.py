
"""

 This module contains a set of functions that computes the basic
 indeces used in descriptive statistics.

"""

__version__ = "1.0"
__author__  = "Angelo Corsaro <corsaro@cs.wustl.edu>"
__date__    = "June 2003"


###########################################################################

from GracePlot import *
import math
import TableIO

def mean(L):
    "Computes the mean for the sample contained in L"
    mean = 0.0
    for x in L:
        mean += x

    mean = float(mean) / len(L);
    return mean;


###########################################################################

def meanAbsDev(L):
    "Computes the M.A.D. for the items in the list"
    m = mean(L);
    mad = 0;
    for x in L:
        mad += abs(x - m)
    mad = float(mad) / len(L);

    return mad;

###########################################################################


def stdDev(L):
    "Computes the standard deviation of the given data set"

    std = 0;
    m = mean(L);
    for x in L:
        std += math.pow((x - m), 2);
    std = math.sqrt(float(std)/(len(L)-1));

    return std;

###########################################################################

def coeffOfVariation(L):
    return stdDev(L)/mean(L);

def coeffOfSkewness(L):
    sk = 0;
    m = mean(L);
    std = stdDev(L);
    for x in L:
        sk += math.pow((x - m), 3);
    sk = float(sk)/(math.pow(std, 3)*len(L));

    return sk;


###########################################################################

def quantile(L, q):
    n = len(L);
    k = math.floor(1 + (n-1)*q);
    L.sort();

    return L[int(k)];

###########################################################################

def IQR(L):
    return (quantile(L, 0.75) -  quantile(L, 0.25));

###########################################################################

def SIQR(L):
    return IQR(L)/2;

###########################################################################

def ITNQR(L):
    "Computes the inter (0.1, 0.9) quantile range"
    return (quantile(L, 0.90) - quantile(L, 0.10));

def INTR(L):
    "Computes the inter (0.1, 0.9) quantile range"
    return (quantile(L, 0.90) - quantile(L, 0.10));

###########################################################################

def nonOutlierRange(L):
    min = quantile(L, .25) - 1.5*IQR(L);
    max = quantile(L, .75) + 1.5*IQR(L);
    return [min, max];

###########################################################################

def nonExtremeRange(L):
    min = quantile(L, .25) - 3*IQR(L);
    max = quantile(L, .75) + 3*IQR(L);

    return [min, max];

###########################################################################

def mode(L):
    fd = freqDistr(L);
    m = fd[0];
    max = fd[0][1];
    modeSet = [];
    for x in fd:
        if x[1] > max:
            max = x[1];
            m = x;
    modeSet.append(m);

    for x in fd:
        if (x[1] == max) and (x not in modeSet):
            modeSet.append(x);

    return modeSet;

###########################################################################

def midRange(L):
    return (min(L) + max(L))/2;

###########################################################################

def median(L):
    L.sort();
    n = len(L);
    if int((float(n)/2)*2) == n:
        m = L[(n+1)/2];
    else:
        m = (L[(n/2)] + L[(n+1)/2])/2;

    return m;

###########################################################################

def freqDistr(L):
    "Computes the frequency distribution for the element in L"
    L.sort();
    last = -L[0];
    i = 0;
    fd = [];
    for x in L:
        if x!= last:
            fd.append([x, L.count(x)]);
            i = i+1;
        last = x;

    return fd;


def cumFreqDistr(L):
    "Computes the cumulative frequency distribution"
    fd = freqDistr(L);
    cfd = [];
    sum = 0
    for x in fd:
        cfd.append([x[0], x[1]+sum]);
        sum += x[1]

    return cfd


def relFreqDistr(L):
    L.sort();
    last = -L[0];
    i = 0;
    rfd = [];
    N = len(L);
    for x in L:
        if x!= last:
            rfd.append([x, float(L.count(x))/N]);
            i = i+1;

    return rfd;

def cumRelFreqDistr(L):
    "Computes the relative cumulative frequency distribution"
    fd = relFredDistr(L);
    cfd = [];
    sum = 0
    for x in L:
        cfd.addpend([x[0], x[1]+sum]);
        sum += x[1]

    return cfd


def scale(L, scaleFactor):
    S = [];
    for x in L:
        S.append(float((int(x*scaleFactor)))/scaleFactor);

    return S



def plotDataSet(dataSet):
    p = GracePlot();
    x1 = dataSet.col(0);
    y1 = dataSet.col(1);

    xs = dataSet.colAsSample(0);
    ys = dataSet.colAsSample(1);
    xTick = Tick(major = (xs.max() - xs.min())/10, minorticks = 4)
    yTick = Tick(major = (ys.max() - ys.min())/10, minorticks = 4)

    pLine = Line(type = 1, linewidth = 2.5, color = 2);

    d=Data(x = x1, y = y1, line = pLine)
    p.plot(d)
    p.xaxis(xmin = xs.min(), xmax = xs.max(), tick = xTick);
    p.yaxis(ymin = ys.min(), ymax = ys.max(), tick = yTick);

    return p

def plotSample(sample):
    p = GracePlot();

    x1 = range(1, len(sample));
    y1 = sample.samples();

    xTick = Tick(major = (len(sample))/10, minorticks = 4)
    yTick = Tick(major = (sample.max() - sample.min())/10, minorticks = 4)

    pLine = Line(type = 1, linewidth = 2.5, color = 2);

    d=Data(x1, y1, line = pLine);
    p.plot(d)
    p.xaxis(xmin = 0, xmax = len(sample) +1, tick = xTick);
    p.yaxis(ymin = sample.min(), ymax = sample.max(), tick = yTick);

    return p


class DataSet(object):

    def __init__(self, size, data):
        self.size_ = size;
        self.data = data;

    def __getitem__(self, k):
        return self.data[k];

    def __setitem__(self, k, value):
        self.data[k] = value;

    def __len__(self):
        return len(self.data);

    def cols(self):
        return self.size_[1];

    def rows(self):
        return self.size_[0];

    def size(self):
        return self.size_;

    def row(self, k):
        return self.data[k];

    def col(self, k):
        colk = [];
        row = 0;
        for row in range(0, self.size_[0]):
            colk.append(self.data[row][k]);

        return colk;

    def colAsSample(self, k):
        return Sample(self.col(k));

    def max(self):
        maxDataSet = [];

    def __str__(self):
        strRep = '';
        for x in self.data:
            for i in range(0, self.size_[1]):
                strRep += str(x[i]) + '\t';
#            strRep += '\n';
        return strRep;


def createSampleFromFile(filename):
    table = TableIO.readTable(filename, '#');
    return Sample(table['data']);

def readSampleFromFile(filename):
    return createSampleFromFile(filename)

def writeSampleToFile(filename, sample):
    T = {}
    T['filename'] = filename
    T['rows'] = len(sample)
    T['columns'] = 1
    T['data'] = sample.dataSet
    TableIO.writeTable(T)


def createSampleFromFileX(filename, separator):
    table = TableIO.readTable(filename, separator);
    return Sample(table['data']);

def readSampleFromFileX(filename, separator):
    return createSampleFromFileX(filename, separator)



class Sample(object):

    def __init__(self, dataSet):
        self.dataSet = dataSet;

    def trim(self, n):
          self.dataSet = self.dataSet[n:]

    def sample(self):
        return self.dataSet
    
    def mean(self):
        return mean(self.dataSet);

    def max(self):
        return max(self.dataSet);

    def min(self):
        return min(self.dataSet);

    def midRange(self):
        return midRange(self.dataSet);

    def median(self):
        return median(self.dataSet);

    def mode(self):
        m = mode(self.dataSet);
        return DataSet([len(m), 2], m);

    def freqDistr(self):
        fd = freqDistr(self.dataSet);
        return DataSet([len(fd), 2], fd);

    def cumFreqDistr(self):
        cfd = cumFreqDistr(self.dataSet);
        return DataSet([len(cfd), 2], cfd);

    def relFreqDistr(self):
        rfd = relFreqDistr(self.dataSet);
        return DataSet([len(rfd), 2], rfd);

    def cumRelFreqDistr(self):
        crfd = cumRelFreqDistr(self.dataSet);
        return DataSet([len(crfd), 2], crfd);


    def round(self, n):
        roundedSample = [];
        for x in self.dataSet:
            roundedSample.append(round(x, n));
        return Sample(roundedSample);


    def scale(self, f):
        scaledData = [];
        for x in self.dataSet:
            scaledData.append(x*f);

        return Sample(scaledData);

    def meanAbsDev(self):
        return meanAbsDev(self.dataSet);

    def stdDev(self):
        return stdDev(self.dataSet);

    def coeffOfVariation(self):
        return coeffOfVariation(self.dataSet);

    def coeffOfSkewness(self):
        return coeffOfSkewness(self.dataSet);

    def quantile(self, q):
        return quantile(self.dataSet, q);

    def IQR(self):
        return IQR(self.dataSet);

    def SIQR(self):
        return SIQR(self.dataSet);

    def ITNQR(self):
        return ITNQR(self.dataSet);

    def INTR(self):
        return INTR(self.dataSet);

    def nonOutlierRange(self):
        return nonOutlierRange(self.dataSet);

    def nonExtremeRange(self):
        return nonExtremeRange(self.dataSet);


    def filterOutliers(self):
        filteredData = [];
        nor = self.nonOutlierRange();

        for x in self.dataSet:
            if x <= nor[1] and x >= nor[0]:
                filteredData.append(x);

        return Sample(filteredData);

    def getUpperOutliers(self):
        filteredData = [];
        nor = self.nonOutlierRange();

        for x in self.dataSet:
            if x >= nor[1]:
                filteredData.append(x);

        return Sample(filteredData);

    def getLowerOutliers(self):
        filteredData = [];
        nor = self.nonOutlierRange();

        for x in self.dataSet:
            if x <= nor[0]:
                filteredData.append(x);

        return Sample(filteredData);

    def filterExtremes(self):
        filteredData = [];
        ner = self.nonExtremeRange();

        for x in self.dataSet:
            if x <= ner[1] and x >= ner[0]:
                filteredData.append(x);

        return Sample(filteredData);


    def getUpperExtremes(self):
        filteredData = [];
        ner = self.nonExtremeRange();

        for x in self.dataSet:
            if x >= ner[1]:
                filteredData.append(x);

        return Sample(filteredData);

    def getLowerExtremes(self):
        filteredData = [];
        ner = self.nonExtremeRange();

        for x in self.dataSet:
            if x <= ner[0]:
                filteredData.append(x);

        return Sample(filteredData);


    def samples(self):
        return self.dataSet;

    def interval(self):
        if (len(self.dataSet) != 0):
            return [self.min(), self.max()];
        else:
            return []

    def __len__(self):
        return len(self.dataSet);

    def __getitem__(self, k):
        return self.dataSet[k];

    def __setitem__(self, k, value):
        self.dataSet[k] = value;

    def confInt(self, z):
        x = self.mean();
        s = self.stdDev();
        n = len(self.dataSet);

        inf = x - ((z * s)/math.sqrt(n));
        sup = x + ((z * s)/math.sqrt(n));

        return [[inf, sup], (sup - inf)];


    def confInt90(self):
        return self.confInt(1.282);

    def confInt95(self):
        return self.confInt(1.645);

    def confInt99(self):
        return self.confInt(2.326);

    def confInt999(self):
        return self.confInt(3.090);


    
