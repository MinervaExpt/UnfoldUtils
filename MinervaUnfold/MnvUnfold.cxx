#ifndef MNV_MnvUnfold_cxx 
#define MNV_MnvUnfold_cxx 1
// HMS if you are getting strange behavior, uncomment this to have code stop when it hits an error
//#define UNFDBG
#include "MinervaUnfold/MnvUnfold.h"

//RooUnfold dependencies
#include "RooUnfold/RooUnfoldBayes.h"
#include "RooUnfold/RooUnfoldSvd.h"
#include "RooUnfold/RooUnfoldTUnfold.h"
#include "RooUnfold/RooUnfoldInvert.h"
#include "RooUnfold/RooUnfoldBinByBin.h"
#include "RooUnfold/RooUnfold.h"
#include "RooUnfold/RooUnfoldResponse.h"

//Root Stuff
#include "TFile.h"
#include "TError.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "assert.h"


//PlotUtils Stuff
#include "PlotUtils/MnvH1D.h"
#include "PlotUtils/MnvH2D.h"
#include "PlotUtils/MnvH3D.h"

using namespace PlotUtils;
using namespace MinervaUnfold;

MnvUnfold::MnvUnfold() :
  m_useBetterStatErrorCalc(false)
{}

MnvUnfold& MnvUnfold::Get()
{
  static MnvUnfold singleton;
  return singleton;
}

bool MnvUnfold::UnfoldHisto( TH1D*& h_unfold, TMatrixD &covmx, const TH2D* h_migration, const TH1D* h_mc_reco, const TH1D* h_mc_true, const TH1D *h_data, RooUnfold::Algorithm method, const Double_t regparam) const
{
  //! Migration matrix here is a TH2D of reco(x-axis) vs true (y-axis) for only-signal events
  //! h_mc_reco and h_mc_true are the Train True/Reco used for building the migration matrix.
  //! If correction by efficiency is done in a step after the unfolding
  //! h_mc_reco and h_mc_true are just the X and Y Projections of h_migration.
  if ( method != RooUnfold::kBayes && method != RooUnfold::kSVD && method != RooUnfold::kTUnfold && method != RooUnfold::kInvert && method != RooUnfold::kBinByBin )
  {
    Error("MnvUnfold::UnfoldHisto","Unknown Unfolding method. The method available are: \nRooUnfold::kBayes\nRooUnfold::kSVD\nRooUnfold::kBinByBin\nRooUnfold::kInvert\nRooUnfold::kTUnfold");
    assert( false);
  }

  //create the output unfold histogram if needed
  if( 0 == h_unfold )
  {
    // HMS - if this is non-square, you need to use the truth bins for the unfolded size.
    h_unfold = dynamic_cast<TH1D*>( h_mc_true->Clone( Form( "%s_unfold", h_data->GetName() ) ) );
    h_unfold->SetDirectory(0); //not assigned to a file
  }
  h_unfold->Reset();

  RooUnfoldResponse response(h_mc_reco, h_mc_true, h_migration);
  response.UseOverflow(1); //using under/overflow bins

  Double_t kReg = 0.;
  if ( regparam == -1e30 )
  {
    if       ( method == RooUnfold::kBayes)    kReg = 4.;
    else if  ( method == RooUnfold::kSVD  )    kReg = 6.;
    else if  ( method == RooUnfold::kTUnfold ) kReg = 1.; //TUnfold::kRegModeSize
  }
  else 
    kReg = regparam;

  RooUnfold *unfold  = RooUnfold::New(method, &response, h_data, kReg, "unfold");

  // Setting Verbosity Levels
#ifdef UNFDBG
  unfold->SetVerbose(3);
#else
  unfold->SetVerbose(0);
#endif

  const int oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;
  
  //Setting error calculation
  /*
  if (m_useBetterStatErrorCalc) {
    unfold->IncludeSystematics(1);
  }
  */

  // Getting the Unfolded Histogram
  // note that Hreco makes a clone
  TH1D *tmp = dynamic_cast<TH1D*>( unfold->Hreco() );
  Int_t cvbins = tmp->GetNbinsX() + 1 ; //including overflow
  for ( int i = 0; i<=cvbins ; ++i )
  {
    h_unfold->SetBinContent(i, tmp->GetBinContent(i) );
    h_unfold->SetBinError(i, tmp->GetBinError(i) );
  }

  // Do not want to Color Filling todo: why not?
  h_unfold->SetFillColor( 0 );

  // Getting Unfolding Covariance Matrix
  covmx.ResizeTo(unfold->Ereco());
  covmx = unfold->Ereco();

  gErrorIgnoreLevel = oldIgnoreLevel;

  tmp->SetDirectory(0);
  delete tmp;
  delete unfold;

  return true;
}


bool MnvUnfold::UnfoldHisto( TH1D*& h_unfold, const TH2D* h_migration, const TH1D* h_mc_reco, const TH1D* h_mc_true, const TH1D *h_data, RooUnfold::Algorithm method, const Double_t regparam) const
{
  TMatrixD dummyErr;
  bool status = UnfoldHisto(h_unfold, dummyErr, h_migration, h_mc_reco, h_mc_true, h_data, method, regparam);
  
  return status;
}

bool MnvUnfold::UnfoldHisto( MnvH1D*& h_unfold, const TH2D* h_migration, const TH1D* h_mc_reco, const TH1D* h_mc_true, const MnvH1D *h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam, bool addSystematics ) const
{
  bool status = false;

  //create the output unfold histogram if needed
  if( 0 == h_unfold )
  {
    //do not copy the error bands, so cast to TH1D.
    //The TH1D constructor doesn't set bin width, so manually set it
    h_unfold = new MnvH1D( *dynamic_cast<const TH1D*>(h_data_nobck) );
    h_unfold->SetNormBinWidth( h_data_nobck->GetNormBinWidth() );
    h_unfold->SetName( Form( "%s_unfold", h_data_nobck->GetName() ) );
    h_unfold->SetDirectory(0);
  }
  h_unfold->Reset();

  // Unfold the CV
  TMatrixD covmx;
  status = UnfoldHisto( (TH1D*&)h_unfold, covmx, h_migration, h_mc_reco, h_mc_true, (TH1D*)h_data_nobck, method, regparam);

  if (addSystematics)
  {
    // Dealing with Systematics
    // Notice the unfolding is done using the same migration matrix 
    // that is used for the central value

    //don't assign histograms to a file
    Bool_t oldStat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);

    // Unfolding and adding Vertical Universes
    std::vector<std::string> vertNames = h_data_nobck->GetVertErrorBandNames();
    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH1D*> vert_hists;
      const MnvVertErrorBand *errBand = h_data_nobck->GetVertErrorBand(vertNames[i]);
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j) );
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_universe->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) );
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, method, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
#ifdef UNFDBG
          assert( false); // Must return false or universes will get out of sync
#else
        return false;
#endif
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( vertNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }

    //Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data_nobck->GetLatErrorBandNames();
    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH1D*> lat_hists;
      const MnvLatErrorBand *errBand = h_data_nobck->GetLatErrorBand(latNames[i]);
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j));
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_universe->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) );
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, method, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand( latNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    // Unfold all uncorrelated errors
    {
      const std::vector<std::string> names = h_data_nobck->GetUncorrErrorNames();

      for( std::vector<std::string>::const_iterator iName = names.begin(); iName != names.end(); ++iName )
      {
        //unfold the error the same way you do the CV since errors work the same way
        const TH1D *h_errGen = h_data_nobck->GetUncorrError( *iName );
        TH1D *h_errUnfolded = dynamic_cast<TH1D*>( h_errGen->Clone( Form( "%s_unfolded", h_errGen->GetName() ) ) );
        bool foldOK = UnfoldHisto( h_errUnfolded, h_migration, h_mc_reco, h_mc_true, h_data_nobck, method, regparam );
        if( ! foldOK )
        {
          Error("MnvUnfold::UnfoldHisto", Form("Couldn't unfold uncorrelated error: %s. Skipping it...", iName->c_str()) );
          delete h_errUnfolded;
          continue;
        }

        //add the error
        h_unfold->AddUncorrError( *iName, h_errUnfolded );

        delete h_errUnfolded;
      }
    }//end of uncorrelated errors

    TH1::AddDirectory (oldStat);

  }//end adding systematics

  //! @todo Adding a Systematic Error related to the Unfolding method.  Why not just add it?
  // A systematic that comes from the propagation of errors from
  // the folded histogram.
  // h_unfold->PushCovMatrix("Unfolding",covmx );

  return status;
}

bool MnvUnfold::UnfoldHisto( TH1D*& h_unfold, const TH2D* h_migration, const TH1D *h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam ) const
{
  // Migration matrix here has to be a TH2D of reco(x-axis) vs true (y-axis) for only-signal events
  // Inefficiency/impurity not considered.
  // Use this function if you have already your own method for Background substraction
  // and efficiency correction.

  bool status = UnfoldHisto( h_unfold, h_migration, h_migration->ProjectionX(), h_migration->ProjectionY(), h_data_nobck, method, regparam );

  return status;
}


bool MnvUnfold::UnfoldHisto( MnvH1D*& h_unfold, const TH2D* h_migration, const MnvH1D *h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam, bool addSystematics ) const
{
  // Migration matrix here has to be a TH2D of reco(x-axis) vs true (y-axis) for only-signal events
  // Inefficiency/impurity not considered.
  // Use this function if you have already your own method for Background substraction
  // and efficiency correction.

  bool status = UnfoldHisto( h_unfold, h_migration, h_migration->ProjectionX(), h_migration->ProjectionY(), h_data_nobck, method, regparam, addSystematics );

  return status;
}

//=====================
//Unfold CCQE/Inclusive using same CV migration matrix or a different one per universe extracted from a file
//=====================
bool MnvUnfold::UnfoldHisto( MnvH1D*& h_unfold, TFile *f_MigrationFile, const MnvH1D* h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam, const std::string name_migration, bool addSystematics, bool useSysVariatedMigrations ) const
{
  bool status = false;

  //To be used with CCQEOneTrack (and CCInclusive)
  TString name( h_data_nobck->GetName() );
  const TH2D* h_migration = (const TH2D*)f_MigrationFile->Get( name_migration.c_str() ); //CV migration

  if (h_migration == NULL)
  {
    Error("MinervaUnfold::unfoldHisto",Form("There was no migration matrix for %s or it was empty.",name.Data() ));
#ifdef UNFDBG
  assert(status);
#endif
    return status;

  }
  if (useSysVariatedMigrations && !addSystematics)
  {
    Error("MinervaUnfold::unfoldHisto","You need to set addSystematics to true first, if you want to use the systematic variated migration matrices");
  #ifdef UNFDBG
    assert(status);
  #endif
    return status;
  }

  //change to the directory of the unfolded result so any new histograms are stored there
  //if( h_unfold->GetDirectory() )
  //I think we don't need this?  h_unfold->GetDirectory()->cd();

  if (!useSysVariatedMigrations)
  {
    status = UnfoldHisto( h_unfold, h_migration, h_data_nobck, method, regparam, addSystematics );
  }
  else
  {

    //create the output unfold histogram if needed
    if( 0 == h_unfold )
    {
      //do not copy the error bands, so cast to TH1D.
      //The TH1D constructor doesn't set bin width, so manually set it
      h_unfold = new MnvH1D( *dynamic_cast<const TH1D*>(h_data_nobck) );
      h_unfold->SetNormBinWidth( h_data_nobck->GetNormBinWidth() );
      h_unfold->SetName( Form( "%s_unfold", h_data_nobck->GetName() ) );
      h_unfold->SetDirectory(0);
    }
    h_unfold->Reset();

    // Filling the CV
    status = UnfoldHisto((TH1D*&)h_unfold, h_migration, h_migration->ProjectionX(), h_migration->ProjectionY(), (const TH1D*)h_data_nobck, method, regparam);

    if (!status)
    {
      Error("MinervaUnfold::unfoldHisto","Coudln't unfold the central value");
      return status;
    }

    // Dealing with Systematics
    // Notice the unfolding is done using different migration matrices

    //don't assign histograms to a file
    Bool_t oldStat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);


    // Unfolding and adding Vertical Universes
    std::vector<std::string> vertNames = h_data_nobck->GetVertErrorBandNames();
    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH1D*> vert_hists;
      const MnvVertErrorBand *errBand = h_data_nobck->GetVertErrorBand(vertNames[i]);
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = (const TH2D*)f_MigrationFile->Get(Form("%s_%s_%i", name_migration.c_str(), vertNames[i].c_str(), j));
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_universe->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) );
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration_universe, h_migration_universe->ProjectionX(), h_migration_universe->ProjectionY(), h_universe, method, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( vertNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }

    //Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data_nobck->GetLatErrorBandNames();
    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH1D*> lat_hists;
      const MnvLatErrorBand *errBand = h_data_nobck->GetLatErrorBand(latNames[i]);
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j));
        const TH2D* h_migration_universe = (const TH2D*)f_MigrationFile->Get(Form("%s_%s_%i", name_migration.c_str(), latNames[i].c_str(), j));
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_universe->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) );
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration_universe, h_migration_universe->ProjectionX(), h_migration_universe->ProjectionY(), h_universe, method, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand( latNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    TH1::AddDirectory (oldStat);

  }//end adding systematics

  return status;
}


//====================================
//Unfold using MnvH2D migration matrix (migration cv + error universes)
//====================================

bool MnvUnfold::UnfoldHisto( MnvH1D*& h_unfold, TMatrixD& covmx, const MnvH2D *h_migration, const MnvH1D *h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam, bool addSystematics, bool useSysVariatedMigrations ) 
{
#ifdef UNFDBG
  std::cout << " unfoldhisto with signature: bool MnvUnfold::UnfoldHisto( MnvH1D*& h_unfold, TMatrixD& covmx, const MnvH2D *h_migration, const MnvH1D *h_data_nobck, RooUnfold::Algorithm method, const Double_t regparam, bool addSystematics, bool useSysVariatedMigrations ) const" << std::endl;
  std::cout << h_unfold->GetName() << " " << " covmx " << " " << h_migration->GetName() << " " << h_data_nobck->GetName() << " " << method << " " << regparam << " " << addSystematics << " " << useSysVariatedMigrations << std::endl;
#endif
  bool status = false;
  if (useSysVariatedMigrations && !addSystematics)
  {
    Error("MinervaUnfold::unfoldHisto","You need to set addSystematics to true first, if you want to use the systematic variated migration matrices");
#ifdef UNFDBG
  assert(status);
#endif
  
    return status;
  }


  if (!useSysVariatedMigrations)
  {
    status = UnfoldHisto( h_unfold, (const TH2D*)h_migration, h_data_nobck, method, regparam, addSystematics );
  }
  else
  {
    
    //create the output unfold histogram if needed
    if( 0 == h_unfold )
    {
      std::cout << " no h_unfold so casting to TH1D " << std::endl;
      //do not copy the error bands, so cast to TH1D.
      //The TH1D constructor doesn't set bin width, so manually set it
      h_unfold = new MnvH1D( *dynamic_cast<const TH1D*>(h_migration->ProjectionY()) ); //HMS for nonsquare
      h_unfold->SetNormBinWidth( h_migration->ProjectionY()->GetNormBinWidth() );//HMS for nonsquare
      h_unfold->SetName( Form( "%s_unfold", h_data_nobck->GetName() ) );
      h_unfold->SetDirectory(0);
    }
    h_unfold->Reset();

    // Filling the CV
    TMatrixD statcov;
    status = UnfoldHisto((TH1D*&)h_unfold, statcov, (const TH2D*)h_migration, h_migration->ProjectionX(), h_migration->ProjectionY(), (TH1D*)h_data_nobck, method, regparam);

    if (!status)
    {
      Error("MinervaUnfold::unfoldHisto","Coudln't unfold the central value");
      return status;
    }

    //Fix statcov binning and push into the histogram
    // There's a bug in RooUnfold that's making it return covariance
  // matrices with two extra bins. Kill them here, with a check.  
    // Conveniently, this bug was being hidden by an offsetting bug in  
    // MnvH2D, which is now fixed
    int correctNbins=h_unfold->fN;
    int matrixRows=statcov.GetNrows();
    if(correctNbins!=matrixRows){
            std::cout << "****************************************************************************" << std::endl;
            std::cout << "*  Fixing unfolding matrix size because of RooUnfold bug. From " << matrixRows << " to " << correctNbins << std::endl;
            std::cout << "****************************************************************************" << std::endl;
      // It looks like this DTRT, since the extra last two bins don't have any content
      statcov.ResizeTo(correctNbins, correctNbins);
    }
    //Zero out the diagonal because we store that in the TH1
    for(int r=1;r<correctNbins;r++) statcov[r][r]=0;
    h_unfold->PushCovMatrix("unfoldingCov",statcov);

    //Turn off added uncertainty which doesn't matter for systematics and just slows down computation
    //    setUseBetterStatErrorCalc(false);//Turn off throwing for sys universes

    // Dealing with Systematics
    // Notice the unfolding is done using different migration matrices
    // Unfolding and adding Vertical Universes
    std::vector<std::string> vertNames = h_data_nobck->GetVertErrorBandNames();
    std::vector<std::string> vertNames_migration = h_migration->GetVertErrorBandNames();

    //Verify if we have the same vertical errors
    if ( vertNames != vertNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto","The data and migration matrix vertical errors are different!");
    for (auto h:vertNames){
      std::cout << h << " " ;
    }
    std::cout << std::endl;
    for (auto h:vertNames_migration){
      std::cout << h << " " ;
    }
    std::cout<< std::endl;
      #ifdef UNFDBG
        assert(false);
      #endif
  
      return false;
    }

    //don't assign histograms to a file
    Bool_t oldStat= TH1::AddDirectoryStatus();
    TH1::AddDirectory (kFALSE);

  //  now that MAT uses vert universes for lateral momements we need to do this for them as well.
  
    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH1D*> vert_hists;
      const MnvVertErrorBand *errBand = h_data_nobck->GetVertErrorBand(vertNames[i]);
      const MnvVertErrorBand2D *errBand_migration = h_migration->GetVertErrorBand(vertNames[i]);
      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto","The data and migration matrix don't have the same number of universes!");
      #ifdef UNFDBG
             assert(false);
      #endif

        return false;
      }
    int nUniverses = errBand->GetNHists();

   

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_unfold->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) );// HMS unsquare
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration_universe, h_migration_universe->ProjectionX(), h_migration_universe->ProjectionY(), h_universe, method, regparam);
#ifdef UNFDBG
      std::cout << " just unfolded " << h_universe->GetName() << " " << j << " " << status_universe << " " << h_universe_unfolded->GetXaxis()->GetNbins() << " " << h_universe->GetXaxis()->GetNbins() << " " << h_migration_universe->ProjectionY()->GetXaxis()->GetNbins() << std::endl;
          h_universe_unfolded->Print("ALL");
#endif
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( vertNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() ); // HMS ???

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }

    
    
    
    //Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data_nobck->GetLatErrorBandNames();
    std::vector<std::string> latNames_migration = h_migration->GetLatErrorBandNames();

    //Verify if we have the same lateral errors
    if ( latNames != latNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto","The data and migration matrix lateral errors are different!");
    #ifdef UNFDBG
           assert(false);
         #endif
     
      return false;
    }

    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH1D*> lat_hists;
      const MnvLatErrorBand *errBand = h_data_nobck->GetLatErrorBand(latNames[i]);
      const MnvLatErrorBand2D *errBand_migration = h_migration->GetLatErrorBand(latNames[i]);
      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto","The data and migration matrix don't have the same number of universes!");
      #ifdef UNFDBG
             assert(false);
           #endif
       
        return false;
      }
      int nUniverses = errBand->GetNHists();

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH1D* h_universe = dynamic_cast<const TH1D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        TH1D* h_universe_unfolded = dynamic_cast<TH1D*>( h_unfold->Clone( Form( "%s_unfolded", h_universe->GetName() ) ) ); // HMS nosquare
        bool status_universe = UnfoldHisto( h_universe_unfolded, h_migration_universe, h_migration_universe->ProjectionX(), h_migration_universe->ProjectionY(), h_universe, method, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand( latNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH1D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }


    // Unfold all uncorrelated errors
    {
      const std::vector<std::string> names = h_data_nobck->GetUncorrErrorNames();

      for( std::vector<std::string>::const_iterator iName = names.begin(); iName != names.end(); ++iName )
      {
        //unfold the error the same way you do the CV since errors work the same way
        const TH1D *h_errGen = h_data_nobck->GetUncorrError( *iName );

        TH1D *h_errUnfolded = dynamic_cast<TH1D*>( h_unfold->Clone( Form( "%s_unfolded", h_errGen->GetName() ) ) );
        bool foldOK = UnfoldHisto( h_errUnfolded, (const TH2D*)h_migration, h_errGen, method, regparam );
        if( ! foldOK )
        {
          Error("MnvUnfold::UnfoldHisto", Form("Couldn't unfold uncorrelated error: %s. Skipping it...", iName->c_str()) );
          delete h_errUnfolded;
          continue;
        }

        //add the error
        h_unfold->AddUncorrError( *iName, h_errUnfolded );

        delete h_errUnfolded;
      }
    }//end of uncorrelated errors


    TH1::AddDirectory (oldStat);

  }//end adding systematics
  //  setUseBetterStatErrorCalc(true);//Turn on throwing again
  return status;

}

//=========================================================
//=========================================================
//
// 1D Folding
//
//=========================================================
//=========================================================
bool MnvUnfold::FoldHisto(
    TH1D* &h_folded,
    const TH2D* h_migration,
    const TH1D* h_generated
    ) const
{
  //create the output unfold histogram if needed
  if( 0 == h_folded )
  {
    h_folded = dynamic_cast<TH1D*>( h_generated->Clone( Form( "%s_folded", h_generated->GetName() ) ) );
    h_folded->SetDirectory(0); //not assigned to a file
  }
  h_folded->Reset();

  const int firstBin = 0;
  const int lastBin = 1 + h_generated->GetNbinsX();


  //store the total number generated in this migration row
  const size_t nBins = lastBin-firstBin+1;
  double nGenMig[nBins];
  //iBin is reco kinematics, jBin is true
  for( size_t jBin = 0; jBin != nBins; ++jBin )
    nGenMig[jBin] = 0.;
  for( size_t iBin = 0; iBin != nBins; ++iBin )
    for( size_t jBin = 0; jBin != nBins; ++jBin )
      nGenMig[jBin] += h_migration->GetBinContent(iBin,jBin);

  //iBin is reco kinematics, jBin is true
  for( int iBin = firstBin; iBin <= lastBin; ++iBin )
  {
    double sumWgtGen  = 0.;
    double sumErrGen2 = 0.;
    for( int jBin = firstBin; jBin <= lastBin; ++jBin )
    {
      //true is stored in the Y axis
      const double nMigrating = h_migration->GetBinContent(iBin,jBin);
      const double genWgt = h_generated->GetBinContent( jBin );
      if( 0. == nMigrating || 0. == genWgt )
        continue;

      //note that nGenMig can't be 0 unless nMigrating is 0
      //todo: include stat error from migration?
      const double statErrGen    = h_generated->GetBinError( jBin );
      const double foldedWgt     = genWgt     * ( nMigrating / nGenMig[jBin] );
      const double foldedErr     = statErrGen * ( nMigrating / nGenMig[jBin] );
      sumWgtGen  += foldedWgt;
      sumErrGen2 += pow( foldedErr, 2 );
    }
    const double foldedErr = ( 0. < sumErrGen2 ) ? sqrt(sumErrGen2) : 0.;

    h_folded->SetBinContent( iBin, sumWgtGen );
    h_folded->SetBinError(   iBin, foldedErr );
  }

  return true;
}  


bool MnvUnfold::FoldHisto(
    MnvH1D* &h_folded,
    const MnvH2D* h_migration,
    const MnvH1D* h_generated,
    bool addSystematics
    ) const
{
  //create the output unfold histogram if needed
  if( 0 == h_folded )
  {
    //do not copy the error bands
    h_folded = new MnvH1D( *h_generated );
    h_folded->ClearAllErrorBands();
    h_folded->SetName( Form( "%s_folded", h_generated->GetName() ) );
    h_folded->SetDirectory(0);
  }
  h_folded->Reset();

  //do the CV
  bool status = FoldHisto( (TH1D*&)h_folded, (const TH2D*)h_migration, (const TH1D*)h_generated );

  if( addSystematics )
  {
    //--------------
    //unfold each systematic universe

    // Histograms don't go to file
    Bool_t oldStat = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);

    // Fold all vert errors
    {
      const std::vector<std::string> migrationErrors = h_migration->GetVertErrorBandNames();
      std::vector<std::string> names = h_generated->GetVertErrorBandNames();

      if( migrationErrors != names )
      {
        Error("MnvUnfold::FoldHisto", "The generated and migration vertical errors are different.  Not unfolding sys errors." );
        TH1::AddDirectory(oldStat);
        return false;
      }

      for( std::vector<std::string>::const_iterator iName = names.begin(); iName != names.end(); ++iName )
      {
        std::vector<TH1D*> hists;
        const MnvVertErrorBand *errBand = h_generated->GetVertErrorBand( *iName );
        const MnvVertErrorBand2D *errBand2D = h_migration->GetVertErrorBand( *iName );
        const int nUniverses = errBand->GetNHists();
        for( int iU = 0; iU != nUniverses; ++iU )
        {
          const TH1D *h_universeGen = errBand->GetHist(iU);
          const TH2D *h_universeMig = errBand2D->GetHist(iU);
          TH1D *h_universeFolded = dynamic_cast<TH1D*>( h_universeGen->Clone( Form( "%s_folded", h_universeGen->GetName() ) ) );
          bool foldOK = FoldHisto( h_universeFolded, h_universeMig, h_universeGen );
          if( ! foldOK )
          {
            Error("MnvUnfold::FoldHisto", Form("Couldn't unfold Universe %i from VertError: %s", iU, iName->c_str()) );
            delete h_universeFolded;
            return false; // Must return false or universes will get out of sync
          }
          hists.push_back( h_universeFolded );
        }
        //add vert error band
        h_folded->AddVertErrorBand( *iName, hists );

        //delete tmp hists
        for( std::vector<TH1D*>::iterator itHist = hists.begin() ; itHist != hists.end() ; ++itHist )
          delete *itHist;
      }
    }//done folding vert errors


    // Fold all lat errors
    {
      const std::vector<std::string> migrationErrors = h_migration->GetLatErrorBandNames();
      std::vector<std::string> names = h_generated->GetLatErrorBandNames();

      if( migrationErrors != names )
      {
        Error("MnvUnfold::FoldHisto", "The generated and migration lateral errors are different.  Not unfolding sys errors." );
        TH1::AddDirectory(oldStat);
        return false;
      }

      for( std::vector<std::string>::const_iterator iName = names.begin(); iName != names.end(); ++iName )
      {
        std::vector<TH1D*> hists;
        const MnvLatErrorBand *errBand = h_generated->GetLatErrorBand( *iName );
        const MnvLatErrorBand2D *errBand2D = h_migration->GetLatErrorBand( *iName );
        const int nUniverses = errBand->GetNHists();
        for( int iU = 0; iU != nUniverses; ++iU )
        {
          const TH1D *h_universeGen = errBand->GetHist(iU);
          const TH2D *h_universeMig = errBand2D->GetHist(iU);
          TH1D *h_universeFolded = dynamic_cast<TH1D*>( h_universeGen->Clone( Form( "%s_folded", h_universeGen->GetName() ) ) );
          bool foldOK = FoldHisto( h_universeFolded, h_universeMig, h_universeGen );
          if( ! foldOK )
          {
            Error("MnvUnfold::FoldHisto", Form("Couldn't unfold Universe %i from LatError: %s", iU, iName->c_str()) );
            delete h_universeFolded;
            return false; // Must return false or universes will get out of sync
          }
          hists.push_back( h_universeFolded );
        }
        //add vert error band
        h_folded->AddLatErrorBand( *iName, hists );

        //delete tmp hists
        for( std::vector<TH1D*>::iterator itHist = hists.begin() ; itHist != hists.end() ; ++itHist )
          delete *itHist;
      }
    }//done folding lat errors


    // Fold all uncorrelated errors
    {
      const std::vector<std::string> names = h_generated->GetUncorrErrorNames();

      for( std::vector<std::string>::const_iterator iName = names.begin(); iName != names.end(); ++iName )
      {
        //fold the error the same way you do the CV since errors work the same way
        const TH1D *h_errGen = h_generated->GetUncorrError( *iName );
        TH1D *h_errFolded = dynamic_cast<TH1D*>( h_errGen->Clone( Form( "%s_folded", h_errGen->GetName() ) ) );
        bool foldOK = FoldHisto( h_errFolded, (const TH2D*)h_migration, h_errGen );
        if( ! foldOK )
        {
          Error("MnvUnfold::FoldHisto", Form("Couldn't unfold uncorrelated error: %s. Skipping it...", iName->c_str()) );
          delete h_errFolded;
          continue;
        }

        //add the error
        h_folded->AddUncorrError( *iName, h_errFolded );

        delete h_errFolded;
      }
    }//end of uncorrelated errors

    TH1::AddDirectory(oldStat);
  }//end of if add systematics

  return status;
}



//========================================================
//========================================================
//
// 2D Unfolding
//
//========================================================
//========================================================
// TODO remove when 2d unfolding is fixed
bool MnvUnfold::HasOverflowContent(const TH2D* h) const
{
  int cellsX = h->GetXaxis()->GetNbins() + 1;
  int cellsY = h->GetYaxis()->GetNbins() + 1;

  double overflow=0;
  overflow+=h->Integral(0,      0,      0,      cellsY);
  overflow+=h->Integral(0,      cellsX, 0,      0);
  overflow+=h->Integral(0,      cellsX, cellsY, cellsY);
  overflow+=h->Integral(cellsX, cellsX, 0,      cellsY);

  return overflow!=0;
}


bool MnvUnfold::UnfoldHisto2D( TH2D* &h_unfold, TMatrixD &cov, const TH2D* h_migration2D, const TH2D* h_mc_reco, const TH2D* h_mc_true, const TH2D *h_data, const Double_t regparam ) 
{
  //! Only Bayesian method supported for two-dimensional unfolding
  //! DocDB 8687 and 8773 have more detail on 2D unfolding 

  // The +2 is for overflow/underflow
  if ( (h_mc_reco->GetNbinsX()+2) * (h_mc_reco->GetNbinsY()+2) != h_migration2D->GetNbinsX() || (h_mc_true->GetNbinsX()+2) * (h_mc_true->GetNbinsY()+2) != h_migration2D->GetNbinsY()  )
  {
    Error("MnvUnfold::UnfoldHisto2D"," The TH2D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    //std::cout<<h_mc_reco->GetName()<<", "<<h_mc_true->GetName()<<", "<<h_migration2D->GetName()<< std::endl;
    //std::cout<<h_mc_reco->GetTitle()<<", "<<h_mc_true->GetTitle()<<", "<<h_migration2D->GetTitle()<< std::endl;
    //std::cout<<(h_mc_reco->GetNbinsX()+2)<<" "<<(h_mc_reco->GetNbinsY()+2)<<" "<<h_migration2D->GetNbinsX()<<" "<<(h_mc_true->GetNbinsX()+2)<<" "<<(h_mc_true->GetNbinsY()+2) <<" "<< h_migration2D->GetNbinsY()<< std::endl;
    
    return false; 
  }

  /*if(HasOverflowContent(h_migration2D) || HasOverflowContent(h_mc_reco)
     || HasOverflowContent(h_mc_true) || HasOverflowContent(h_data)){
    Error("MnvUnfold::UnfoldHisto2D", " One of the histograms passed has entries in the under/overflow bin, which cannot be unfolded correctly. Use MnvResponse::BringWithinHistLimits() to adjust the x and y values before filling");
    return false;
  }*/

  RooUnfoldResponse response(h_mc_reco, h_mc_true, h_migration2D);
  //response.UseOverflow(0); // under/overflow bins not considered in the two-dimensional case (yet)
  RooUnfold *unfold  = RooUnfold::New(RooUnfold::kBayes, &response, h_data, regparam, "unfold2D");
  //Setting error calculation

  /*
  if (m_useBetterStatErrorCalc) {
    unfold->IncludeSystematics(1);
  }
  */

  // Setting Verbosity Levels
  unfold->SetVerbose(0);
  const int oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;

  // Getting the Unfolded Histogram
  //!@todo this is a leak.  Hreco() returns a clone
  h_unfold = dynamic_cast<TH2D*>(unfold->Hreco()->Clone( Form( "%s_unfold", h_data->GetName() ) ));
  
  // Getting Unfolding Covariance Matrix
  cov.ResizeTo(unfold->Ereco());
  cov = unfold->Ereco();
  //unfold->Print();

  gErrorIgnoreLevel = oldIgnoreLevel;

  delete unfold;

  return true;
}

bool MnvUnfold::UnfoldHisto2D( TH2D* &h_unfold, const TH2D* h_migration2D, const TH2D* h_mc_reco, const TH2D* h_mc_true, const TH2D *h_data, const Double_t regparam ) 
{
  //! DocDB 8687 and 8773 have more detail on 2D unfolding 
  TMatrixD dummyErr;
  bool status = UnfoldHisto2D(h_unfold, dummyErr, h_migration2D, h_mc_reco, h_mc_true, h_data, regparam);

  return status;
}

bool MnvUnfold::UnfoldHisto2D(TH2D* &h_unfold,  TFile *f_EventSelection, const TH2D* h_nobck, const Double_t regparam, const std::string& name_migration_enu_q2, const std::string& name_enu_q2_mcreco , const std::string& name_enu_q2_mctrue ) 
{
  //! DocDB 8687 and 8773 have more detail on 2D unfolding 
  //To be used with CCQEOneTrack (and eventually CCInclusive)
  const TH2D* h_reco = (const TH2D*)f_EventSelection->Get( name_enu_q2_mcreco.c_str() );
  const TH2D* h_true = (const TH2D*)f_EventSelection->Get( name_enu_q2_mctrue.c_str() );
  const TH2D* h_migration2D = (const TH2D*)f_EventSelection->Get( name_migration_enu_q2.c_str() );

  //change to the directory of the unfolded result so any new histograms are stored there
  //!@todo I though the convention was to pass in a NULL h_unfold, so this should segfault for ccqe and ccinclusive
  if( h_unfold->GetDirectory() )
    h_unfold->GetDirectory()->cd();

  bool status = UnfoldHisto2D(h_unfold, h_migration2D, h_reco, h_true, h_nobck, regparam);

  return status;
}

bool MnvUnfold::UnfoldHisto2D(MnvH2D* &h_unfold, const TH2D *h_migration, const TH2D *h_mc_reco, const TH2D *h_mc_true, const MnvH2D *h_data, const Double_t regparam, bool addSystematics )
{
  //! DocDB 8687 and 8773 have more detail on 2D unfolding 
  bool status = false;
 
  /*if(HasOverflowContent(h_migration) || HasOverflowContent(h_mc_reco)
     || HasOverflowContent(h_mc_true) || HasOverflowContent(h_data)){
    Error("MnvUnfold::UnfoldHisto2D", " One of the histograms passed has entries in the under/overflow bin, which cannot be unfolded correctly. Use MnvResponse::BringWithinHistLimits() to adjust the x and y values before filling");
    return false;
  }*/

  if ( h_unfold != NULL )
  {
    std::cout<<"[MnvUnfold::UnfoldHisto2D] Warning: The first MnvH2D parameter is not NULL. The object will allocate the unfolded histogram and the previous info will be lost" << std::endl;
    //delete h_unfold;
  }

  const TH2D *h_data_cv = dynamic_cast<const TH2D*>( h_data->GetCVHistoWithStatError().Clone( h_data->GetName() ) );
  TH2D *h_cv = NULL; //unfolded CV

  //! Filling the CV
  status = UnfoldHisto2D(h_cv, h_migration, h_mc_reco, h_mc_true, h_data_cv, regparam);

  h_unfold = new MnvH2D( h_data->GetCVHistoWithStatError() );
  //h_unfold->Reset();

  //!@todo check whether to include under/overflow or not
  Int_t cvbins = h_cv->GetBin( h_cv->GetNbinsX() + 1, h_cv->GetNbinsY() + 1 ) ; //including overflow

  for ( int i = 0; i<=cvbins ; ++i )
  {
    h_unfold->SetBinContent(i, h_cv->GetBinContent(i) );
    h_unfold->SetBinError(i, h_cv->GetBinError(i) );
  }

  //! Setting the Unfolding Name
  h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );

  if (addSystematics)
  {
    Bool_t oldStat= TH2::AddDirectoryStatus();
    TH2::AddDirectory (kFALSE);
    //    setUseBetterStatErrorCalc(false);//We don't need to throw the universes for every universe when they aren't used!
    //! Dealing with Systematics
    //! Notice the unfolding is done using the same migration matrix 
    //! that is used for the central value
    //!Unfolding and adding Vertical Universes
    std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH2D*> vert_hists;
      const MnvVertErrorBand2D *errBand = h_data->GetVertErrorBand( vertNames[i] );
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH2D* h_universe = dynamic_cast<const TH2D*>( errBand->GetHist(j) );
        TH2D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto2D( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto2D", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( vertNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH2D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }
    //!Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data->GetLatErrorBandNames();
    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH2D*> lat_hists;
      const MnvLatErrorBand2D *errBand = h_data->GetLatErrorBand( latNames[i] );
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH2D* h_universe = dynamic_cast<const TH2D*>( errBand->GetHist(j) );
        TH2D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto2D( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto2D", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand( latNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      //cleaning
      for (std::vector<TH2D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    TH2::AddDirectory (oldStat);
  }//end adding systematics
  //  setUseBetterStatErrorCalc(true);//Make sure this goes back to true since we potentially switched it to false in the if(sys) loop
  //cleaning
  delete h_data_cv;
  delete h_cv;

  return status;
}

bool MnvUnfold::UnfoldHisto2D(MnvH2D* &h_unfold, const MnvH2D *h_migration, const MnvH2D *h_mc_reco, const MnvH2D *h_mc_true, const MnvH2D *h_data, const Double_t regparam, bool addSystematics, bool useSysVariatedMigrations ) 
{
  /*if(HasOverflowContent(h_migration) || HasOverflowContent(h_mc_reco)
     || HasOverflowContent(h_mc_true) || HasOverflowContent(h_data)){
    Error("MnvUnfold::UnfoldHisto2D", " One of the histograms passed has entries in the under/overflow bin, which cannot be unfolded correctly. Use MnvResponse::BringWithinHistLimits() to adjust the x and y values before filling");
    return false;
  }*/
  std::cout << "calling MnvUnfold::UnfoldHisto2D(MnvH2D* &h_unfold, const MnvH2D *h_migration, const MnvH2D *h_mc_reco, const MnvH2D *h_mc_true, const MnvH2D *h_data, const Double_t regparam, bool addSystematics, bool useSysVariatedMigrations ) " << std::endl;
  //! DocDB 8687 and 8773 have more detail on 2D unfolding 
  bool status = false;
  //cv migration matrix
  const TH2D* h_migration_cv = dynamic_cast<const TH2D*> ( h_migration->GetCVHistoWithStatError().Clone( h_migration->GetName() ) );
  const TH2D* h_reco_cv = dynamic_cast<const TH2D*> ( h_mc_reco->GetCVHistoWithStatError().Clone( h_mc_reco->GetName() ) );
  const TH2D* h_truth_cv = dynamic_cast<const TH2D*> ( h_mc_true->GetCVHistoWithStatError().Clone( h_mc_true->GetName() ) );
  TMatrixD statcov;
  if (h_migration_cv == NULL)
  {
    Error("MinervaUnfold::unfoldHisto2D","There were problems getting the CV Migration Matrix." );
    return status;
  }
  if (useSysVariatedMigrations && !addSystematics)
  {
    Error("MinervaUnfold::unfoldHisto2D","You need to set addSystematics to true first, if you want to use the systematic variated migration matrices");
    return status;
  }
  if (!useSysVariatedMigrations)
    status = UnfoldHisto2D( h_unfold, h_migration_cv, h_reco_cv, h_truth_cv, h_data, regparam, addSystematics );


  if (useSysVariatedMigrations)
  {
    const TH2D *h_data_cv = dynamic_cast<const TH2D*>( h_data->GetCVHistoWithStatError().Clone( h_data->GetName() ) );
    // HMS make this work for asymmetric reco/truth binning
    TH2D *h_cv = dynamic_cast<TH2D*>( h_mc_true->GetCVHistoWithStatError().Clone( h_data->GetName() ) );
    //TH2D *h_cv = NULL;
    
    //! Filling the CV
    status = UnfoldHisto2D(h_cv, statcov, h_migration_cv, h_reco_cv, h_truth_cv, h_data_cv, regparam);

    if (!status)
    {
      Error("MinervaUnfold::unfoldHisto2D","Coudln't unfold the central value");
      return status;
    }
    h_unfold = new MnvH2D( h_mc_true->GetCVHistoWithStatError() );
    h_unfold->Reset();

    //!@todo check whether to include under/overflow or not
    Int_t cvbins = h_cv->GetBin( h_cv->GetNbinsX() + 1, h_cv->GetNbinsY() + 1 ) ; //including overflow

    for ( int i = 0; i<=cvbins ; ++i )
    {
      h_unfold->SetBinContent(i, h_cv->GetBinContent(i) );
      h_unfold->SetBinError(i, h_cv->GetBinError(i) );
    }
    //! Setting the Unfolding Name
    h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );

    //Fix statcov binning and push into the histogram
    // There's a bug in RooUnfold that's making it return covariance
  // matrices with two extra bins. Kill them here, with a check.  
    // Conveniently, this bug was being hidden by an offsetting bug in  
    // MnvH2D, which is now fixed
    int correctNbins=h_unfold->fN;
    int matrixRows=statcov.GetNrows();
    if(correctNbins!=matrixRows){
            std::cout << "****************************************************************************" << std::endl;
            std::cout << "*  Fixing 2D unfolding matrix size because of RooUnfold bug. From " << matrixRows << " to " << correctNbins << std::endl;
            std::cout << "****************************************************************************" << std::endl;
      // It looks like this DTRT, since the extra last two bins don't have any content
      statcov.ResizeTo(correctNbins, correctNbins);
    }
    //Zero out the diagonal because we store that in the TH1
    for(int r=0;r<correctNbins;r++) statcov[r][r]=0;
    h_unfold->PushCovMatrix("unfoldingCov",statcov);

    //! Dealing with Systematics
    //! Notice the unfolding is done using different migration matrices
    //!Unfolding and adding Vertical Universes
    Bool_t oldStat= TH2::AddDirectoryStatus();
    TH2::AddDirectory (kFALSE);
    //    setUseBetterStatErrorCalc(false);//Turn off throwing for sys universes
    std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
    std::vector<std::string> vertNames_migration = h_migration->GetVertErrorBandNames();

    //Verify if we have the same vertical errors
    if ( vertNames != vertNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto2D","The data and migration matrix vertical errors are different!");
      return false;
    }

    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH2D*> vert_hists;
      const MnvVertErrorBand2D *errBand = h_data->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand2D *errBand_migration = h_migration->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand2D *errBand_reco = h_mc_reco->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand2D *errBand_truth = h_mc_true->GetVertErrorBand( vertNames[i] );

      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","The data and migration matrix don't have the same number of universes!");
        return false;
      }
      if ( errBand_reco->GetNHists() != errBand_truth->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","h_mc_reco and h_mc_true don't have the same number of universes!");
        return false;
      }
      if ( errBand_migration->GetNHists() != errBand_reco->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","h_mc_reco/MCTruth and the migration matrix don't have the same number of universes!");
        return false;
      }
      
      int nUniverses = errBand->GetNHists();

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH2D* h_universe = dynamic_cast<const TH2D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        const TH2D* h_reco_universe = dynamic_cast<const TH2D*>( errBand_reco->GetHist(j) );
        const TH2D* h_truth_universe = dynamic_cast<const TH2D*>( errBand_truth->GetHist(j) );

        TH2D* h_universe_unfolded = dynamic_cast< TH2D*>( errBand_truth->GetHist(j)->Clone(h_universe->GetName()) );
        bool status_universe = UnfoldHisto2D( h_universe_unfolded, h_migration_universe, h_reco_universe, h_truth_universe, h_universe, regparam );

        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto2D", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( vertNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      // unfold the error band's copy of the CV...
      for ( int j = 0; j<=cvbins ; ++j )
      {
        h_unfold->GetVertErrorBand(vertNames[i])->SetBinContent(j, h_cv->GetBinContent(j) );
        h_unfold->GetVertErrorBand(vertNames[i])->SetBinError(j, h_cv->GetBinError(j) );
      }

      //cleaning
      for (std::vector<TH2D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }
    //!Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data->GetLatErrorBandNames();
    std::vector<std::string> latNames_migration = h_migration->GetLatErrorBandNames();

    //Verify if we have the same lateral errors
    if ( latNames != latNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto2D","The data and migration matrix lateral errors are different!");
      return false;
    }

    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH2D*> lat_hists;
      const MnvLatErrorBand2D *errBand = h_data->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand2D *errBand_migration = h_migration->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand2D *errBand_reco = h_mc_reco->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand2D *errBand_truth = h_mc_true->GetLatErrorBand( latNames[i] );

      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","The data and migration matrix don't have the same number of universes!");
        return false;
      }
      if ( errBand_reco->GetNHists() != errBand_truth->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","h_mc_reco and h_mc_true don't have the same number of universes!");
        return false;
      }
      if ( errBand_migration->GetNHists() != errBand_reco->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto2D","h_mc_reco/MCTruth and the migration matrix don't have the same number of universes!");
        return false;
      }

      int nUniverses = errBand->GetNHists();

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH2D* h_universe = dynamic_cast<const TH2D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        const TH2D* h_reco_universe = dynamic_cast<const TH2D*>( errBand_reco->GetHist(j) );
        const TH2D* h_truth_universe = dynamic_cast<const TH2D*>( errBand_truth->GetHist(j) );

        TH2D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto2D( h_universe_unfolded, h_migration_universe, h_reco_universe, h_truth_universe, h_universe, regparam );

        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto2D", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);
      if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand( latNames[i] )->SetUnivWgts( *errBand->GetUnivWgts() );

      // unfold the error band's copy of the CV...
      for ( int j = 0; j<=cvbins ; ++j )
      {
        h_unfold->GetLatErrorBand(latNames[i])->SetBinContent(j, h_cv->GetBinContent(j) );
        h_unfold->GetLatErrorBand(latNames[i])->SetBinError(j, h_cv->GetBinError(j) );
      }

      //cleaning
      for (std::vector<TH2D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    TH2::AddDirectory (oldStat);
    delete h_data_cv;
    delete h_cv;
  }//end adding systematics
  //  setUseBetterStatErrorCalc(true);//turn it back on
  //cleaning
  delete h_migration_cv;
  delete h_reco_cv;
  delete h_truth_cv;

  return status;

}

//3D Unfolding

bool MnvUnfold::UnfoldHisto3D( TH3D* &h_unfold, TMatrixD &cov, const TH2D* h_migration2D, const TH3D* h_mc_reco, const TH3D* h_mc_true, const TH3D *h_data, const Double_t regparam ) const
{
  // Only Bayesian method supported for two-dimensional unfolding

    
 Error("MnvUnfold::UnfoldHisto3D","The next line definitely won't work. I'll let you guess why");
  if ( h_mc_reco->GetNbinsX() * h_mc_reco->GetNbinsY() * h_mc_reco->GetNbinsY() != h_migration2D->GetNbinsX() || h_mc_true->GetNbinsX() * h_mc_true->GetNbinsY() * h_mc_true->GetNbinsY() != h_migration2D->GetNbinsY()  )
  {
    Error("MnvUnfold::UnfoldHisto3D"," The TH3D Measured and/or Train histogram bins don't match with the ones used for building the Migration Matrix");
    return false; 
  }
  RooUnfoldResponse response(h_mc_reco, h_mc_true, h_migration2D);
  response.UseOverflow(0); // under/overflow bins not considered in the three-dimensional case (yet)
  RooUnfold *unfold  = RooUnfold::New(RooUnfold::kBayes, &response, h_data, regparam, "unfold3D");

  // Setting Verbosity Levels
  unfold->SetVerbose(0);
  const int oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;

  //! Getting the Unfolded Histogram
  h_unfold = dynamic_cast<TH3D*>(unfold->Hreco()->Clone( Form( "%s_unfold", h_data->GetName() ) ));

  //! Getting Unfolding Covariance Matrix
  cov.ResizeTo(unfold->Ereco());
  cov = unfold->Ereco();

  gErrorIgnoreLevel = oldIgnoreLevel;

  delete unfold;

  return true;
}

bool MnvUnfold::UnfoldHisto3D( TH3D* &h_unfold, const TH2D* h_migration2D, const TH3D* h_mc_reco, const TH3D* h_mc_true, const TH3D *h_data, const Double_t regparam ) const
{
  TMatrixD dummyErr;
  bool status = UnfoldHisto3D(h_unfold, dummyErr, h_migration2D, h_mc_reco, h_mc_true, h_data, regparam);

  return status;
}

bool MnvUnfold::UnfoldHisto3D(MnvH3D* &h_unfold, const TH2D *h_migration, const TH3D *h_mc_reco, const TH3D *h_mc_true, const MnvH3D *h_data, const Double_t regparam, bool addSystematics ) const
{
  bool status = false;

  if ( h_unfold != NULL )
  {
    std::cout<<"[MnvUnfold::UnfoldHisto3D] Warning: The first MnvH3D parameter is not NULL. The object will allocate the unfolded histogram and the previous info will be lost" << std::endl;
    //delete h_unfold;
  }

  const TH3D *h_data_cv = dynamic_cast<const TH3D*>( h_data->GetCVHistoWithStatError().Clone( h_data->GetName() ) );
  TH3D *h_cv = NULL; //unfolded CV

  //! Filling the CV
  status = UnfoldHisto3D(h_cv, h_migration, h_mc_reco, h_mc_true, h_data_cv, regparam);

  h_unfold = new MnvH3D( h_data->GetCVHistoWithStatError() );
  //h_unfold->Reset();

  //!@todo check whether to include under/overflow or not
  Int_t cvbins = h_cv->GetBin( h_cv->GetNbinsX() + 1, h_cv->GetNbinsY() + 1, h_cv->GetNbinsZ() + 1 ) ; //including overflow

  for ( int i = 0; i<=cvbins ; ++i )
  {
    h_unfold->SetBinContent(i, h_cv->GetBinContent(i) );
    h_unfold->SetBinError(i, h_cv->GetBinError(i) );
  }

  //! Setting the Unfolding Name
  h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );

  if (addSystematics)
  {
    Bool_t oldStat= TH3::AddDirectoryStatus();
    TH3::AddDirectory (kFALSE);

    //! Dealing with Systematics
    //! Notice the unfolding is done using the same migration matrix 
    //! that is used for the central value
    //!Unfolding and adding Vertical Universes
    std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH3D*> vert_hists;
      const MnvVertErrorBand3D *errBand = h_data->GetVertErrorBand( vertNames[i] );
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH3D* h_universe = dynamic_cast<const TH3D*>( errBand->GetHist(j) );
        TH3D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto3D( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto3D", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);

      //cleaning
      for (std::vector<TH3D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }
    //!Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data->GetLatErrorBandNames();
    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH3D*> lat_hists;
      const MnvLatErrorBand3D *errBand = h_data->GetLatErrorBand( latNames[i] );
      int nUniverses = errBand->GetNHists();
      for (int j = 0; j != nUniverses; ++j)
      {
        const TH3D* h_universe = dynamic_cast<const TH3D*>( errBand->GetHist(j) );
        TH3D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto3D( h_universe_unfolded, h_migration, h_mc_reco, h_mc_true, h_universe, regparam);
        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto3D", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);

      //cleaning
      for (std::vector<TH3D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    TH3::AddDirectory (oldStat);
  }//end adding systematics

  //cleaning
  delete h_data_cv;
  delete h_cv;

  return status;
}

bool MnvUnfold::UnfoldHisto3D(MnvH3D* &h_unfold, const MnvH2D *h_migration, const MnvH3D *h_mc_reco, const MnvH3D *h_mc_true, const MnvH3D *h_data, const Double_t regparam, bool addSystematics, bool useSysVariatedMigrations ) const
{

  bool status = false;
  //cv migration matrix
  const TH2D* h_migration_cv = dynamic_cast<const TH2D*> ( h_migration->GetCVHistoWithStatError().Clone( h_migration->GetName() ) );
  const TH3D* h_reco_cv = dynamic_cast<const TH3D*> ( h_mc_reco->GetCVHistoWithStatError().Clone( h_mc_reco->GetName() ) );
  const TH3D* h_truth_cv = dynamic_cast<const TH3D*> ( h_mc_true->GetCVHistoWithStatError().Clone( h_mc_true->GetName() ) );

  if (h_migration_cv == NULL)
  {
    Error("MinervaUnfold::unfoldHisto3D","There were problems getting the CV Migration Matrix." );
    return status;
  }
  if (useSysVariatedMigrations && !addSystematics)
  {
    Error("MinervaUnfold::unfoldHisto3D","You need to set addSystematics to true first, if you want to use the systematic variated migration matrices");
    return status;
  }
  if (!useSysVariatedMigrations)
    status = UnfoldHisto3D( h_unfold, h_migration_cv, h_reco_cv, h_truth_cv, h_data, regparam, addSystematics );


  if (useSysVariatedMigrations)
  {
    const TH3D *h_data_cv = dynamic_cast<const TH3D*>( h_data->GetCVHistoWithStatError().Clone( h_data->GetName() ) );
    TH3D *h_cv = NULL;
    //! Filling the CV
    status = UnfoldHisto3D(h_cv, h_migration_cv, h_reco_cv, h_truth_cv, h_data_cv, regparam);

    if (!status)
    {
      Error("MinervaUnfold::unfoldHisto3D","Coudln't unfold the central value");
      return status;
    }
    h_unfold = new MnvH3D( h_data->GetCVHistoWithStatError() );
    //h_unfold->Reset();

    //!@todo check whether to include under/overflow or not
    Int_t cvbins = h_cv->GetBin( h_cv->GetNbinsX() + 1, h_cv->GetNbinsY() + 1, h_cv->GetNbinsZ() + 1 ) ; //including overflow

    for ( int i = 0; i<=cvbins ; ++i )
    {
      h_unfold->SetBinContent(i, h_cv->GetBinContent(i) );
      h_unfold->SetBinError(i, h_cv->GetBinError(i) );
    }
    //! Setting the Unfolding Name
    h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );

    //! Dealing with Systematics
    //! Notice the unfolding is done using different migration matrices
    //!Unfolding and adding Vertical Universes
    Bool_t oldStat= TH3::AddDirectoryStatus();
    TH3::AddDirectory (kFALSE);
    std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
    std::vector<std::string> vertNames_migration = h_migration->GetVertErrorBandNames();

    //Verify if we have the same vertical errors
    if ( vertNames != vertNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto3D","The data and migration matrix vertical errors are different!");
      return false;
    }

    for( unsigned int i = 0; i != vertNames.size(); ++i )
    {
      std::vector<TH3D*> vert_hists;
      const MnvVertErrorBand3D *errBand = h_data->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand2D *errBand_migration = h_migration->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand3D *errBand_reco = h_mc_reco->GetVertErrorBand( vertNames[i] );
      const MnvVertErrorBand3D *errBand_truth = h_mc_true->GetVertErrorBand( vertNames[i] );

      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","The data and migration matrix don't have the same number of universes!");
        return false;
      }
      if ( errBand_reco->GetNHists() != errBand_truth->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","h_mc_reco and h_mc_true don't have the same number of universes!");
        return false;
      }
      if ( errBand_migration->GetNHists() != errBand_reco->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","h_mc_reco/MCTruth and the migration matrix don't have the same number of universes!");
        return false;
      }
      int nUniverses = errBand->GetNHists();

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH3D* h_universe = dynamic_cast<const TH3D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        const TH3D* h_reco_universe = dynamic_cast<const TH3D*>( errBand_reco->GetHist(j) );
        const TH3D* h_truth_universe = dynamic_cast<const TH3D*>( errBand_truth->GetHist(j) );

        TH3D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto3D( h_universe_unfolded, h_migration_universe, h_reco_universe, h_truth_universe, h_universe, regparam );

        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto3D", Form("Couldn't unfold Universe %i from VertError: %s",j, vertNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        vert_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddVertErrorBand(vertNames[i], vert_hists);

      //cleaning
      for (std::vector<TH3D*>::iterator itHist = vert_hists.begin() ; itHist != vert_hists.end() ; ++ itHist)
        delete *itHist;
    }
    //!Unfolding and adding Lateral Universes
    std::vector<std::string> latNames = h_data->GetLatErrorBandNames();
    std::vector<std::string> latNames_migration = h_migration->GetLatErrorBandNames();

    //Verify if we have the same lateral errors
    if ( latNames != latNames_migration )
    {
      Error("MinervaUnfold::unfoldHisto3D","The data and migration matrix lateral errors are different!");
      return false;
    }

    for( unsigned int i = 0; i != latNames.size(); ++i )
    {
      std::vector<TH3D*> lat_hists;
      const MnvLatErrorBand3D *errBand = h_data->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand2D *errBand_migration = h_migration->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand3D *errBand_reco = h_mc_reco->GetLatErrorBand( latNames[i] );
      const MnvLatErrorBand3D *errBand_truth = h_mc_true->GetLatErrorBand( latNames[i] );

      if ( errBand->GetNHists() != errBand_migration->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","The data and migration matrix don't have the same number of universes!");
        return false;
      }
      if ( errBand_reco->GetNHists() != errBand_truth->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","h_mc_reco and h_mc_true don't have the same number of universes!");
        return false;
      }
      if ( errBand_migration->GetNHists() != errBand_reco->GetNHists() )
      {
        Error("MinervaUnfold::unfoldHisto3D","h_mc_reco/MCTruth and the migration matrix don't have the same number of universes!");
        return false;
      }
      int nUniverses = errBand->GetNHists();

      for (int j = 0; j != nUniverses; ++j)
      {
        const TH3D* h_universe = dynamic_cast<const TH3D*>( errBand->GetHist(j) );
        const TH2D* h_migration_universe = dynamic_cast<const TH2D*>( errBand_migration->GetHist(j) );
        const TH3D* h_reco_universe = dynamic_cast<const TH3D*>( errBand_reco->GetHist(j) );
        const TH3D* h_truth_universe = dynamic_cast<const TH3D*>( errBand_truth->GetHist(j) );

        TH3D* h_universe_unfolded = NULL;
        bool status_universe = UnfoldHisto3D( h_universe_unfolded, h_migration_universe, h_reco_universe, h_truth_universe, h_universe, regparam );

        if (!status_universe)
        {
          Error("MnvUnfold::UnfoldHisto3D", Form("Couldn't unfold Universe %i from LatError: %s",j, latNames[i].c_str()) );
          return false; // Must return false or universes will get out of sync
        }
        lat_hists.push_back(h_universe_unfolded);
      }
      h_unfold->AddLatErrorBand(latNames[i], lat_hists);

      //cleaning
      for (std::vector<TH3D*>::iterator itHist = lat_hists.begin() ; itHist != lat_hists.end() ; ++ itHist)
        delete *itHist;
    }

    TH3::AddDirectory (oldStat);
    delete h_data_cv;
    delete h_cv;
  }//end adding systematics

  //cleaning
  delete h_migration_cv;
  delete h_reco_cv;
  delete h_truth_cv;

  return status;

}


bool MnvUnfold::UnfoldHisto2D( TH2D* &h_unfold, TMatrixD &cov, RooUnfoldResponse* response, const TH2D *h_data, const Double_t regparam ) 
{
  //! Only Bayesian method supported for two-dimensional unfolding
  response->UseOverflow(0);
  RooUnfold *unfold  = RooUnfold::New(RooUnfold::kBayes, response, h_data, regparam, "unfold2D");

  // Setting Verbosity Levels
  unfold->SetVerbose(0);
  const int oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;

  //! Getting the Unfolded Histogram
  h_unfold = dynamic_cast<TH2D*>(unfold->Hreco()->Clone( Form( "%s_unfold", h_data->GetName() ) ));

  //! Getting Unfolding Covariance Matrix
  cov.ResizeTo(unfold->Ereco());
  cov = unfold->Ereco();

  gErrorIgnoreLevel = oldIgnoreLevel;

  delete unfold;

  return true;
}

bool MnvUnfold::UnfoldHisto2D( TH2D* &h_unfold, RooUnfoldResponse* response, const TH2D *h_data, const Double_t regparam ) 
{
  TMatrixD dummyErr;
  bool status = UnfoldHisto2D(h_unfold, dummyErr, response, h_data, regparam);

  return status;
}

template <class T>
TH1D* AugmentHistogram(const T* const hist) {
  int ND = hist->GetSize();
  TH1D* tmpHist = new TH1D(Form( "%s_augmented", hist->GetName()),hist->GetTitle(),ND,0,ND);
  for (int i=0;i<hist->GetSize();++i) {
    tmpHist->SetBinContent(i+1,hist->GetBinContent(i));
    tmpHist->SetBinError(i+1,hist->GetBinError(i));
  }
  return tmpHist;
}

TH2D* AugmentMigration(const TH2* const mig,int ne, int nc) {
  TH2D* tmpHist = new TH2D(Form( "%s_augmented", mig->GetName()),mig->GetTitle(),ne,0,ne,nc,0,nc);
  int xmax = std::min(mig->GetNbinsX()+2,ne);
  int ymax = std::min(mig->GetNbinsY()+2,nc);
  for (int i=0;i<xmax;++i) {
    for (int j=0;j<ymax;++j) {
      tmpHist->SetBinContent(i+1,j+1,mig->GetBinContent(i,j));
      tmpHist->SetBinError(i+1,j+1,mig->GetBinError(i,j));
    }
  }
  return tmpHist;
}

template <class T>
bool MnvUnfold::UnfoldHistoWithFakes(T* h_unfold, TMatrixD &covmx, const TH2* const h_migration, const T* const h_data, const T* const  h_model_reco, const T* const h_model_truth,  const T* const h_model_background, double regparam) const {
  if (!h_migration || !h_data) {
    Error("MnvUnfold::UnfoldHistoWithFakes","Empty migration matrix or data. I can't unfold without them.");
    return false;
  }
  int ne= h_data->GetSize();
  int nc = (h_model_truth?h_model_truth->GetSize():h_data->GetSize())+ (h_model_background? 1:0);
  TH2D* modelMig = AugmentMigration(h_migration,ne,nc);
  if (h_model_background) {
    for (int i=0;i<ne;++i) {
      modelMig->SetBinContent(i+1,nc,h_model_background->GetBinContent(i));
      modelMig->SetBinError(i+1,nc,h_model_background->GetBinError(i));
    }
  }
  TH1D* modeltruth = modelMig->ProjectionY();
  if (h_model_truth) {
    for (int i=0;i<h_model_truth->GetSize();++i) {
      modeltruth->SetBinContent(i+1,h_model_truth->GetBinContent(i));
      modeltruth->SetBinError(i+1,h_model_truth->GetBinError(i));
    }
  }

  TH1D* modelreco = h_model_reco ? AugmentHistogram(h_model_reco) : modelMig->ProjectionX();
  TH1D* augdata = AugmentHistogram(h_data);

  RooUnfoldResponse response(modelreco, modeltruth, modelMig);
  RooUnfold *unfold  = RooUnfold::New( RooUnfold::kBayes, &response, augdata,regparam, "unfold");
  // Setting Verbosity Levels
  unfold->SetVerbose(0);
  const int oldIgnoreLevel = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kWarning;
  if (m_useBetterStatErrorCalc) {
    unfold->IncludeSystematics(1);
  }

  TH1D* h_unfold_tmp = dynamic_cast<TH1D*>(unfold->Hreco());
  covmx.ResizeTo(h_unfold_tmp->GetNbinsX(),h_unfold_tmp->GetNbinsX());
  covmx = unfold->Ereco();

  if (!h_unfold) {
    h_unfold=dynamic_cast<T*>((h_model_truth?h_model_truth:h_data)->Clone(Form( "%s_unfold", h_data->GetName())));
  }
  h_unfold->Reset();

  for (int i = 0;i<h_unfold->GetSize();++i) {
    h_unfold->SetBinContent(i,h_unfold_tmp->GetBinContent(i+1));
    h_unfold->SetBinError(i,h_unfold_tmp->GetBinError(i+1));
  }
  //additional entries are fakes, get rid of them.
  covmx.ResizeTo(h_unfold->GetSize(),h_unfold->GetSize());

  gErrorIgnoreLevel = oldIgnoreLevel;
  delete modelMig;
  delete modelreco;
  delete modeltruth;
  delete augdata;
  delete unfold;
  delete h_unfold_tmp;
  return true;
}

template <class T>
bool MnvUnfold::UnfoldVertErrorBands(T* h_unfold, const PlotUtils::MnvH2D* const h_migration, const T* const h_data, const T* const  h_model_reco, const T* const h_model_truth,  const T* const h_model_background, std::string errorband_name,double regparam,bool useSysVariatedMigrations) const {
  auto errBand = h_unfold->GetVertErrorBand(errorband_name);
  if (errBand) {
    int nUniverses = errBand->GetNHists();
    for (int j = 0; j != nUniverses; ++j) {
      auto h_data_universe = h_data->GetVertErrorBand(errorband_name)->GetHist(j);
      auto h_unfold_universe = h_unfold->GetVertErrorBand(errorband_name)->GetHist(j);
      const TH2D* h_migration_universe = h_migration;
      auto h_model_reco_universe= h_data_universe;
      h_model_reco_universe=h_model_reco;
      auto h_model_truth_universe= h_data_universe;
      h_model_truth_universe=h_model_truth;
      auto h_model_background_universe=h_data_universe;
      h_model_background_universe=h_model_background;
      if (useSysVariatedMigrations) {
        h_migration_universe = (h_migration->GetVertErrorBand(errorband_name)->GetHist(j));
        if (h_model_reco) h_model_reco_universe = (h_model_reco->GetVertErrorBand(errorband_name)->GetHist(j));
        if (h_model_truth) h_model_truth_universe = (h_model_truth->GetVertErrorBand(errorband_name)->GetHist(j));
        if (h_model_background) h_model_background_universe = (h_model_background->GetVertErrorBand(errorband_name)->GetHist(j));
      }
      TMatrixD Dummy(1,1);
      bool status_universe = UnfoldHistoWithFakes( h_unfold_universe, Dummy,h_migration_universe, h_data_universe, h_model_reco_universe, h_model_truth_universe, h_model_background_universe, regparam);
      if (!status_universe) {
        Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from VertError: %s",j, errorband_name.c_str()) );
        return false;
      }
    }
    if ( errBand->GetUnivWgts() ) h_unfold->GetVertErrorBand( errorband_name )->SetUnivWgts( *errBand->GetUnivWgts() );
    return true;
  }
  return false;
}

template <class T>
bool MnvUnfold::UnfoldLatErrorBands(T* h_unfold, const PlotUtils::MnvH2D* const h_migration, const T* const h_data, const T* const  h_model_reco, const T* const h_model_truth,  const T* const h_model_background, std::string errorband_name,double regparam,bool useSysVariatedMigrations) const {
  auto errBand = h_unfold->GetLatErrorBand(errorband_name);
  if (errBand) {
    int nUniverses = errBand->GetNHists();
    for (int j = 0; j != nUniverses; ++j) {
      auto h_data_universe = h_data->GetLatErrorBand(errorband_name)->GetHist(j);
      auto h_unfold_universe = h_unfold->GetLatErrorBand(errorband_name)->GetHist(j);
      const TH2D* h_migration_universe = h_migration;
      auto h_model_reco_universe= h_data_universe;
      h_model_reco_universe=h_model_reco;
      auto h_model_truth_universe= h_data_universe;
      h_model_truth_universe=h_model_truth;
      auto h_model_background_universe=h_data_universe;
      h_model_background_universe=h_model_background;
      if (useSysVariatedMigrations) {
        h_migration_universe = (h_migration->GetLatErrorBand(errorband_name)->GetHist(j));
        if (h_model_reco) h_model_reco_universe = (h_model_reco->GetLatErrorBand(errorband_name)->GetHist(j));
        if (h_model_truth) h_model_truth_universe = (h_model_truth->GetLatErrorBand(errorband_name)->GetHist(j));
        if (h_model_background) h_model_background_universe = (h_model_background->GetLatErrorBand(errorband_name)->GetHist(j));
      }
      TMatrixD Dummy(1,1);
      bool status_universe = UnfoldHistoWithFakes( h_unfold_universe, Dummy,h_migration_universe, h_data_universe, h_model_reco_universe, h_model_truth_universe, h_model_background_universe, regparam);
      if (!status_universe) {
        Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from LatError: %s",j, errorband_name.c_str()) );
        return false;
      }
    }
    if ( errBand->GetUnivWgts() ) h_unfold->GetLatErrorBand(errorband_name )->SetUnivWgts( *errBand->GetUnivWgts() );
    return true;
  }
  return false;
}

template <class T>
bool MnvUnfold::UnfoldUncorrError(T* h_unfold, const PlotUtils::MnvH2D* const h_migration, const T* const h_data, const T* const  h_model_reco, const T* const h_model_truth,  const T* const h_model_background, std::string errorband_name,double regparam,bool useSysVariatedMigrations) const {
  auto err = h_unfold->GetUncorrError(errorband_name);
  if (err) {
    auto h_data_universe = h_data->GetUncorrError(errorband_name);
    auto h_unfold_universe = h_unfold->GetUncorrError(errorband_name);
    const TH2D* h_migration_universe = h_migration;
    auto h_model_reco_universe= h_data_universe;
    h_model_reco_universe=h_model_reco;
    auto h_model_truth_universe= h_data_universe;
    h_model_truth_universe=h_model_truth;
    auto h_model_background_universe=h_data_universe;
    h_model_background_universe=h_model_background;
    if (useSysVariatedMigrations) {
      if (h_model_reco) h_model_reco_universe = (h_model_reco->GetUncorrError(errorband_name));
      if (h_model_truth) h_model_truth_universe = (h_model_truth->GetUncorrError(errorband_name));
      if (h_model_background) h_model_background_universe = (h_model_background->GetUncorrError(errorband_name));
    }
    TMatrixD Dummy(1,1);
    bool status_universe = UnfoldHistoWithFakes( h_unfold_universe, Dummy,h_migration_universe, h_data_universe, h_model_reco_universe, h_model_truth_universe, h_model_background_universe, regparam);
    if (!status_universe) {
      //Error("MnvUnfold::Unfold", Form("Couldn't unfold Universe %i from LatError: %s",j, errorband_name.c_str()) );
      return false;
    }
    return true;
  }
  return false;
}

bool MnvUnfold::UnfoldHistoWithFakes(PlotUtils::MnvH1D* &h_unfold, TMatrixD &covmx, const PlotUtils::MnvH2D* const h_migration, const PlotUtils::MnvH1D* const h_data, const PlotUtils::MnvH1D* const  h_model_reco, const PlotUtils::MnvH1D* const h_model_truth, const PlotUtils::MnvH1D* const h_model_background, double regparam,bool addSystematics,bool useSysVariatedMigrations) const {
   if ( !h_unfold ) {
     //do not copy the error bands, so cast to TH1D.
     //The TH1D constructor doesn't set bin width, so manually set it
     h_unfold = new MnvH1D( *dynamic_cast<const TH1D*>(h_model_truth? h_model_truth:h_data) );
     //h_unfold->SetNormBinWidth( h_data_nobck->GetNormBinWidth() );
     h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );
     h_unfold->SetDirectory(0);
   }
   h_unfold->Reset();
   TH1D* CV_unfolded =h_unfold;
   UnfoldHistoWithFakes<TH1D>(CV_unfolded,covmx,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,regparam);
   // pushing cov matrix to MnvHXD
   // remove diagonal element of cov matrix b/c it is stored in histogram bin error.
   TMatrixD tmp = covmx;
   for (int i = 0;i<covmx.GetNrows();++i) {
     tmp[i][i]=0;
   }
   h_unfold->PushCovMatrix("unfoldingCov",tmp);

   if (addSystematics) {
     bool oldStat= TH1::AddDirectoryStatus();
     TH1::AddDirectory (kFALSE);
     h_unfold->AddMissingErrorBandsAndFillWithCV(*h_data);

     //vertical shifts
     std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
     for( unsigned int i = 0; i != vertNames.size(); ++i ) {
       UnfoldVertErrorBands(h_unfold,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,vertNames[i],regparam,useSysVariatedMigrations);
       h_unfold->GetVertErrorBand(vertNames[i])->TH1D::operator=(*CV_unfolded);
     }

     //Lateral errorbands
     vertNames = h_data->GetLatErrorBandNames();
     for( unsigned int i = 0; i != vertNames.size(); ++i ) {
       UnfoldLatErrorBands(h_unfold,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,vertNames[i],regparam,useSysVariatedMigrations);
       h_unfold->GetLatErrorBand(vertNames[i])->TH1D::operator=(*CV_unfolded);
     }

     vertNames = h_data->GetUncorrErrorNames();
     for( unsigned int i = 0; i != vertNames.size(); ++i ) {
       UnfoldUncorrError(h_unfold,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,vertNames[i],regparam,useSysVariatedMigrations);
     }

     TH1::AddDirectory (oldStat);
   }
   return true;
 }

bool MnvUnfold::UnfoldHistoWithFakes(PlotUtils::MnvH2D* &h_unfold, TMatrixD &covmx, const PlotUtils::MnvH2D* const h_migration, const PlotUtils::MnvH2D* const h_data, const PlotUtils::MnvH2D* const  h_model_reco, const PlotUtils::MnvH2D* const h_model_truth, const PlotUtils::MnvH2D* const h_model_background, double regparam,bool addSystematics,bool useSysVariatedMigrations) const {
   if ( !h_unfold ) {
     //do not copy the error bands, so cast to TH1D.
     //The TH1D constructor doesn't set bin width, so manually set it
     h_unfold = new MnvH2D( *dynamic_cast<const TH2D*>(h_model_truth? h_model_truth:h_data) );
     //h_unfold->SetNormBinWidth( h_data_nobck->GetNormBinWidth() );
     h_unfold->SetName( Form( "%s_unfold", h_data->GetName() ) );
     h_unfold->SetDirectory(0);
   }
   h_unfold->Reset();
   TH2D* CV_unfolded =h_unfold;
   UnfoldHistoWithFakes<TH2D>(CV_unfolded,covmx,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,regparam);
   TMatrixD tmp = covmx;
   for (int i = 0;i<covmx.GetNrows();++i) {
     tmp[i][i]=0;
   }
   h_unfold->PushCovMatrix("unfoldingCov",tmp);

   if (addSystematics) {
     bool oldStat= TH1::AddDirectoryStatus();
     TH1::AddDirectory (kFALSE);
     h_unfold->AddMissingErrorBandsAndFillWithCV(*h_data);

     //vertical shifts
     std::vector<std::string> vertNames = h_data->GetVertErrorBandNames();
     for( unsigned int i = 0; i != vertNames.size(); ++i ) {
       UnfoldVertErrorBands(h_unfold,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,vertNames[i],regparam,useSysVariatedMigrations);
       h_unfold->GetVertErrorBand(vertNames[i])->TH2D::operator=(*CV_unfolded);
     }

     //Lateral errorbands
     vertNames = h_data->GetLatErrorBandNames();
     for( unsigned int i = 0; i != vertNames.size(); ++i ) {
       UnfoldLatErrorBands(h_unfold,h_migration,h_data,h_model_reco,h_model_truth,h_model_background,vertNames[i],regparam,useSysVariatedMigrations);
       h_unfold->GetLatErrorBand(vertNames[i])->TH2D::operator=(*CV_unfolded);
     }
     // 2D histogram don't have Uncorr Errors.

     TH1::AddDirectory (oldStat);
   }
   return true;
 }

bool MnvUnfold::UnfoldHistoWithFakes(PlotUtils::MnvH3D* &h_unfold, TMatrixD &covmx, const PlotUtils::MnvH2D* const h_migration, const PlotUtils::MnvH3D* const h_data, const PlotUtils::MnvH3D* const  h_model_reco, const PlotUtils::MnvH3D* const h_model_truth, const PlotUtils::MnvH3D* const h_model_background, double regparam,bool addSystematics,bool useSysVariatedMigrations) const {
  Error("MnvUnfold::Unfold","3D unfolding with fake is not supported.");
  return false;
 }



#endif
