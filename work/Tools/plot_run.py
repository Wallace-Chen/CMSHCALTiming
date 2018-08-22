import ROOT as r
import sys
import commands
import time
from scipy import stats

_YaverageChargeTiming = [2,4]
_YrecHitTiming = [-10,10]
start = time.time()
# 18 color options
_color = [2,3,4,5,6,7,8,9,11,12,15,24,29,30,38,40,41,46]
#color options:
HEADER   = '\033[95m'
BLUE     = '\033[94m'
GREEN    = '\033[92m'
WARNING  = '\033[93m'
FAIL     = '\033[91m'
BOLD     = '\033[1m'
UNDERLINE= '\033[4m'
END      = '\033[0m'


r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

infile0 = sys.argv[1]

status,infile = commands.getstatusoutput("echo " + infile0 + " | sed 's/\.[^.]*$//'")

f1 = r.TFile(infile0)
if (f1.IsZombie() or (not f1.IsOpen())):
  print FAIL + "Error: cannot open " + infile0 + " or the file is not valid,please check if filename is valid!" + END
  exit()
  
print HEADER + "Open " + infile0 + " successfully, now plotting..." + END

r.gStyle.SetTitleFontSize(.06)
r.gStyle.SetTitleXSize(.05)
r.gStyle.SetTitleYSize(.05)
#r.gStyle.SetPadBottomMargin(.13)
#r.gStyle.SetPadLeftMargin(.12)
r.gStyle.SetPadRightMargin(.18)
#r.gStyle.SetPadTopMargin(.1)

#----------------------------------------------------------------------------------------------------------------
#User Defined Function
#----------------------------------------------------------------------------------------------------------------
def getAverage(hist):
  binNum = (hist.GetXaxis().GetXmax() - hist.GetXaxis().GetXmin()) / hist.GetXaxis().GetBinWidth(0)
  _list = []
  for i in range(0,int(binNum)):
    if hist.GetBinContent(i) != 0:
      _list.append(hist.GetBinContent(i))
  length = len(_list)

  average = sum(_list)/float(length)
  print "Total # of the list is: " + str(length) + ", the average is: " + str(average)
  return average

def linReg(hist):
  binNum = (hist.GetXaxis().GetXmax() - hist.GetXaxis().GetXmin()) / hist.GetXaxis().GetBinWidth(0)
  _list = []
  _bin  = []
  for i in range(0,int(binNum)):
    if hist.GetBinContent(i) != 0:
      _bin.append(i)
      _list.append(hist.GetBinContent(i))
  length = len(_list)
  slope, intercept, r_value, p_value, std_err = stats.linregress(_bin,_list)

  print "Total # of the list is: " + str(length) + ", the slope is: " + str(slope)
  return slope, intercept

def getLine(y,hist,style=7,color=r.kBlack):
  line = r.TLine(hist.GetXaxis().GetXmin(),y,hist.GetXaxis().GetXmax(),y)
  line.SetLineStyle(style)
  line.SetLineColor(color)
  return line

def getRun(hist):
  binNum = (hist.GetXaxis().GetXmax() - hist.GetXaxis().GetXmin()) / hist.GetXaxis().GetBinWidth(0)
  _list = []
  for i in range(0,int(binNum)):
    if hist.GetBinContent(i) != 0:
      _list.append(int(hist.GetXaxis().GetXmin()) + i)
  length = len(_list)
  print "Test purpose: the first run number is : " + str(hist.GetXaxis().GetXmin()) 
  print GREEN + BOLD + "There are totally: " + str(length) + " runs, there are:\n"
  print _list
  print END
 
#def getText(ave,ymin,ymax):
#  x0 = 0.1
#  y0 = (ave-ymin)/(ymax-ymin)
# # Text = r.TText(x0,y0,str(ave))
#  Text = r.TText(0.5,0.5,"Hello")
#  #Text.SetTextColor(r.kBlack)
#  return Text

#----------------------------------------------------------------------------------------------------------------
#Main Function
#----------------------------------------------------------------------------------------------------------------

canvas = r.TCanvas()
outfile = infile + "_HB_timing_run.pdf"
canvas.SaveAs(outfile + '[')

HBHE_run = "HBHE_run"
HBHE_run = f1.Get(HBHE_run)
HBHE_run.Draw()
HBHE_run.SetTitle("Run Range;Run number;")
canvas.SaveAs(outfile)


HBHE_chargeAveragedTiming_vs_run = "HBHE_chargeAveragedTiming_vs_run"
HBHE_chargeAveragedTiming_vs_run = f1.Get(HBHE_chargeAveragedTiming_vs_run)
HBHE_chargeAveragedTiming_vs_run.Draw("AXIS")
#HBHE_chargeAveragedTiming_vs_run.Draw("colz")
#HBHE_chargeAveragedTiming_vs_run.SetTitle("HB Charge Averaged Timing vs Run;Run number;Timing [TS]")

HBHE_chargeAveragedTiming_vs_run_p = HBHE_chargeAveragedTiming_vs_run.ProfileX()
HBHE_chargeAveragedTiming_vs_run_p.SetLineColor(r.kBlack)
HBHE_chargeAveragedTiming_vs_run_p.SetMarkerStyle(15)
HBHE_chargeAveragedTiming_vs_run_p.SetAxisRange(_YaverageChargeTiming[0],_YaverageChargeTiming[1],"Y")
HBHE_chargeAveragedTiming_vs_run_p.Draw("same")
ave = getAverage(HBHE_chargeAveragedTiming_vs_run_p)
slope, intercept = linReg(HBHE_chargeAveragedTiming_vs_run_p)

HBHE_chargeAveragedTiming_vs_run.SetTitle("HB Charge Averaged Timing vs Run;Run number;Timing [TS] "+str(ave))
line = getLine(ave,HBHE_chargeAveragedTiming_vs_run_p)
line.Draw("same")
canvas.SaveAs(outfile)


HBHE_RecTiming_vs_run = "HBHE_RecTiming_vs_run"
HBHE_RecTiming_vs_run = f1.Get(HBHE_RecTiming_vs_run)
HBHE_RecTiming_vs_run.Draw("AXIS")
#HBHE_RecTiming_vs_run.Draw("colz")
#HBHE_RecTiming_vs_run.SetTitle("HB RecHit Timing vs Run;Run number;Timing [ns]")

HBHE_RecTiming_vs_run_p = HBHE_RecTiming_vs_run.ProfileX()
HBHE_RecTiming_vs_run_p.SetLineColor(r.kBlack)
HBHE_RecTiming_vs_run_p.SetMarkerStyle(15)
HBHE_RecTiming_vs_run_p.Draw("same")

line = getLine(0,HBHE_RecTiming_vs_run_p,1)
line.Draw("same")
ave1 = getAverage(HBHE_RecTiming_vs_run_p)
HBHE_RecTiming_vs_run.SetTitle("HB RecHit Timing vs Run;Run number;Timing [ns] "+str(ave1))
line1 = getLine(ave1,HBHE_RecTiming_vs_run_p)
line1.Draw("same")
canvas.SaveAs(outfile)

#---------------------------------------------------------------------------------------
# Plot for Timing vs Run for different ietas
#---------------------------------------------------------------------------------------
# for charge averaged timing
color  = 0
marker = 20
leg_chargeAveragedTiming = r.TLegend(0.1,0.8,0.6,0.9)
leg_chargeAveragedTiming.SetNColumns(4)
for eta in range(0,16):
  HBHE_chargeAveragedTiming_vs_run_x = "HBHE_chargeAveragedTiming_vs_run abs(IEta) " + str(eta+1)
  HBHE_chargeAveragedTiming_vs_run = f1.Get(HBHE_chargeAveragedTiming_vs_run_x)
  if HBHE_chargeAveragedTiming_vs_run:
    if eta == 0:
      HBHE_chargeAveragedTiming_vs_run.SetAxisRange(_YaverageChargeTiming[0],_YaverageChargeTiming[1],"Y")
      HBHE_chargeAveragedTiming_vs_run.Draw("AXIS")
      HBHE_chargeAveragedTiming_vs_run.SetTitle("HB Charge Averaged Timing vs Run;Run number;Timing [TS] "+str(ave))

    HBHE_chargeAveragedTiming_vs_run_p = HBHE_chargeAveragedTiming_vs_run.ProfileX()
    HBHE_chargeAveragedTiming_vs_run_p.SetMarkerColor(_color[color])
    HBHE_chargeAveragedTiming_vs_run_p.SetLineColor(_color[color])
    HBHE_chargeAveragedTiming_vs_run_p.SetMarkerStyle(3+((eta+1)%2)*4)
    HBHE_chargeAveragedTiming_vs_run_p.Draw("same")
    leg_chargeAveragedTiming.AddEntry(HBHE_chargeAveragedTiming_vs_run_p,"abs(IEta)={}".format(eta+1))
    color = color + 1
    #marker = marker + 1
leg_chargeAveragedTiming.Draw("same")
line = getLine(ave,HBHE_chargeAveragedTiming_vs_run_p)
line.Draw("same")
canvas.SaveAs(outfile)

# for rechit timing
color  = 0
marker = 20
leg_RecTiming = r.TLegend(0.1,0.8,0.6,0.9)
leg_RecTiming.SetNColumns(4)
for eta in range(0,16):
  HBHE_RecTiming_vs_run_x = "HBHE_RecTiming_vs_run abs(IEta) " + str(eta+1)
  HBHE_RecTiming_vs_run = f1.Get(HBHE_RecTiming_vs_run_x)
  if HBHE_RecTiming_vs_run:
    if eta == 0:
      HBHE_RecTiming_vs_run.Draw("AXIS")
      HBHE_RecTiming_vs_run.SetTitle("HB RecHit Timing vs Run;Run number;Timing [ns] "+str(ave1))
      #line = r.TLine(HBHE_RecTiming_vs_run.GetXaxis().GetXmin(),0,HBHE_RecTiming_vs_run.GetXaxis().GetXmax(),0)
      #line.SetLineStyle(7)
      #line.SetLineColor(r.kBlack)
      #line.Draw("same")
    
    HBHE_RecTiming_vs_run_p = HBHE_RecTiming_vs_run.ProfileX()
    HBHE_RecTiming_vs_run_p.SetMarkerColor(_color[color])
    HBHE_RecTiming_vs_run_p.SetLineColor(_color[color])
    HBHE_RecTiming_vs_run_p.SetMarkerStyle(3+((eta+1)%2)*4)
    HBHE_RecTiming_vs_run_p.Draw("same")
    leg_RecTiming.AddEntry(HBHE_RecTiming_vs_run_p,"abs(IEta)={}".format(eta+1))
    color = color + 1
    #marker = marker + 1

    line1 = getLine(0,HBHE_RecTiming_vs_run_p,1)
    line1.Draw("same")
    line = getLine(ave1,HBHE_RecTiming_vs_run_p)
    line.Draw("same")
leg_RecTiming.Draw("same")
canvas.SaveAs(outfile)

for eta in range(0,16):
  HBHE_chargeAveragedTiming_vs_run_x = "HBHE_chargeAveragedTiming_vs_run abs(IEta) " + str(eta+1)
  HBHE_chargeAveragedTiming_vs_run = f1.Get(HBHE_chargeAveragedTiming_vs_run_x)
  if HBHE_chargeAveragedTiming_vs_run:
   # HBHE_chargeAveragedTiming_vs_run.Draw("colz")
    HBHE_chargeAveragedTiming_vs_run.SetAxisRange(_YaverageChargeTiming[0],_YaverageChargeTiming[1],"Y")
    HBHE_chargeAveragedTiming_vs_run.Draw("AXIS")
    #HBHE_chargeAveragedTiming_vs_run.SetTitle("HB Charge Averaged Timing vs Run abs(IEta)={};Run number;Timing [TS]".format(eta+1))
    
    HBHE_chargeAveragedTiming_vs_run_p = HBHE_chargeAveragedTiming_vs_run.ProfileX()
    HBHE_chargeAveragedTiming_vs_run_p.SetLineColor(r.kBlack)
    HBHE_chargeAveragedTiming_vs_run_p.SetMarkerStyle(15)
    HBHE_chargeAveragedTiming_vs_run_p.Draw("same")
    ave = getAverage(HBHE_chargeAveragedTiming_vs_run_p)
    HBHE_chargeAveragedTiming_vs_run.SetTitle("HB Charge Averaged Timing vs Run abs(IEta)={};Run number;Timing [TS] ".format(eta+1)+str(ave))
    line = getLine(ave,HBHE_chargeAveragedTiming_vs_run_p)
    line.Draw("same")
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_chargeAveragedTiming_vs_run_x + END


  HBHE_RecTiming_vs_run_x = "HBHE_RecTiming_vs_run abs(IEta) " + str(eta+1)
  HBHE_RecTiming_vs_run = f1.Get(HBHE_RecTiming_vs_run_x)
  if HBHE_RecTiming_vs_run:
    #HBHE_RecTiming_vs_run.Draw("colz")
    HBHE_RecTiming_vs_run.Draw("AXIS")
    #HBHE_RecTiming_vs_run.SetTitle("HB RecHit Timing vs Run abs(IEta)={};Run number;Timing [ns]".format(eta+1))
    
    HBHE_RecTiming_vs_run_p = HBHE_RecTiming_vs_run.ProfileX()
    HBHE_RecTiming_vs_run_p.SetLineColor(r.kBlack)
    HBHE_RecTiming_vs_run_p.SetMarkerStyle(15)
    HBHE_RecTiming_vs_run_p.Draw("same")

    line1 = getLine(0,HBHE_RecTiming_vs_run_p,1)
    line1.Draw("same")
    ave = getAverage(HBHE_RecTiming_vs_run_p)
    HBHE_RecTiming_vs_run.SetTitle("HB RecHit Timing vs Run abs(IEta)={};Run number;Timing [ns] ".format(eta+1)+str(ave))
    line = getLine(ave,HBHE_RecTiming_vs_run_p)
    line.Draw("same")
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_RecTiming_vs_run_x + END


canvas.SaveAs(outfile + ']')
print GREEN + outfile + " dumped" + END

elapsed = time.time() - start
print GREEN +  "Done, time used is: " + str(elapsed) + " secs..." + END

getRun(HBHE_run) 
