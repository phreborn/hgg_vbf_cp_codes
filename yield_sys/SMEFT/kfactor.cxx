#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
 
using namespace std;

void readInKFs(TString inFileName, map<TString, vector<float>> &varMaps)
{
  ifstream infile(inFileName.Data());
  string line; //getline(infile, line);
  while (getline(infile, line))
  {
    istringstream sin(line);
    string tmp;

    getline(sin, tmp, ':');
    TString head = tmp.data(); //cout<<head<<endl;

    getline(sin, tmp, ':');
    istringstream values(tmp);

    while (getline(values, tmp, ',')) 
    {
      float value = atof(TString(tmp).ReplaceAll(" ", "").Data());
      varMaps[head].push_back(value); //cout<<value<<endl;
    }
  }
}

void readInYVs(TString inFileName, map<TString, vector<float>> &varMaps)
{
  ifstream infile(inFileName.Data());
  string line; //getline(infile, line);
  while (getline(infile, line))
  {
    istringstream sin(line);
    string tmp;

    getline(sin, tmp, ',');
    TString head = tmp.data(); //cout<<head<<endl;

    getline(sin, tmp, ',');
    varMaps[head].push_back(atof(TString(tmp).ReplaceAll(" ", "").Data())); //cout<<atof(TString(tmp).ReplaceAll(" ", "").Data())<<endl;

    getline(sin, tmp, ',');
    varMaps[head].push_back(atof(TString(tmp).ReplaceAll(" ", "").Data())); //cout<<atof(TString(tmp).ReplaceAll(" ", "").Data())<<endl;
  }
}

void readInYds(TString inFileName, map<TString, float> &varMaps)
{
  ifstream infile(inFileName.Data());
  string line; //getline(infile, line);
  while (getline(infile, line))
  {
    istringstream sin(line);
    string tmp;

    getline(sin, tmp, ',');
    TString head = tmp.data(); //cout<<head<<endl;

    getline(sin, tmp, ',');
    varMaps[head] = atof(TString(tmp).ReplaceAll(" ", "").Data()); //cout<<atof(TString(tmp).ReplaceAll(" ", "").Data())<<endl;
  }
}

void kfactor(){

  map<TString, TString> cHWs;
  cHWs["m0d00"] = "-1.56e-07";
  cHWs["m0d05"] = "-0.05";
  cHWs["m0d10"] = "-0.1";
  cHWs["m0d15"] = "-0.15";
  cHWs["m0d20"] = "-0.2";
  cHWs["m0d25"] = "-0.25";
  cHWs["m0d30"] = "-0.3";
  cHWs["m0d35"] = "-0.35";
  cHWs["m0d40"] = "-0.4";
  cHWs["m0d45"] = "-0.45";
  cHWs["m0d50"] = "-0.5";
  cHWs["m0d55"] = "-0.55";
  cHWs["m0d60"] = "-0.6";
  cHWs["m0d65"] = "-0.65";
  cHWs["m0d70"] = "-0.7";
  cHWs["m0d75"] = "-0.75";
  cHWs["m0d80"] = "-0.8";
  cHWs["m0d85"] = "-0.85";
  cHWs["m0d90"] = "-0.9";
  cHWs["m0d95"] = "-0.95";
  cHWs["p0d05"] = "0.05";
  cHWs["p0d10"] = "0.1";
  cHWs["p0d15"] = "0.15";
  cHWs["p0d20"] = "0.2";
  cHWs["p0d25"] = "0.25";
  cHWs["p0d30"] = "0.3";
  cHWs["p0d35"] = "0.35";
  cHWs["p0d40"] = "0.4";
  cHWs["p0d45"] = "0.45";
  cHWs["p0d50"] = "0.5";
  cHWs["p0d55"] = "0.55";
  cHWs["p0d60"] = "0.6";
  cHWs["p0d65"] = "0.65";
  cHWs["p0d70"] = "0.7";
  cHWs["p0d75"] = "0.75";
  cHWs["p0d80"] = "0.8";
  cHWs["p0d85"] = "0.85";
  cHWs["p0d90"] = "0.9";
  cHWs["p0d95"] = "0.95";
  cHWs["p1d00"] = "1";

  /*cHWs["m2d5"] = "-2.5";
  cHWs["m2d0"] = "-2";
  cHWs["m1d5"] = "-1.5";
  cHWs["m1d0"] = "-1";
  cHWs["m0d5"] = "-0.5";
  cHWs["m0d0"] = "-0";
  cHWs["p0d5"] = "0.5";
  cHWs["p1d0"] = "1";
  cHWs["p1d5"] = "1.5";
  cHWs["p2d0"] = "2";
  cHWs["p2d5"] = "2.5";*/
  std::vector<TString> bdtCats{"TT", "TL", "LT", "LL"};
  std::map<int, TString> bins;
  bins[1] = "b1";
  bins[2] = "b2";
  bins[3] = "b3";
  bins[4] = "b4";
  bins[5] = "b5";
  bins[6] = "b6";

  map<TString, float> Yds;
  readInYds("../csv/N_yield.csv", Yds);

  map<TString, vector<float>> Syss;
  readInYVs("../csv/mag_yield_346214_m00_LL_b1.csv", Syss);

  ofstream ofyield("csv/N_yield.csv", ios::ate);
  if(!ofyield){
    ofyield.close();
    cout<<"error can't open file for record"<<endl;
  }

  for(auto cat : bdtCats){
    TString catName = cat;
    for(auto bin : bins){
      int binNum = bin.first;
      TString binName = bin.second;
      map<TString, vector<float>> YVs;
      readInYVs("../csv/mag_yield_346214_m00_"+catName+"_"+binName+".csv", YVs);

      float ySM = Yds["VBF_m00_"+catName+"_"+binName];

      for(auto cHW : cHWs){
        TString cHWName = cHW.first;
        TString cHWNum = cHW.second; cout<<cHWNum<<endl;

        map<TString, vector<float>> nomKFs;
        //cout<<"kfactor_SMEFT/"+catName+"_EG_RESOLUTION_ALL_cHW_"+cHWNum+".txt"<<endl;
        readInKFs("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/theory/kfactor_SMEFT_fine/"+catName+"_EG_RESOLUTION_ALL_cHW_"+cHWNum+".txt", nomKFs);

        float kf = nomKFs["Nominal"].at(binNum-1);
        float ycHW = ySM*kf; cout<<"kfactor at "<<binName<<": "<<kf<<endl;
        ofyield<<"VBF_"+cHWName+"_"+catName+"_"+binName<<","<<ycHW<<endl;

        //for(auto sys : Syss){
        //  TString sysName = sys.first;
        //  map<TString, vector<float>> KFs;
        //  readInKFs("kfactor_SMEFT/"+catName+"_"+sysName+"_cHW_"+cHWNum+".txt", KFs);
        //}
      }
    }
  }
}
