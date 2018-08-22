import ROOT as r
import sys
import commands
import time

start = time.time()

#------Please Do not edit, lists will be added after executing the ./main
#_phi = [0,7,23,25,52,53,66]
_phi = [0,14,32,50]
#------Please Do not edit before this line
output = []
#color options:
HEADER   = '\033[95m'
BLUE     = '\033[94m'
GREEN    = '\033[92m'
WARNING  = '\033[93m'
FAIL     = '\033[91m'
BOLD     = '\033[1m'
UNDERLINE= '\033[4m'
END      = '\033[0m'

energy_threshold = 30
timing_threshold = -7.65

maxeta=17
minieta=-16
maxphi=2
miniphi=1
maxdepth=3
minidepth=1
nmethod=3

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

#--------------------------------------------------------User defined functiond------------------------------------------------------------#
def compareHists(hist1,hist2,adjust=0):#adjust is to adjust the difference between the bin number and the bin physical meaning
  #binNum1 = (hist1.GetXaxis().GetXmax() - hist1.GetXaxis().GetXmin()) / hist1.GetXaxis().GetBinWidth(0)
  binNum1 = hist1.GetNbinsX()
  #binNum2 = (hist2.GetXaxis().GetXmax() - hist2.GetXaxis().GetXmin()) / hist2.GetXaxis().GetBinWidth(0)
  binNum2 = hist2.GetNbinsX()
  name1 = hist1.GetName()
  name2 = hist2.GetName()
  _list1 = []
  _list2 = []
  _list1not2 = []
  _list2not1 = []
  for i in range(0,int(binNum1)):
    #if hist1.GetBinContent(i) != 0:
    if hist1.GetBinContent(i) > 0:
      _list1.append(int(i)+adjust)
  for i in range(0,int(binNum2)):
    #if hist2.GetBinContent(i) != 0:
    if hist2.GetBinContent(i) > 0:
      _list2.append(int(i)+adjust)
      if not (i+adjust) in _list1:
        _list2not1.append(int(i)+adjust)
  for i in _list1:
    if not i in _list2:
      _list1not2.append(i)

#  print "Elements in " + name1 + " are: "
#  print _list1
#  print "Elements in " + name2 + " are: "
#  print _list2
  print "Test Purpose, the first element is: " + str(_list1[0])
  print "In " + name1 + " not in " + name2 + " is: "
  print _list1not2
  print "In " + name2 + " not in " + name1 + " is: "
  print _list2not1


#--------------------------------------------------------Making plots for General Information----------------------------------------------#
canvas = r.TCanvas()
outfile = infile + "_HB_timing_M0_check.pdf"
canvas.SaveAs(outfile + '[')


r.gPad.SetLogy(r.kFALSE)
r.gPad.SetGridx(r.kTRUE)
r.gPad.SetGridy(r.kTRUE)

#plots for timing in eta-phi plane
HBHE_SecondPeakMap_nEvt = "HBHE_SecondPeakMap_nEvt"
HBHE_NormalMap_nEvt = "HBHE_NormalMap_nEvt"
HBHE_bxDis_SecondPeak = "HBHE_bxDis_SecondPeak"
HBHE_bxDis_Normal = "HBHE_bxDis_Normal"
HBHE_lumiDis_SecondPeak = "HBHE_lumiDis_SecondPeak"
HBHE_lumiDis_Normal = "HBHE_lumiDis_Normal"

HBHE_SecondPeakMap_nEvt = f1.Get(HBHE_SecondPeakMap_nEvt)
HBHE_NormalMap_nEvt = f1.Get(HBHE_NormalMap_nEvt)
HBHE_bxDis_SecondPeak = f1.Get(HBHE_bxDis_SecondPeak)
HBHE_bxDis_Normal = f1.Get(HBHE_bxDis_Normal)
HBHE_lumiDis_SecondPeak = f1.Get(HBHE_lumiDis_SecondPeak)
HBHE_lumiDis_Normal = f1.Get(HBHE_lumiDis_Normal)

#-------------Compare unhealth and health events START---------------------------------------------------------------------
r.gPad.SetLogx(r.kFALSE)
HBHE_SecondPeakMap_nEvt.Draw("colz")
HBHE_SecondPeakMap_nEvt.SetTitle("HB Second Peak Distribution;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_NormalMap_nEvt.Draw("colz")
HBHE_NormalMap_nEvt.SetTitle("HB Normal Events Distribution;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_bxDis_SecondPeak.Draw("PL")
HBHE_bxDis_SecondPeak.SetTitle("HB Second Peak BX Distribution;BX;Hits")
canvas.SaveAs(outfile)

HBHE_bxDis_Normal.Draw("PL")
HBHE_bxDis_Normal.SetTitle("HB Normal Events BX Distribution;BX;Hits")
canvas.SaveAs(outfile)

print BLUE + "Now will compare BX distribution:" + END
compareHists(HBHE_bxDis_SecondPeak,HBHE_bxDis_Normal,-1)

HBHE_lumiDis_SecondPeak.Draw("PL")
HBHE_lumiDis_SecondPeak.SetTitle("HB Second Peak Lumi Distribution;LS;Hits")
canvas.SaveAs(outfile)

HBHE_lumiDis_Normal.Draw("PL")
HBHE_lumiDis_Normal.SetTitle("HB Normal Events Lumi Distribution;LS;Hits")
canvas.SaveAs(outfile)

print BLUE + "Now will compare lumi distribution:" + END
compareHists(HBHE_lumiDis_SecondPeak,HBHE_lumiDis_Normal)
#--------------Compare unhealth and health events END----------------------------------------------------------------
for method in range(1,nmethod):
  if method == 0:
    Timing = "HB Charge-Averaged Timing"
    Energy = "Digi Energy"
  elif method == 1:
    Timing = "HB RecHit Timing"
    Energy = "RecHit Energy"
  elif method == 2:
    Timing = "HB RecHit Timing M0"
    Energy = "RecHit Energy M0"

  #plot for check channels and compare channels:
  HBHE_Timing_vs_Energy_check = "HBHE_Timing_vs_Energy check channels " + str(method)
  HBHE_Timing_vs_Energy_check_p = f1.Get(HBHE_Timing_vs_Energy_check)
  if HBHE_Timing_vs_Energy_check_p:
    HBHE_Timing_vs_Energy_check_p.Draw("colz")
    HBHE_Timing_vs_Energy_check_p.SetTitle(Timing + " vs " + Energy + " for Specific Channels 1;"+Energy+";"+Timing)
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_check + END
  
  for num in range(1,3):
    HBHE_Timing_vs_Energy_compare = "HBHE_Timing_vs_Energy compare channels " + str(num) + " " + str(method)
    HBHE_Timing_vs_Energy_compare_p = f1.Get(HBHE_Timing_vs_Energy_compare)
    if HBHE_Timing_vs_Energy_compare_p:
      HBHE_Timing_vs_Energy_compare_p.Draw("colz")
      HBHE_Timing_vs_Energy_compare_p.SetTitle(Timing + " vs " + Energy + "for Specific Channels {};".format(num+1)+Energy+";"+Timing)
      canvas.SaveAs(outfile)
    else:
      print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_compare + END


r.gPad.SetLogx(r.kFALSE)
r.gPad.SetGridx(r.kFALSE)
r.gPad.SetGridy(r.kFALSE)

canvas.SaveAs(outfile + ']')
print GREEN + outfile + " dumped" + END
output.append(outfile)

elapsed = time.time() - start
print GREEN +  "General Information part is done, time used is: " + str(elapsed) + " secs..." + END
exit()
