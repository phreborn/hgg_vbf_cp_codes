#ifdef __CLING__
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.C"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.C"
#endif

void plotOOCompare(){
  SetAtlasStyle();
  gStyle->SetOptStat(0);
  gStyle->SetErrorX(0.5);

  std::vector<TString> bdtCats = {"TT", "TL", "LT", "LL"};

  for(auto cat : bdtCats){
    TFile *fv = new TFile(Form("VBF_oo_hists_m00_%s.root", cat.Data()), "read");
    TFile *fvsys = new TFile(Form("syst_VBF_oo_hists_%s.root", cat.Data()), "read");
    TFile *fvstat = new TFile(Form("stat_VBF_oo_hists_%s.root", cat.Data()), "read");
    TFile *fvm = new TFile(Form("VBF_oo_hists_m02_%s.root", cat.Data()), "read");
    TFile *fvp = new TFile(Form("VBF_oo_hists_p02_%s.root", cat.Data()), "read");
    TFile *fg = new TFile(Form("ggF_oo_hists_SM_%s.root", cat.Data()), "read");
    TFile *fy = new TFile(Form("diphoton_oo_hists_%s.root", cat.Data()), "read");

    TH1F *hv = (TH1F*) fv->Get("hoo");
    TH1F *hv_sys = (TH1F*) fvsys->Get("hoo");
    TH1F *hv_stat = (TH1F*) fvstat->Get("hoo");
    TH1F *hvm = (TH1F*) fvm->Get("hoo");
    TH1F *hvp = (TH1F*) fvp->Get("hoo");
    TH1F *hg = (TH1F*) fg->Get("hoo");
    TH1F *hy = (TH1F*) fy->Get("hoo");

    hv->Scale(1./hv->Integral());
    hv_sys->Scale(1./hv_sys->Integral());
    hv_stat->Scale(1./hv_stat->Integral());
    hvm->Scale(1./hvm->Integral());
    hvp->Scale(1./hvp->Integral());
    hg->Scale(1./hg->Integral());
    hy->Scale(1./hy->Integral());

    hv->SetMinimum(0.);
    hv->SetMaximum((hv->GetMaximum())*1.8);

    TLegend *lg = new TLegend(0.19, 0.65, 0.57, 0.92);
    lg->SetFillStyle(0);
    lg->SetBorderSize(0);

    TCanvas *canv = new TCanvas("c", "canvas", 800, 600);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();

    hv->SetStats(0);
    hv->GetYaxis()->SetTitle("A.U.");
    hv->GetYaxis()->SetTitleOffset(1);
    hv->Draw("hist");

    hvm->SetLineColor(kViolet-2);
    hvm->SetLineStyle(kDashed);
    hvm->Draw("same hist");

    hvp->SetLineColor(kGreen+2);
    hvp->SetLineStyle(kDashed);
    hvp->Draw("same hist");

    hg->SetLineColor(kRed+1);
    hg->Draw("same hist");

    hy->SetLineColor(kBlue+1);
    hy->Draw("same hist");

    lg->AddEntry(hv, "VBF", "l");
    lg->AddEntry(hvm, "VBF #tilde{d}=-0.02", "l");
    lg->AddEntry(hvp, "VBF #tilde{d}=+0.02", "l");
    lg->AddEntry(hg, "ggF", "l");
    lg->AddEntry(hy, "diphoton", "l");
    lg -> SetFillStyle(0);
    lg -> SetBorderSize(0);
    lg->Draw("same");

    canv->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.11);
    pad2->Draw();
    pad2->cd();

    TH1F *hv_noErr = (TH1F*) hv->Clone(Form("%s_noErr", hv->GetName()));
    for(int i=1; i<=hv_noErr->GetNbinsX(); i++){
      hv_noErr->SetBinError(i, 0.);
    }

    TH1F *rhv = (TH1F*) hv->Clone("rhv");
    TH1F *rhv_sys = (TH1F*) hv_sys->Clone("rhv_sys");
    TH1F *rhv_stat = (TH1F*) hv_stat->Clone("rhv_stat");
    TH1F *rhvm = (TH1F*) hvm->Clone("rhv");
    TH1F *rhvp = (TH1F*) hvp->Clone("rhv");
    TH1F *rhg = (TH1F*) hg->Clone("rhg");
    TH1F *rhy = (TH1F*) hy->Clone("rhy");

    rhv->Divide(hv_noErr);
    rhv_sys->Divide(hv_noErr);
    rhv_stat->Divide(hv_noErr);
    rhvm->Divide(hv_noErr);
    rhvp->Divide(hv_noErr);
    rhg->Divide(hv_noErr);
    rhy->Divide(hv_noErr);

    rhv->SetMinimum(-2);
    rhv->SetMaximum(4);
    rhv->SetStats(0);
    rhv->GetXaxis()->SetTitle("OO1");
    rhv->GetXaxis()->SetTitleSize((hv->GetXaxis()->GetTitleSize())*7/3);
    rhv->GetXaxis()->SetTitleOffset(0.3);
    rhv->GetYaxis()->SetTitle("ratio");
    rhv->GetYaxis()->SetTitleSize((hv->GetYaxis()->GetTitleSize())*7/3);
    rhv->GetYaxis()->SetTitleOffset(0.4);
    rhv->SetLineWidth(0);
    rhv->GetXaxis()->SetLabelSize(0.15);
    rhv->GetYaxis()->SetLabelSize((hv->GetYaxis()->GetLabelSize())*7/3);
    rhv->GetYaxis()->SetNdivisions(505);
    rhv->Draw("hist");

    rhv_sys->SetMarkerSize(0);
    rhv_sys->SetFillStyle(3325);
    rhv_sys->SetFillColor(kRed);
    rhv_sys->Draw("same e2");

    rhv_stat->SetMarkerSize(0);
    rhv_stat->SetFillStyle(3352);
    rhv_stat->SetFillColor(kGray+2);
    rhv_stat->Draw("same e2");

    rhvm->SetLineColor(kViolet-2);
    rhvm->SetLineStyle(kDashed);
    rhvm->Draw("same hist");

    rhvp->SetLineColor(kGreen+2);
    rhvp->SetLineStyle(kDashed);
    rhvp->Draw("same hist");

    rhg->SetLineColor(kRed+1);
    rhg->Draw("same hist");

    rhy->SetLineColor(kBlue+1);
    rhy->Draw("same hist");

    lg->AddEntry(rhv_sys, "stat.+syst. uncer.", "f");
    lg->AddEntry(rhv_stat, "stat. uncer.", "f");

    hv->GetXaxis()->SetTitleSize(0);
    hv->GetXaxis()->SetLabelSize(0);
    //for(int i=1; i<=hv->GetNbinsX(); i++){
    //  hv->GetXais()->SetBinLabel(i, 0.);
    //}

    canv->SaveAs("ooPlot_"+cat+".png");
  }
}
