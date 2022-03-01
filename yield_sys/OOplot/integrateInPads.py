#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import math
import argparse
from array import array
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

rebinarray = array('d', [-10, -2, -1, 0, 1, 2, 10])

def rebinHist(hist, oriNbin, rebinArray):
  nbins = hist.GetNbinsX()
  nbinsA = len(rebinArray)
  if nbins != nbinsA-1: print "WARING!!! in rebinHist()"

  loweredge = rebinArray[0]
  upperedge = rebinArray[-1]
  h = TH1F('h', '', oriNbin, loweredge, upperedge)
  reh = h.Rebin(nbinsA-1, 'reh', rebinArray)

  for ib in range(nbins):
    cont = hist.GetBinContent(ib+1)
    err = hist.GetBinError(ib+1)
    xint = rebinArray[ib+1]-rebinArray[ib]
    cont = cont/xint
    err = err/xint
    reh.SetBinContent(ib+1, cont)
    reh.SetBinError(ib+1, err)

  return reh.Clone(hist.GetName())

hfmodel = THStack('hfmodel', '')
hfdata = TH1F('hfdata', '', nbin, 0, nbin)
hdatatmp = hfdata
hdatanew = rebinHist(hdatatmp, 20, rebinarray)
hfdata = hdatanew

bkghists = {}
for proc in procs:
  hname = proc
  print hname
  bkghists[hname] = TH1F(hname, '', nbin, 0, nbin)
  htmp = bkghists[hname]
  hnew = rebinHist(htmp, 20, rebinarray)
  bkghists[hname] = hnew
  bkghists[hname].SetFillColorAlpha(TColor.GetColor(colors[hname]), 1)
  bkghists[hname].SetLineWidth(0)

cpv1hists = {}
for proc in procs:
  hname = proc
  print hname
  cpv1hists[hname] = TH1F(hname, '', nbin, 0, nbin)
  htmp = cpv1hists[hname]
  hnew = rebinHist(htmp, 20, rebinarray)
  cpv1hists[hname] = hnew
  cpv1hists[hname].SetFillColor(TColor.GetColor(colors[hname]))
  cpv1hists[hname].SetLineWidth(0)

weights = {}
hmodels = {}
hdatas = {}
for cat in cats:
  fmat = TFile("plotsPostfit/plotMaterials_"+cat+"_m00_SW.root", 'read')
  hmodel = fmat.Get('hs').Clone('hmodel_'+cat)
  hdata = fmat.Get('Asimov_data').Clone('hdata_'+cat)

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

  for hname in procs:
    htmp = fmat.Get('cl_'+hname)
    bkghists[hname].Add(htmp, weight)

  fcpv1 = TFile("plotsPostfit/plotMaterials_"+cat+"_p07_SW.root", 'read')
  for hname in procs:
    htmp = fcpv1.Get('cl_'+hname)
    if hname == 'VBF': htmp.SetFillColorAlpha(0, 0)
    cpv1hists[hname].Add(htmp, weight)

print 'data addup numbers:', hfdata.GetSumOfWeights()

for hname in procs:
  hfmodel.Add(bkghists[hname])
  print hname, 'addup numbers:', bkghists[hname].GetSumOfWeights()

hfmodelcpv1 = THStack('hfmodelcpv1', '')
for hname in procs:
  hfmodelcpv1.Add(cpv1hists[hname])
  #print hname, 'addup numbers:', cpv1hists[hname].GetSumOfWeights()
hcpv1 = hfmodelcpv1.GetStack().Last().Clone('hcpv1')
hcpv1.SetLineColor(kViolet)
hcpv1.SetLineWidth(2)
hcpv1.SetFillColorAlpha(0, 0)
hcpv1.SetMarkerSize(0)

############# only need to prepare hfdata and dfmodel above #############

c = TCanvas("c", "canvas", 800, 700);

pad1 = TPad("pad1", "pad1", 0, 0.4, 1, 1.0);
pad1.SetBottomMargin(0.01)
pad1.Draw()
pad1.cd()

lg = TLegend(0.65, 0.6, 0.85, 0.90)
lg.SetFillColorAlpha(kBlue, 0)

for st in hfmodel.GetStack():
  if st.GetName() == 'spurious': continue
  lg.AddEntry(st, st.GetName().replace('cl_', ''), 'f')

ymax = hfdata.GetMaximum()
ymax = 2.0 * ymax
hfmodel.SetMaximum(ymax)
hfmodel.SetMinimum(10e-3)

hfmodel.Draw('hist')

#lg.AddEntry(hcpv1, 'VBF(#tilde{d}=0.07)', 'f')
#hcpv1.Draw('hist same')

lg.AddEntry(hfdata, 'data', 'lep')
hfdata.Draw('same e')

lg.SetBorderSize(0);
lg.Draw("same");

ATLASLabel(0.22,0.85,"Preliminary");
myText(0.22, 0.80, 1, "#sqrt{s}= 13 TeV, 139 fb^{-1}");
myText(0.22, 0.75, 1, "VBF CP H #rightarrow #it{#gamma#gamma}");
myText(0.22, 0.70, 1, "m_{#gamma#gamma} in [118, 132] GeV", 0.05); # SWcut

c.cd()
pad3 = TPad("pad3", "pad3", 0, 0.05, 1, 0.2)
pad3.SetTopMargin(0)
pad3.SetBottomMargin(0.2)
pad3.Draw()
pad3.cd()

rhdata = hfdata.Clone('ratio_data')
rhmodel = hfmodel.GetStack().Last().Clone('ratio_model')

bkgtmp = hfmodel.GetStack().Last().Clone('ratio_denominator')
for i in range(nbin):
  bkgtmp.SetBinError(i+1, 0)
#  rhdata.GetXaxis().SetBinLabel(i+1, binlabels[i])

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

###############################################################################################
############# below only need to prepare background subtracted hfdata and dfmodel #############
###############################################################################################

print ""
print "#################################### the middle pad #######################################"
print ""

mhfmodel = THStack('mhfmodel', '')

mhfcpv2 = TH1F('mhfcpv2', '', nbin, 0, nbin)
mhcpv2tmp = mhfcpv2
mhcpv2new = rebinHist(mhcpv2tmp, 20, rebinarray)
mhfcpv2 = mhcpv2new

mhfsig = TH1F('mhfsig', '', nbin, 0, nbin)
mhsigtmp = mhfsig
mhsignew = rebinHist(mhsigtmp, 20, rebinarray)
mhfsig = mhsignew

mhfdata = TH1F('mhfdata', '', nbin, 0, nbin)
mhdatatmp = mhfdata
mhdatanew = rebinHist(mhdatatmp, 20, rebinarray)
mhfdata = mhdatanew

mbkghists = {}
for proc in procs:
  hname = proc
  print hname
  mbkghists[hname] = TH1F(hname, '', nbin, 0, nbin)
  htmp = mbkghists[hname]
  hnew = rebinHist(htmp, 20, rebinarray)
  mbkghists[hname] = hnew
  mbkghists[hname].SetFillColorAlpha(TColor.GetColor(colors[hname]), 1)
  mbkghists[hname].SetLineWidth(0)

cpv2hists = {}
for proc in procs:
  hname = proc
  print hname
  cpv2hists[hname] = TH1F(hname, '', nbin, 0, nbin)
  htmp = cpv2hists[hname]
  hnew = rebinHist(htmp, 20, rebinarray)
  cpv2hists[hname] = hnew
  cpv2hists[hname].SetFillColor(TColor.GetColor(colors[hname]))
  cpv2hists[hname].SetLineWidth(0)

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

  mhfdata.Add(hdata, weight)
  mhfsig.Add(hsig, weight)

  for hname in procs:
    htmp = fmat.Get('cl_'+hname)
    mbkghists[hname].Add(htmp, weight)

  fcpv2 = TFile("plotsPostfit/plotMaterials_"+cat+"_p07_SW_subBkg.root", 'read') # subBkg
  for hname in procs:
    htmp = fcpv2.Get('cl_'+hname)
    if hname == 'VBF': htmp.SetFillColorAlpha(0, 0)
    cpv2hists[hname].Add(htmp, weight)
  hcpv2 = fcpv2.Get('cl_VBF').Clone('hsig_'+cat) # subBkg
  mhfcpv2.Add(hcpv2, weight)

print 'data addup numbers:', mhfdata.GetSumOfWeights()

for hname in procs:
  mhfmodel.Add(mbkghists[hname])
  print hname, 'addup numbers:', mbkghists[hname].GetSumOfWeights()

mhfmodelcpv2 = THStack('mhfmodelcpv2', '')
for hname in procs:
  mhfmodelcpv2.Add(cpv2hists[hname])
  #print hname, 'addup numbers:', cpv2hists[hname].GetSumOfWeights()
#hcpv2 = mhfmodelcpv2.GetStack().Last().Clone('hcpv2')
mhfcpv2.SetLineColor(kViolet)
mhfcpv2.SetLineWidth(2)
mhfcpv2.SetFillColorAlpha(0, 0)
mhfcpv2.SetMarkerSize(0)

############# below draw background subtracted mhfdata and dfmodel #############

c.cd()
pad2 = TPad("pad2", "pad2", 0, 0.2, 1, 0.4)
pad2.SetTopMargin(0)
pad2.SetBottomMargin(0.03)
pad2.Draw()
pad2.cd()

ymax = mhfdata.GetMaximum()
ymax = 1.6 * ymax
mhfsig.SetMaximum(ymax)
mhfsig.SetMinimum(1e-2)

mhfsig.SetFillColorAlpha(TColor.GetColor(colors['VBF']), 1)
mhfsig.SetLineWidth(0)
mhfsig.Draw('hist')

mhfcpv2.Draw('hist same')
lg.AddEntry(mhfcpv2, 'VBF(#tilde{d}=0.07)', 'f')

mhfdata.Draw('same e')

c.SaveAs("plotsPostfit/ooPostfit_SW_addup.png")
c.SaveAs("plotsPostfit/ooPostfit_SW_addup.pdf")
