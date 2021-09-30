#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.h"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasStyle.h"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasLabels.C"
#include "/scratchfs/atlas/huirun/atlaswork/ATLAS_style/atlasrootstyle/AtlasUtils.C"
#endif

#include "../sysUtils.h"

void sigmaVar(){
  SetAtlasStyle();

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

  std::map<TString, TString> ids;
  ids["vbf"] = "346214";
  //ids["ggh"] = "343981";

  std::map<TString, double> dtmp;


  for(auto i : ids){
    TString sample = i.first; cout<<sample<<endl;
    TString id = i.second;
    if(sample=="vbf") dtmp = d_map;
    else dtmp = {{"SM", 0.0}};

    for(auto d : dtmp){
      TString dName = d.first;

      for(auto c : catCuts){
        TString cat = c.first;

        std::vector<TString> sys;
        std::vector<float> vu;
        std::vector<float> vd;
      
        ifstream file;
        file.open(Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/sigma_%s_%s.csv", dName.Data(), cat.Data()));
        if( ! file.is_open())
        {
            cout<<"can not open file! "<<Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/sigma_%s_%s.csv", dName.Data(), cat.Data())<<endl;
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
          if(pos!=string::npos && line != "") vu.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else vu.push_back(0.);
      
          ptmp = pos;
          pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
          if(line != "") vd.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
          else vd.push_back(0.);
        }
      
        //gStyle->SetPadBottomMargin(0.13);
      
        int nsys = sys.size();

        int csys = 0;
        for(int i = 0; i < nsys; i++){
          if(!sys[i].Contains("RESO")) continue;
          if(sys[i].Contains("EG_RESOLUTION_AF2")) continue;
          csys++;
        }
        TH1F *hu = new TH1F("var_up", "", csys, 0, csys);
        TH1F *hd = new TH1F("var_down", "", csys, 0, csys);
        TAxis *axis = hu->GetXaxis();
        int j = 0;
        for(int i = 0; i < nsys; i++){
          if(!sys[i].Contains("RESO")) continue;
          if(sys[i].Contains("EG_RESOLUTION_AF2")) continue;
          axis->SetBinLabel(j+1, sys[i].ReplaceAll("EG_RESOLUTION_", ""));
          hu->Fill(j, vu[i]);
          hd->Fill(j, vd[i]);
          cout<<sys[i]<<": "<<vu[i]<<", "<<vd[i]<<endl;
          j++;
        }
        axis->SetLabelSize(0.03);
        //axis->LabelsOption("v");
        axis->LabelsOption("u");

        double ymax = hu->GetMaximum(); if(hd->GetMaximum() > ymax) ymax = hd->GetMaximum();
        double ymin = hu->GetMinimum() > hd->GetMinimum() ? hd->GetMinimum() : hu->GetMinimum();
        hu->SetMaximum(ymax+0.5*(ymax-ymin));
        hu->SetMinimum(ymin-0.5*(ymax-ymin));

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

        myText(0.22, 0.88, 1, (cat+",  EG_RESOLUTION,  #sigma_{CB}").Data());

        canv->SaveAs("sigmaVari_"+dName+"_"+cat+".png");
      }
    }
  }
}
