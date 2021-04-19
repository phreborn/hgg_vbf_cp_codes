#include "TROOT.h"
#include "TLegend.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"

#include <iostream>

void variationPlot(TString id, TString sys, TString cat, TString dname){
  //TString sys = "JET_EtaIntercalibration_Modelling";
  //TString id = "343981";

  gROOT->SetBatch();

  TFile *f = new TFile("hist_"+id+"_"+sys+".root", "read");

  //TString cat = "TT_b1";
  //TString dname = "SM";

  TH1F *hnom = (TH1F*) f->Get("Nominal_"+dname+"_"+cat);
  TH1F *hup = (TH1F*) f->Get(sys+"__1up_"+dname+"_"+cat);
  TH1F *hdown = (TH1F*) f->Get(sys+"__1down_"+dname+"_"+cat);

  TLegend *lg = new TLegend(0.5, 0.7, 0.88, 0.88);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->AddEntry(hnom, "nominal", "l");
  lg->AddEntry(hup, sys+"_up", "l");
  lg->AddEntry(hdown, sys+"_down", "l");

  hnom->Rebin(10);
  hup->Rebin(10);
  hdown->Rebin(10);

  float ymax = hnom->GetMaximum();
  if(hup->GetMaximum() > ymax) ymax = hup->GetMaximum();
  if(hdown->GetMaximum() > ymax) ymax = hdown->GetMaximum();
  hnom->SetMaximum(ymax*1.09);

  hnom->SetStats(0);

  hup->SetLineStyle(kDashed);
  hdown->SetLineStyle(kDashed);

  hup->SetLineColor(kBlue);
  hdown->SetLineColor(kRed);

  TCanvas *canv = new TCanvas("c", "canvas", 800, 600);
  hnom->Draw("hist");
  hup->Draw("same hist");
  hdown->Draw("same hist");
  lg->Draw("same");
  canv->SaveAs("histVari_"+id+"_"+sys+"_"+cat+"_"+dname+".png");

  delete f;
  delete lg;
  delete canv;
}

int main(int argc, char* argv[]){
  if(argc < 4){
    std::cout<<"para num less than 4, quit."<<std::endl;
    return 1;
  }
  TString id = TString(argv[1]);
  TString sys = TString(argv[2]);
  TString cat = TString(argv[3]);
  TString dname = TString(argv[4]);
  variationPlot(id, sys, cat, dname);
  return 0;
}
