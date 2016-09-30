import time
#import pyupm_grovemd as upmGrovemd
import cmd
from math import sqrt
from xml.dom import minidom
import re
import traceback
import numpy as np
import math
import svg.path
from svg.path import parse_path

def binomial(i, n):
    """Binomial coefficient"""
    return math.factorial(n) / float(
        math.factorial(i) * math.factorial(n - i))


def bernstein(t, i, n):
    """Bernstein polynom"""
    return binomial(i, n) * (t ** i) * ((1 - t) ** (n - i))


def bezier(t, points):
    """Calculate coordinate of a point in the bezier curve"""
    n = len(points) - 1
    x = y = 0
    for i, pos in enumerate(points):
        bern = bernstein(t, i, n)
        x += pos[0] * bern
        y += pos[1] * bern
    return x, y


def bezier_curve_range(n, points):
    """Range of points in a curve bezier"""
    for i in xrange(n):
        t = i / float(n - 1)
        yield bezier(t, points)

class svgHandler:
  
  def __init__(self):
    self.filename = ""
    self.lines = [] #list of segments of the form [x0 y0 x1 y1 RGB] where x and y are 0-1 floats
  
  def importFile(self,filename):
    doc = minidom.parse(filename)  # parseString also exists
    paths = doc.getElementsByTagName('path')
    pathsandcolours = [] 
    self.lines = []
    for p in paths:
        commands = parse_path(p.getAttribute('d'))
        for c in commands:
            #print type(c)
            if 'Line' in str(type(c)):
                self.lines.append((c.start.real,c.start.imag,c.end.real,c.end.imag))
            elif 'Cubic' in str(type(c)):
                #print dir(c)
                #print c.a
                controlPoints = [ (c.start.real,c.start.imag), (c.control1.real,c.control1.imag),(c.control2.real,c.control2.imag),(c.end.real,c.end.imag)]
                firstLine = False
                start_x,start_y = (0,0)
                for point in bezier_curve_range(10, controlPoints):
                    if not firstLine: 
                        start_x,start_y = point 
                        firstLine = True
                    else:
                        p_x,p_y = point    
#                       unScaledLines.append([start_x,start_y,p_x,p_y])
                        self.lines.append([start_x,start_y,p_x,p_y])
                        start_x = p_x
                        start_y = p_y
    return
    """
    Go through the paths and store their coordinate strings and colours in a list of tuples
    """
    for p in paths:
      
      #styleValues =  p.attributes['style'].value.split(";")
      #for val in styleValues:
      #  if val.split(":")[0] == 'stroke':
      #    rgb = val.split(":")[1]     
      pathsandcolours.append((p.getAttribute('d')))

    """
    For each path and its corresponding colour, convert to a set of line segments of the form [x0 y0 x1 y1 RGB]
    """
    xMin = float("inf")
    xMax = -float("inf")
    yMin = float("inf")
    yMax = -float("inf")
    unScaledLines = []
    
    for path in pathsandcolours:
      pathCoords = re.split(r'([CcLlMmZz])',path)
      print pathCoords
      startX = 0
      startY = 0
      lastX = 0
      lastY = 0
      newLine = True
      closeLine = False
      cubicB = False
      #we are assuming absolute coordinates in the SVG
      
      for coord in pathCoords:
        coord = coord.strip()
        if len(coord) == 0:
          pass
        elif coord == 'M': #'M' is move cursor to absolute position
          newLine = True
        elif coord == 'L': #'L' is draw line to absolute position
          newLine = False
        elif coord == 'Z' or coord.strip() == 'z': #close line to initial point
          closeLine = True
          print 'close'
          lastX = x
          lastY = y
          startX = start_poly_x
          startY = start_poly_y
          #unScaledLines.append([lastX,lastY,startX,startY])
          self.lines.append([lastX,lastY,startX,startY])
        elif coord =='C':
          cubicB = True
          print 'cubicB'
        else:
          try:
            if not cubicB:
                x = float(re.split(r'[\s,]',coord)[0])
                y = float(re.split(r'[\s,]',coord)[1])
                xMin = min(xMin,x)
                yMin = min(yMin,y)
                xMax = max(xMax,x)
                yMax = max(yMax,y)

            if cubicB == True:
                if ',' in coord:
                    (p1,p2,p3) = coord.split(' ')
                    print p1,p2,p3
                    controlPoints = (np.array(p1.split(',')).astype('float').tolist(),np.array(p2.split(',')).astype('float').tolist(),np.array(p3.split(',')).astype('float').tolist())
                    print controlPoints
                else:
                    controlPoints = []
                    coords = coord.split(' ')
                    for i in range(len(coords)/2):
                        controlPoints.append( (float(coords[i*2]),float(coords[i*2+1])))
                
                firstLine = False
                start_x,start_y = (0,0)
                for point in bezier_curve_range(10, controlPoints):
                    if not firstLine: 
                        start_x,start_y = point 
                        firstLine = True
                    else:
                        p_x,p_y = point    
#                       unScaledLines.append([start_x,start_y,p_x,p_y])
                        self.lines.append([start_x,start_y,p_x,p_y])
                        start_x = p_x
                        start_y = p_y
            if closeLine:
              print 'close'
              lastX = x
              lastY = y
              startX = start_poly_x
              startY = start_poly_y
              #unScaledLines.append([lastX,lastY,startX,startY])
              self.lines.append([lastX,lastY,startX,startY])
            elif newLine:
              lastX = x
              lastY = y
              startX = x
              startY = y
              start_poly_x = x
              start_poly_y = y
            else:
              self.lines.append([lastX,lastY,x,y])
              #unScaledLines.append([lastX,lastY,x,y])
              #self.lines.append([lastX,lastY,x,y,rgb])
              lastX = x
              lastY = y

          except:
            print "unhandled command: ",coord
            print "Exception in user code:"
            print '-'*60
            traceback.print_exc(file=sys.stdout)
            print '-'*60        
      #print "xMin,yMin",xMin,yMin
      #print "xMax,yMax",xMax,yMax        
        if not cubicB:
            xTotal = xMax-xMin
            yTotal = yMax-yMin
    
        
#        for x0,y0,x1,y1 in unScaledLines:
#          X0normalised = (x0-xMin)/xTotal
#          X1normalised = (x1-xMin)/xTotal
#          Y0normalised = (y0-yMin)/yTotal
#          Y1normalised = (y1-yMin)/yTotal
#         self.lines.append([X0normalised,Y0normalised,X1normalised,Y1normalised])
    doc.unlink()
    
  def plotPath(self):
    import matplotlib.pyplot as plt
    min_x = -0.15
    max_x = 0.15
    min_y = -0.1
    max_y = 0.20
    fd = open('output.gcode','w')
    plt.gca().invert_yaxis()
    print len(self.lines)
    prev_x = 0
    prev_y = 0
    
    min_x_px = 1000
    max_x_px = -1000
    min_y_px = 1000
    max_y_px = -1000
    for x0,y0,x1,y1 in self.lines:
        min_x_px = min(min_x_px, x0)
        min_x_px = min(min_x_px, x1)
        min_y_px = min(min_y_px, y0)
        min_y_px = min(min_y_px, y1)
        max_x_px = max(max_x_px, x0)
        max_x_px = max(max_x_px, x1)
        max_y_px = max(max_y_px, y0)
        max_y_px = max(max_y_px, y1)
    
    self.lines.append((max_x_px,max_y_px,max_x_px-200,max_y_px))
    self.lines.append((max_x_px-200,max_y_px,max_x_px-200,max_y_px-200))
    self.lines.append((max_x_px-200,max_y_px-200,max_x_px,max_y_px-200))
    self.lines.append((max_x_px,max_y_px-200,max_x_px,max_y_px))
    
    for x0,y0,x1,y1 in self.lines:
        norm_x0 = (x0 - min_x_px)/(max_x_px - min_x_px) * (max_x - min_x) + min_x 
        norm_y0 = (y0 - min_y_px)/(max_y_px - min_y_px) * (max_y - min_y) + min_y 
        norm_x1 = (x1 - min_x_px)/(max_x_px - min_x_px) * (max_x - min_x) + min_x 
        norm_y1 = (y1 - min_y_px)/(max_y_px - min_y_px) * (max_y - min_y) + min_y 
        
        if prev_x != x0 or prev_y != y0:
            print >> fd,""
            print >> fd,"%s,%s"%(str(-norm_x0),str(norm_y0))
            plt.plot([norm_x0,norm_x0],[-norm_y0,-norm_y0],marker='o',markersize=8)
        plt.plot([norm_x0,norm_x1],[-norm_y0,-norm_y1],color='0.75')
        print >> fd,"%s,%s"%(str(-norm_x1),str(norm_y1))
        prev_x = x1
        prev_y = y1
      
    #print >> fd,""
    #print >> fd,"%s,%s"%(str(min_x),str(max_y))
    #print >> fd,"%s,%s"%(str(min_x+0.05),str(max_y))
    #print >> fd,"%s,%s"%(str(min_x+0.05),str(max_y-0.05))
    #print >> fd,"%s,%s"%(str(min_x),str(max_y-0.05))
    #print >> fd,"%s,%s"%(str(min_x),str(max_y))
    fd.close()
    plt.savefig(self.filename.split('.')[0]+'.png')
    plt.show()
    
    
    
import sys
svgh = svgHandler()
svgh.importFile(sys.argv[1])
svgh.plotPath()