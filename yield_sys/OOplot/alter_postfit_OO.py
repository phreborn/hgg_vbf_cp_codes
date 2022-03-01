#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
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

parser = argparse.ArgumentParser()
parser.add_argument('-c', '--bdtcat', type=str, default='TT')
parser.add_argument('-d', '--dval', type=str, default='m00')
parser.add_argument('--rebin', action="store_true")

result = parser.parse_args()
bdtcat = result.bdtcat
dval = result.dval

yScales = {'TT':3, 'TL':2, 'LT':4}

samples = ['VBF_RW', 'ggH', 'background', 'spurious']
poiname = 'mu_VBF_RW'
colors = {
'ggH':'#400D12',
'background':'#FF0000',
'spurious':'#663366',
}

class Syst:
  'systematic'
  def __init__(self, name, constrType, central, *mag):
    self.name = name
    self.constr = constrType
    self.central = central
    self.mag = mag
    self.posthat = -99
    self.postmag = -99

class Sample:
  'sample'
  def __init__(self, name, yld, syslist):
    self.name = name
    self.yld = yld
    self.syslist = syslist
    self.postImplmented = False
    self.yldhat = 0
    self.quadrSysVar = 0

  def getPostHatAndMag(self, sys, fitResult):
      sysname = sys.name
      constr = sys.constr

      preup = sys.mag[0]
      predn = sys.mag[1] if constr == 'asym' else 0. 
      if preup < 0 and constr != 'asym':
        print 'WARNING: negative non-asym mag:', preup, 'using absolute value'
        preup = abs(preup)

      globhat = fitResult[sysname][0]
      posthat = globhat*preup
      if constr == 'asym':
        posthat = globhat*preup if globhat>=0 else abs(globhat)*predn # !!

      posterr = fitResult[sysname][1]
      postup, postdn = posterr*preup, posterr*predn
      postmag = postup
      ## this is wrong but for now acceptable..
      if constr == 'asym':
        postmag = abs(postup-posthat) if abs(postup-posthat)>abs(posthat-postdn) else abs(posthat-postdn) # !!

      return posthat, postmag

  def implementPostResult(self, fitResult):
    yldhat = self.yld
    for sys in self.syslist:
      sys.posthat, sys.postmag = self.getPostHatAndMag(sys, fitResult)
    self.postImplemented = True

  def getYldHat(self):
    yldhat = self.yld
    if self.postImplemented:
      for sys in self.syslist:
        central = sys.central
        if central == 1: yldhat *= (1+sys.posthat)
        else: print 'WARNING: central value != 1'
      self.yldhat = yldhat
      return yldhat
    else:
      print 'ERROR: posthat not implemented!'
      sys.exit(0)

def clearBinError(hist):
  for ib in range(hist.GetNbinsX()): hist.SetBinError(ib+1, 0)

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

### get post fit params
#frslt = TFile("/scratchfs/atlas/huirun/atlaswork/VBF_CP/WSBuilder/xmlAnaWSBuilder/run/outTT_allSys/out_"+dval+".root", 'read')
frslt = TFile("/scratchfs/atlas/huirun/atlaswork/VBF_CP/WSBuilder/xmlAnaWSBuilder/run/outAllCats_allSys/out_"+dval+".root", 'read')
fitResult = frslt.Get("fitResult");
paras = {}
for par in fitResult.floatParsFinal():
  pname = par.GetName()
  phat = par.getVal()
  perr = par.getError()
  #if 'EG_' in pname and 'ALL' not in pname: continue
  if 'p1_' in pname or 'p2_' in pname: continue
  #print pname, phat, perr
  paras[pname] = [phat, perr]

### get yields and prefit mags from xml
binlabels = ['-99:-2', '-2:-1', '-1:0', '0:1', '1:2', '2:99']
#bdtcats = ['TT', 'TL', 'LT', 'LL']
nbin = len(binlabels)

doRebin = result.rebin
xrebins = [-10, -2, -1, 0, 1, 2, 10]

rebinarray = array('d')
for edge in xrebins:
  rebinarray.append(edge)

# get cats name
path_cfgCats="../../../nom_WS/cats.cfg"
cats=[]
with open(path_cfgCats,'r') as f:
  for line in f.readlines():
    if '#' in line or ':' not in line: continue
    cat=line.split(':')[0]
    #print cat
    cats.append(cat)

# get bkg fraction in signal window
bkgFracs = {}
with open("bkgSRFrac_%s.txt"%(dval), 'r') as f:
  for line in f.readlines():
    for cat in cats:
      if cat not in line: continue
      lattr = line.replace('\n', '').split(',')
      frac = float(lattr[1])
      bkgFracs[cat] = frac

oobins = {}
for i in range(nbin):
  binname = 'b'+str(i+1)
  catname = bdtcat+'_'+binname
  print '\n\n'+catname
  xmlPath = "../../../WSBuilder/xmlAnaWSBuilder/run/configAllCats/vbf_cp_%s/channel/category_OO_%s.xml"%(dval, catname)
  xmlPath = "configAllCats/vbf_cp_%s/channel/category_OO_%s.xml"%(dval, catname) # SWcut
  print xmlPath
  xtree = ET.parse(xmlPath)
  xroot = xtree.getroot()

  # get common syst
  commonSyst = []
  for sys in xtree.getroot().findall('Systematic'):
    if 'yield' != sys.attrib['WhereTo']: continue
    name = sys.attrib['Name'].replace(':category:', 'OO_'+catname)
    mag = [float(num) for num in sys.attrib['Mag'].split(',')]
    central = float(sys.attrib['CentralValue'])
    constr = sys.attrib['Constr']
    sys = Syst(name, constr, central, *mag)
    commonSyst.append(sys)

  ## check commonSyst
  #for sys in commonSyst:
  #  print sys.name, sys.constr, sys.mag
  
  # get syst in individual MC channel
  sampleList = {}
  for proc in xroot.iter('Sample'):
    procname = proc.attrib['Name']
    sysList = []
    yld = 0.
    for child in proc:
      if child.tag == 'NormFactor' and 'yield_' in child.attrib['Name']:
        yld = float(child.attrib['Name'].split('[')[1].replace(']', ''))
      if child.tag == 'NormFactor' and 'nbkg_' in child.attrib['Name']:
        yld = float(child.attrib['Name'].split('[')[1].replace(']', '').split(',')[0])
      if child.tag != 'Systematic': continue
      name = child.attrib['Name'].replace(':category:', 'OO_'+catname)
      mag = [float(num) for num in child.attrib['Mag'].split(',')]
      central = float(child.attrib['CentralValue'])
      constr = child.attrib['Constr']
      sys = Syst(name, constr, central, *mag)
      sysList.append(sys)
    sysList.extend(commonSyst)
    sample = Sample(procname, yld, sysList)
#    sample.implementPostResult(paras);
    sampleList[procname] = sample

  ## adding continuum process
  bkgyld = paras['nbkg_OO_'+catname][0]
  bkgyld = paras['nbkg_OO_'+catname][0] * bkgFracs[catname] # SWcut
  bkgquadrSysVar = abs(paras['nbkg_OO_'+catname][1])
  sampleList['background'].yld = bkgyld
  sampleList['background'].quadrSysVar = bkgquadrSysVar
  #print sampleList['background'].name, [sys.name for sys in sampleList['background'].syslist]

  ### adding spurious process
  ## Todo: check if any issue in the implementation of spurious
  spurMag = 0
  for sys in sampleList['spurious'].syslist:
    if 'ATLAS_Hgg_BIAS_' in sys.name:
      spurMag = sys.mag[0]
      print 'spur mag:',spurMag
  spuryld = paras['ATLAS_Hgg_BIAS_OO_'+catname][0]*spurMag
  spurquadrSysVar = abs(paras['ATLAS_Hgg_BIAS_OO_'+catname][1]*spurMag)
  sampleList['spurious'].yld = spuryld
  sampleList['spurious'].quadrSysVar = spurquadrSysVar
  print sampleList['spurious'].name, [sys.name for sys in sampleList['spurious'].syslist]

  ## check importings from xml
  #for proc in sampleList:
  #  print proc.name, proc.yld
  #  for sys in proc.syslist:
  #    print sys.name, sys.constr, paras[sys.name], sys.mag
  #    if proc.postImplemented: print sys.posthat, sys.postmag

  ### POI post fit

  ### summing up sys variations in quadratic
  for proc in sampleList.values():
    sample = proc.name
    if sample not in samples: continue
    print '\n'+sample

    proc.implementPostResult(paras);

    yld = proc.yld
    print 'yld, yldhat: %.9f, %.9f'%(yld, proc.getYldHat())

    yldhat = proc.yldhat
    sqVar = 0
    for sys in proc.syslist:
      if sys.central != 1: continue
      varYield = yldhat*sys.postmag/(1+sys.posthat) # !!
      print sys.name, sys.postmag/(1+sys.posthat), varYield
      sqVar += varYield*varYield
    sqrtVar = sqVar**0.5
    print 'initial quadrSysVar:', proc.quadrSysVar
    proc.quadrSysVar = pow(pow(sqrtVar, 2)+pow(proc.quadrSysVar, 2), 0.5)
    print yldhat, sqVar, sqrtVar, 'final quadrSysVar:', proc.quadrSysVar

  oobins[binname] = sampleList

### get asimov data yields
hdata = TH1F('Asimov_data', '', nbin, 0, nbin)
for i in range(nbin):
  binname = 'b'+str(i+1)
  catname = bdtcat+'_'+binname
#  with open("data_observed.txt", 'r') as f:
  with open("data_observed_SR.txt", 'r') as f: # SWcut
    for line in f.readlines():
      if catname not in line: continue
      ndata = float(line.split(':')[1])
      hdata.SetBinContent(i+1, ndata)
      hdata.SetBinError(i+1, ndata**0.5)
  #print catname, hdata.GetBinContent(i+1)

if doRebin:
  hdatatmp = hdata
  hdatanew = rebinHist(hdatatmp, 20, rebinarray)
  hdata = hdatanew

### fill histograms
hists = {}
for proc in oobins[binname].values():
  if proc.name not in samples: continue
  hists[proc.name] = TH1F(proc.name, '', nbin, 0, nbin)

for i in range(nbin):
  binname = 'b'+str(i+1)
  sumyld = 0
  sumSqVar = 0
  for proc in oobins[binname].values():
    if proc.name not in samples: continue
#    hists[proc.name].GetXaxis().SetBinLabel(i+1, binlabels[i])
    yldhat = proc.yldhat
    quadrSysVar = proc.quadrSysVar
    # !! signal POI best fit and error
    if proc.name == 'VBF_RW':
      yldMuErr = abs(paras[poiname][1])*yldhat
      quadrSysVar = pow(pow(quadrSysVar, 2) + pow(yldMuErr, 2), 0.5)
      yldhat *= paras[poiname][0]
      print 'VBF yield:', yldhat, 'variation by poi:',yldMuErr, 'variation by sys:', proc.quadrSysVar, 'tot variation: ', quadrSysVar
    hists[proc.name].SetBinContent(i+1, yldhat)
    hists[proc.name].SetBinError(i+1, quadrSysVar)

if doRebin:
  for proc in oobins[binname].values():
    if proc.name not in samples: continue
    htmp = hists[proc.name]
    hnew = rebinHist(htmp, 20, rebinarray)
    hists[proc.name] = hnew

### drawing
fout = TFile("plotsPostfit/plotMaterials_"+bdtcat+"_"+dval+"_SW_subBkg.root", "recreate")
c = TCanvas("c", "canvas", 800, 800);

pad1 = TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
pad1.SetBottomMargin(0)
pad1.Draw()
pad1.cd()

lg = TLegend(0.65, 0.7, 0.85, 0.90)
lg.SetFillColorAlpha(kBlue, 0)

Sig = hists['VBF_RW'].Clone('cl_VBF')
Sig.SetFillColor(TColor.GetColor('#4FD5D6'))
Sig.SetLineWidth(0)
lg.AddEntry(Sig, 'VBF ('+dval+')', 'f')

### VBF only, background removed
hdata_minus_bkg = hdata.Clone("data_minus_bkg")
hdata_minus_bkg.SetLineWidth(1)

Bkg = THStack("hs", "");
for hname in hists.keys():
  if hname == 'VBF_RW': continue
  htmp = hists[hname].Clone('cl_'+hname)
  htmp.SetFillColor(TColor.GetColor(colors[hname]))
  htmp.SetLineWidth(0)
  fout.cd()
  htmp.Write()
  Bkg.Add(htmp)
  if hname != 'VBF_RW':
    # TODO discuss minus bkg MC w/ or w/o uncer.
    #hdata_minus_bkg.Add(htmp, -1)
    hnoerr = htmp.Clone()
    clearBinError(hnoerr)
    hdata_minus_bkg.Add(hnoerr, -1)
  if hname == 'spurious': continue
#  lg.AddEntry(htmp, hname, "f")
fout.cd()
Bkg.GetStack().Last().Clone('cl_bkgs').Write()
Bkg.Add(Sig)

ymax = Sig.GetMaximum()
ymax = 2.0 * ymax
Sig.SetMaximum(ymax)
Sig.SetMinimum(0)
print 'oo mean', bdtcat, hdata_minus_bkg.GetMean()-3, '+/-', hdata_minus_bkg.GetMeanError()

#Bkg.GetYaxis().SetTitle('nEvents')

Sig.Draw('hist')

lg.AddEntry(hdata_minus_bkg, 'Asimov data', 'lep')
hdata_minus_bkg.Draw('same e')

lg.SetBorderSize(0);
lg.Draw("same");

ATLASLabel(0.22,0.85,"Preliminary");
myText(0.22, 0.80, 1, "#sqrt{s}= 13 TeV, 139 fb^{-1}");
myText(0.22, 0.75, 1, "VBF CP H #rightarrow #it{#gamma#gamma}, "+bdtcat);
myText(0.22, 0.70, 1, "m_{#gamma#gamma} in [118, 132] GeV", 0.05); # SWcut

c.cd()
pad2 = TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
pad2.SetTopMargin(0)
pad2.SetBottomMargin(0.2)
pad2.Draw()
pad2.cd()

rhdata = hdata_minus_bkg.Clone('ratio_data')
errmodel = Bkg.GetStack().Last().Clone('error_of_model')
rhmodel = Sig.Clone('ratio_signal')

bkgtmp = Sig.Clone('ratio_denominator')
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

yscale = yScales[bdtcat]
rhmax = rhdata.GetMaximum()
rhmin = rhdata.GetMinimum()
rhdata.SetMaximum(rhmax*yscale)
rhdata.SetMinimum(1-abs(rhmax*yscale-1))
rhdata.Draw('ep')
rhmodel.Draw('same e2')

c.SaveAs("plotsPostfit/alter_ooPostfit_"+bdtcat+"_"+dval+"_SW.png")
c.SaveAs("plotsPostfit/alter_ooPostfit_"+bdtcat+"_"+dval+"_SW.pdf")

fout.cd()

Sig.Write()
pad1.Write()
Bkg.Write()
hdata_minus_bkg.Write()
lg.Write()
pad2.Write()
rhdata.Write()
rhmodel.Write()
