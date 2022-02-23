#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import math
import argparse
import xml.etree.ElementTree as ET

from ROOT import *

gROOT.SetBatch()

gROOT.LoadMacro("AtlasStyle.C")
gROOT.LoadMacro("AtlasUtils.C")
gROOT.LoadMacro("AtlasLabels.C")

SetAtlasStyle()

gStyle.SetErrorX(0.5); # !!! must after SetAtlasStyle()
gStyle.SetOptStat(0)

colors = {
'ggH':'#E6DF6C',
'background':'#EB7188',
'spurious':'#663366',
'VBF':'#83CAE6',
}

cats = ['TT', 'TL', 'LT']

procs = ['background', 'ggH', 'VBF', 'spurious']
procs_re = ['VBF', 'ggH', 'background', 'spurious']

binlabels = ['-99:-2', '-2:-1', '-1:0', '0:1', '1:2', '2:99']
nbin = len(binlabels)

hfmodel = THStack('hfmodel', '')
hfdata = TH1F('hfdata', '', nbin, 0, nbin)
hfsig = TH1F('hfsig', '', nbin, 0, nbin)
hfp01 = TH1F('hfp01', '', nbin, 0, nbin)

bkghists = {}
for proc in procs:
  hname = proc
  print hname
  bkghists[hname] = TH1F(hname, '', nbin, 0, nbin)
  bkghists[hname].SetFillColorAlpha(TColor.GetColor(colors[hname]), 1)
  bkghists[hname].SetLineWidth(0)

p01hists = {}
for proc in procs:
  hname = proc
  print hname
  p01hists[hname] = TH1F(hname, '', nbin, 0, nbin)
  p01hists[hname].SetFillColor(TColor.GetColor(colors[hname]))
  p01hists[hname].SetLineWidth(0)

weights = {}
hmodels = {}
hdatas = {}
for cat in cats:
  fmat = TFile("plotsPostfit/plotMaterials_"+cat+"_m00_SW_subBkg.root", 'read') # subBkg
  hmodel = fmat.Get('hs').Clone('hmodel_'+cat)
  hdata = fmat.Get('data_minus_bkg').Clone('hdata_'+cat) # subBkg
  hsig = fmat.Get('cl_VBF').Clone('hsig_'+cat) # subBkg

  hmodels[cat] = hmodel
  hdatas[cat] = hdata

  nbkg = fmat.Get('cl_bkgs').GetSumOfWeights()
  nsig = fmat.Get('cl_VBF').GetSumOfWeights()
  weight = math.log(1+nsig/nbkg)
  print '======= %s ======='%(cat)
  print '%s ln(1+%0.2f/%0.2f) = %0.3f'%(cat, nsig, nbkg, weight)
  print 'data numbers:', hdata.GetSumOfWeights()
  for proc in procs:
    print proc, fmat.Get('cl_'+proc).GetSumOfWeights()
  print '======='

  weights[cat] = weight

  hfdata.Add(hdata, weight)
  hfsig.Add(hsig, weight)

  for hname in procs:
    htmp = fmat.Get('cl_'+hname)
    bkghists[hname].Add(htmp, weight)

  fp01 = TFile("plotsPostfit/plotMaterials_"+cat+"_p01_SW_subBkg.root", 'read') # subBkg
  for hname in procs:
    htmp = fp01.Get('cl_'+hname)
    if hname == 'VBF': htmp.SetFillColorAlpha(0, 0)
    p01hists[hname].Add(htmp, weight)
  hp01 = fp01.Get('cl_VBF').Clone('hsig_'+cat) # subBkg
  hfp01.Add(hp01, weight)

print 'data addup numbers:', hfdata.GetSumOfWeights()

for hname in procs:
  hfmodel.Add(bkghists[hname])
  print hname, 'addup numbers:', bkghists[hname].GetSumOfWeights()

hfmodelp01 = THStack('hfmodelp01', '')
for hname in procs:
  hfmodelp01.Add(p01hists[hname])
  #print hname, 'addup numbers:', p01hists[hname].GetSumOfWeights()
#hp01 = hfmodelp01.GetStack().Last().Clone('hp01')
hfp01.SetLineColor(kViolet)
hfp01.SetLineWidth(2)
hfp01.SetFillColorAlpha(0, 0)
hfp01.SetMarkerSize(0)

############# only need to prepare hfsig, hfdata and dfmodel above #############

c = TCanvas("c", "canvas", 800, 800);

pad1 = TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
pad1.SetBottomMargin(0)
pad1.Draw()
pad1.cd()

lg = TLegend(0.65, 0.7, 0.85, 0.90)
lg.SetFillColorAlpha(kBlue, 0)

#for st in hfmodel.GetStack():
#  if st.GetName() == 'spurious': continue
#  lg.AddEntry(st, st.GetName().replace('cl_', ''), 'f')

ymax = hfdata.GetMaximum()
ymax = 2.0 * ymax
hfsig.SetMaximum(ymax)
hfsig.SetMinimum(0)

hfsig.SetFillColorAlpha(TColor.GetColor(colors['VBF']), 1)
hfsig.SetLineWidth(0)
lg.AddEntry(hfsig, 'VBF', 'f')
hfsig.Draw('hist')

lg.AddEntry(hfp01, 'VBF(#tilde{d}=0.01)', 'f')
hfp01.Draw('hist same')

lg.AddEntry(hfdata, 'data', 'lep')
hfdata.Draw('same e')

lg.SetBorderSize(0);
lg.Draw("same");

ATLASLabel(0.22,0.85,"Preliminary");
myText(0.22, 0.80, 1, "#sqrt{s}= 13 TeV, 139 fb^{-1}");
myText(0.22, 0.75, 1, "VBF CP H #rightarrow #it{#gamma#gamma}");
myText(0.22, 0.70, 1, "m_{#gamma#gamma} in [118, 132] GeV", 0.05); # SWcut

c.cd()
pad2 = TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
pad2.SetTopMargin(0)
pad2.SetBottomMargin(0.2)
pad2.Draw()
pad2.cd()

rhdata = hfdata.Clone('ratio_data')
errmodel = hfmodel.GetStack().Last().Clone('error_of_model')
rhmodel = hfsig.Clone('ratio_model')

bkgtmp = hfsig.Clone('ratio_denominator')
for i in range(nbin):
  rhmodel.SetBinError(i+1, errmodel.GetBinError(i+1))
  bkgtmp.SetBinError(i+1, 0)
  rhdata.GetXaxis().SetBinLabel(i+1, binlabels[i])

rhdata.Divide(bkgtmp)
rhmodel.Divide(bkgtmp)

rhmodel.SetFillStyle(3001)
rhmodel.SetFillColor(kBlack)
rhmodel.SetMarkerStyle(20)
rhmodel.SetMarkerSize(0)

rhdata.GetYaxis().SetTitle('Data/Model')
rhdata.GetYaxis().SetTitleSize(0.1)
rhdata.GetYaxis().SetTitleOffset(0.5)
rhdata.GetYaxis().SetLabelSize(0.1)
#rhdata.GetYaxis().SetNdivisions(rhdata.GetYaxis().GetNdivisions()/3)
rhdata.GetXaxis().SetTitle('OO')
rhdata.GetXaxis().SetTitleSize(0.1)
rhdata.GetXaxis().SetTitleOffset(0.8)
rhdata.GetXaxis().SetLabelSize(rhdata.GetXaxis().GetLabelSize()*3)

yscale = 1.8
rhmax = rhdata.GetMaximum()
rhmin = rhdata.GetMinimum()
rhdata.SetMaximum(rhmax*yscale)
rhdata.SetMinimum(1-abs(rhmax*yscale-1))
rhdata.Draw('ep')
rhmodel.Draw('same e2')

c.SaveAs("plotsPostfit/ooPostfit_SW_addup_subBkg.png") # subBkg
c.SaveAs("plotsPostfit/ooPostfit_SW_addup_subBkg.pdf") # subBkg
