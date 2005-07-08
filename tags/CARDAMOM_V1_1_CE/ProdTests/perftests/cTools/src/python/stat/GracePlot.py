"""
GracePlot.py -- A high-level Python interface to the Grace plotting package

The intended purpose of GracePlot is to allow easy programmatic and interactive
command line plotting with convenience functions for the most common commands. 
The Grace UI (or the grace_np module) can be used if more advanced
functionality needs to be accessed. 

The data model in Grace, (mirrored in GracePlot) goes like this:  Each grace 
session is like virtual sheet of paper called a Plot.  Each Plot can have 
multiple Graphs, which are sets of axes (use GracePlot.multi() to get multiple
axes in GracePlot).  Each Graph has multiple data Sets.  Data Sets are added to
graphs with the plot and histoPlot functions in GracePlot.

The biggest difference in use of my module over Nathaniel Gray's is that I have
abstracted nearly everything into objects. You can only plot a data object, which
contains all the information about symbols and lines for itself. This is also how
future support of other graph types will be builtin, for example error bars and xyz
etc... Currently, only 2d plots are directly implemented.
a typical session might look like:
#################################################################3
from GracePlot import *

p = GracePlot() # A grace session opens

x=[1,2,3,4,5,6,7,8,9,10]
y=[1,2,3,4,5,6,7,8,9,10]

s1=Symbol(symbol=circle,fillcolor=red)
l1=Line(type=none)

d1=Data(x=x,y=y,symbol=s1,line=l1)

p.plot(d1)

p.text('test',.51,.51,color=2)

p.title('Funding: Ministry of Silly Walks')
p.ylabel('Funding (Pounds\S10\N)')
p.xlimit(0, 6)  # Set limits of x-axis
######################################################################

The best place to find documentation is in the docstrings for each
function/class. In general, default values are used by xmgrace unless
you set them to something else. I have done some things like if you
set the fill color of a symbol, then it automatically sets the fill
pattern to solid, unless you set it to something else.

I have basically taken the output of xmgrace and reverse engineered
everything in the Gui and agr files to figure out all these details.
The documentation for grace is not that complete, and has been that
way a long time.

An important thing to realize about GracePlot is that it only has a
one-way communications channel with the Grace session. This means that
if you make changes to your plot interactively (such as changing the
number/layout of graphs) then GracePlot will have NO KNOWLEDGE of the
changes. This should not often be an issue, since the only state that
GracePlot saves is the number and layout of graphs, the number of Sets
that each graph has, and the hold state for each graph. Originally,
this code started out from:

__version__ = "0.5.1"
__author__ = "Nathaniel Gray <n8gray@caltech.edu>"
__date__ = "September 16, 2001"
"""

__version__ = "1.3.1"
__author__ = "John Kitchin <jkitchin@udel.edu>"
__date__ = "September 17, 2002"

import grace_np
import math
import string
import Numeric as N

on='on'
off='off'
# shortcuts for colors
white=0
black=1
red=2
green=3
blue=4
yellow=5
brown=6
grey,gray=7,7
violet=8
cyan=9
magenta=10
orange=11
indigo=12
maroon=13
turquoise=14
green4=15

#shortcuts for symbols
none=0
circle=1
square=2
diamond=3
triangle_up=4
triangle_left=5
triangle_down=6
triangle_right=7
plus=8
x=9
star=10
character=11
#shortcuts for linestyle
solid=1
dotted=2
dashed=3
long_dashed=4
dot_dashed=5

#shortcuts for fill patterns
none=0
solid=1
opaque=8

#graph types: Not all are supported yet.
xy='xy'
xydx='xydx'
xydy='xydy'
xydxdy='xydxdy'
xydxdx='xydxdx'
xydydy='xydydy'
xydxdxdydy='xydxdxdydy'
bar='bar'
bardy='bardy'
bardydy='bardydy'
xyhilo='xyhilo'
xyz='xyz'
xyr='xyr'
xysize='xysize'
xycolor='xycolor'
xycolpat='xycolpat'
xyvmap='xyvmap'
xyboxplot='xyboxplot'

#string justification
left=0
center=2
right=1

true='true'
false='false'

#frame types
closed=0
halfopen=1
breaktop=2
breakbotton=3
breakleft=4
breakright=5

class GracePlot:   
    def __init__(self,width=6,height=4):

        self.resolution=72 #change if needed
        self.pagewidth=width
        self.pageheight=height
        
        self.grace = grace_np.GraceProcess(fixedsize=(width*self.resolution,height*self.resolution))
        self.g = [ GraceGraph(self.grace, 0) ]
        self.curr_graph = self.g[0]
        self.rows = 1
        self.cols = 1
        self.focus(0,0)

        self.curr_graph.SetView()
        
    def _send(self, cmd): 
        #print cmd
        self.grace.command(cmd)
        
    def _flush(self):
        #print 'flush()'
        self.grace.flush()
        
    def __del__(self):
        """Destroy the pipe but leave the grace window open for interaction.
        This is the best action for the destructor so that unexpected crashes
        don't needlessly destroy plots."""
        self.grace = None

    def exit(self):
        """Nuke the grace session.  (more final than GracePlot.__del__())"""
        self.grace.exit()

    def redraw(self):
        """Refresh the plot"""
        #print 'redraw'
        self.grace('redraw')
        
    def save(self, filename, format='agr'):
        """Save the current plot
        Default format is Grace '.agr' file, but other possible formats
        are: x11, postscript, eps, pdf, mif, svg, pnm, jpeg, png, metafile
        Note:  Not all drivers are created equal.  See the Grace documentation
            for caveats that apply to some of these formats."""
        devs = {'agr':'.agr', 'eps':'.eps', 'jpeg':'.jpeg', 'metafile':'',
                'mif':'', 'pdf':'.pdf', 'png':'.png', 'pnm':'', 
                'postscript':'.ps', 'svg':'', 'x11':''}
        try:
            ext = devs[string.lower(format)]
        except KeyError:
            print 'Unknown format.  Known formats are\n%s' % devs.keys()
            return
            
        if filename[-len(ext):] != ext:
            filename = filename + ext
        if ext == '.agr':
            self._send('saveall "%s"' % filename)
        #I basically only use PNG, so I haven't worked in support for the other ones. This
        #code is basically Nate Gray's but it didn't work for me, so I minimally fixed it.
        # I will probably extend it similar to the png example, since I like graphics
        # to be basically the same each time, and each device is subtly different
        elif ext == '.png': 
            self._send('PAGE RESIZE 600,400')
            self._send('device "PNG" dpi 600')
            self._send('hardcopy device "PNG"')
            self._send('print to "%s"' % filename)
            self._send('print')
        self._flush()
    
        
    def focus( self, row, col ):
        """Set the currently active graph
        rows start at 0, columns start at 0"""
        self.curr_graph = self.g[row*self.cols + col]
        self._send('focus g%s' % self.curr_graph.gID)
        self._send('with g%s' % self.curr_graph.gID)
        self._flush()
        self.redraw()
        
        for i in ['plot', 'title', 'subtitle', 'xlabel', 'ylabel',
                  'xaxis','yaxis','frame','text','line',
                  'kill', 'clear', 'legend', 'hold', 'xlimit', 'ylimit',
                  'redraw','plotxy']:
            setattr( self, i, getattr(self.curr_graph, i) )

        return self.curr_graph        

    def resize( self, xdim, ydim, rescale=1 ):
        """Change the page dimensions (in pp).  If rescale==1, then also
        rescale the current plot accordingly.  Don't ask me what a pp is--I
        don't know."""
        if rescale:
            self._send('page resize %s %s' % (xdim, ydim))
        else:
            self._send('page size %s %s' % (xdim, ydim))

    def __getitem__( self, item ):
        """Access a specific graph.  Can use either p[num] or p[row, col]."""
        if type(item) == type(1):
            return self.g[item]
        elif type(item) == type( () ) and len(item) <= 2:
            if item[0] >= self.rows or item[1] >= self.cols:
                raise IndexError, 'graph index out of range'
            return self.g[item[0]*self.cols + item[1]]
        else:
            raise TypeError, 'graph index must be integer or two integers'

class GraceGraph:
    """
    view_x is a tuple of (xmin,xmax)
    view_y is a tuple of (ymin,ymax)
    """
    def __init__(self, grace, gID,view_xmin=0.15,view_xmax=1.3,view_ymin=0.15,view_ymax=0.85):

        self._hold = 0       # Set _hold=1 to add datasets to a graph

        self.grace = grace
        self.nSets = 0
        self.gID = gID

        self.world_xmin= 10000000000000.
        self.world_xmax=-10000000000000.
        self.world_ymin= 10000000000000.
        self.world_ymax=-10000000000000.
            
        self.SetView(xmin=view_xmin,xmax=view_xmax,ymin=view_ymin,ymax=view_ymax)
        
    def _send(self, cmd):
        self.grace.command(cmd)
        
    def _flush(self):
        #print 'flush()'
        self.grace.flush()


    def hold(self, onoff=None):
        """Turn on/off overplotting for this graph.
        
        Call as hold() to toggle, hold(1) to turn on, or hold(0) to turn off.
        Returns the previous hold setting.
        """
        lastVal = self._hold
        if onoff is None:
            self._hold = not self._hold
            return lastVal
        if onoff not in [0, 1]:
            raise RuntimeError, "Valid arguments to hold() are 0 or 1."
        self._hold = onoff
        return lastVal
    
    def title(self,string=None,font=None,size=None,color=None):
        """Sets the graph title"""
        if string is not None:
            self._send('title "%s"'% string)
        if font is not None:
            self._send('title font %d' % font)
        if size is not None:
            self._send('title size %f' % size)
        if color is not None:
            self._send('title color %d' % color)
        
    def subtitle(self,string=None,font=None,size=None,color=None):
        """Sets the graph subtitle"""
        if string is not None:
            self._send('subtitle "%s"'% string)
        if font is not None:
            self._send('subtitle font %d' % font)
        if size is not None:
            self._send('subtitle size %f' % size)
        if color is not None:
            self._send('subtitle color %d' % color)
        
    def redraw(self):
        """Refresh the plot"""
        self.grace('redraw')
        
    def xaxis(self,xmin=None,xmax=None, scale=None, invert=None,
              offset=None, label=None, tick=None, bar=None):
        """
        offset must be a tuple
        invert is a string 'on' or 'off'
        scale is a string, 'Normal','reciprocal','logarithmic'
        
        """
        if xmin is not None:
            self.world_xmin=xmin
            self._send('with g%s; world xmin %f' % (self.gID,xmin))
        if xmax is not None:
            self.world_xmax=xmax
            self._send('with g%s; world xmax %f' % (self.gID,xmax))
        if scale is not None:
            self._send('xaxes scale %s' % scale)
        if invert is not None:
            self._send('xaxes invert %s' % invert)
        if offset is not None:
            self._send('xaxis offset %f, %f' % offset)
            
        if label is not None:
            for i in label.output('xaxis'):
                self._send(i)
                                
        if tick is not None:
            for i in tick.output('xaxis'):
                self._send(i)
                
        if bar is not None:
            for i in bar.output('xaxis'):
                self._send(i)
        self.redraw()
        
    def yaxis(self,ymin=None,ymax=None,  scale=None, invert=None,
              offset=None, label=None, tick=None, bar=None):
        """
        offset must be a tuple
        see xaxis
        """
        if ymin is not None:
            self.world_ymin=ymin
            self._send('with g%s; world ymin %f' % (self.gID,ymin))
        if ymax is not None:
            self.world_ymax=ymax
            self._send('with g%s; world ymax %f' % (self.gID,ymax))

        if scale is not None:
            self._send('yaxes scale %s' % scale)
        if invert is not None:
            self._send('yaxes invert %s' % invert)
        if offset is not None:
            self._send('yaxis offset %f, %f' % offset)

        if label is not None:
            for i in label.output('yaxis'):
                self._send(i)

        if tick is not None:
            for i in tick.output('yaxis'):
                self._send(i)

        if bar is not None:
            for i in bar.output('yaxis'):
                self._send(i)
        self.redraw()
        
    def xlimit(self, lower=None, upper=None):
        """Convenience function to set the lower and/or upper bounds of the x-axis."""
        self.xaxis(xmin=lower,xmax=upper)

    def ylimit(self, lower=None, upper=None):
        """Convenience function to set the lower and/or upper bounds of the y-axis."""
        self.yaxis(ymin=lower,ymax=upper)

    def xlabel(self,label):
        """Convenience function to set the xaxis label"""
        self._send('with g%s' % self.gID)
        self._send('xaxis label "' +str(label)+'"')
        self.redraw()

    def ylabel(self,label):
        """Convenience function to set the yaxis label"""
        self._send('with g%s' % self.gID)
        self._send('yaxis label "' +str(label)+'"')
        self.redraw()
        
        
    def kill(self):
        """Kill the plot"""
        self._send('kill g%s' % self.gID)
        self._send('g%s on' % self.gID)
        self.redraw()
        self.nSets = 0
        self._hold = 0

    def clear(self):
        """Erase all lines from the plot and set hold to 0"""
        for i in range(self.nSets):
            self._send('kill g%s.s%s' % (self.gID, i))
        self.redraw()
        self.nSets=0
        self._hold=0

    def legend(self,strings=None,x=None,y=None,
               boxcolor=None,boxpattern=None,boxlinewidth=None,
               boxlinestyle=None,boxfillcolor=None,boxfillpattern=None,
               font=None,charsize=None,
               color=None,length=None,vgap=None,hgap=None,invert=None):
        """Sets up the legend
        x and y are the position of the upper left corner
        boxcolor is the color of the legend box lines
        boxpattern is the pattern of the legend box lines
        boxlinewidth is the thickness of the line
        boxlinestyle
        boxfillcolor
        boxfillpattern
        font is the font used in the legend
        charsize controls the size of the characters
        length controls the length of the box must be an integer
        vgap controls the vertical space between entries, can be a float
        hgap controls horizontal spacing in the box can be a float
        invert is 'true' or 'false', controls the order of entries, either in the order they are entered, or the opposite.
        """
        self._send('legend on')
        self._send('legend loctype view')
        if strings is not None:
            for i in range(len(strings)):
                self._send( ('g%s.s%s legend "' % (self.gID, i)) + strings[i] + '"' )
                self._send('with g%s; legend on' % self.gID)
        self.redraw()
        if strings is None:
            strings=self.legend_strings
            for i in range(len(strings)):
                self._send( ('g%s.s%s legend "' % (self.gID, i)) + strings[i] + '"' )
                self._send('with g%s; legend on' % self.gID)
            self.redraw()

        if x is not None and y is not None:
            self._send('legend %f, %f' % (self.xtranslate(x),self.ytranslate(y)))
        if boxcolor is not None:
            self._send('legend box color %d' % boxcolor)
        if boxpattern is not None:
            self._send('legend box pattern %d' % boxpattern)
        if boxlinewidth is not None:
            self._send('legend box linewidth %f' % boxlinewidth)
        if boxlinestyle is not None:
            self._send('legend box linestyle %d' % boxlinestyle)
        if boxfillcolor is not None:
            self._send('legend box fill color %d' % boxfillcolor)
        if boxfillpattern is not None:
            self._send('legend box fill pattern %d' % boxfillpattern)
        if font is not None:
            self._send('legend font %d' % font)
        if charsize is not None:
            self._send('legend char size %f' %charsize)
        if color is not None:
            self._send('legend color %d' % color)
        if length is not None:
             self._send('legend length %d' % length)
        if vgap is not None:
            self._send('legend vgap %d' % vgap)
        if hgap is not None:
            self._send('legend hgap %d' % hgap)
        if invert is not None:
            self._send('legend invert %s' % invert)

    def frame(self,type=None,linestyle=None,linewidth=None,
              color=None,pattern=None,
              backgroundcolor=None,backgroundpattern=None):
        """
        type= closed,halfopen,breaktop,breakbottom,breakleft,breakright
        linestyle
        linewidth
        color
        pattern
        backgroundcolor
        backgroundpattern
        """
        if type is not None:
            self._send('frame type %d' % type)
        if linestyle is not None:
            self._send('frame linestyle %d' % linestyle)
        if linewidth is not None:
            self._send('frame linewidth %d' % linewidth)
        if color is not None:
            self._send('frame color %d' % color)
        if backgroundcolor is not None:
            self._send('frame background color %d' % backgroundcolor)
            self._send('frame background pattern %d' % solid)
        if backgroundpattern is not None:
            self._send('frame background pattern %d' % backgroundpattern)


    def plot(self, *DataSets):
        """
        plot Data instances, see Data class
        currently only Data objects are supported.
        """
        #This allows a list of Data objects to be passed
        #for the convenience plotxy method
        if len(DataSets)==1:
            if type(DataSets[0])==type([]):
                DataSets=DataSets[0]
        
        if len(DataSets)>1:
            self.hold()

        legend=[]
        count=0
        for dataset in DataSets:
            if isinstance(dataset,Data):
                for i in dataset.output(self,count):
                    self._send(i)

                legend.append(dataset.legend)
                count=count+1
                    
        if len(legend)>0:
            for i in range(len(legend)):
                if legend[i] is None:
                    legend[i]='n/a'
        
        self.legend_strings=legend
        ### Do these for every type of dataset
        #these lines are necessary so the variables get set.
        # it is my own version of autoscaling, it adds 10%
        # to the borders
        percent=0.10

        self.world_xmax=self.world_xmax+percent*(self.world_xmax-self.world_xmin)
        self.world_xmin=self.world_xmin-percent*(self.world_xmax-self.world_xmin)
        self.world_ymax=self.world_ymax+percent*(self.world_ymax-self.world_ymin)
        self.world_ymin=self.world_ymin-percent*(self.world_ymax-self.world_ymin)
        
        self.xaxis(xmin=self.world_xmin,xmax=self.world_xmax,tick=Tick(major=math.ceil((self.world_xmax-self.world_xmin)/10),minorticks=1))
        self.yaxis(ymin=self.world_ymin,ymax=self.world_ymax,tick=Tick(major=math.ceil((self.world_ymax-self.world_ymin)/10),minorticks=1))
            
        
    def plotxy(self,*xydatasets):
        # can take a shortcut, and use plotxy([x,y])
        datasets=[]
        for set in xydatasets:
            try:
                self.x=set[0]
                self.y=set[1]
            except:
                raise IndexError,'xy data is in wrong form'      
            datasets.append(Data(x=self.x,y=self.y))
            
        self.plot(datasets)
        
    def text(self,string=None, x=None,y=None,
                 color=None, rot=None, font=None,
                 just=None, charsize=None):
        """ The coordinates are the cartesian cooridinates for a single graph,
        they don't work yet for multi graphs."""
        self.string=string
        self.x=self.xtranslate(x)
        self.y=self.ytranslate(y)        
        self.color=color        
        self.rot=rot        
        self.font=font      
        self.just=just        
        self.charsize=charsize

        self._send('with string')
        self._send('string on')
        self._send('string loctype view')

        if self.x is not None and self.y is not None:
            self._send('string %f, %f' % (self.x,self.y))
        if self.color is not None:
            self._send('string color %d' % self.color)
        if self.rot is not None:
            self._send('string rot %f' % self.rot)
        if self.font is not None:
            self._send('string font %d' % self.font)
        if self.just is not None:
            self._send('string just %d' % self.just)
        if self.charsize is not None:
            self._send('string char size %f' % self.charsize)
        if self.string is not None:
            self._send('string def "%s"' % self.string)

    def line(self,x1=None,y1=None,x2=None,y2=None,
                 linewidth=None,linestyle=None,
                 color=None,
                 arrow=None,arrowtype=None,arrowlength=None,arrowlayout=None):
        """
        coordinates are the cartesian cooridinates for a single graph,
        they don't work yet for multi graphs.
        arrow tells where the arrowhead is and is 0,1,2, or 3 for none, start, end, both ends
        arrowtype is for line (0), filled (1), or opaque (2), and only have an effect if the arrowlayout is not (1,1)
        arrowlayout must be a list of 2 numbers,  arrowlayout=(1,1) the first number relates to d/L and the second is I/L
             the meaning of which is unclear, but they affect the arrow shape.
        """
        self.x1=self.xtranslate(x1)
        self.x2=self.xtranslate(x2)
        self.y1=self.ytranslate(y1)
        self.y2=self.ytranslate(y2)
        self.linewidth=linewidth
        self.linestyle=linestyle
        self.color=color
        self.arrow=arrow
        self.arrowtype=arrowtype
        self.arrowlength=arrowlength
        self.arrowlayout=arrowlayout


        self._send('with line')
        self._send('line on')
        self._send('line loctype view')
        if None not in [self.x1,self.x2,self.y1,self.y2]:
            self._send('line %f, %f, %f,%f' %(self.x1,self.y1,self.x2,self.y2))
        if self.linewidth is not None:
            self._send('line linewidth %f' % self.linewidth)
        if self.linestyle is not None:
            self._send('line linestyle %d' % self.linestyle)
        if self.color is not None:
            self._send('line color %d' % self.color)
        if self.arrow is not None:
            self._send('line arrow %d' % self.arrow)
        if self.arrowtype is not None:
            self._send('line arrow type %d' % self.arrowtype)
        if self.arrowlength is not None:
            self._send('line arrow length %f' % self.arrowlength)
        if self.arrowlayout is not None:
            self._send('line arrow layout %f,%f' % self.arrowlayout)

        self._send('line def')

    def xtranslate(self,x_world_coord):
        if x_world_coord is not None:
            xnew=self.view_xmax+((self.view_xmax-self.view_xmin)/
                                 (self.world_xmax-self.world_xmin))*(x_world_coord-self.world_xmax)
            return xnew
        else:
            pass

    def ytranslate(self,y_world_coord):
        if y_world_coord is not None:
            ynew=self.view_ymax+((self.view_ymax-self.view_ymin)/
                                 (self.world_ymax-self.world_ymin))*(y_world_coord-self.world_ymax)
            return ynew
        else:
            pass
    
    
    def SetView(self,xmin=0.15,ymin=0.15,xmax=1.3,ymax=0.85):
        """
        this sets the viewport coords so they are available later
        for translating string and line coords.
        """
        self.view_xmin=xmin
        self.view_xmax=xmax
        self.view_ymin=ymin
        self.view_ymax=ymax

        if self.view_xmin is not None:
            self._send('view xmin %f' % xmin)
        if self.view_xmax is not None:
            self._send('view xmax %f' % xmax)
        if self.view_ymin is not None:
            self._send('view ymin %f' % ymin)
        if self.view_ymax is not None:
            self._send('view ymax %f' % ymax)
        
class Data:
    counter=0
    def __init__(self, 
                 x=[],y=[],
                 dx=[],dy=[],
                 dx_left=[],dx_right=[],dy_up=[],dy_down=[],
                 z=[],
                 symbol=None,line=None,legend=None,comment=None,type='xy',):
        self.x=list(x)
        self.y=list(y)
        self.dx=list(dx)
        self.dy=list(dy)
        self.dx_left=list(dx_left)
        self.dx_right=list(dx_right)
        self.dy_up=list(dy_up)
        self.dy_down=list(dy_down)
        self.z=list(z)
        self.symbol=symbol
        self.line=line
        self.legend=legend
        self.comment=comment
        self.type=type
        
        self.__class__.counter=self.__class__.counter+1

    def output(self,graceGraph,count):
        """ No checking is done to make sure the datasets are
        consistent with each other, same number of x and y etc...
        Support of None values is only in the xy graph.
        """
        gID=graceGraph.gID
               
        if self.type=='xy':
            x = self.x
            y = self.y
            #I had to implement this myself, because of the way that python treats None
            # apparently, None is less than everything.
            xmin=10000000000000.
            xmax=-10000000000000.
            ymin=10000000000000.
            ymax=-10000000000000.
            if type(x)==type([]):
                length=len(x)
                for i in x:
                    if i is not None and i < xmin:
                        xmin=i
                    if i is not None and i > xmax:
                        xmax=i
            else:
                length=1
                xmin=x
                xmax=x
            
            if xmin < graceGraph.world_xmin:
                graceGraph.world_xmin=xmin  #establishes the minimum x            
            if xmax > graceGraph.world_xmax:
                graceGraph.world_xmax=xmax #establishes the maximum x
                
            if type(y)==type([]):
                for i in y:
                    if i is not None and i < ymin:
                        ymin=i
                    if i is not None and i > ymax:
                        ymax=i
            else:
                ymin=y
                ymax=y
            
            if ymin < graceGraph.world_ymin:
                graceGraph.world_ymin=ymin #establishes the minimum y
            if ymax > graceGraph.world_ymax:
                graceGraph.world_ymax=ymax #establishes the minimum y

            list=[]
            list.append('g%s.s%s on' %(gID,count))
            list.append('g%s.s%s type %s' % (gID,count,self.type))
            # i don't know why you pass a single point with a None in it, but this catches it
            if length==1 and x is not None and y is not None:
                list.append('g%s.s%s point %s, %s' % (gID,count,x,y))
            else:
                for i in range(length):
                    #check that neither value is None before passing it.
                    if x[i] is not None and y[i] is not None:
                        list.append('g%s.s%s point %s, %s' % (gID,count,x[i],y[i]))
                
            if self.symbol is not None:
                list=list+self.symbol.output('s%d' % count)
            if self.line is not None:
                list=list+self.line.output('s%d' % count)

            
        if self.type=='xydy' or self.type=='xydx':
            x = self.x
            y = self.y
            if self.type=='xydx':
                error=self.dx
            else:
                error=self.dy
                
            xmin=min(x)
            if xmin < graceGraph.world_xmin:
                graceGraph.world_xmin=xmin  #establishes the minimum x
            xmax=max(x)
            if xmax > graceGraph.world_xmax:
                graceGraph.world_xmax=xmax #establishes the maximum x          
            ymin=min(y)
            if ymin < graceGraph.world_ymin:
                graceGraph.world_ymin=ymin #establishes the minimum y
            ymax=max(y)
            if ymax > graceGraph.world_ymax:
                graceGraph.world_ymax=ymax #establishes the minimum y
                
            list=[]
            list.append('g%s.s%s on' %(gID,count))
            list.append('g%s.s%s type %s' % (gID,count,self.type))

            for i in range(len(x)):
                if x[i] is not None and y[i] is not None:
                    #print x[i],y[i]
                    list.append('g%s.s%s point %s, %s' % (gID,count,x[i],y[i]))
                    list.append('g%s.s%s.y1[%s] = %s' % (gID,count,i,error[i]))


        if self.type=='xydxdx' or self.type=='xydydy' or self.type=='xydxdy':
            x = self.x
            y = self.y
            xmin=min(x)
            if xmin < graceGraph.world_xmin:
                graceGraph.world_xmin=xmin  #establishes the minimum x
            xmax=max(x)
            if xmax > graceGraph.world_xmax:
                graceGraph.world_xmax=xmax #establishes the maximum x          
            ymin=min(y)
            if ymin < graceGraph.world_ymin:
                graceGraph.world_ymin=ymin #establishes the minimum y
            ymax=max(y)
            if ymax > graceGraph.world_ymax:
                graceGraph.world_ymax=ymax #establishes the minimum y
                
            if self.type=='xydxdx':
               error1=self.dx_left
               error2=self.dx_right
            if self.type=='xydxdy':
                error1=self.dx
                error2=self.dy
            if self.type=='xydydy':
                error1=self.dy_up
                error2=self.dy_down
                
            list=[]
            list.append('g%s.s%s on' %(gID,count))
            list.append('g%s.s%s type %s' % (gID,count,self.type))
            
            for i in range(len(x)):
                list.append('g%s.s%s point %s, %s' % (gID,count,x[i],y[i]))
                list.append('g%s.s%s.y1[%s] = %s' % (gID,count,i,error1[i]))
                list.append('g%s.s%s.y2[%s] = %s' % (gID,count,i,error2[i]))

        if self.type=='xydxdxdydy':
            x = self.x
            y = self.y
            xmin=min(x)
            if xmin < graceGraph.world_xmin:
                graceGraph.world_xmin=xmin  #establishes the minimum x
            xmax=max(x)
            if xmax > graceGraph.world_xmax:
                graceGraph.world_xmax=xmax #establishes the maximum x          
            ymin=min(y)
            if ymin < graceGraph.world_ymin:
                graceGraph.world_ymin=ymin #establishes the minimum y
            ymax=max(y)
            if ymax > graceGraph.world_ymax:
                graceGraph.world_ymax=ymax #establishes the minimum y  
                
            dx_left=self.dx_left
            dx_right=self.dx_right
            dy_up=self.dy_up
            dy_down=self.dy_down

            list=[]
            list.append('g%s.s%s on' %(gID,count))
            list.append('g%s.s%s type %s' % (gID,count,self.type))
            
            for i in range(len(x)):
                list.append('g%s.s%s point %s, %s' % (gID,count,x[i],y[i]))
                list.append('g%s.s%s.y1[%s] = %s' % (gID,count,i,dx_left[i]))
                list.append('g%s.s%s.y2[%s] = %s' % (gID,count,i,dx_right[i]))
                list.append('g%s.s%s.y3[%s] = %s' % (gID,count,i,dy_up[i]))
                list.append('g%s.s%s.y4[%s] = %s' % (gID,count,i,dy_down[i]))


        if self.type=='bar':
            x=self.x
            y=self.y
            
            xmin=min(x)
            if xmin < graceGraph.world_xmin:
                graceGraph.world_xmin=xmin  #establishes the minimum x
            xmax=max(x)
            if xmax > graceGraph.world_xmax:
                graceGraph.world_xmax=xmax #establishes the maximum x          
            ymin=min(y)
            if ymin < graceGraph.world_ymin:
                graceGraph.world_ymin=ymin #establishes the minimum y
            ymax=max(y)
            if ymax > graceGraph.world_ymax:
                graceGraph.world_ymax=ymax #establishes the minimum y
                
            list=[]
            list.append('g%s.s%s on' %(gID,count))
            list.append('g%s.s%s type %s' % (gID,count,self.type))
            
            for i in range(len(x)):
                list.append('g%s.s%s point %s, %s' % (gID,count,x[i],y[i]))
        
        if self.symbol is not None:
            list=list+self.symbol.output('s%d' % count)
        if self.line is not None:
            list=list+self.line.output('s%d' % count)
        return list
    
class Symbol:
    """
    type can be 'xy','bar'
    symbol:
    0 None
    1 circle
    2 square
    3 diamond
    4 triangle up
    5 triangle left
    6 triangle down
    7 triangle right
    8 +
    9 x
    10 *
    11 character
    
    size is self explanatory, it should be a number.
    
    color (for the outline of the symbol):
    1 black
    2 red orange
    3. lt green
    4 royal blue
    5 yellow
    6 lt brown
    7 lt grey
    8 purple 1
    9 lt blue 1
    10 magenta
    11 orange
    12 purple 2
    13 maroon
    14 lt blue 2
    15 forest green

    pattern appears to be the pattern of the outline of the symbol, normally it will be 1, which is solid.
    it can be any integer from 0 to 8 i think.

    linewidth is the thickness of the outline of the symbol
    linestyle will normally be 1, for solid

    fillcolor is the color the symbol is filled with, by default it is the same as the outline color.

    fillpattern is the pattern of the fill, 1 is solid, 0 is None, there are about 20 choices.
    """
    
    def __init__(self,dataset=None,type=None,symbol=None,size=None,color=black,
                 pattern=None,linewidth=None,linestyle=None,
                 filltype=None,fillrule=None,
                 fillcolor=None,fillpattern=None,
                 char=None,charfont=None,skip=None,
                 annotation=None,errorbar=None):
        self.dataset=dataset
        self.type=type
        self.symbol=symbol
        self.size=size
        self.color=color
        self.pattern=pattern
        self.linewidth=linewidth
        self.linestyle=linestyle
        self.filltype=filltype
        self.fillrule=fillrule
        self.fillcolor=fillcolor
        self.fillpattern=fillpattern
        self.char=char
        self.charfont=charfont
        self.skip=skip
        self.annotation=annotation
        self.errorbar=errorbar

    def output(self,dataset):
        list=[]
        if self.type is not None:
            list.append(dataset+" type %s" % self.type)
        if self.symbol is not None:
            list.append(dataset+" symbol %d" % self.symbol)
        if self.size is not None:
            list.append(dataset+" symbol size %f" % self.size)
        if self.color is not None:
            list.append(dataset+" symbol color %d" % self.color)
        if self.pattern is not None:
            list.append(dataset+" symbol pattern %d" % self.pattern)
        if self.filltype is not None:
            list.append(dataset+" symbol fill type %d" % self.filltype)
        if self.fillrule is not None:
            list.append(dataset+" symbol fill rule %d" % self.fillrule)
        if self.fillcolor is not None:
            list.append(dataset+" symbol fill color %d" % self.fillcolor)
            list.append(dataset+" symbol fill pattern 1")
        if self.fillpattern is not None:
            list.append(dataset+" symbol fill pattern %d" % self.fillpattern)
        if self.linewidth is not None:
            list.append(dataset+" symbol linewidth %d" % self.linewidth)
        if self.linestyle is not None:
            list.append(dataset+" symbol linestyle %d" % self.linestyle)
        if self.char is not None:
            list.append(dataset+" symbol char %d" % self.char)
        if self.charfont is not None:
            list.append(dataset+" symbol char font %d" % self.charfont)
        if self.skip is not None:
            list.append(dataset+" symbol skip %d" % self.skip)

        if self.annotation is not None:
            list=list+ self.annotation.output(dataset)
                
        if self.errorbar is not None:
            list=list+ self.errorbar.output(dataset)
                
                
        return list
class Line:
    """
    Best guesses for acceptable values:
    type will normally be set to 1
    
    style:
    1 is normal
    2 is dotted
    3 is dashed
    4 is long dashed
    5 is dot-dashed

    width:
    goes from 1 to 6 in increasing thickness

    color:
    1 black
    2 red orange
    3. lt green
    4 royal blue
    5 yellow
    6 lt brown
    7 lt grey
    8 purple 1
    9 lt blue 1
    10 magenta
    11 orange
    12 purple 2
    13 maroon
    14 lt blue 2
    15 forest green
    """
    
    def __init__(self,type=None,linestyle=None,linewidth=None,color=None,pattern=None,
                 baselinetype=None, baseline=None,dropline=None):
        
        self.type=type
        self.linestyle=linestyle
        self.linewidth=linewidth
        self.color=color
        self.pattern=pattern
        self.baseline=baseline
        self.baselinetype=baselinetype
        self.dropline=dropline

    def output(self,dataset):
        list=[]
        if self.type is not None:
            list.append(dataset+" line type %s" % self.type)
        if self.linestyle is not None:
            list.append(dataset+" line linestyle %s" % self.linestyle)
        if self.linewidth is not None:
            list.append(dataset+" line linewidth %s" % self.linewidth)
        if self.color is not None:
            list.append(dataset+" line color %s" % self.color)
        if self.pattern is not None:
            list.append(dataset+" line pattern %s" % self.pattern)
        if self.baseline is not None:
            list.append(dataset+" baseline %s" % self.baseline)
        if self.baselinetype is not None:
            list.append(dataset+" baseline type %d" % self.baselinetype)
        if self.dropline is not None:
            list.append(dataset+" dropline %s" % self.dropline)
        return list

class Label:
    """
    Used for labels of the x-axis and y-axis
    """
    def __init__(self,string=None,
                 layout=None,place=None,
                 charsize=None, font=None,
                 color=None,axis=None,):
        self.axis=axis
        self.label=string
        self.layout=layout
        self.place=place
        self.charsize=charsize
        self.font=font
        self.color=color
        self.place=place

    def output(self,axis):
        list=[]
        if self.label is not None:
            list.append(axis+' label "%s"' % self.label)
        if self.layout is not None:
            list.append(axis+' label layout %s' % self.layout)
        if self.place is not None:
            list.append(axis+' label place %s' % self.place)
        if self.charsize is not None:
            list.append(axis+' label char size %f' % self.charsize)
        if self.font is not None:
            list.append(axis+' label font %d' % self.font)
        if self.color is not None:
            list.append(axis+' label color %d' % self.color)
        if self.place is not None:
            list.append(axis+' label place %s' % self.place)
        return list
    
class Bar:
    """
    this class controls the x and y bars in the frame apparently
    usually it is off
    onoff is 'on' or 'off'
    the rest are like everything else
    """
    def __init__(self,axis=None,onoff='on',color=None,linestyle=None,linewidth=None):
        self.axis=axis
        self.onoff=onoff
        self.color=color
        self.linestyle=linestyle
        self.linewidth=linewidth
    def output(self,axis):
        list=[]
        if self.onoff is not None:
            list.append(axis+' bar %s' % self.onoff)
        if self.color is not None:
            list.append(axis+' bar color %d' % self.color)
        if self.linestyle is not None:
            list.append(axis+' bar linestyle %d' % self.linestyle)
        if self.linewidth is not None:
            list.append(axis+' bar linewidth %f' % self.linewidth)
        return list
class Tick:
    """
    Controls appearence of ticks on an axis.
    
    onoff is either 'on' or 'off'
    major is the space between ticks?
    minorticks is the number of minorticks between major ticks?
    inout determines if they point 'in' or 'out' or 'both'
    majorsize determines how long the major ticks are
    majorlinewidth is how thick the major ticks are
    majorlinestyle is controls the linestle of the ticks and major gridlines
    majorgrid turns the major grid lines 'on' or 'off'
    minorcolor is the color of the minor tick lines
    minorlinewidth
    minorlinestyle controls the linestle of the ticks and minor gridlines
    minorgrid turns the minor gridlines on
    minorsize is the lengthe of the minor gridlines
    op is? it is usually set to 'both'
    type is ? it is usually set to 'auto'
    default is ? a number
    """
    def __init__(self,axis=None,onoff='on',major=None,minorticks=None,inout=None,
                 majorsize=None,majorcolor=None,majorlinewidth=None,majorlinestyle=None,
                 majorgrid=None,minorcolor=None,minorlinewidth=None,minorlinestyle=None,
                 minorgrid=None,minorsize=None,op=None,type=None,default=None,TickLabel=None):
        self.onoff=onoff
        self.major=major
        self.minorticks=minorticks
        self.inout=inout
        self.majorsize=majorsize
        self.majorcolor=majorcolor
        self.majorlinewidth=majorlinewidth
        self.majorlinestyle=majorlinestyle
        self.majorgrid = majorgrid
        self.minorcolor=minorcolor
        self.minorlinewidth=minorlinewidth
        self.minorlinestyle=minorlinestyle
        self.minorgrid=minorgrid
        self.minorsize=minorsize
        self.op=op
        self.type=type
        self.default=default
        self.TickLabel=TickLabel

    def output(self,axis):
        list=[]
        if self.onoff is not None:
            list.append(axis+' tick %s' % self.onoff)
        if self.major is not None:
            list.append(axis+' tick major %d' % self.major)
        if self.minorticks is not None:
            list.append(axis+' tick minor ticks %d' % self.minorticks)
        if self.inout is not None:
            list.append(axis+' tick %s' % self.inout)
        if self.majorsize is not None:
            list.append(axis+' tick major size %f' % self.majorsize)
        if self.majorcolor is not None:
            list.append(axis+' tick major color %d' % self.majorcolor)
        if self.majorlinewidth is not None:
            list.append(axis+' tick major linewidth %f' % self.majorlinewidth)
        if self.majorlinestyle is not None:
            list.append(axis+' tick major linestyle %d' % self.majorlinestyle)
        if self.majorgrid is not None:
            list.append(axis+' tick major grid %s' % self.majorgrid)
        if self.minorcolor is not None:
            list.append(axis+' tick minor color %d' % self.minorcolor)
        if self.minorlinewidth is not None:
            list.append(axis+' tick minor linewidth %f' % self.minorlinewidth)
        if self.minorlinestyle is not None:
            list.append(axis+' tick minor linestyle %d' % self.minorlinestyle)
        if self.minorgrid is not None:
            list.append(axis+' tick minor grid %s' % self.minorgrid)
        if self.minorsize is not None:
            list.append(axis+' tick minor size %f' % self.minorsize)
        if self.op is not None:
            list.append(axis+' tick op %s' % self.op)
        if self.type is not None:
            list.append(axis+' tick type %s' % self.type)
        if self.default is not None:
            list.append(axis+' tick default %s' % self.default)
        if self.TickLabel is not None:
            for i in self.TickLabel.output(axis):
                list.append(i)

        return list
            

class TickLabel:
    def __init__(self,axis=None,onoff='on',type=None,prec=None,format=None,append=None,prepend=None,
                 angle=None,placeon=None,skip=None,stagger=None,op=None,
                 sign=None,starttype=None,start=None,stoptype=None,stop=None,
                 charsize=None,font=None,color=None):
        """
        
        type is ? it is set to 'auto'
        prec is ?
        format is a string ,'decimal' is default
        append is a string that is added to the end of the label
        prepend is a string added to the beginning of the label
        angle is an integer with degrees? of rotation
        placeon is a string, 'ticks'
        skip is an integer which skips some labels somehow
        stagger is an integer that staggers the labels somehow
        op is 'bottom' for x-axis, 'left' for y-axis
        sign is a string 'normal'
        starttype is a string 'auto'
        start is a float don;t know what it does
        stoptype is a string 'auto'
        stop is a float purpose?
        charsize is a float for character size
        font is an integer for the font
        color is an integer for the color
        """
        self.onoff=onoff
        self.type=type
        self.prec=prec
        self.format=format
        self.append=append
        self.prepend=prepend
        self.angle=angle
        self.placeon=placeon
        self.skip=skip
        self.stagger=stagger
        self.op=op
        self.sign=sign
        self.starttype=starttype
        self.start=start
        self.stoptype=stoptype
        self.stop=stop
        self.charsize=charsize
        self.font=font
        self.color=color

    def output(self,axis):
        list=[]
        if self.onoff is not None:
            list.append(axis+' ticklabel %s'% self.onoff)
        if self.type is not None:
            list.append(axis+' ticklabel type %s' % self.type)
        if self.prec is not None:
            list.append(axis+' ticklabel prec %d' % self.prec)
        if self.format is not None:
            list.append(axis+' ticklabel format %s' % self.format)
        if self.append is not None:
            list.append(axis+' ticklabel append "%s"' % self.append)            
        if self.prepend is not None:
            list.append(axis+' ticklabel prepend "%s"' % self.prepend)
        if self.angle is not None:
            list.append(axis+' ticklabel angle %d' % self.angle)
        if self.placeon is not None:
            list.append(axis+' ticklabel place on %s' % self.placeon)
        if self.skip is not None:
            list.append(axis+' ticklabel skip %d' % self.skip)
        if self.stagger is not None:
            list.append(axis+' ticklabel stagger %d' % self.stagger)
        if self.op is not None:
            list.append(axis+' ticklabel op %s' % self.op)
        if self.sign is not None:
            list.append(axis+' ticklabel sign %s' % self.sign)
        if self.starttype is not None:
            list.append(axis+' ticklabel start type %s' % self.starttype)
        if self.start is not None:
            list.append(axis+' ticklabel start %f' % self.start)
        if self.stoptype is not None:
            list.append(axis+' ticklabel stop type %s' % self.stoptype)
        if self.stop is not None:
            list.append(axis+' ticklabel stop %f' % self.stop)
        if self.charsize is not None:
            list.append(axis+' ticklabel char size %f' % self.charsize)
        if self.font is not None:
            list.append(axis+' ticklabel font %d' % self.font)
        if self.color is not None:
            list.append(axis+' ticklabel color %d' % self.color)

        return list



class Annotation:
    def __init__(self,onoff='on',type=None,charsize=None,font=None,
                 color=None,rot=None,format=None,prec=None,prepend=None,
                 append=None,offset=None):
        """
        controls annotation
        onoff is 'on' or 'off', on by default, why else would you make one?
        type is a number? 2

        format is a string, 'general','exponential','decimal','power','scientific','engineering'

        offset must be a tuple
        """
        self.onoff=onoff
        self.type=type
        self.charsize=charsize
        self.font=font
        self.color=color
        self.rot=rot
        self.format=format
        self.prec=prec
        self.prepend=prepend
        self.append=append
        self.offset=offset

    def output(self,dataset):
        list=[]
        if self.onoff is not None:
            list.append(dataset+' avalue %s' % self.onoff)
        if self.type is not None:
            list.append(dataset+' avalue type %d' % self.type)
        if self.charsize is not None:
            list.append(dataset+' avalue char size %f' % self.charsize)
        if self.font is not None:
            list.append(dataset+' avalue font %d' % self.font)
        if self.color is not None:
            list.append(dataset+' avalue color %d' % self.color)
        if self.rot is not None:
            list.append(dataset+' avalue rot %d' % self.rot)
        if self.format is not None:
            list.append(dataset+' avalue format %s' % self.format)
        if self.prec is not None:
            list.append(dataset+' avalue prec %d' % self.prec)
        if self.prepend is not None:
            list.append(dataset+' avalue prepend "%s"' % self.prepend)
        if self.append is not None:
            list.append(dataset+' avalue append "%s"' % self.append)
        if self.offset is not None:
            list.append(dataset+' avalue offset %f , %f' %self.offset)

        return list
class Errorbar:
    """
    onoff turns the error bars on or off, by default if you make an errorbar, they are on.
    place 'normal'
    color
    pattern
    linewidht
    linestyle
    riserlinewidth risers are the lines from the symbol to the end
    riserlinestyle
    riserclip set to on or off, determines if an arrow is drawn for error bars offscale
    risercliplength
    """
    def __init__(self,onoff='on',place=None, color=None, pattern=None,size=None,
                 linewidth=None, linestyle=None, riserlinewidth=None,riserlinestyle=None,
                 riserclip=None,risercliplength=None):
        self.onoff=onoff
        self.place=place
        self.color=color
        self.pattern=pattern
        self.size=size
        self.linewidth=linewidth
        self.linestyle=linestyle
        self.riserlinewidth=riserlinewidth
        self.riserlinestyle=riserlinestyle
        self.riserclip=riserclip
        self.risercliplength=risercliplength

    def output(self,symbol):
        list=[]
        list.append('%s errorbar %s' % (symbol,self.onoff))
        if self.place is not None:
            list.append('%s errorbar place %s' %(symbol,self.place))
        if self.color is not None:
            list.append('%s errorbar color %d' %(symbol,self.color))
        if self.pattern is not None:
            list.append('%s errorbar pattern %d' %(symbol,self.pattern))
        if self.size is not None:
            list.append('%s errorbar size %f' %(symbol,self.size))
        if self.linewidth is not None:
            list.append('%s errorbar linewidth %f' %(symbol,self.linewidth))
        if self.linestyle is not None:
            list.append('%s errorbar linestyle %d' %(symbol,self.linestyle))
        if self.riserlinewidth is not None:
            list.append('%s errorbar riser linewidth %f' %(symbol,self.riserlinewidth))
        if self.riserlinestyle is not None:
            list.append('%s errorbar riser linestyle %d' %(symbol,self.riserlinestyle))
        if self.riserclip is not None:
            list.append('%s errorbar riser clip %s' %(symbol,self.riserclip))
        if self.risercliplength is not None:
            list.append('%s errorbar riser clip length %f' %(symbol,self.risercliplength))

        return list
