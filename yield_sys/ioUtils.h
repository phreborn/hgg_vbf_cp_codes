#include <regex>

using namespace std;

void regMatch(string istr, string sreg, std::vector<float> &varVec){
  std::regex reg(sreg);
  std::sregex_iterator pos(istr.cbegin(), istr.cend(), reg);
  decltype(pos) end;
  for (; pos != end; ++pos){
    varVec.push_back(atof(pos->str().data()));  //cout<<pos->str()<<endl;
  }

}

float getIPattern(string line, int ibin, string sreg = "\\s(-?[0-9]+\\.[0-9]+(e-[0-9]+)?)|\\s(0)\\s|\\s(0)$"){
  string istr = "Up: 0 , 2.58684e-05 , 0 , 0.000126958 , -0.00305438 , 0";
  std::vector<float> varVec;
  regMatch(line, sreg, varVec); // for(auto var : varVec) cout<<var<<endl;
  return varVec.at(ibin);
}

void regToken(string istr, string stok, std::vector<string> &varVec){
  std::regex reg(stok);

  std::sregex_token_iterator pos(istr.cbegin(), istr.cend(), reg, -1);
  decltype(pos) end;
  for (; pos != end; ++pos){
    varVec.push_back(pos->str().data()); // cout<<pos->str()<<endl;
  }

}

string getTok(string line, int ibin, string stok = ","){
  std::vector<string> varVec;
  regToken(line, stok, varVec); // for(auto var : varVec) cout<<var<<endl;
  return varVec.at(ibin);
}

bool getVariation(TString fpath, std::map<TString, std::pair<float,float>> &sysud){
  ifstream file;
  file.open(fpath.Data());
  if( ! file.is_open())
  {
      cout<<"can not open file! "<<fpath<<endl;
      return false;
  }
  char tmp[1000];
  while(!file.eof()){
    file.getline(tmp,1000);
    string line(tmp);
    size_t ptmp;

    if(line == "") continue;

    TString sys = "";
    size_t pos = line.find(','); std::cout<<line.substr(0,pos)<<std::endl;
    if(pos!=string::npos) sys = line.substr(0,pos).data();
    else continue;
    float utmp = atof(getTok(line, 1).data()); cout<<utmp<<endl;
    float dtmp = atof(getTok(line, 2).data()); cout<<dtmp<<endl;

    sysud[sys] = make_pair(utmp, dtmp);
  }
  return true;
}

void writeCsv(map<string, pair<float, float>> sysV, string file){
  ofstream ofsyst(file, ios::out);
  if(!ofsyst){
    ofsyst.close();
    cout<<file<<endl;
    cout<<"error can't open file for record"<<endl;
  }
  for(auto sys : sysV){
    string sysName = sys.first;
    float vu = sys.second.first;
    float vd = sys.second.second;
    ofsyst<<sysName<<","<<vu<<","<<vd<<endl;
  }
  ofsyst.close();
}
