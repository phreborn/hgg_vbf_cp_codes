#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.h"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasStyle.h"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.h"

#ifdef __CLING__
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.C"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.C"
#endif

#include "../shape_sys/sysUtils.h"
#include "ioUtils.h"

void dDep_variationYield(TString systodraw = "JET_Flavor_Composition"){
  SetAtlasStyle();

  char *cf_cats = (char*)"../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  vector<TString> d_map;
  d_map.push_back("m20");
  d_map.push_back("m18");
  d_map.push_back("m16");
  d_map.push_back("m14");
  d_map.push_back("m12");
  d_map.push_back("m10");
  d_map.push_back("m08");
  d_map.push_back("m07");
  d_map.push_back("m06");
  d_map.push_back("m05");
  d_map.push_back("m04");
  d_map.push_back("m03");
  d_map.push_back("m02");
  d_map.push_back("m01");
  d_map.push_back("m00");
  d_map.push_back("p01");
  d_map.push_back("p02");
  d_map.push_back("p03");
  d_map.push_back("p04");
  d_map.push_back("p05");
  d_map.push_back("p06");
  d_map.push_back("p07");
  d_map.push_back("p08");
  d_map.push_back("p10");
  d_map.push_back("p12");
  d_map.push_back("p14");
  d_map.push_back("p16");
  d_map.push_back("p18");
  d_map.push_back("p20");

  std::map<TString, TString> ids;
  ids["vbf"] = "346214";

  std::vector<TString> dtmp;


  for(auto i : ids){
    TString sample = i.first; cout<<sample<<endl;
    TString id = i.second;
    if(sample=="vbf") dtmp = d_map;
    else dtmp = {"SM"};

    for(auto c : catCuts){
      TString cat = c.first;

      int dnum = d_map.size();
      TH1F *hu = new TH1F("var_up", "", dnum, 0, dnum);
      TH1F *hd = new TH1F("var_down", "", dnum, 0, dnum);
      TAxis *axis = hu->GetXaxis();
      int i = 0;
      for(auto d : dtmp){
        TString dName = d;

//        std::vector<TString> sys;
//        std::vector<float> vu;
//        std::vector<float> vd;
//      
//        ifstream file;
//        string filepath = Form("csv/mag_yield_%s_%s_%s.csv", id.Data(), dName.Data(), cat.Data());
//        file.open(filepath);
//        if( ! file.is_open())
//        {
//            cout<<"can not open file! "<<filepath<<endl;
//            return;
//        }
//        char tmp[1000];
//        while(!file.eof()){
//          file.getline(tmp,1000);
//          string line(tmp);
//          size_t ptmp;
//      
//          size_t pos = line.find(','); std::cout<<line.substr(0,pos)<<std::endl;
//          if(pos!=string::npos && line != "") sys.push_back(line.substr(0,pos).data());
//          else continue;
//      
//          ptmp = pos;
//          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
//          if(pos!=string::npos && line != "") vu.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
//          else vu.push_back(0.);
//      
//          ptmp = pos;
//          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
//          if(line != "") vd.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
//          else vd.push_back(0.);
//        }

        TString fpath = "csv/mag_yield_"+id+"_"+dName+"_"+cat+".csv";
        std::map<TString, std::pair<float,float>> sysud;
        getVariation(fpath, sysud);

        TString jdfpath = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv_jd/mag_yield_"+id+"_"+dName+"_"+cat+".csv";
        std::map<TString, std::pair<float,float>> jdsysud;
        getVariation(jdfpath, jdsysud);
      
//        gStyle->SetPadBottomMargin(0.41);
      
        for(auto sys : sysud){
          TString sysname = sys.first;
          if(sysname!=systodraw) continue;
          axis->SetBinLabel(i+1, dName);
          hu->Fill(i, sysname.Contains("JER_") ? jdsysud[sysname].first : sysud[sysname].first);
          hd->Fill(i, sysname.Contains("JER_") ? jdsysud[sysname].second : sysud[sysname].second);
          cout<<sysname<<": "<<sysud[sysname].first<<", "<<sysud[sysname].second<<endl;
          i++;
        }// end sys
      }// end dmap
      axis->SetLabelSize(0.03);

      double ymax = hu->GetMaximum(); if(hd->GetMaximum() > ymax) ymax = hd->GetMaximum();
      double ymin = hu->GetMinimum() > hd->GetMinimum() ? hd->GetMinimum() : hu->GetMinimum();
      hu->SetMaximum(ymax+0.05*(ymax-ymin));
      hu->SetMinimum(ymin-0.05*(ymax-ymin));

      TLegend *lg = new TLegend(0.22, 0.72, 0.40, 0.85);
      lg->SetFillStyle(0);
      lg->SetBorderSize(0);
      lg->AddEntry(hu, "up", "l");
      lg->AddEntry(hd, "down", "l");
      
      hu->GetYaxis()->SetTitle("(var.-nom.)/nom.");
      
      hu->SetLineColor(kRed);
      hd->SetLineColor(kBlue);

      TCanvas *canv = new TCanvas("c", "canvas", 800, 600);
      hu->Draw("hist");
      hd->Draw("same hist");
      lg->Draw("same");

      myText(0.22, 0.88, 1, (sample+",  "+cat+",  yield").Data());

      canv->SaveAs("plotFit/dDep_yieldVari_"+sample+"_"+systodraw+"_"+cat+".png");
      canv->SaveAs("plotFit/dDep_yieldVari_"+sample+"_"+systodraw+"_"+cat+".pdf");
    }// end cat
  }// id
}
