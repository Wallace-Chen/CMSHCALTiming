import ROOT as r
import sys
import commands
import time

start = time.time()

#------Please Do not edit, lists will be added after executing the ./main
_phi = [0,7,23,25,52,53,66]
#------Please Do not edit before this line
#color options:
HEADER   = '\033[95m'
BLUE     = '\033[94m'
GREEN    = '\033[92m'
WARNING  = '\033[93m'
FAIL     = '\033[91m'
BOLD     = '\033[1m'
UNDERLINE= '\033[4m'
END      = '\033[0m'

maxeta=17
minieta=-16
maxphi=2
miniphi=1
maxdepth=3
minidepth=1

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


#--------------------------------------------------------Making plots for General Information----------------------------------------------#
canvas = r.TCanvas()
outfile = infile + "_HB_timing_general.pdf"
canvas.SaveAs(outfile + '[')



#r.gPad.SetLogx(r.kTRUE)
r.gPad.SetLogy(r.kFALSE)
r.gPad.SetGridx(r.kTRUE)
r.gPad.SetGridy(r.kTRUE)

#plots for timing in eta-phi plane
HBHE_TSRatioMap_nEvt_Dx = "HBHE_TSRatioMap_nEvt"
HBHE_TSRatioMap_Avg_Dx = "HBHE_TSRatioMap_Avg"
HBHE_TSRatioMap_Rms_Dx = "HBHE_TSRatioMap_Rms"
HBHE_TSRatioMap_Pull_Dx = "HBHE_TSRatioMap_Pull"
HBHE_TSRatioMap_Avg_fC1_Dx = "HBHE_TSRatioMap_Avg_fC1"
HBHE_TSRatioMap_Avg_fC2_Dx = "HBHE_TSRatioMap_Avg_fC2"
HBHE_TSRatioMap_Avg_fC3_Dx = "HBHE_TSRatioMap_Avg_fC3"
HBHE_TSRatioMap_Avg_fC4_Dx = "HBHE_TSRatioMap_Avg_fC4"
HBHE_EnerWeiTiming_Dx = "HBHE_EnerWeiTiming"

HBHE_TSRatioMap_nEvt_D = f1.Get(HBHE_TSRatioMap_nEvt_Dx)
HBHE_TSRatioMap_Avg_D = f1.Get(HBHE_TSRatioMap_Avg_Dx)
HBHE_TSRatioMap_Rms_D = f1.Get(HBHE_TSRatioMap_Rms_Dx)
HBHE_TSRatioMap_Pull_D = f1.Get(HBHE_TSRatioMap_Pull_Dx)
HBHE_TSRatioMap_Avg_fC1_D = f1.Get(HBHE_TSRatioMap_Avg_fC1_Dx)
HBHE_TSRatioMap_Avg_fC2_D = f1.Get(HBHE_TSRatioMap_Avg_fC2_Dx)
HBHE_TSRatioMap_Avg_fC3_D = f1.Get(HBHE_TSRatioMap_Avg_fC3_Dx)
HBHE_TSRatioMap_Avg_fC4_D = f1.Get(HBHE_TSRatioMap_Avg_fC4_Dx)
HBHE_EnerWeiTiming_D = f1.Get(HBHE_EnerWeiTiming_Dx)

HBHE_TSRatioMap_nEvt_D.Draw("colz")
HBHE_TSRatioMap_nEvt_D.SetTitle("HB nHits"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_D.Draw("colz")
HBHE_TSRatioMap_Avg_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Rms_D.Draw("colz")
HBHE_TSRatioMap_Rms_D.SetTitle("HB Rms_ChargeRatio_TS4/TS3"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Pull_D.Draw("colz")
HBHE_TSRatioMap_Pull_D.SetTitle("HB Pull_of_ChargeRatio_TS4/TS3"+" TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC1_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC1_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 100fC>TS3>30fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC2_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC2_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 200fC>TS3>100fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC3_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC3_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 600fC>TS3>200fC;IEta;IPhi")
canvas.SaveAs(outfile)

HBHE_TSRatioMap_Avg_fC4_D.Draw("colz")
HBHE_TSRatioMap_Avg_fC4_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3"+" 2000fC>TS3>600fC;IEta;IPhi")
canvas.SaveAs(outfile)

#HBHE_EnerWeiTiming_D.Draw("colz")
#HBHE_EnerWeiTiming_D.SetTitle("HB Charge Weighted Timing"+" TS3>30fC;IEta;IPhi")
#canvas.SaveAs(outfile)

for method in range(0,3):
  if method == 0:
    Timing = "HB Charge-Averaged Timing"
    Energy = "Digi Energy"
  elif method == 1:
    Timing = "HB DigiRec Timing"
    Energy = "DigiRec Energy"
  elif method == 2:
    Timing = "HB RecHit Timing"
    Energy = "RecHit Energy"

    
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

  #plot for Timins vs Energy
  r.gPad.SetLogx(r.kTRUE)
  HBHE_Timing_vs_Energy_x = "HBHE_Timing_vs_Energy_" + str(method)
  HBHE_Timing_vs_Energy = f1.Get(HBHE_Timing_vs_Energy_x)
  if HBHE_Timing_vs_Energy:
    HBHE_Timing_vs_Energy.Draw("colz")
    HBHE_Timing_vs_Energy.SetTitle(Timing + " vs " + Energy + " ALL Channels;"+Energy+";"+Timing)
    canvas.SaveAs(outfile)
  else:
    print WARNING + "Warning: cannot find the histogram: " + HBHE_Timing_vs_Energy_x + END
  
  
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

elapsed = time.time() - start
print GREEN +  "General Information part is done, time used is: " + str(elapsed) + " secs..." + END

#--------------------------------------------------------Making plots for Different Depths----------------------------------------------#
for depth in range(1,3):
  canvas = r.TCanvas()
  #r.gPad.SetLogy(r.kTRUE)
  outfile = infile + "_HB_timing_depth{}.pdf".format(depth)
  canvas.SaveAs(outfile + '[')

  HBHE_TSRatio_Dx = "HBHE_TSRatio_D" + str(depth)
  HBHE_TSRatioMap_nEvt_Dx = "HBHE_TSRatioMap_nEvt_D" + str(depth)
  HBHE_TSRatioMap_Avg_Dx = "HBHE_TSRatioMap_Avg_D" + str(depth)
  HBHE_TSRatioMap_Rms_Dx = "HBHE_TSRatioMap_Rms_D" + str(depth)
  HBHE_TSRatioMap_Pull_Dx = "HBHE_TSRatioMap_Pull_D" + str(depth)
  HBHE_TSRatioMap_Avg_fC1_Dx = "HBHE_TSRatioMap_Avg_fC1_D" + str(depth)
  HBHE_TSRatioMap_Avg_fC2_Dx = "HBHE_TSRatioMap_Avg_fC2_D" + str(depth)
  HBHE_TSRatioMap_Avg_fC3_Dx = "HBHE_TSRatioMap_Avg_fC3_D" + str(depth)
  HBHE_TSRatioMap_Avg_fC4_Dx = "HBHE_TSRatioMap_Avg_fC4_D" +str(depth)
  HBHE_EnerWeiTiming_Dx = "HBHE_EnerWeiTiming_D" + str(depth)
  
  HBHE_TSRatio_D = f1.Get(HBHE_TSRatio_Dx)
  HBHE_TSRatioMap_nEvt_D = f1.Get(HBHE_TSRatioMap_nEvt_Dx)
  HBHE_TSRatioMap_Avg_D = f1.Get(HBHE_TSRatioMap_Avg_Dx)
  HBHE_TSRatioMap_Rms_D = f1.Get(HBHE_TSRatioMap_Rms_Dx)
  HBHE_TSRatioMap_Pull_D = f1.Get(HBHE_TSRatioMap_Pull_Dx)
  HBHE_TSRatioMap_Avg_fC1_D = f1.Get(HBHE_TSRatioMap_Avg_fC1_Dx)
  HBHE_TSRatioMap_Avg_fC2_D = f1.Get(HBHE_TSRatioMap_Avg_fC2_Dx)
  HBHE_TSRatioMap_Avg_fC3_D = f1.Get(HBHE_TSRatioMap_Avg_fC3_Dx)
  HBHE_TSRatioMap_Avg_fC4_D = f1.Get(HBHE_TSRatioMap_Avg_fC4_Dx)
  HBHE_EnerWeiTiming_D = f1.Get(HBHE_EnerWeiTiming_Dx)
  
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

  HBHE_TSRatioMap_Avg_D.Draw("colz")
  HBHE_TSRatioMap_Avg_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3_Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Rms_D.Draw("colz")
  HBHE_TSRatioMap_Rms_D.SetTitle("HB Rms_ChargeRatio_TS4/TS3_Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Pull_D.Draw("colz")
  HBHE_TSRatioMap_Pull_D.SetTitle("HB Pull_of_ChargeRatio_TS4/TS3_Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Avg_fC1_D.Draw("colz")
  HBHE_TSRatioMap_Avg_fC1_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 100fC>TS3>30fC;IEta;IPhi")
  canvas.SaveAs(outfile)

  HBHE_TSRatioMap_Avg_fC2_D.Draw("colz")
  HBHE_TSRatioMap_Avg_fC2_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 200fC>TS3>100fC;IEta;IPhi")
  canvas.SaveAs(outfile)
  
  HBHE_TSRatioMap_Avg_fC3_D.Draw("colz")
  HBHE_TSRatioMap_Avg_fC3_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 600fC>TS3>200fC;IEta;IPhi")
  canvas.SaveAs(outfile)
  
  HBHE_TSRatioMap_Avg_fC4_D.Draw("colz")
  HBHE_TSRatioMap_Avg_fC4_D.SetTitle("HB Avg_ChargeRatio_TS4/TS3_Depth"+str(depth)+" 2000fC>TS3>600fC;IEta;IPhi")
  canvas.SaveAs(outfile)

#  HBHE_EnerWeiTiming_D.Draw("colz")
#  HBHE_EnerWeiTiming_D.SetTitle("HB Charge Weighted Timing Depth"+str(depth)+" TS3>30fC;IEta;IPhi")
#  canvas.SaveAs(outfile)

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
          Timing = "HB DigiRec Timing"
          Energy = "DigiRec Energy"
        elif method == 2:
          Timing = "HB RecHit Timing"
          Energy = "RecHit Energy"

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
    if _phi and (len(_phi) > 1):
      phi_list = _phi[1:]

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
      r.gPad.SetLogx(r.kTRUE)
      leg_totalCharge = r.TLegend(0.6,0.7,0.8,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_totalCharge_ietax_iphix_idepthx = "HBHE_totalCharge_ieta{}_iphi{}_idepth{}".format(eta,phi,depth)
        HBHE_totalCharge_ieta_iphi_idepth = f1.Get(HBHE_totalCharge_ietax_iphix_idepthx)
        if HBHE_totalCharge_ieta_iphi_idepth:
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
      r.gPad.SetLogx(r.kFALSE)

      # Draw charge averaged timing distribution
      leg_chargeAveragedTiming = r.TLegend(0.1,0.7,0.3,0.9)
      colorNo = 2
      for phi in phi_list:
        HBHE_chargeAveragedTiming_ietax_iphix_idepthx = "HBHE_chargeAveragedTiming_ieta{}_iphi{}_idepth{}".format(eta,phi,depth)
        HBHE_chargeAveragedTiming_ieta_iphi_idepth = f1.Get(HBHE_chargeAveragedTiming_ietax_iphix_idepthx)
        if HBHE_chargeAveragedTiming_ieta_iphi_idepth:
          HBHE_chargeAveragedTiming_ieta_iphi_idepth.SetLineColor(colorNo)
          #HBHE_chargeAveragedTiming_ieta_iphi_idepth.SetAxisRange(0,700,"Y")
          if phi_list.index(phi) == 0:
            HBHE_chargeAveragedTiming_ieta_iphi_idepth.Draw()
          else:
            HBHE_chargeAveragedTiming_ieta_iphi_idepth.Draw("same")
          HBHE_chargeAveragedTiming_ieta_iphi_idepth.SetTitle("HB Charge Averaged Timing Distribution IEta{} Depth{};Timing;Hits".format(eta,depth))
          leg_chargeAveragedTiming.AddEntry(HBHE_chargeAveragedTiming_ieta_iphi_idepth,"timing IPhi {}".format(phi))

          colorNo = colorNo + 1
        else:
          print WARNING + "Warning: cannot find the histogram: " + HBHE_chargeAveragedTiming_ietax_iphix_idepthx + END
      leg_chargeAveragedTiming.SetTextSize(0.025)
      leg_chargeAveragedTiming.Draw("same")
      canvas.SaveAs(outfile)




  canvas.SaveAs(outfile + ']')
  print GREEN + outfile + " dumped" + END

  elapsed = time.time() - start
  print GREEN + "Dep" + str(depth) + " is done, time used is: " + str(elapsed) + " secs..." + END
