#ifndef MNV_MnvResponse_cxx
#define MNV_MnvResponse_cxx 1

#include "MinervaUnfold/MnvResponse.h"
#include <assert.h>
#include <map>
using namespace std;
using namespace MinervaUnfold;

//Default constructor
MnvResponse::MnvResponse()
{}

//Default destructor
MnvResponse::~MnvResponse()
{}

//1D constructors
MnvResponse::MnvResponse(const char* name, const char* title, TH1D* MCReco, TH1D* MCTruth)
{
  Init();
  Setup(name, title, MCReco, MCTruth);
}

MnvResponse::MnvResponse(const char* name, const char* title, const Int_t nbins_reco, const Double_t* bins_reco, const Int_t nbins_truth, const Double_t* bins_truth)
{
  Init();
  Setup(name, title, nbins_reco, bins_reco, nbins_truth, bins_truth);
}

MnvResponse::MnvResponse(const char* name, const char* title, PlotUtils::axis_binning reco, PlotUtils::axis_binning truth)
{
  Init();
  Setup(name, title, reco, truth);
}

//lets try to implement the NSF version
//template <class T>
MnvResponse::MnvResponse(const char* name,const char* title,PlotUtils::axis_binning reco,PlotUtils::axis_binning truth,std::map<const std::string,  int > &error_bands){
  Init();
  Setup(name,title,reco,truth,error_bands);

}


//new HMS // accept an array or a vector of bins

MnvResponse::MnvResponse(const char* name,const char* title,const Int_t nbins_reco, const Double_t* bins_reco,const Int_t nbins_truth, const Double_t* bins_truth ,std::map< std::string, int > &error_bands){
  Init();

  Setup(name,title,nbins_reco, bins_reco, nbins_truth, bins_truth, error_bands);

}

// 2D constructors

MnvResponse::MnvResponse(const char* name, const char* title, TH2D* MCReco, TH2D* MCTruth)
{
  Init();
  Setup(name, title, MCReco, MCTruth);
}

MnvResponse::MnvResponse(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth)
{
  Init();
  Setup(name, title, x_reco, y_reco, x_truth, y_truth);
}

//template <class T>
MnvResponse::MnvResponse(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth,std::map <const std::string, int >& error_bands)
{
  Init();
  //std::cout<<"MnvResponse Constructor Called "<<error_bands.size()<<std::endl;
  Setup(name, title, x_reco, y_reco, x_truth, y_truth,error_bands);
}


template <class T>
MnvResponse::MnvResponse(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth,std::map < std::string, T* >& error_bands)
{
  Init();
  //std::cout<<"MnvResponse Constructor Called "<<error_bands.size()<<std::endl;
 // Setup(name, title, x_reco, y_reco, x_truth, y_truth,error_bands);
}


MnvResponse::MnvResponse(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth)
{
  Init();
  Setup(name, title, nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco, nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth);
}


MnvResponse::MnvResponse(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth,std::map <const std::string, int >& error_bands)
{
  Init();
  Setup(name, title, nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco, nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth, error_bands);
}


MnvResponse::MnvResponse(const char* name, const char* title, std::vector<double> xbins_vector, std::vector<double> ybins_vector ,std::map <const std::string, int >& error_bands)
{

  Init();
  Setup(name, title, xbins_vector.size()-1, xbins_vector.data(), ybins_vector.size()-1, ybins_vector.data(), xbins_vector.size()-1, xbins_vector.data(), ybins_vector.size()-1, ybins_vector.data(), error_bands);




}



// 3D constructors

MnvResponse::MnvResponse(const char* name, const char* title, TH3D* MCReco, TH3D* MCTruth)
{
  Init();
  Setup(name, title, MCReco, MCTruth);
}

MnvResponse::MnvResponse(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning z_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth, PlotUtils::axis_binning z_truth)
{
  Init();
  Setup(name, title, x_reco, y_reco, z_reco, x_truth, y_truth, z_truth);
}



MnvResponse::MnvResponse(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsz_reco, const Double_t* zbins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth, const Int_t nbinsz_truth, const Double_t* zbins_truth)
{
  Init();
  Setup(name, title, nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco, nbinsz_reco, zbins_reco, nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth, nbinsz_truth, zbins_truth);
}


///////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////

//! 1D name/binning Setup
void MnvResponse::Setup(const char* name, const char* title, TH1D* MCReco, TH1D* MCTruth)
{
  checkIfMnvH1DNull(fReco1D, "Reco MnvH1D Histogram");
  fReco1D = new MnvH1D( *MCReco );
  fReco1D->Reset();
  checkIfMnvH1DNull(fTruth1D, "Truth MnvH1D Histogram");
  fTruth1D = new  MnvH1D( *MCTruth );
  fTruth1D->Reset();

  SetupResponse1D( Form("%s_migration", name), Form("%s Migration", title) );
  is1D();

  return;
}

void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning reco, PlotUtils::axis_binning truth)
{
  Double_t recobins[reco.nbins], truthbins[truth.nbins];
  for( unsigned int i = 0; i <= reco.nbins; i++ )
    recobins[i] = reco.bin_edges[i];
  for( unsigned int i = 0; i <= truth.nbins; i++ )
    truthbins[i] = truth.bin_edges[i];

  checkIfMnvH1DNull(fReco1D, "Reco MnvH1D Histogram");
  fReco1D = new MnvH1D(Form("%s_reco", name) , Form("%s Reco", title), reco.nbins, recobins);

  checkIfMnvH1DNull(fTruth1D, "Truth MnvH1D Histogram");
  fTruth1D = new MnvH1D(Form("%s_truth", name), Form("%s Truth", title), truth.nbins, truthbins);

  SetupResponse1D( Form("%s_migration", name), Form("%s Migration", title) );
  is1D();

  return;
}

//template <class T>
void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning reco, PlotUtils::axis_binning truth,std::map<const std::string,  int>&error_bands)
{
  Double_t recobins[reco.nbins], truthbins[truth.nbins];
  for( unsigned int i = 0; i <= reco.nbins; i++ )
    recobins[i] = reco.bin_edges[i];
  for( unsigned int i = 0; i <= truth.nbins; i++ )
    truthbins[i] = truth.bin_edges[i];

  checkIfMnvH1DNull(fReco1D, "Reco MnvH1D Histogram");
  fReco1D = new MnvH1D(Form("%s_reco", name) , Form("%s Reco", title), reco.nbins, recobins);


  checkIfMnvH1DNull(fTruth1D, "Truth MnvH1D Histogram");
  fTruth1D = new MnvH1D(Form("%s_truth", name), Form("%s Truth", title), truth.nbins, truthbins);


  SetupResponse1D( Form("%s_migration", name), Form("%s Migration", title) );
  is1D();

   for(itr = error_bands.begin();itr!=error_bands.end();++itr){
    std::string Errorname = itr->first;
    if (Errorname=="CV" ||  Errorname =="cv")continue;
    const int nhists = itr->second;
    AddVertErrorBand(itr->first, nhists);
  //  std::cout<<"MnvResponse: Added VertErrorBand "<<itr->first<<std::endl;

 //  }

  }

  is1D();

  return;
}

// new HMS array of bins
void MnvResponse::Setup(const char* name, const char* title, const Int_t reco_nbins, const Double_t* recobins, const Int_t truth_nbins, const Double_t*  truthbins,std::map<std::string,  int> error_bands)
{

  //  for( unsigned int i = 0; i <= reco.nbins; i++ )
  //    recobins[i] = reco.bin_edges[i];
  //  for( unsigned int i = 0; i <= truth.nbins; i++ )
  //    truthbins[i] = truth.bin_edges[i];

  checkIfMnvH1DNull(fReco1D, "Reco MnvH1D Histogram");
  fReco1D = new MnvH1D(Form("%s_reco", name) , Form("%s Reco", title), reco_nbins, recobins);

  checkIfMnvH1DNull(fTruth1D, "Truth MnvH1D Histogram");
  fTruth1D = new MnvH1D(Form("%s_truth", name), Form("%s Truth", title), truth_nbins, truthbins);


  SetupResponse1D( Form("%s_migration", name), Form("%s Migration", title) );
  is1D();
  //at this point we have created the reco, truth and migration matrices.....
  // std::cout<<"MnvResponse::Setup "<<error_bands.size()<<std::endl;
  if(error_bands.size()==0)return;
  std::map<std::string,int>::iterator itr;
  for(itr = error_bands.begin();itr!=error_bands.end();++itr){
      std::string Errorname = itr->first;
    const int nhists = itr->second;
    if (Errorname=="CV" ||  Errorname =="cv")continue;
    AddVertErrorBand(itr->first, nhists);
    //std::cout<<"MnvResponse: Added VertErrorBand "<<itr->first<<std::endl;

    //  }

  }


  // std::cout << " is1D " << fIs1D << std::endl;
  return;
}

void MnvResponse::Setup(const char* name, const char* title, const Int_t nbins_reco, const Double_t* bins_reco, const Int_t nbins_truth, const Double_t* bins_truth)
{
  checkIfMnvH1DNull(fReco1D, "Reco MnvH1D Histogram");
  fReco1D = new MnvH1D( Form("%s_reco", name) , Form("%s Reco", title), nbins_reco, bins_reco );

  checkIfMnvH1DNull(fTruth1D, "Truth MnvH1D Histogram");
  fTruth1D = new  MnvH1D( Form("%s_truth", name) , Form("%s Truth", title), nbins_truth, bins_truth );

  SetupResponse1D( Form("%s_migration", name), Form("%s Migration", title) );
  is1D();

  return;
}

//! 2D name/binning Setup
void MnvResponse::Setup(const char* name, const char* title, TH2D* MCReco, TH2D* MCTruth)
{
  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( *MCReco );
  fReco2D->Reset();

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new  MnvH2D( *MCTruth );
  fTruth2D->Reset();

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
  is2D();

  return;
}

void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth)
{
  Double_t x_recobins[x_reco.nbins], y_recobins[y_reco.nbins], x_truthbins[x_truth.nbins], y_truthbins[y_truth.nbins];
  for( unsigned int i = 0; i <= x_reco.nbins; i++ )
    x_recobins[i] = x_reco.bin_edges[i];
  for( unsigned int i = 0; i <= y_reco.nbins; i++ )
    y_recobins[i] = y_reco.bin_edges[i];
  for( unsigned int i = 0; i <= x_truth.nbins; i++ )
    x_truthbins[i] = x_truth.bin_edges[i];
  for( unsigned int i = 0; i <= y_truth.nbins; i++ )
    y_truthbins[i] = y_truth.bin_edges[i];

  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( Form("%s_reco", name) , Form("%s Reco", title), x_reco.nbins, x_recobins, y_reco.nbins, y_recobins);

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new MnvH2D( Form("%s_truth", name) , Form("%s Truth", title), x_truth.nbins, x_truthbins, y_truth.nbins, y_truthbins);

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
  is2D();

  return;
}

void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth,std::map<const std::string,  int>&error_bands)
{
  //std::cout<<"MnvResponse Setup called "<<std::endl;
  Double_t x_recobins[x_reco.nbins], y_recobins[y_reco.nbins], x_truthbins[x_truth.nbins], y_truthbins[y_truth.nbins];
  for( unsigned int i = 0; i <= x_reco.nbins; i++ )
    x_recobins[i] = x_reco.bin_edges[i];
  for( unsigned int i = 0; i <= y_reco.nbins; i++ )
    y_recobins[i] = y_reco.bin_edges[i];
  for( unsigned int i = 0; i <= x_truth.nbins; i++ )
    x_truthbins[i] = x_truth.bin_edges[i];
  for( unsigned int i = 0; i <= y_truth.nbins; i++ )
    y_truthbins[i] = y_truth.bin_edges[i];

  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( Form("%s_reco", name) , Form("%s Reco", title), x_reco.nbins, x_recobins, y_reco.nbins, y_recobins);

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new MnvH2D( Form("%s_truth", name) , Form("%s Truth", title), x_truth.nbins, x_truthbins, y_truth.nbins, y_truthbins);

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
     is2D();

    // std::map<std::string,int>::iterator itr;

   for(itr = error_bands.begin();itr!=error_bands.end();++itr){
     std::string Errorname = itr->first;
    if (Errorname=="CV" ||  Errorname =="cv")continue;
   // std::cout<<"MnvResponse Adding Vert error band "<<itr->first<<std::endl;
    AddVertErrorBand(itr->first, itr->second);

 //  }

  }


   // is2D();

  return;
}
/*
template <typename T>
void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth,std::map< std::string, std::vector<T*>>&error_bands)
{
  //std::cout<<"MnvResponse Setup called "<<std::endl;
  Double_t x_recobins[x_reco.nbins], y_recobins[y_reco.nbins], x_truthbins[x_truth.nbins], y_truthbins[y_truth.nbins];
  for( unsigned int i = 0; i <= x_reco.nbins; i++ )
    x_recobins[i] = x_reco.bin_edges[i];
  for( unsigned int i = 0; i <= y_reco.nbins; i++ )
    y_recobins[i] = y_reco.bin_edges[i];
  for( unsigned int i = 0; i <= x_truth.nbins; i++ )
    x_truthbins[i] = x_truth.bin_edges[i];
  for( unsigned int i = 0; i <= y_truth.nbins; i++ )
    y_truthbins[i] = y_truth.bin_edges[i];

  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( Form("%s_reco", name) , Form("%s Reco", title), x_reco.nbins, x_recobins, y_reco.nbins, y_recobins);
  WReco2D = new Hist2DWrapper<T*>(fReco2D,error_bands);

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new MnvH2D( Form("%s_truth", name) , Form("%s Truth", title), x_truth.nbins, x_truthbins, y_truth.nbins, y_truthbins);
  WTruth2D = new Hist2DWrapper<T>(fTruth2D,error_bands);

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
  WMigration = new Hist2DWrapper<T>(fMigration,error_bands);
     is2D();



  return;
}
*/

void MnvResponse::Setup(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth)
{
  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( Form("%s_reco", name) , Form("%s Reco", title), nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco );

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new  MnvH2D( Form("%s_truth", name) , Form("%s Truth", title), nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth );

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
  is2D();

  return;
}


void MnvResponse::Setup(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth, std::map<const std::string,  int>&error_bands)
{
  Setup(name,title,  nbinsx_reco,  xbins_reco,  nbinsy_reco,  ybins_reco,  nbinsx_truth,  xbins_truth,  nbinsy_truth,  ybins_truth);

   for(itr = error_bands.begin();itr!=error_bands.end();++itr){
     // std::cout<<"MnvResponse Adding Vert error band "<<itr->first<<std::endl;
    std::string Errorname = itr->first;	
    if (Errorname=="CV" ||  Errorname =="cv")continue;
     AddVertErrorBand(itr->first, itr->second);
   }

  return;
}





/*

void Setup(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth ,std::map<const std::string, const int > & error_bands)
{
  checkIfMnvH2DNull(fReco2D, "Reco MnvH2D Histogram");
  fReco2D = new MnvH2D( Form("%s_reco", name) , Form("%s Reco", title), nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco );

  checkIfMnvH2DNull(fTruth2D, "Truth MnvH2D Histogram");
  fTruth2D = new  MnvH2D( Form("%s_truth", name) , Form("%s Truth", title), nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth );

  SetupResponse2D( Form("%s_migration", name), Form("%s Migration", title) );
  //is2D();

  std::map<std::string,int>::iterator itr;
  for(itr = error_bands.begin();itr!=error_bands.end();++itr)
  {
    const int nhists = itr->second;
    //if (name=="CV" or name=="cv")continue;
    if(strcmp("CV",name)==0) continue;
    if(strcmp("cv",name)==0) continue;
    AddVertErrorBand(itr->first, nhists);
  }




  return;

}
*/

//! 3D name/binning Setup
void MnvResponse::Setup(const char* name, const char* title, TH3D* MCReco, TH3D* MCTruth)
{
  checkIfMnvH3DNull(fReco3D, "Reco MnvH3D Histogram");
  fReco3D = new MnvH3D( *MCReco );
  fReco3D->Reset();

  checkIfMnvH3DNull(fTruth3D, "Truth MnvH3D Histogram");
  fTruth3D = new  MnvH3D( *MCTruth );
  fTruth3D->Reset();

  SetupResponse3D( Form("%s_migration", name), Form("%s Migration", title) );
  is3D();

  return;
}

void MnvResponse::Setup(const char* name, const char* title, PlotUtils::axis_binning x_reco, PlotUtils::axis_binning y_reco, PlotUtils::axis_binning z_reco, PlotUtils::axis_binning x_truth, PlotUtils::axis_binning y_truth, PlotUtils::axis_binning z_truth)
{
  Double_t x_recobins[x_reco.nbins], y_recobins[y_reco.nbins], z_recobins[z_reco.nbins], x_truthbins[x_truth.nbins], y_truthbins[y_truth.nbins], z_truthbins[z_truth.nbins];
  for( unsigned int i = 0; i <= x_reco.nbins; i++ )
    x_recobins[i] = x_reco.bin_edges[i];
  for( unsigned int i = 0; i <= y_reco.nbins; i++ )
    y_recobins[i] = y_reco.bin_edges[i];
  for( unsigned int i = 0; i <= z_reco.nbins; i++ )
    z_recobins[i] = z_reco.bin_edges[i];
  for( unsigned int i = 0; i <= x_truth.nbins; i++ )
    x_truthbins[i] = x_truth.bin_edges[i];
  for( unsigned int i = 0; i <= y_truth.nbins; i++ )
    y_truthbins[i] = y_truth.bin_edges[i];
  for( unsigned int i = 0; i <= z_truth.nbins; i++ )
    z_truthbins[i] = z_truth.bin_edges[i];

  checkIfMnvH3DNull(fReco3D, "Reco MnvH3D Histogram");
  fReco3D = new MnvH3D( Form("%s_reco", name) , Form("%s Reco", title), x_reco.nbins, x_recobins, y_reco.nbins, y_recobins, z_reco.nbins, z_recobins);

  checkIfMnvH3DNull(fTruth3D, "Truth MnvH3D Histogram");
  fTruth3D = new MnvH3D( Form("%s_truth", name) , Form("%s Truth", title), x_truth.nbins, x_truthbins, y_truth.nbins, y_truthbins, z_truth.nbins, z_truthbins);

  SetupResponse3D( Form("%s_migration", name), Form("%s Migration", title) );
  is3D();

  return;
}

void MnvResponse::Setup(const char* name, const char* title, const Int_t nbinsx_reco, const Double_t* xbins_reco, const Int_t nbinsy_reco, const Double_t* ybins_reco, const Int_t nbinsz_reco, const Double_t* zbins_reco, const Int_t nbinsx_truth, const Double_t* xbins_truth, const Int_t nbinsy_truth, const Double_t* ybins_truth, const Int_t nbinsz_truth, const Double_t* zbins_truth)
{
  checkIfMnvH3DNull(fReco3D, "Reco MnvH3D Histogram");
  fReco3D = new MnvH3D( Form("%s_reco", name) , Form("%s Reco", title), nbinsx_reco, xbins_reco, nbinsy_reco, ybins_reco, nbinsz_reco, zbins_reco );

  checkIfMnvH3DNull(fTruth3D, "Truth MnvH3D Histogram");
  fTruth3D = new  MnvH3D( Form("%s_truth", name) , Form("%s Truth", title), nbinsx_truth, xbins_truth, nbinsy_truth, ybins_truth, nbinsz_truth, zbins_truth );

  SetupResponse3D( Form("%s_migration", name), Form("%s Migration", title) );
  is3D();

  return;
}

/**********************************************************************************/
/* Migration / Response Matrix Utitlities for two/three-dimensional unfolding     */
/**********************************************************************************/

bool MnvResponse::SetupResponse3D( const char* name, const char* title )
{
  if ( fReco3D == NULL || fTruth3D == NULL )
  {
    Error("MnvResponse::SetupResponse3D","Reco and/or Truth binning were not setup");
    return false;
  }
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  checkIfMnvH2DNull(fMigration, "MnvH2D Migration Histogram");

  //Setting response bins
  // the +2s are to include space for overflows
  Int_t responseBinsX = (fReco3D->GetNbinsX()+2) * (fReco3D->GetNbinsY()+2) * (fReco3D->GetNbinsZ()+2);
  Int_t responseBinsY = (fTruth3D->GetNbinsX()+2) * (fTruth3D->GetNbinsY()+2) * (fTruth3D->GetNbinsZ()+2);
  fMigration = new MnvH2D( name, title, responseBinsX,0.0,Double_t(responseBinsX), responseBinsY, 0.0, Double_t(responseBinsY) );

  return true;
}

bool MnvResponse::SetupResponse2D( const char* name, const char* title )
{
  if ( fReco2D == NULL || fTruth2D == NULL )
  {
    Error("MnvResponse::SetupResponse2D","Reco and/or Truth binning were not setup");
    return false;
  }
  checkIfMnvH2DNull(fMigration, "MnvH2D Migration Histogram");

  //Setting response bins
  // the +2s are to include space for overflows
  Int_t responseBinsX = (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2);
  Int_t responseBinsY = (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2);

  fMigration = new MnvH2D( name, title, responseBinsX,0.0,Double_t(responseBinsX), responseBinsY, 0.0, Double_t(responseBinsY) );

  return true;
}

bool MnvResponse::SetupResponse1D( const char* name, const char* title )
{
  if ( fReco1D == NULL || fTruth1D == NULL )
  {
    Error("MnvResponse::SetupResponse1D","Reco and/or Truth binning were not setup");
    return false;
  }
  checkIfMnvH2DNull(fMigration, "MnvH2D Migration Histogram");

  //Setting response bins
  Int_t responseBinsX = fReco1D->GetNbinsX();
  Int_t responseBinsY = fTruth1D->GetNbinsX();
  fMigration = new MnvH2D( name, title, responseBinsX,0.0,Double_t(responseBinsX), responseBinsY, 0.0, Double_t(responseBinsY) );

  return true;
}



Int_t MnvResponse::Fill( double meas, double truth, double w )
{
  if ( !fIs1D )
    Error("MnvResponse::Fill"," Attempting to fill migration histogram for a one-dimensional unfolding, but the binning is setup for a different dimension");

  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return -2;
  }

  //Fill Reco, True, and Migration
  fReco1D->Fill(meas, w);
  fTruth1D->Fill(truth, w);

  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin(fReco1D, meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin(fTruth1D, truth)+1) ;

  Int_t nbins = fMigration->Fill(hBinX, hBinY, w);

  return nbins;
}

//NSF implementation for 1D migration matrix....
Int_t MnvResponse::Fill( double meas, double truth,std::string error_name,const int univ, double w )
{
  if ( !fIs1D )
    Error("MnvResponse::Fill"," Attempting to fill migration histogram for a one-dimensional unfolding, but the binning is setup for a different dimension");

  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return -2;
  }


   Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco1D, meas)+1) ;
    Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth1D, truth)+1) ;


  // Int_t nbins = fMigration->GetVertErrorBand(error_name)->GetHist(univ)->Fill(hBinX, hBinY, w);

    if(error_name=="CV"||error_name=="cv"){
      fReco1D->Fill(meas,w);
      fTruth1D->Fill(truth,w);
      fMigration->Fill(hBinX, hBinY,w);
     }
   else{
    fMigration->GetVertErrorBand(error_name)->GetHist(univ)->Fill(hBinX, hBinY, w);
    fReco1D->GetVertErrorBand(error_name)->GetHist(univ)->Fill(meas, w);
    fTruth1D->GetVertErrorBand(error_name)->GetHist(univ)->Fill(truth, w);
   }

  return 1;
}





Int_t MnvResponse::Fill( double meas_x, double meas_y, double truth_x, double truth_y, double w )
{
  if ( !fIs2D )
    Error("MnvResponse::Fill"," Attempting to fill migration histogram for a two-dimensional unfolding, but the binning is setup for a different dimension");

  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return -2;
  }

  //BringWithinHistLimits( fReco2D, meas_x, meas_y );
  //BringWithinHistLimits( fTruth2D, truth_x, truth_y );

  fReco2D->Fill(meas_x, meas_y, w);
  fTruth2D->Fill(truth_x, truth_y, w);

  //Fill Migration 2D
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, meas_x, meas_y)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, truth_x, truth_y)+1) ;

  Int_t nbins = fMigration->Fill(hBinX, hBinY, w);

  return nbins;
}

//template <class T>
Int_t MnvResponse::Fill( double meas_x, double meas_y, double truth_x, double truth_y,const std::string error_name,const int univ, double w )

{
  if ( !fIs2D )
    Error("MnvResponse::Fill"," Attempting to fill migration histogram for a two-dimensional unfolding, but the binning is setup for a different dimension");

  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return -2;
  }

    Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, meas_x, meas_y)+1) ;
    Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, truth_x, truth_y)+1) ;
    Int_t nbins;

    if(error_name=="CV"||error_name=="cv"){
     fReco2D->Fill(meas_x,meas_y,w);
     fTruth2D->Fill(truth_x,truth_y,w);
     nbins = fMigration->Fill(hBinX, hBinY,w);
   }
   else {
     fReco2D->GetVertErrorBand(error_name)->GetHist(univ)->Fill(meas_x, meas_y, w);
     fTruth2D->GetVertErrorBand(error_name)->GetHist(univ)->Fill(truth_x, truth_y, w);
     nbins = fMigration->GetVertErrorBand(error_name)->GetHist(univ)->Fill(hBinX, hBinY, w);
   }



  return nbins;
}


Int_t MnvResponse::Fill( double meas_x, double meas_y, double meas_z, double truth_x, double truth_y, double truth_z, double w )
{
  if ( !fIs3D )
    Error("MnvResponse::Fill"," Attempting to fill migration histogram for a two-dimensional unfolding, but the binning is setup for a different dimension");
  // TODO this won't work in 3d bc overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ()!= fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return -2;
  }

  fReco3D->Fill(meas_x, meas_y, meas_z, w);
  fTruth3D->Fill(truth_x, truth_y, truth_z, w);

  //Fill Migration 3D
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, meas_x, meas_y, meas_z)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, truth_x, truth_y, truth_z)+1) ;

  Int_t nbins = fMigration->Fill(hBinX, hBinY, w);

  return nbins;
}

bool MinervaUnfold::MnvResponse::HasLatErrorBand(const std::string& name) const
{
	if (!fMigration)
		return false;

	return fMigration->HasLatErrorBand(name);
}


bool MinervaUnfold::MnvResponse::HasVertErrorBand(const std::string& name) const
{
	if (!fMigration)
		return false;

	return fMigration->HasVertErrorBand(name);
}

std::vector<std::string> MinervaUnfold::MnvResponse::GetLatErrorBandNames() const
{
	std::vector<std::string> names;
	if (fMigration)
		names = fMigration->GetLatErrorBandNames();
	return names;
}


std::vector<std::string> MinervaUnfold::MnvResponse::GetVertErrorBandNames() const
{
	std::vector<std::string> names;
	if (fMigration)
		names = fMigration->GetVertErrorBandNames();
	return names;
}

bool MnvResponse::AddVertErrorBand( const std::string& name, const int nhists /*= -1*/ )
{
  bool ok = true;
 // std::cout<<"MnvResponse::AddVertErrorBand "<<fIs1D<<" "<<name<<" "<<nhists<<std::endl;
  fMigration->AddVertErrorBand(name, nhists);
  if ( fIs1D )
  {
   //std::cout<<"Tring to add 1D "<<std::endl;
    fReco1D->AddVertErrorBand(name, nhists);
    fTruth1D->AddVertErrorBand(name, nhists);
  }
  else if ( fIs2D )
  {
   // std::cout<<"MnvResponse::AddVertErrorBand "<<name<<" "<<nhists<<std::endl;
    fReco2D->AddVertErrorBand(name, nhists);
    fTruth2D->AddVertErrorBand(name, nhists);
  }
  else if ( fIs3D )
  {
    fReco3D->AddVertErrorBand(name, nhists);
    fTruth3D->AddVertErrorBand(name, nhists);
  }
  else
  {
    Error("MnvResponse::AddVertErrorBand","Unfolding dimension was not defined");
    ok = false;
  }

  return ok;
}

bool MnvResponse::AddLatErrorBand( const std::string& name, const int nhists /*= -1*/ )
{
  bool ok = true;
  fMigration->AddLatErrorBand(name, nhists);
  if ( fIs1D )
  {
    fReco1D->AddLatErrorBand(name, nhists);
    fTruth1D->AddLatErrorBand(name, nhists);
  }
  else if ( fIs2D )
  {
    fReco2D->AddLatErrorBand(name, nhists);
    fTruth2D->AddLatErrorBand(name, nhists);
  }
  else if ( fIs3D )
  {
    fReco3D->AddLatErrorBand(name, nhists);
    fTruth3D->AddLatErrorBand(name, nhists);
  }
  else
  {
    Error("MnvResponse::AddLatErrorBand","Unfolding dimension was not defined");
    ok = false;
  }

  return ok;
}

// Fill 1D Vertical Errors
bool MnvResponse::FillVertErrorBand( const std::string& name, const double meas, const double truth, const std::vector<double>& weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillVertErrorBand(name, meas, weights, cvweight, cvWeightFromMe);
  fTruth1D->FillVertErrorBand(name, truth, weights, cvweight, cvWeightFromMe);
  fMigration->FillVertErrorBand(name, meas, truth, weights, cvweight, cvWeightFromMe);

  return true;
}

bool MnvResponse::FillVertErrorBand( const std::string& name, const double meas, const double truth, const double* weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillVertErrorBand(name, meas, weights, cvweight, cvWeightFromMe);
  fTruth1D->FillVertErrorBand(name, truth, weights, cvweight, cvWeightFromMe);
  fMigration->FillVertErrorBand(name, meas, truth, weights, cvweight, cvWeightFromMe);

  return true;
}

//! Fill the weights of an MnvVertErrorBand's 2 universes with these 2 weights (must have 2)
bool MnvResponse::FillVertErrorBand( const std::string& name, const double meas, const double truth, const double weightDown, const double weightUp, const double cvweight /* = 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillVertErrorBand(name, meas, weightDown, weightUp, cvweight, cvWeightFromMe);
  fTruth1D->FillVertErrorBand(name, truth, weightDown, weightUp, cvweight, cvWeightFromMe);
  bool ok = fMigration->FillVertErrorBand(name, meas, truth, weightDown, weightUp, cvweight, cvWeightFromMe);

  return ok;
}
// Fill 2D Vertical Errors
//! Fill the weights of an MnvVertErrorBand's universes from a vector
bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const std::vector<double>& weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillVertErrorBand( name, x_meas, y_meas, weights, cvweight, cvWeightFromMe);
  fTruth2D->FillVertErrorBand( name, x_truth, y_truth, weights, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weights, cvweight, cvWeightFromMe);

  return ok;
}

bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const double* weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillVertErrorBand( name, x_meas, y_meas, weights, cvweight, cvWeightFromMe);
  fTruth2D->FillVertErrorBand( name, x_truth, y_truth, weights, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weights, cvweight, cvWeightFromMe);

  return ok;
}

//! Fill the weights of an MnvVertErrorBand's 2 universes with these 2 weights (must have 2)
bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const double weightDown, const double weightUp, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillVertErrorBand( name, x_meas, y_meas, weightDown, weightUp, cvweight, cvWeightFromMe);
  fTruth2D->FillVertErrorBand( name, x_truth, y_truth, weightDown, weightUp, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weightDown, weightUp, cvweight, cvWeightFromMe);

  return ok;
}

// Fill 3D Vertical Errors
//! Fill the weights of an MnvVertErrorBand's universes from a vector
bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const std::vector<double>& weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO won't work in 3d bc overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ() != fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillVertErrorBand( name, x_meas, y_meas, z_meas, weights, cvweight, cvWeightFromMe);
  fTruth3D->FillVertErrorBand( name, x_truth, y_truth, z_truth, weights, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weights, cvweight, cvWeightFromMe);

  return ok;
}

bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const double* weights, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO won't work in 3d bc overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ()!= fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ() != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillVertErrorBand( name, x_meas, y_meas, z_meas, weights, cvweight, cvWeightFromMe);
  fTruth3D->FillVertErrorBand( name, x_truth, y_truth, z_truth, weights, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weights, cvweight, cvWeightFromMe);

  return ok;
}

//! Fill the weights of an MnvVertErrorBand's 2 universes with these 2 weights (must have 2)
bool MnvResponse::FillVertErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const double weightDown, const double weightUp, const double cvweight  /*= 1.0*/, double cvWeightFromMe /*= 1.*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO won't work in 3d bc overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ()!= fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillVertErrorBand( name, x_meas, y_meas, z_meas, weightDown, weightUp, cvweight, cvWeightFromMe);
  fTruth3D->FillVertErrorBand( name, x_truth, y_truth, z_truth, weightDown, weightUp, cvweight, cvWeightFromMe);
  //FillVertical
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_truth)+1) ;

  //! @Double check the weights here!
  bool ok = fMigration->FillVertErrorBand(name, hBinX, hBinY, weightDown, weightUp, cvweight, cvWeightFromMe);

  return ok;
}


// Fill 1D Lateral Errors
bool MnvResponse::FillLatErrorBand( const std::string& name, const double meas, const double truth, const std::vector<double>& meas_shifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= 0*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillLatErrorBand(name, meas, meas_shifts, cvweight, fillcv, weights);
  // create a vector of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int nuniverses = meas_shifts.size();
  std::vector<double> truth_shifts(nuniverses, 0.0);
  for (int i = 0 ; i<nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_shifts[i] ) )
      truth_shifts.at(i) = MnvHist::NotPhysicalShiftNumber;
  }

  fTruth1D->FillLatErrorBand(name, truth, truth_shifts, cvweight, fillcv, weights);
  fMigration->FillLatErrorBand(name, meas, truth, meas_shifts, truth_shifts, cvweight, fillcv, weights);

  return true;
}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double meas, const double truth, const double *meas_shifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= 0*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillLatErrorBand(name, meas, meas_shifts, cvweight, fillcv, weights);
  // create a vector of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int nuniverses = fReco1D->GetLatErrorBand(name)->GetNHists();
  double truth_shifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_shifts[i] = ( MnvHist::IsNotPhysicalShift( meas_shifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth1D->FillLatErrorBand(name, truth, truth_shifts, cvweight, fillcv, weights);
  fMigration->FillLatErrorBand(name, meas, truth, meas_shifts, truth_shifts, cvweight, fillcv, weights);

  return true;
}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double meas, const double truth, const double meas_shiftDown, const double meas_shiftUp, const double cvweight /*= 1.0*/, const bool fillcv /*= true*/ )
{
  if ( !fIs1D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  if ( fReco1D->GetNbinsX() != fMigration->GetNbinsX() || fTruth1D->GetNbinsX()  != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::Fill"," The MnvH1D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco1D->FillLatErrorBand(name, meas, meas_shiftDown, meas_shiftUp, cvweight, fillcv);

  double truth_shiftDown , truth_shiftUp;

  truth_shiftDown = ( MnvHist::IsNotPhysicalShift( meas_shiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_shiftUp    = ( MnvHist::IsNotPhysicalShift( meas_shiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth1D->FillLatErrorBand(name, truth, truth_shiftDown, truth_shiftUp, cvweight, fillcv);
  fMigration->FillLatErrorBand(name, meas, truth, meas_shiftDown, meas_shiftUp, truth_shiftDown, truth_shiftUp, cvweight, fillcv);

  return true;
}

// Fill 2D Lateral Errors
bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const std::vector<double>& meas_xshifts, const std::vector<double>& meas_yshifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= 0*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillLatErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillLatErrorBand( name, x_meas, y_meas, meas_xshifts, meas_yshifts, cvweight, fillcv, weights);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int x_nuniverses = meas_xshifts.size();
  std::vector<double> truth_xshifts(x_nuniverses, 0.0);
  for (int i = 0 ; i<x_nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) )
      truth_xshifts.at(i) = MnvHist::NotPhysicalShiftNumber;
  }

  const int y_nuniverses = meas_yshifts.size();
  if (x_nuniverses != y_nuniverses )
  {
    Error("MnvResponse::FillLatErrorBand", "xshifts and yshifts number of elements/universes were different");
    return false;
  }
  std::vector<double> truth_yshifts(y_nuniverses, 0.0);
  for (int i = 0 ; i<y_nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )
      truth_yshifts.at(i) = MnvHist::NotPhysicalShiftNumber;
  }

  fTruth2D->FillLatErrorBand( name, x_truth, y_truth, truth_xshifts, truth_yshifts, cvweight, fillcv, weights);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1);
  std::vector<double> meas_hBinXshifts, meas_hBinYshifts;

  for (int i = 0; i<x_nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )
      meas_hBinXshifts.push_back(MnvHist::NotPhysicalShiftNumber);
    else
      meas_hBinXshifts.push_back(fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas+meas_xshifts[i], y_meas+meas_yshifts[i])+1) - hBinX );
    if ( MnvHist::IsNotPhysicalShift( truth_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( truth_yshifts[i] ) )
      meas_hBinYshifts.push_back(MnvHist::NotPhysicalShiftNumber);
    else
      meas_hBinYshifts.push_back( fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth+truth_xshifts[i], y_truth+truth_yshifts[i])+1) - hBinY );
  }
  //! @todo Double check the weights here!
  //bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_xshifts, meas_yshifts, cvweight, fillcv, weights);
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshifts, meas_hBinYshifts, cvweight, fillcv, weights);

  return ok;
}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const double *meas_xshifts, const double *meas_yshifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= 0*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillLatErrorBand( name, x_meas, y_meas, meas_xshifts, meas_yshifts, cvweight, fillcv, weights);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int nuniverses = fReco2D->GetLatErrorBand(name)->GetNHists();
  double truth_xshifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_xshifts[i] = ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  double truth_yshifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_yshifts[i] = ( MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth2D->FillLatErrorBand( name, x_truth, y_truth, truth_xshifts, truth_yshifts, cvweight, fillcv, weights);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1) ;
  double meas_hBinXshifts[nuniverses], meas_hBinYshifts[nuniverses];

  for (int i = 0; i<nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )
      meas_hBinXshifts[i] = MnvHist::NotPhysicalShiftNumber;
    else
      meas_hBinXshifts[i] = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas+meas_xshifts[i], y_meas+meas_yshifts[i])+1) - hBinX ;
    if ( MnvHist::IsNotPhysicalShift( truth_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( truth_yshifts[i] ) )
      meas_hBinYshifts[i] = MnvHist::NotPhysicalShiftNumber;
    else
      meas_hBinYshifts[i] = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth+truth_xshifts[i], y_truth+truth_yshifts[i])+1) - hBinY ;
  }
  //! @Double check the weights here!
  //bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_xshifts, meas_yshifts, cvweight, fillcv, weights);
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshifts, meas_hBinYshifts, cvweight, fillcv, weights);

  return ok;

}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double x_truth, const double y_truth, const double meas_xshiftDown, const double meas_xshiftUp, const double meas_yshiftDown, const double meas_yshiftUp, const double cvweight /*= 1.0*/, const bool fillcv /*= true*/ )
{
  if ( !fIs2D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  if ( (fReco2D->GetNbinsX()+2) * (fReco2D->GetNbinsY()+2) != fMigration->GetNbinsX() || (fTruth2D->GetNbinsX()+2) * (fTruth2D->GetNbinsY()+2) != fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco2D->FillLatErrorBand( name, x_meas, y_meas, meas_xshiftDown, meas_xshiftUp, meas_yshiftDown, meas_yshiftUp,  cvweight, fillcv);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  double truth_xshiftDown , truth_xshiftUp, truth_yshiftDown , truth_yshiftUp;

  truth_xshiftDown = ( MnvHist::IsNotPhysicalShift( meas_xshiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_xshiftUp    = ( MnvHist::IsNotPhysicalShift( meas_xshiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_yshiftDown = ( MnvHist::IsNotPhysicalShift( meas_yshiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_yshiftUp    = ( MnvHist::IsNotPhysicalShift( meas_yshiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth2D->FillLatErrorBand( name, x_truth, y_truth, truth_xshiftDown, truth_xshiftUp, truth_yshiftDown, truth_yshiftUp, cvweight, fillcv);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas, y_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth, y_truth)+1) ;

  double meas_hBinXshiftDown, meas_hBinXshiftUp, meas_hBinYshiftDown, meas_hBinYshiftUp;

  if ( MnvHist::IsNotPhysicalShift( meas_xshiftDown ) || MnvHist::IsNotPhysicalShift( meas_yshiftDown ) )
    meas_hBinXshiftDown = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinXshiftDown = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas+meas_xshiftDown, y_meas+meas_yshiftDown)+1) - hBinX ;
  if ( MnvHist::IsNotPhysicalShift( meas_xshiftUp ) || MnvHist::IsNotPhysicalShift( meas_yshiftUp ) )
    meas_hBinXshiftUp = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinXshiftUp = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco2D, x_meas+meas_xshiftUp, y_meas+meas_yshiftUp)+1) - hBinX ;
  if ( MnvHist::IsNotPhysicalShift( truth_xshiftDown ) || MnvHist::IsNotPhysicalShift( truth_yshiftDown ))
    meas_hBinYshiftDown = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinYshiftDown = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth+truth_xshiftDown, y_truth+truth_yshiftDown)+1) - hBinY ;
  if ( MnvHist::IsNotPhysicalShift( truth_xshiftUp ) || MnvHist::IsNotPhysicalShift( truth_yshiftUp ))
    meas_hBinYshiftUp = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinYshiftUp = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth2D, x_truth+truth_xshiftUp, y_truth+truth_yshiftUp)+1) - hBinY ;


  //! @Double check the weights here!
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshiftDown, meas_hBinXshiftUp, meas_hBinYshiftDown, meas_hBinYshiftUp, cvweight, fillcv);

  return ok;
}

// Fill 3D Lateral Errors
bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const std::vector<double>& meas_xshifts, const std::vector<double>& meas_yshifts, const std::vector<double>& meas_zshifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= NULL*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO this always errors bc of overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ() != fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillLatErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillLatErrorBand( name, x_meas, y_meas, z_meas, meas_xshifts, meas_yshifts, meas_zshifts, cvweight, fillcv, weights);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int nuniverses = meas_xshifts.size();
  if (meas_xshifts.size() != meas_yshifts.size() || meas_xshifts.size() != meas_zshifts.size())
  {
    Error("MnvResponse::FillLatErrorBand", "xshifts, yshifts, zshifts number of elements/universes were different");
    return false;
  }
  std::vector<double> truth_xshifts(nuniverses, 0.0);
  std::vector<double> truth_yshifts(nuniverses, 0.0);
  std::vector<double> truth_zshifts(nuniverses, 0.0);
  for (int i = 0 ; i<nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) )
      truth_xshifts.at(i) = MnvHist::NotPhysicalShiftNumber;

    if ( MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )
      truth_yshifts.at(i) = MnvHist::NotPhysicalShiftNumber;

    if ( MnvHist::IsNotPhysicalShift( meas_zshifts[i] ) )
      truth_zshifts.at(i) = MnvHist::NotPhysicalShiftNumber;
  }

  fTruth3D->FillLatErrorBand( name, x_truth, y_truth, z_truth, truth_xshifts, truth_yshifts, truth_zshifts, cvweight, fillcv, weights);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_truth)+1);
  std::vector<double> meas_hBinXshifts, meas_hBinYshifts;

  for (int i = 0; i<nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) || MnvHist::IsNotPhysicalShift( meas_zshifts[i] ) )
      meas_hBinXshifts.push_back(MnvHist::NotPhysicalShiftNumber);
    else
      meas_hBinXshifts.push_back(fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas+meas_xshifts[i], y_meas+meas_yshifts[i], z_meas+meas_zshifts[i])+1) - hBinX );

    if ( MnvHist::IsNotPhysicalShift( truth_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( truth_yshifts[i] ) || MnvHist::IsNotPhysicalShift( truth_zshifts[i] ) )
      meas_hBinYshifts.push_back(MnvHist::NotPhysicalShiftNumber);
    else
      meas_hBinYshifts.push_back( fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth+truth_xshifts[i], y_truth+truth_yshifts[i], z_truth+truth_zshifts[i])+1) - hBinY );
  }
  //! @todo Double check the weights here!
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshifts, meas_hBinYshifts, cvweight, fillcv, weights);

  return ok;
}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const double *meas_xshifts, const double *meas_yshifts, const double *meas_zshifts, const double cvweight  /*= 1.0*/, const bool fillcv /*= true*/, const double* weights /*= NULL*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO this will always make an error since I added overflow
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ() != fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillLatErrorBand( name, x_meas, y_meas, z_meas, meas_xshifts, meas_yshifts, meas_zshifts, cvweight, fillcv, weights);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  const int nuniverses = fReco3D->GetLatErrorBand(name)->GetNHists();
  double truth_xshifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_xshifts[i] = ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  double truth_yshifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_yshifts[i] = ( MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  double truth_zshifts[nuniverses];
  for (int i = 0 ; i<nuniverses ; ++i)
    truth_zshifts[i] = ( MnvHist::IsNotPhysicalShift( meas_zshifts[i] ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth3D->FillLatErrorBand( name, x_truth, y_truth, z_truth, truth_xshifts, truth_yshifts, truth_zshifts, cvweight, fillcv, weights);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_truth)+1) ;
  double meas_hBinXshifts[nuniverses], meas_hBinYshifts[nuniverses];

  for (int i = 0; i<nuniverses ; ++i)
  {
    if ( MnvHist::IsNotPhysicalShift( meas_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( meas_yshifts[i] ) || MnvHist::IsNotPhysicalShift( meas_zshifts[i] ) )
      meas_hBinXshifts[i] = MnvHist::NotPhysicalShiftNumber;
    else
      meas_hBinXshifts[i] = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas+meas_xshifts[i], y_meas+meas_yshifts[i], z_meas+meas_zshifts[i])+1) - hBinX ;
    if ( MnvHist::IsNotPhysicalShift( truth_xshifts[i] ) ||  MnvHist::IsNotPhysicalShift( truth_yshifts[i] ) || MnvHist::IsNotPhysicalShift( truth_zshifts[i] ) )
      meas_hBinYshifts[i] = MnvHist::NotPhysicalShiftNumber;
    else
      meas_hBinYshifts[i] = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth+truth_xshifts[i], y_truth+truth_yshifts[i], z_truth+truth_zshifts[i])+1) - hBinY ;
  }
  //! @todo Double check the weights here!
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshifts, meas_hBinYshifts, cvweight, fillcv, weights);

  return ok;

}

bool MnvResponse::FillLatErrorBand( const std::string& name, const double x_meas, const double y_meas, const double z_meas, const double x_truth, const double y_truth, const double z_truth, const double meas_xshiftDown, const double meas_xshiftUp, const double meas_yshiftDown, const double meas_yshiftUp, const double meas_zshiftDown, const double meas_zshiftUp, const double cvweight /*= 1.0*/, const bool fillcv /*= true*/ )
{
  if ( !fIs3D )
  {
    Error("MnvResponse::GetUnfoldingDimension","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  // TODO this won't work in 3d bc overflow bins
  Error("MnvResponse::Fill","3D unfolding is unsupported");
  assert(false);
  if ( fReco3D->GetNbinsX() * fReco3D->GetNbinsY() * fReco3D->GetNbinsZ()!= fMigration->GetNbinsX() || fTruth3D->GetNbinsX() * fTruth3D->GetNbinsY() * fTruth3D->GetNbinsZ()!= fMigration->GetNbinsY()  )
  {
    Error("MnvResponse::FillVertErrorBand"," The MnvH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false;
  }
  fReco3D->FillLatErrorBand( name, x_meas, y_meas, meas_xshiftDown, meas_xshiftUp, meas_yshiftDown, meas_yshiftUp, meas_zshiftDown, meas_zshiftUp, cvweight, fillcv);
  // create vectors of shifts for the truth values,
  // these will only contain the nonphysical shifts from meas_shifts
  double truth_xshiftDown , truth_xshiftUp, truth_yshiftDown , truth_yshiftUp, truth_zshiftDown , truth_zshiftUp;

  truth_xshiftDown = ( MnvHist::IsNotPhysicalShift( meas_xshiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_xshiftUp    = ( MnvHist::IsNotPhysicalShift( meas_xshiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_yshiftDown = ( MnvHist::IsNotPhysicalShift( meas_yshiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_yshiftUp    = ( MnvHist::IsNotPhysicalShift( meas_yshiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_zshiftDown = ( MnvHist::IsNotPhysicalShift( meas_zshiftDown ) )? MnvHist::NotPhysicalShiftNumber : 0.0;
  truth_zshiftUp    = ( MnvHist::IsNotPhysicalShift( meas_zshiftUp ) )? MnvHist::NotPhysicalShiftNumber : 0.0;

  fTruth3D->FillLatErrorBand( name, x_truth, y_truth, z_truth, truth_xshiftDown, truth_xshiftUp, truth_yshiftDown, truth_yshiftUp, truth_zshiftDown, truth_zshiftUp, cvweight, fillcv);
  //Fill Migration
  Double_t hBinX = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas, y_meas, z_meas)+1) ;
  Double_t hBinY = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth, y_truth, z_meas)+1) ;

  double meas_hBinXshiftDown, meas_hBinXshiftUp, meas_hBinYshiftDown, meas_hBinYshiftUp;

  if ( MnvHist::IsNotPhysicalShift( meas_xshiftDown ) || MnvHist::IsNotPhysicalShift( meas_yshiftDown ) || MnvHist::IsNotPhysicalShift( meas_zshiftDown ))
    meas_hBinXshiftDown = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinXshiftDown = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas+meas_xshiftDown, y_meas+meas_yshiftDown, z_meas+meas_zshiftDown)+1) - hBinX ;
  if ( MnvHist::IsNotPhysicalShift( meas_xshiftUp ) || MnvHist::IsNotPhysicalShift( meas_yshiftUp ) || MnvHist::IsNotPhysicalShift( meas_zshiftUp ))
    meas_hBinXshiftUp = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinXshiftUp = fMigration->GetXaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fReco3D, x_meas+meas_xshiftUp, y_meas+meas_yshiftUp, z_meas+meas_zshiftUp)+1) - hBinX ;
  if ( MnvHist::IsNotPhysicalShift( truth_xshiftDown ) || MnvHist::IsNotPhysicalShift( truth_yshiftDown ) || MnvHist::IsNotPhysicalShift( truth_zshiftDown ) )
    meas_hBinYshiftDown = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinYshiftDown = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth+truth_xshiftDown, y_truth+truth_yshiftDown, z_truth+truth_zshiftDown)+1) - hBinY ;
  if ( MnvHist::IsNotPhysicalShift( truth_xshiftUp ) || MnvHist::IsNotPhysicalShift( truth_yshiftUp ) || MnvHist::IsNotPhysicalShift( truth_zshiftUp ) )
    meas_hBinYshiftUp = MnvHist::NotPhysicalShiftNumber;
  else
    meas_hBinYshiftUp = fMigration->GetYaxis()->GetBinCenter(RooUnfoldResponse::FindBin (fTruth3D, x_truth+truth_xshiftUp, y_truth+truth_yshiftUp, z_truth+truth_zshiftUp)+1) - hBinY ;

  //! @todo Double check the weights here!
  bool ok = fMigration->FillLatErrorBand(name, hBinX, hBinY, meas_hBinXshiftDown, meas_hBinXshiftUp, meas_hBinYshiftDown, meas_hBinYshiftUp, cvweight, fillcv);

  return ok;
}


Int_t MnvResponse::GetUnfoldingDimension()
{
  Int_t dim = 0;

  if ( fIs1D )
    dim = 1;
  else if ( fIs2D )
    dim = 2;
  else if ( fIs3D )
    dim = 3;
  else
    Error("MnvResponse::GetUnfoldingDimension","Unfolding Dimension is not defined");

  return dim;
}

bool MnvResponse::GetMigrationObjects(MnvH2D* &h_migration, MnvH1D* &h_reco, MnvH1D* &h_truth)
{
  //!==========================================================================================
  //! This method returns the migration matrix, reco and truth histograms for 1D unfolding.
  //! They have been created as private data members in the Fill function
  //! of MnvResponse and are available for public use here.
  //!==========================================================================================
  if ( !fIs1D )
  {
    Error("MnvResponse::GetMigrationObjects","MnvResponse wasn't setup for 1D unfolding");
    return false;
  }
  checkIfMnvH2DNull(h_migration);
  checkIfMnvH1DNull(h_reco);
  checkIfMnvH1DNull(h_truth);

  //Creating new objects
  h_migration = (MnvH2D*)fMigration->Clone();
  h_reco = (MnvH1D*)fReco1D->Clone();
  h_truth = (MnvH1D*)fTruth1D->Clone();

  return true;
}

bool MnvResponse::GetMigrationObjects(MnvH2D* &h_migration, MnvH2D* &h_reco, MnvH2D* &h_truth)
{
  //!==========================================================================================
  //! This method returns the migration matrix, reco and truth histograms for 2D unfolding.
  //! They have been created as private data members in the Fill function
  //! of MnvResponse and are available for public use here.
  //!==========================================================================================
  if ( !fIs2D )
  {
    Error("MnvResponse::GetMigrationObjects","MnvResponse wasn't setup for 2D unfolding");
    return false;
  }
  checkIfMnvH2DNull(h_migration);
  checkIfMnvH2DNull(h_reco);
  checkIfMnvH2DNull(h_truth);

  //Creating new objects
  h_migration = new MnvH2D(*fMigration);
  h_reco = new MnvH2D(*fReco2D);
  h_truth = new MnvH2D(*fTruth2D);

  return true;
}

bool MnvResponse::GetMigrationObjects(MnvH2D* &h_migration, MnvH3D* &h_reco, MnvH3D* &h_truth)
{
  //!==========================================================================================
  //! This method returns the migration matrix, reco and truth histograms for 3D unfolding.
  //! They have been created as private data members in the Fill function
  //! of MnvResponse and are available for public use here.
  //!==========================================================================================
  if ( !fIs3D )
  {
    Error("MnvResponse::GetMigrationObjects","MnvResponse wasn't setup for 3D unfolding");
    return false;
  }
  checkIfMnvH2DNull(h_migration);
  checkIfMnvH3DNull(h_reco);
  checkIfMnvH3DNull(h_truth);

  //Creating new objects
  h_migration = (MnvH2D*)fMigration->Clone();
  h_reco = (MnvH3D*)fReco3D->Clone();
  h_truth = (MnvH3D*)fTruth3D->Clone();

  return true;
}

/***********************/
/***Helper Functions****/
/***********************/
bool MnvResponse::checkIfMnvH1DNull(const MnvH1D *hist1D, const char* name)
{
  if ( hist1D == NULL )
    return true;

  std::cout<< Form("Warning [MnvResponse]: There was a previous binning for the %s and it will be replaced",name) << std::endl;
  hist1D = NULL;

  return false;
}

bool MnvResponse::checkIfMnvH2DNull(const MnvH2D *hist2D, const char* name)
{
  if ( hist2D == NULL)
    return true;

  std::cout<< Form("Warning [MnvResponse]: There was a previous binning for the %s and it will be replaced",name) << std::endl;
  hist2D = NULL;

  return false;
}

bool MnvResponse::checkIfMnvH3DNull(const MnvH3D *hist3D, const char* name)
{
  if ( hist3D == NULL)
    return true;

  std::cout<< Form("Warning [MnvResponse]: There was a previous binning for the %s and it will be replaced",name) << std::endl;
  hist3D = NULL;

  return false;
}

void MnvResponse::is1D()
{
  fIs1D = true;
  fIs2D = false;
  fIs3D = false;
}

void MnvResponse::is2D()
{
  fIs1D = false;
  fIs2D = true;
  fIs3D = false;
}

void MnvResponse::is3D()
{
  fIs1D = false;
  fIs2D = false;
  fIs3D = true;
}

void MnvResponse::Init()
{
  fReco1D = fTruth1D= NULL;
  fReco2D = fTruth2D = NULL;
  fReco3D = fTruth3D = NULL;
  fMigration = NULL;
}

#endif
