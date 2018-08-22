import ROOT as r
import sys
import commands
import time

start = time.time()

#------Please Do not edit, lists will be added after executing the ./main
#_phi = [0,7,23,25,52,53,66]
#_phi = [0,14,32,50]
_phi = [0,20,23,31,60]
#_phi = [0,20,23,31,40,60,70]
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
timing_threshold = -2

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
outfile = infile + "_HB_timing_general.pdf"
canvas.SaveAs(outfile + '[')


HBHE_PulseShape_ALL = "HBHE_PulseShape_ALL"
HBHE_PulseShape_ALL = f1.Get(HBHE_PulseShape_ALL)
HBHE_PulseShape_ALL.Draw()
HBHE_PulseShape_ALL.SetTitle("HB Pulse Charge ALL Channels;TS;fC")
canvas.SaveAs(outfile)

# Plot for totalCharge vs IEta
r.gPad.SetLogy(r.kTRUE)
r.gPad.SetLogz(r.kTRUE)
HBHE_totalCharge_vs_iEta_ALL = "HBHE_totalCharge_vs_iEta_ALL"
HBHE_totalCharge_vs_iEta_ALL = f1.Get(HBHE_totalCharge_vs_iEta_ALL)
HBHE_totalCharge_vs_iEta_ALL.Draw("colz");
HBHE_totalCharge_vs_iEta_ALL.SetTitle("HB Total Charge vs IEta ALL Channels;IEta;Charge")

HBHE_totalCharge_vs_iEta_ALL_p = HBHE_totalCharge_vs_iEta_ALL.ProfileX()
HBHE_totalCharge_vs_iEta_ALL_p.SetLineColor(r.kBlack)
HBHE_totalCharge_vs_iEta_ALL_p.SetMarkerStyle(15)
HBHE_totalCharge_vs_iEta_ALL_p.Draw("same")
canvas.SaveAs(outfile)

#r.gPad.SetLogx(r.kTRUE)
r.gPad.SetLogy(r.kFALSE)
r.gPad.SetGridx(r.kTRUE)
r.gPad.SetGridy(r.kTRUE)

#plots for timing in eta-phi plane
HBHE_TSRatioMap_nEvt_Dx = "HBHE_TSRatioMap_nEvt"
HBHE_TSRatioMap_Avg_Dx = "HBHE_TSRatioMap_Ave"
HBHE_TSRatioMap_Avg_fC1_Dx = "HBHE_TSRatioMap_Ave_fC1"
HBHE_TSRatioMap_Avg_fC2_Dx = "HBHE_TSRatioMap_Ave_fC2"
HBHE_TSRatioMap_Avg_fC3_Dx = "HBHE_TSRatioMap_Ave_fC3"
HBHE_TSRatioMap_Avg_fC4_Dx = "HBHE_TSRatioMap_Ave_fC4"
HBHE_EnerWeiTiming_Dx = "HBHE_EnerWeiTiming"

#HBHE_bxDis_SecondPeak = "HBHE_bxDis_SecondPeak"
#HBHE_bxDis_Normal = "HBHE_bxDis_Normal"
#HBHE_lumiDis_SecondPeak = "HBHE_lumiDis_SecondPeak"
#HBHE_lumiDis_Normal = "HBHE_lumiDis_Normal"

HBHE_TSRatioMap_nEvt_D = f1.Get(HBHE_TSRatioMap_nEvt_Dx)
HBHE_TSRatioMap_Avg_D = f1.Get(HBHE_TSRatioMap_Avg_Dx)
HBHE_TSRatioMap_Avg_fC1_D = f1.Get(HBHE_TSRatioMap_Avg_fC1_Dx)
HBHE_TSRatioMap_Avg_fC2_D = f1.Get(HBHE_TSRatioMap_Avg_fC2_Dx)
HBHE_TSRatioMap_Avg_fC3_D = f1.Get(HBHE_TSRatioMap_Avg_fC3_Dx)
HBHE_TSRatioMap_Avg_fC4_D = f1.Get(HBHE_TSRatioMap_Avg_fC4_Dx)

#HBHE_bxDis_SecondPeak = f1.Get(HBHE_bxDis_SecondPeak)
#HBHE_bxDis_Normal = f1.Get(HBHE_bxDis_Normal)
#HBHE_lumiDis_SecondPeak = f1.Get(HBHE_lumiDis_SecondPeak)
#HBHE_lumiDis_Normal = f1.Get(HBHE_lumiDis_Normal)

HBHE_TSRatioMap_nEvt_D.Draw("colz")
HBHE_TSRatioMap_nEvt_D.SetTitle("HB nHits"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_D = HBHE_TSRatioMap_Avg_D.Project3DProfile("yx")
HBHE_TSRatioMap_Avg_D.Draw("colz")
HBHE_TSRatioMap_Avg_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC1_D = HBHE_TSRatioMap_Avg_fC1_D.Project3DProfile("yx")
HBHE_TSRatioMap_Avg_fC1_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC1_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 100fC>TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC2_D = HBHE_TSRatioMap_Avg_fC2_D.Project3DProfile("yx")
HBHE_TSRatioMap_Avg_fC2_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC2_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 200fC>TS3>100fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC3_D = HBHE_TSRatioMap_Avg_fC3_D.Project3DProfile("yx")
HBHE_TSRatioMap_Avg_fC3_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC3_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 600fC>TS3>200fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC4_D = HBHE_TSRatioMap_Avg_fC4_D.Project3DProfile("yx")
HBHE_TSRatioMap_Avg_fC4_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC4_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 2000fC>TS3>600fC;IEta;IPhi")
canvas.SaveAs(outfile)

r.gPad.SetLogx(r.kTRUE)
r.gPad.SetLogz(r.kFALSE)
HBHE_Timing_vs_Energy_M0_from_MAHI_ = "HBHE_Timing_vs_Energy_M0_from_MAHI"
HBHE_Timing_vs_Energy_M0_from_MAHI = f1.Get(HBHE_Timing_vs_Energy_M0_from_MAHI_)
if HBHE_Timing_vs_Energy_M0_from_MAHI:
  HBHE_Timing_vs_Energy_M0_from_MAHI.Draw("colz")
  HBHE_Timing_vs_Energy_M0_from_MAHI.SetTitle("HB M0 Timings vs Energy for second peak in MAHI Timing;Energy;Timing")
  
  #l1 = r.TLine(30,-2,HBHE_Timing_vs_Energy.GetXaxis().GetXmax(),-2)
  #l2 = r.TLine(30,-50,30,-2)
  l1 = r.TF1("f1","3.28325*TMath::Log10(x)-12.5",30,HBHE_Timing_vs_Energy_M0_from_MAHI.GetXaxis().GetXmax())
  l2 = r.TLine(30,-50,30,-7.65)
  l1.SetLineColor(r.kRed)
  l2.SetLineColor(r.kRed)
  l1.Draw("same")
  l2.Draw("same")
  canvas.SaveAs(outfile)
else:
  print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_M0_from_MAHI_ + END

HBHE_Timing_vs_Energy_MAHI_from_M0_ = "HBHE_Timing_vs_Energy_MAHI_from_M0"
HBHE_Timing_vs_Energy_MAHI_from_M0 = f1.Get(HBHE_Timing_vs_Energy_MAHI_from_M0_)
if HBHE_Timing_vs_Energy_MAHI_from_M0:
  HBHE_Timing_vs_Energy_MAHI_from_M0.Draw("colz")
  HBHE_Timing_vs_Energy_MAHI_from_M0.SetTitle("HB MAHI Timings vs Energy for second peak in M0 Timing;Energy;Timing")
  
  l1 = r.TLine(30,-2,HBHE_Timing_vs_Energy_MAHI_from_M0.GetXaxis().GetXmax(),-2)
  l2 = r.TLine(30,-50,30,-2)
  #l1 = r.TF1("f1","3.28325*TMath::Log10(x)-12.5",30,HBHE_Timing_vs_Energy.GetXaxis().GetXmax())
  #l2 = r.TLine(30,-50,30,-7.65)
  l1.SetLineColor(r.kRed)
  l2.SetLineColor(r.kRed)
  l1.Draw("same")
  l2.Draw("same")
  canvas.SaveAs(outfile)
else:
  print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_MAHI_from_M0_ + END

r.gPad.SetLogx(r.kFALSE)
r.gPad.SetLogz(r.kTRUE)

for method in range(0,nmethod):
  if method == 0:
    Timing = "HB Charge-Averaged Timing"
    Energy = "Digi Energy"
    m_name = "Charge-Averaged"
  elif method == 1:
    Timing = "HB MAHI Timing"
    Energy = "MAHI Energy"
    m_name = "MAHI"
  elif method == 2:
    Timing = "HB M0 Timing"
    Energy = "M0 Energy"
    m_name = "M0"

    
  #plot for Timing vs IEta
  r.gPad.SetLogx(r.kFALSE)
  HBHE_Timing_vs_iEta_x = "HBHE_Timing_vs_iEta_" + str(method)
  HBHE_Timing_vs_iEta    = f1.Get(HBHE_Timing_vs_iEta_x)
  if HBHE_Timing_vs_iEta:
    HBHE_Timing_vs_iEta.Draw("colz")
    HBHE_Timing_vs_iEta.SetTitle(Timing + " vs IEta ALL Channels;IEta;"+Timing)

    HBHE_Timing_vs_iEta_p = HBHE_Timing_vs_iEta.ProfileX()
    HBHE_Timing_vs_iEta_p.SetLineColor(r.kBlack)
    HBHE_Timing_vs_iEta_p.SetMarkerStyle(15)
    HBHE_Timing_vs_iEta_p.Draw("same")
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_iEta_x + END

  HBHE_Timing_vs_Energy_x = "TEST_" + str(method)
  HBHE_Timing_vs_Energy_M_x = "TEST_M_" + str(method)
  HBHE_Timing_vs_Energy_P_x = "TEST_P_" + str(method)
  
  #plot for Timins vs Energy, general
  r.gPad.SetLogx(r.kTRUE)
  HBHE_Timing_vs_Energy = f1.Get(HBHE_Timing_vs_Energy_x)
  if HBHE_Timing_vs_Energy:
    HBHE_Timing_vs_Energy.Draw("colz")
    HBHE_Timing_vs_Energy.SetTitle(Timing + " vs " + Energy + " ALL Channels;"+Energy+";"+Timing)

    #l1 = r.TLine(30,-2,HBHE_Timing_vs_Energy.GetXaxis().GetXmax(),-2)
    #l2 = r.TLine(30,-50,30,-2)
    l1 = r.TF1("f1","3.28325*TMath::Log10(x)-12.5",30,HBHE_Timing_vs_Energy.GetXaxis().GetXmax())
    l2 = r.TLine(30,-50,30,-7.65)
    l1.SetLineColor(r.kRed)
    l2.SetLineColor(r.kRed)
    l1.Draw("same")
    l2.Draw("same")
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_x + END
  
  #Minus Side
  HBHE_Timing_vs_Energy_M = f1.Get(HBHE_Timing_vs_Energy_M_x)
  if HBHE_Timing_vs_Energy_M:
    HBHE_Timing_vs_Energy_M.Draw("colz")
    HBHE_Timing_vs_Energy_M.SetTitle(Timing + " vs " + Energy + " Minus Side;"+Energy+";"+Timing)
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_M_x + END
  
  #Plus Side
  HBHE_Timing_vs_Energy_P = f1.Get(HBHE_Timing_vs_Energy_P_x)
  if HBHE_Timing_vs_Energy_P:
    HBHE_Timing_vs_Energy_P.Draw("colz")
    HBHE_Timing_vs_Energy_P.SetTitle(Timing + " vs " + Energy + " Plus Side;"+Energy+";"+Timing)
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_P_x + END


  r.gPad.SetLogx(r.kFALSE)
  r.gPad.SetLogz(r.kFALSE)
  #r.gPad.SetLogz(r.kTRUE)

  HBHE_D1_SecondPeakMap_nEvt_x = "HBHE_D1_SecondPeakMap_nEvt_{}".format(method)
  HBHE_D2_SecondPeakMap_nEvt_x = "HBHE_D2_SecondPeakMap_nEvt_{}".format(method)
  HBHE_D1_SecondPeakMap_nEvt_ = f1.Get(HBHE_D1_SecondPeakMap_nEvt_x)
  HBHE_D2_SecondPeakMap_nEvt_ = f1.Get(HBHE_D2_SecondPeakMap_nEvt_x)
  if HBHE_D1_SecondPeakMap_nEvt_ and HBHE_D2_SecondPeakMap_nEvt_:
    HBHE_D1_SecondPeakMap_nEvt_.Add(HBHE_D2_SecondPeakMap_nEvt_)
    HBHE_D1_SecondPeakMap_nEvt_.Draw("colz")
    HBHE_D1_SecondPeakMap_nEvt_.SetTitle("Distribution of Second Peak {};IEta;IPhi".format(m_name))
    canvas.SaveAs(outfile)
    HBHE_D1_SecondPeakMap_nEvt_.Add(HBHE_D2_SecondPeakMap_nEvt_,-1)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_D1_SecondPeakMap_nEvt_x + " and " + HBHE_D2_SecondPeakMap_nEvt_x + END

  HBHE_D1_NormalMap_nEvt_x = "HBHE_D1_NormalMap_nEvt_{}".format(method)
  HBHE_D2_NormalMap_nEvt_x = "HBHE_D2_NormalMap_nEvt_{}".format(method)
  HBHE_D1_NormalMap_nEvt_ = f1.Get(HBHE_D1_NormalMap_nEvt_x)
  HBHE_D2_NormalMap_nEvt_ = f1.Get(HBHE_D2_NormalMap_nEvt_x)
  if HBHE_D1_NormalMap_nEvt_ and HBHE_D2_NormalMap_nEvt_:
    HBHE_D1_NormalMap_nEvt_.Add(HBHE_D2_NormalMap_nEvt_)
    HBHE_D1_NormalMap_nEvt_.Draw("colz")
    HBHE_D1_NormalMap_nEvt_.SetTitle("Distribution of Normal Events {};IEta;IPhi".format(m_name))
    canvas.SaveAs(outfile)
    HBHE_D1_NormalMap_nEvt_.Add(HBHE_D2_NormalMap_nEvt_,-1)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_D1_NormalMap_nEvt_x + " and " + HBHE_D2_NormalMap_nEvt_x + END

  #Second Peak
  #HBHE_SecondPeakMap_nEvt = f1.Get(HBHE_SecondPeakMap_nEvt_x)
  #if HBHE_SecondPeakMap_nEvt:
  #  HBHE_SecondPeakMap_nEvt.Draw("colz")
  #  HBHE_SecondPeakMap_nEvt.SetTitle("Distribution of Second Peak Events M{};IEta;IPhi".format(method))
  #  canvas.SaveAs(outfile)
  #else:
  #  print WARNING + "Warning: cannot find the histogram: " + HBHE_SecondPeakMap_nEvt_x + END

  #Normal Events
  #HBHE_NormalMap_nEvt = f1.Get(HBHE_NormalMap_nEvt_x)
  #if HBHE_NormalMap_nEvt:
  #  HBHE_NormalMap_nEvt.Draw("colz")
  #  HBHE_NormalMap_nEvt.SetTitle("Distribution of Normal Events M{};IEta;Iphi".format(method))
  #  canvas.SaveAs(outfile)
  #else:
  #  print WARNING + "Warning: cannot find the histogram: " + HBHE_NormalMap_nEvt_x + END

  #r.gPad.SetLogz(r.kFALSE)
  r.gPad.SetLogx(r.kTRUE)
  r.gPad.SetLogz(r.kTRUE)


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

  #plot for timing map
  r.gPad.SetLogx(r.kFALSE)
  HBHE_TimingMap_x = "HBHE_TimingMap_" + str(method)
  HBHE_TimingMap   = f1.Get(HBHE_TimingMap_x)
  if HBHE_TimingMap:
    HBHE_TimingMap = HBHE_TimingMap.Project3DProfile("yx")
    HBHE_TimingMap.Draw("colz")
    HBHE_TimingMap.SetTitle(Timing+";IEta;IPhi")
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_TimingMap_x + END


r.gPad.SetLogx(r.kFALSE)
r.gPad.SetGridx(r.kFALSE)
r.gPad.SetGridy(r.kFALSE)

canvas.SaveAs(outfile + ']')
print GREEN + outfile + " dumped" + END
output.append(outfile)

elapsed = time.time() - start
print GREEN +  "General Information part is done, time used is: " + str(elapsed) + " secs..." + END

#exit()
#--------------------------------------------------------Making plots for Different Depths and IEtas----------------------------------------------#
for depth in range(1,3):
  canvas = r.TCanvas()
  #r.gPad.SetLogy(r.kTRUE)
  #r.gPad.SetLogz(r.kTRUE)
  outfile = infile + "_HB_timing_depth{}.pdf".format(depth)
  canvas.SaveAs(outfile + '[')

  HBHE_TSRatio_Dx = "HBHE_TSRatio_D" + str(depth)
  HBHE_TSRatioMap_nEvt_Dx = "HBHE_TSRatioMap_nEvt_D" + str(depth)
  HBHE_TSRatioMap_Ave_Dx = "HBHE_TSRatioMap_Ave_D" + str(depth)
  HBHE_TSRatioMap_Ave_fC1_Dx = "HBHE_TSRatioMap_Ave_D" + str(depth) + "_fC1"
  HBHE_TSRatioMap_Ave_fC2_Dx = "HBHE_TSRatioMap_Ave_D" + str(depth) + "_fC2"
  HBHE_TSRatioMap_Ave_fC3_Dx = "HBHE_TSRatioMap_Ave_D" + str(depth) + "_fC3"
  HBHE_TSRatioMap_Ave_fC4_Dx = "HBHE_TSRatioMap_Ave_D" +str(depth) + "_fC4"

  HBHE_TSRatio_D = f1.Get(HBHE_TSRatio_Dx)
  HBHE_TSRatioMap_nEvt_D = f1.Get(HBHE_TSRatioMap_nEvt_Dx)
  HBHE_TSRatioMap_Ave_D = f1.Get(HBHE_TSRatioMap_Ave_Dx)
  HBHE_TSRatioMap_Ave_fC1_D = f1.Get(HBHE_TSRatioMap_Ave_fC1_Dx)
  HBHE_TSRatioMap_Ave_fC2_D = f1.Get(HBHE_TSRatioMap_Ave_fC2_Dx)
  HBHE_TSRatioMap_Ave_fC3_D = f1.Get(HBHE_TSRatioMap_Ave_fC3_Dx)
  HBHE_TSRatioMap_Ave_fC4_D = f1.Get(HBHE_TSRatioMap_Ave_fC4_Dx)


  r.gPad.SetLogy(r.kTRUE)
  HBHE_TSRatio_D.Draw()
  HBHE_TSRatio_D.SetTitle("HB ChargeRatio_TS4/TS3_Depth"+str(depth)+" TS3>30fC;TS4/TS3;hits")
  canvas.SaveAs(outfile)

#  r.gStyle.SetPadGridX(r.kTRUE)
#  r.gStyle.SetPadGridY(r.kTRUE)
  r.gPad.SetGridx(r.kTRUE)
  r.gPad.SetGridy(r.kTRUE)
  r.gPad.SetLogy(r.kFALSE)

  HBHE_TSRatioMap_nEvt_D.Draw("colz")
  HBHE_TSRatioMap_nEvt_D.SetTitle("HB nHits_Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Ave_D = HBHE_TSRatioMap_Ave_D.Project3DProfile("yx")
  HBHE_TSRatioMap_Ave_D.Draw("colz")
  HBHE_TSRatioMap_Ave_D.SetTitle("HB Ave_ChargeRatio_TS4/TS3_Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Ave_fC1_D = HBHE_TSRatioMap_Ave_fC1_D.Project3DProfile("yx")
  HBHE_TSRatioMap_Ave_fC1_D.Draw("colz")
  HBHE_TSRatioMap_Ave_fC1_D.SetTitle("HB Ave_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 100fC>TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Ave_fC2_D = HBHE_TSRatioMap_Ave_fC2_D.Project3DProfile("yx")
  HBHE_TSRatioMap_Ave_fC2_D.Draw("colz")
  HBHE_TSRatioMap_Ave_fC2_D.SetTitle("HB Ave_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 200fC>TS3>100fC;IEta;IPhi")
  canvas.SaveAs(outfile)
  
  HBHE_TSRatioMap_Ave_fC3_D = HBHE_TSRatioMap_Ave_fC3_D.Project3DProfile("yx")
  HBHE_TSRatioMap_Ave_fC3_D.Draw("colz")
  HBHE_TSRatioMap_Ave_fC3_D.SetTitle("HB Ave_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 600fC>TS3>200fC;IEta;IPhi")
  canvas.SaveAs(outfile)
  
  HBHE_TSRatioMap_Ave_fC4_D = HBHE_TSRatioMap_Ave_fC4_D.Project3DProfile("yx")
  HBHE_TSRatioMap_Ave_fC4_D.Draw("colz")
  HBHE_TSRatioMap_Ave_fC4_D.SetTitle("HB Ave_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 2000fC>TS3>600fC;IEta;IPhi")
  canvas.SaveAs(outfile)

#Plots for Timing map
  for method in range(0,nmethod):
    if method == 0:
      Timing = "HB Charge-Averaged Timing"
      Energy = "Digi Energy"
      m_name = "Charge-Averaged"
    elif method == 1:
      Timing = "HB MAHI Timing"
      Energy = "MAHI Energy"
      m_name = "MAHI"
    elif method == 2:
      Timing = "HB M0 Timing"
      Energy = "M0 Energy"
      m_name = "M0"
    
    r.gPad.SetLogx(r.kTRUE)
    HBHE_Timing_vs_Energy_Dx = "TEST_D{}_{}".format(depth,method)
    HBHE_Timing_vs_Energy_D = f1.Get(HBHE_Timing_vs_Energy_Dx)
    if HBHE_Timing_vs_Energy_D:
      HBHE_Timing_vs_Energy_D.Draw("colz")
      HBHE_Timing_vs_Energy_D.SetTitle(Energy+" vs "+Timing+" D{};".format(depth)+Timing+";"+Energy)
      canvas.SaveAs(outfile)
    else:
       print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_Dx + END
    r.gPad.SetLogx(r.kFALSE)

    r.gPad.SetLogz(r.kFALSE)
    HBHE_Dx_SecondPeakMap_nEvt_x = "HBHE_D{}_SecondPeakMap_nEvt_{}".format(depth,method)
    HBHE_Dx_SecondPeakMap_nEvt_ = f1.Get(HBHE_Dx_SecondPeakMap_nEvt_x)
    if HBHE_Dx_SecondPeakMap_nEvt_:
      HBHE_Dx_SecondPeakMap_nEvt_.Draw("colz")
      HBHE_Dx_SecondPeakMap_nEvt_.SetTitle("Distribution of Second Peak Depth{} {};IEta;IPhi".format(depth,m_name))
      canvas.SaveAs(outfile)
    else:
      print WARNING + "Warning: cannot find the histogram: " + HBHE_Dx_SecondPeakMap_nEvt_x + END

    HBHE_D1_NormalMap_nEvt_x = "HBHE_D{}_NormalMap_nEvt_{}".format(depth,method)
    HBHE_D1_NormalMap_nEvt_ = f1.Get(HBHE_D1_NormalMap_nEvt_x)
    if HBHE_D1_NormalMap_nEvt_:
      HBHE_D1_NormalMap_nEvt_.Draw("colz")
      HBHE_D1_NormalMap_nEvt_.SetTitle("Distribution of Normal Events Depth{} {};IEta;IPhi".format(depth,m_name))
      canvas.SaveAs(outfile)
    else:
      print WARNING + "Warning: cannot find the histogram: " + HBHE_D1_NormalMap_nEvt_x + END
    r.gPad.SetLogz(r.kTRUE)

    HBHE_TimingMap_Dx_x = "HBHE_TimingMap_D{}_{}".format(depth,method)
    HBHE_TimingMap_Dx = f1.Get(HBHE_TimingMap_Dx_x)
    if HBHE_TimingMap_Dx:
      HBHE_TimingMap_Dx = HBHE_TimingMap_Dx.Project3DProfile("yx")
      HBHE_TimingMap_Dx.Draw("colz")
      HBHE_TimingMap_Dx.SetTitle(Timing+" Depth "+str(depth)+";IEta;IPhi")
      canvas.SaveAs(outfile)
    else:
      print WARNING + "Warning: cannot find the histogram: " + HBHE_TimingMap_Dx_x + END

  for eta in range(minieta,maxeta):
    for phi in range(miniphi,maxphi):
#------------------------------
# Plots for Timing vs Energy
#------------------------------
      r.gPad.SetLogx(r.kTRUE)
      r.gPad.SetGridx(r.kTRUE)
      r.gPad.SetGridy(r.kTRUE)
      for method in range(0,3):
        if method == 0:
          Timing = "HB Charge-Averaged Timing"
          Energy = "Digi Energy"
        elif method == 1:
          Timing = "HB MAHI Timing"
          Energy = "MAHI Energy"
        elif method == 2:
          Timing = "HB M0 Timing"
          Energy = "M0 Energy"

        HBHE_Timing_vs_Energy_ietax_iphix_idepthx = "HBHE_Timing_vs_Energy_ieta{}_iphi{}_idepth{}_".format(eta,0,depth) + str(method)
        HBHE_Timing_vs_Energy_ieta_iphi_idepth = f1.Get(HBHE_Timing_vs_Energy_ietax_iphix_idepthx)
        if HBHE_Timing_vs_Energy_ieta_iphi_idepth:
          HBHE_Timing_vs_Energy_ieta_iphi_idepth.Draw("colz")
          HBHE_Timing_vs_Energy_ieta_iphi_idepth.SetTitle(Timing + " vs " + Energy +"IEta{} Depth{};".format(eta,depth)+Energy+";"+Timing)
          canvas.SaveAs(outfile)
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_ietax_iphix_idepthx + END

#-------------------------------
# Plots for Pulse Shape
#-------------------------------
      r.gPad.SetLogx(r.kFALSE)
      r.gPad.SetGridx(r.kFALSE)
      r.gPad.SetGridy(r.kFALSE)

      HBHE_PulseShape_ietax_iphix_idepthx = "HBHE_PulseShape_ieta{}_iphi{}_idepth{}".format(eta,0,depth)
      HBHE_PulseShape_ieta_iphi_idepth = f1.Get(HBHE_PulseShape_ietax_iphix_idepthx)
      if HBHE_PulseShape_ieta_iphi_idepth:
        HBHE_PulseShape_ieta_iphi_idepth.Draw()
        HBHE_PulseShape_ieta_iphi_idepth.SetTitle("HB Pulse Shape IEta{} Depth{};TS;fC".format(eta,depth))
        canvas.SaveAs(outfile)
      else:
        print WARNING + "Warning: cannot find the histogram: " + HBHE_PulseShape_ietax_iphix_idepthx + END

#----------------------------------------------------------------
# Plots for Distribution of totalCharge and chargeAveraged Timing
#----------------------------------------------------------------
    r.gPad.SetLogx(r.kTRUE)
    HBHE_totalCharge_ietax_iphix_idepthx = "HBHE_totalCharge_ieta{}_iphi{}_idepth{}".format(eta,0,depth)
    HBHE_totalCharge_ieta_iphi_idepth = f1.Get(HBHE_totalCharge_ietax_iphix_idepthx)
    HBHE_totalCharge_ieta_iphi_idepth.Draw()
    HBHE_totalCharge_ieta_iphi_idepth.SetTitle("HB Total Charge Distribution IEta{} Depth{} All IPhis;Charge fC;Hits".format(eta,depth))
    canvas.SaveAs(outfile)
   
    r.gPad.SetGridx(r.kTRUE)
    r.gPad.SetGridy(r.kTRUE)
    if _phi and (len(_phi) > 1):
      phi_list = _phi[1:]
    # Draw Timing vs Energy for different IPhis
      for method in range(0,nmethod):
        if method == 0:
          m_name = "Charge-Averaged"
        elif method == 1:
          m_name = "MAHI"
        elif method == 2:
          m_name = "M0"
        for phi in phi_list:
          
          HBHE_Timing_vs_Energy_ietax_iphix_idepthx = "HBHE_Timing_vs_Energy_ieta{}_iphi{}_idepth{}_{}".format(eta,phi,depth,method)
          HBHE_Timing_vs_Energy_ieta_iphi_idepth = f1.Get(HBHE_Timing_vs_Energy_ietax_iphix_idepthx)
          if HBHE_Timing_vs_Energy_ieta_iphi_idepth:
            HBHE_Timing_vs_Energy_ieta_iphi_idepth.Draw("colz")
            HBHE_Timing_vs_Energy_ieta_iphi_idepth.SetTitle("HB Timing vs Energy IEta {} IPhi {} Depth {} {};Energy;Timing".format(eta,phi,depth,m_name))
            canvas.SaveAs(outfile)
          else:
            print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_ietax_iphix_idepthx + END


      r.gPad.SetLogx(r.kFALSE)
    # Draw Pulse Shape
      leg_PulseShape = r.TLegend(0.6,0.7,0.8,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_PulseShape_ietax_iphix_idepthx = "HBHE_PulseShape_ieta{}_iphi{}_idepth{}".format(eta,phi,depth)
        HBHE_PulseShape_ieta_iphi_idepth = f1.Get(HBHE_PulseShape_ietax_iphix_idepthx)
        if HBHE_PulseShape_ieta_iphi_idepth:
          HBHE_PulseShape_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_PulseShape_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_PulseShape_ieta_iphi_idepth.Draw()
          else:
            HBHE_PulseShape_ieta_iphi_idepth.Draw("same")
          HBHE_PulseShape_ieta_iphi_idepth.SetTitle("HB Pulse Shape Distribution IEta{} Depth{};Charge fC;Hits".format(eta,depth))
          leg_PulseShape.AddEntry(HBHE_PulseShape_ieta_iphi_idepth,"total charge IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_PulseShape_ietax_iphix_idepthx + END
      leg_PulseShape.SetTextSize(0.025)
      leg_PulseShape.Draw("same")
      canvas.SaveAs(outfile)

    # Draw total charge distribution
      r.gPad.SetLogy(r.kTRUE)
      #r.gPad.SetLogx(r.kTRUE)
      leg_totalCharge = r.TLegend(0.6,0.7,0.8,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_totalCharge_ietax_iphix_idepthx = "HBHE_totalCharge_ieta{}_iphi{}_idepth{}".format(eta,phi,depth)
        HBHE_totalCharge_ieta_iphi_idepth = f1.Get(HBHE_totalCharge_ietax_iphix_idepthx)
        if HBHE_totalCharge_ieta_iphi_idepth:
          HBHE_totalCharge_ieta_iphi_idepth.GetXaxis().SetRangeUser(0,500)
          HBHE_totalCharge_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_totalCharge_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_totalCharge_ieta_iphi_idepth.Draw()
          else:
            HBHE_totalCharge_ieta_iphi_idepth.Draw("same")
          HBHE_totalCharge_ieta_iphi_idepth.SetTitle("HB Total Charge Distribution IEta{} Depth{};Charge fC;Hits".format(eta,depth))
          leg_totalCharge.AddEntry(HBHE_totalCharge_ieta_iphi_idepth,"total charge IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_totalCharge_ietax_iphix_idepthx + END
      leg_totalCharge.SetTextSize(0.025)
      leg_totalCharge.Draw("same")
      canvas.SaveAs(outfile)
      r.gPad.SetLogy(r.kFALSE)
      #r.gPad.SetLogx(r.kFALSE)

      # Draw charge averaged timing distribution, MAHI Timing
      leg_RecHitTiming = r.TLegend(0.1,0.7,0.3,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_RecHitTiming_ietax_iphix_idepthx = "HBHE_RecHitTiming_ieta{}_iphi{}_idepth{}_{}".format(eta,phi,depth,1)
        HBHE_RecHitTiming_ieta_iphi_idepth = f1.Get(HBHE_RecHitTiming_ietax_iphix_idepthx)
        if HBHE_RecHitTiming_ieta_iphi_idepth:
          HBHE_RecHitTiming_ieta_iphi_idepth.GetXaxis().SetRangeUser(-10,10)
          HBHE_RecHitTiming_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_RecHitTiming_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_RecHitTiming_ieta_iphi_idepth.Draw()
          else:
            HBHE_RecHitTiming_ieta_iphi_idepth.Draw("same")
          HBHE_RecHitTiming_ieta_iphi_idepth.SetTitle("HB RecHit Timing Distribution IEta{} Depth{} {};Timing;Hits".format(eta,depth,"MAHI Timing"))
          leg_RecHitTiming.AddEntry(HBHE_RecHitTiming_ieta_iphi_idepth,"timing IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_RecHitTiming_ietax_iphix_idepthx + END
      leg_RecHitTiming.SetTextSize(0.025)
      leg_RecHitTiming.Draw("same")
      canvas.SaveAs(outfile)

      # Draw charge averaged timing distribution, M0 Timing
      leg_RecHitTiming = r.TLegend(0.1,0.7,0.3,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_RecHitTiming_ietax_iphix_idepthx = "HBHE_RecHitTiming_ieta{}_iphi{}_idepth{}_{}".format(eta,phi,depth,2)
        HBHE_RecHitTiming_ieta_iphi_idepth = f1.Get(HBHE_RecHitTiming_ietax_iphix_idepthx)
        if HBHE_RecHitTiming_ieta_iphi_idepth:
          HBHE_RecHitTiming_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_RecHitTiming_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_RecHitTiming_ieta_iphi_idepth.Draw()
          else:
            HBHE_RecHitTiming_ieta_iphi_idepth.Draw("same")
          HBHE_RecHitTiming_ieta_iphi_idepth.SetTitle("HB RecHit Timing Distribution IEta{} Depth{} {};Timing;Hits".format(eta,depth,"M0 Timing"))
          leg_RecHitTiming.AddEntry(HBHE_RecHitTiming_ieta_iphi_idepth,"timing IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_RecHitTiming_ietax_iphix_idepthx + END
      leg_RecHitTiming.SetTextSize(0.025)
      leg_RecHitTiming.Draw("same")
      canvas.SaveAs(outfile)

      r.gPad.SetLogy(r.kTRUE)
      # Draw charge averaged timing distribution, MAHI Energy
      leg_RecHitTiming = r.TLegend(0.6,0.7,0.8,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_RecHitEnergy_ietax_iphix_idepthx = "HBHE_RecHitEnergy_ieta{}_iphi{}_idepth{}_{}".format(eta,phi,depth,1)
        HBHE_RecHitEnergy_ieta_iphi_idepth = f1.Get(HBHE_RecHitEnergy_ietax_iphix_idepthx)
        if HBHE_RecHitEnergy_ieta_iphi_idepth:
          HBHE_RecHitEnergy_ieta_iphi_idepth.GetXaxis().SetRangeUser(0,100)
          HBHE_RecHitEnergy_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_RecHitTiming_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_RecHitEnergy_ieta_iphi_idepth.Draw()
          else:
            HBHE_RecHitEnergy_ieta_iphi_idepth.Draw("same")
          HBHE_RecHitEnergy_ieta_iphi_idepth.SetTitle("HB RecHit Energy Distribution IEta{} Depth{} {};Energy;Hits".format(eta,depth,"MAHI Energy"))
          leg_RecHitTiming.AddEntry(HBHE_RecHitEnergy_ieta_iphi_idepth,"Energy IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_RecHitEnergy_ietax_iphix_idepthx + END
      leg_RecHitTiming.SetTextSize(0.025)
      leg_RecHitTiming.Draw("same")
      canvas.SaveAs(outfile)

      # Draw charge averaged timing distribution, M0 Energy
      leg_RecHitTiming = r.TLegend(0.6,0.7,0.8,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_RecHitEnergy_ietax_iphix_idepthx = "HBHE_RecHitEnergy_ieta{}_iphi{}_idepth{}_{}".format(eta,phi,depth,2)
        HBHE_RecHitEnergy_ieta_iphi_idepth = f1.Get(HBHE_RecHitEnergy_ietax_iphix_idepthx)
        if HBHE_RecHitEnergy_ieta_iphi_idepth:
          HBHE_RecHitEnergy_ieta_iphi_idepth.GetXaxis().SetRangeUser(0,100)
          HBHE_RecHitEnergy_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_RecHitTiming_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_RecHitEnergy_ieta_iphi_idepth.Draw()
          else:
            HBHE_RecHitEnergy_ieta_iphi_idepth.Draw("same")
          HBHE_RecHitEnergy_ieta_iphi_idepth.SetTitle("HB RecHit Energy Distribution IEta{} Depth{} {};Energy;Hits".format(eta,depth,"M0 Energy"))
          leg_RecHitTiming.AddEntry(HBHE_RecHitEnergy_ieta_iphi_idepth,"Energy IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_RecHitEnergy_ietax_iphix_idepthx + END
      leg_RecHitTiming.SetTextSize(0.025)
      leg_RecHitTiming.Draw("same")
      canvas.SaveAs(outfile)
      r.gPad.SetLogy(r.kFALSE)


  canvas.SaveAs(outfile + ']')
  print GREEN + outfile + " dumped" + END
  output.append(outfile)

  elapsed = time.time() - start
  print GREEN + "Dep" + str(depth) + " is done, time used is: " + str(elapsed) + " secs..." + END

print GREEN + "Output " + str(len(output)) + " files, they are: \n"
print output
print END
