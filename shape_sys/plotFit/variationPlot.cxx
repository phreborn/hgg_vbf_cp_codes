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

void variationPlot(TString sys){
  TString workarea = "/scratchfs/atlas/chenhr/atlaswork/VBF_CP/syst/shape_sys/plotFit/";

  char *cf_cats = (char*)"../../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  TFile *fnom = new TFile(workarea+"/ws_Nominal_m00.root", "read");

  TFile *fup = new TFile(workarea+"/ws_"+sys+"__1up_m00.root", "read");
  TFile *fdown = new TFile(workarea+"/ws_"+sys+"__1down_m00.root", "read");

  for(auto cat : catCuts){
    if(cat.first != "TT_b2") continue;
    TCanvas *canv = new TCanvas("c", "canvas", 800, 600);
  
    RooWorkspace *wsnom = (RooWorkspace*)fnom->Get("ws");
    RooPlot* myy_frame = wsnom->var("m_yy")->frame(Title("fit to m_yy distribution"));
    wsnom->data(("dh_myy_sig_"+cat.first).Data())->plotOn(myy_frame);
    //wsnom->Print();
    wsnom->pdf(("sig_"+cat.first).Data())->plotOn(myy_frame);

    myy_frame->Draw();

    canv->SaveAs("plot_"+sys+"_m00_"+cat.first+".png");
  }
}

int main(int argc, char* argv[]){
  TString sys = TString(argv[1]);
  variationPlot(sys);
  return 1;
}
