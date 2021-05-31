#ifdef __CLING__
#include "/scratchfs/atlas/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.C"
#include "/scratchfs/atlas/chenhr/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.C"
#endif

#include "../../shape_sys/sysUtils.h"

void ggF(){
   SetAtlasStyle();
   gStyle->SetOptStat(0);
   gStyle->SetErrorX(0.5);

  char *cf_cats = (char*)"../../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  map<TString, double> d_map;
  d_map["m00"] = 0.;
  //d_map["m01"] = -0.01;
  //d_map["m02"] = -0.02;
  //d_map["m03"] = -0.03;
  //d_map["m04"] = -0.04;
  //d_map["m05"] = -0.05;
  //d_map["m06"] = -0.06;
  //d_map["m07"] = -0.07;
  //d_map["m08"] = -0.08;
  //d_map["m10"] = -0.10;
  //d_map["m12"] = -0.12;
  //d_map["m14"] = -0.14;
  //d_map["m16"] = -0.16;
  //d_map["m18"] = -0.18;
  //d_map["m20"] = -0.20;
  //d_map["p01"] = 0.01;
  //d_map["p02"] = 0.02;
  //d_map["p03"] = 0.03;
  //d_map["p04"] = 0.04;
  //d_map["p05"] = 0.05;
  //d_map["p06"] = 0.06;
  //d_map["p07"] = 0.07;
  //d_map["p08"] = 0.08;
  //d_map["p10"] = 0.10;
  //d_map["p12"] = 0.12;
  //d_map["p14"] = 0.14;
  //d_map["p16"] = 0.16;
  //d_map["p18"] = 0.18;
  //d_map["p20"] = 0.20;

  std::vector<TString> bdtCats = {"TT", "TL", "LT", "LL"};

  std::map<int, TString> bins;
  bins[1] = "-99:-2";
  bins[2] = "-2:-1";
  bins[3] = "-1:0";
  bins[4] = "0:1";
  bins[5] = "1:2";
  bins[6] = "2:99";

  std::map<TString, TString> ids;
  //ids["vbf"] = "346214";
  ids["ggh"] = "343981";

  std::map<TString, double> dtmp;


  for(auto i : ids){
    TString sample = i.first; cout<<sample<<endl;
    TString id = i.second;
    if(sample=="vbf") dtmp = d_map;
    else dtmp = {{"SM", 0.0}};

    for(auto d : dtmp){
      TString dName = d.first;

      //for(auto c : catCuts){
      //  TString cat = c.first;
      for(auto c : bdtCats){
        TString cat = c;

        TFile *fout = new TFile(Form("ggF_oo_hists_%s_%s.root", dName.Data(), cat.Data()), "recreate");

        TH1F *hoo = new TH1F("hoo", "", 6, -3, 3);
        TAxis *axis = hoo->GetXaxis();
        TH1F *hoo_sys = new TH1F("hoo_sys", "", 6, -3, 3);
        TH1F *hoo_stat = new TH1F("hoo_stat", "", 6, -3, 3);
      for(auto b : bins){
        TString bname = b.second;
        int bnum = b.first;
        TString bin = TString(Form("b%i", bnum));

        std::vector<TString> sys;
        std::vector<float> vn;
        std::vector<float> ve;
        std::vector<float> vu;
        std::vector<float> vd;
      
        ifstream file;
        file.open(Form("../csv/yield_%s_%s_%s_%s.csv", id.Data(), dName.Data(), cat.Data(), bin.Data()));
        if( ! file.is_open())
        {
            cout<<"can not open file! "<<Form("../csv/yield_%s_%s_%s_%s.csv", id.Data(), dName.Data(), cat.Data(), bin.Data())<<endl;
            return;
        }
        char tmp[1000];
        while(!file.eof()){
          file.getline(tmp,1000);
          string line(tmp);
          size_t ptmp;
      
          size_t pos = line.find(','); std::cout<<line.substr(0,pos)<<std::endl;
          if(pos!=string::npos && line != "") sys.push_back(line.substr(0,pos).data());
          else continue;
      
          ptmp = pos;
          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
          if(pos!=string::npos && line != "") vn.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else vn.push_back(0.);
      
          ptmp = pos;
          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
          if(pos!=string::npos && line != "") ve.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else ve.push_back(0.);
      
          ptmp = pos;
          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
          if(pos!=string::npos && line != "") vu.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else vu.push_back(0.);
      
          ptmp = pos;
          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
          if(line != "") vd.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else vd.push_back(0.);
        }
      
        gStyle->SetPadBottomMargin(0.32);

        axis->SetBinLabel(bnum, bname);
        hoo->SetBinContent(bnum, vn[0]);
        hoo_sys->SetBinContent(bnum, vn[0]);
        hoo_stat->SetBinContent(bnum, vn[0]);
      
        int nsys = sys.size();
        //TH1F *hu = new TH1F("var_up", "", nsys, 0, nsys);
        //TH1F *hd = new TH1F("var_down", "", nsys, 0, nsys);
        //TAxis *axis = hu->GetXaxis();
        float sqErr = 0.;
        for(int i = 0; i < nsys; i++){
          float errui = std::abs(vu[i]-vn[i]);
          float errdi = std::abs(vd[i]-vn[i]);
          float erri = errui > errdi? errui : errdi;
          sqErr += erri*erri;
          //axis->SetBinLabel(i+1, sys[i]);
          //hu->Fill(i, vu[i]);
          //hd->Fill(i, vd[i]);
          cout<<sys[i]<<": "<<vu[i]<<", "<<vd[i]<<endl;
        }
        float sErr = std::sqrt(sqErr); cout<<cat+"_"+bin<<", "<<sErr<<endl;
        hoo_sys->SetBinError(bnum, sErr);
        hoo_stat->SetBinError(bnum, ve[0]);
        //axis->SetLabelSize(0.03);

        //double ymax = hu->GetMaximum(); if(hd->GetMaximum() > ymax) ymax = hd->GetMaximum();
        //double ymin = hu->GetMinimum() > hd->GetMinimum() ? hd->GetMinimum() : hu->GetMinimum();
        //hu->SetMaximum(ymax+0.05*(ymax-ymin));
        //hu->SetMinimum(ymin-0.05*(ymax-ymin));

        //lg->AddEntry(hu, "up", "l");
        //lg->AddEntry(hd, "down", "l");
      
        //hu->GetYaxis()->SetTitle("(vari.-nom.)/nom.");
      
        //hu->SetLineColor(kRed);
        //hd->SetLineColor(kBlue);
        //hu->Draw("hist");
        //hd->Draw("same hist");
      }
      TLegend *lg = new TLegend(0.19, 0.80, 0.37, 0.93);
      lg->SetFillStyle(0);
      lg->SetBorderSize(0);

      TCanvas *canv = new TCanvas("c", "canvas", 800, 600);

      hoo->SetMinimum(0.);
      hoo->Draw("hist");

      hoo_sys->SetMarkerSize(0);
      //hoo_sys->SetFillStyle(3325);
      hoo_sys->SetFillStyle(3001);
      hoo_sys->SetFillColor(kBlack+1);
      hoo_sys->Draw("same e2");

      hoo_stat->SetMarkerSize(0);
      //hoo_stat->SetFillStyle(3352);
      hoo_stat->SetFillStyle(3004);
      hoo_stat->SetFillColor(kBlack);
      hoo_stat->Draw("same e2");

      lg->AddEntry(hoo, "VBF", "l");
      lg->AddEntry(hoo_sys, "sys. uncer.", "f");
      lg->AddEntry(hoo_stat, "stat. uncer.", "f");
      lg->Draw("same");

      canv->SaveAs("ooPlot_"+sample+"_"+dName+"_"+cat+".png");

      fout->cd();
      hoo->Write();
      hoo_sys->Write();
      hoo_stat->Write();
      }
    }
  }
}
