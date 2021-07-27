#include "sysUtils.h"

//#include "HggTwoSidedCBPdf.cxx"
//#include "HggTwoSidedCBPdf.h"
#include "RooTwoSidedCBShape.cxx"
#include "RooTwoSidedCBShape.h"

#include "RooWorkspace.h"
#include "RooAbsData.h"
#include "RooPlot.h"

#include "TCanvas.h"

using namespace RooFit;

void variPlot_idvProc(TString sys, TString prc){
  TString workarea = "/scratchfs/atlas/huirun/atlaswork/VBF_CP/syst/shape_sys/plotFit/";

  char *cf_cats = (char*)"../../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  TFile *fnom = new TFile(workarea+"/ws_Nominal_m00.root", "read");

  TFile *fup = new TFile(workarea+"/ws_"+sys+"__1up_m00.root", "read");
  TFile *fdown = new TFile(workarea+"/ws_"+sys+"__1down_m00.root", "read");

  for(auto cat : catCuts){
    //if(cat.first != "TT_b1") continue;
    TCanvas *canv = new TCanvas("c", "canvas", 1000, 600);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0., 0.7, 1.0);
    TPad *pad2 = new TPad("pad2", "pad2", 0.7, 0.5, 1, 1.0);
    TPad *pad3 = new TPad("pad3", "pad3", 0.7, 0., 1, 0.5);

    TLegend* lg = new TLegend(0.4, 0.75, 0.65, 0.9);

    pad1->Draw();
    pad1->cd();

    RooWorkspace *wsnom = (RooWorkspace*)fnom->Get("ws");
    wsnom->var("m_yy")->setRange("SR", 118000, 132000);
    RooPlot* myy_frame = wsnom->var("m_yy")->frame(Title(prc+" "+cat.first));
    wsnom->pdf(("sig_"+cat.first).Data())->fitTo(*(wsnom->data(("dh_myy_"+prc+"_"+cat.first).Data())));
    wsnom->data(("dh_myy_"+prc+"_"+cat.first).Data())->plotOn(myy_frame, MarkerSize(0.1), DataError(RooAbsData::None), Binning(550));
    wsnom->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame, LineWidth(1), Range(118000, 132000));
  
    RooWorkspace *wsup = (RooWorkspace*)fup->Get("ws");
    //wsup->data(("dh_myy_sig_"+cat.first).Data())->plotOn(myy_frame, MarkerSize(0.25));
    wsup->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame, LineStyle(kDashed), LineColor(kGreen), LineWidth(1));
  
    RooWorkspace *wsdown = (RooWorkspace*)fdown->Get("ws");
    //wsdown->data(("dh_myy_sig_"+cat.first).Data())->plotOn(myy_frame, MarkerSize(0.25));
    wsdown->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame, LineStyle(kDashed), LineColor(kViolet), LineWidth(1), Range(118000, 132000));

    myy_frame->Draw();

    canv->cd();

    pad2->Draw();
    pad2->cd();
    RooPlot* myy_frame2 = wsnom->var("m_yy")->frame(Title(sys+"_up"));
    wsup->pdf(("sig_"+cat.first).Data())->fitTo(*(wsup->data(("dh_myy_"+prc+"_"+cat.first).Data())));
    wsup->data(("dh_myy_"+prc+"_"+cat.first).Data())->plotOn(myy_frame2, MarkerSize(0.25));
    wsup->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame2, LineStyle(kDashed), LineColor(kGreen), LineWidth(1));
    myy_frame2->Draw();

    canv->cd();

    pad3->Draw();
    pad3->cd();
    RooPlot* myy_frame3 = wsnom->var("m_yy")->frame(Title(sys+"_down"));
    wsdown->pdf(("sig_"+cat.first).Data())->fitTo(*(wsdown->data(("dh_myy_"+prc+"_"+cat.first).Data())));
    wsdown->data(("dh_myy_"+prc+"_"+cat.first).Data())->plotOn(myy_frame3, MarkerSize(0.25));
    wsdown->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame3, LineStyle(kDashed), LineColor(kViolet), LineWidth(1));
    myy_frame3->Draw();

    canv->SaveAs("shapeVariation/plot_"+prc+"_"+sys+"_m00_"+cat.first+".png");
    canv->SaveAs("shapeVariation/plot_"+prc+"_"+sys+"_m00_"+cat.first+".pdf");
  }
}

int main(int argc, char* argv[]){
  TString sys = TString(argv[1]);
  std::vector<TString> procs = {"ggH", "VBF"};
  for(auto prc : procs) variPlot_idvProc(sys, prc);
  return 1;
}
