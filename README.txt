You may be able to build this standalone by typing cmake build followed by make in this directory.  Tested for ROOT6/OSX only so far 7-13-2019

In order to use UnfoldUtils inside cint, you will need to add the following lines to your $HOME/.rootlogon.C

  if( gSystem->Getenv("PLOTUTILSROOT") )
  {
    gInterpreter->AddIncludePath( gSystem->ExpandPathName("$PLOTUTILSROOT") );
    string newpath = string(gROOT->GetMacroPath()) + ":" + string(gSystem->ExpandPathName("$PLOTUTILSROOT")) + "/PlotUtils";
    gROOT->SetMacroPath( newpath.c_str() );

    gSystem->Load( "libCintex.so" );  // needed to process the dictionaries for the objects
    Cintex::Enable();
    gSystem->Load( gSystem->ExpandPathName("$PLOTUTILSROOT/$CMTCONFIG/libplotutils.so") );

    gInterpreter->ExecuteMacro("PlotStyle.C");
  }

  if( gSystem->Getenv("UNFOLDUTILSROOT") )
  {
    gInterpreter->AddIncludePath( gSystem->ExpandPathName("$UNFOLDUTILSROOT") );
    gSystem->Load( gSystem->ExpandPathName("$UNFOLDUTILSROOT/$CMTCONFIG/libRooUnfold.so") );
    gSystem->Load( gSystem->ExpandPathName("$UNFOLDUTILSROOT/$CMTCONFIG/libUnfoldUtils.so") );
  }

If you are using a version of the Minerva Framework below v10r6p1, use this instead:

  if( gSystem->Getenv("PLOTUTILSROOT") )
  {
    gInterpreter->AddIncludePath( gSystem->ExpandPathName("$PLOTUTILSROOT") );
    gSystem->Load( gSystem->ExpandPathName("$PLOTUTILSROOT/$CMTCONFIG/libplotutils.so") );
  }
  if( gSystem->Getenv("UNFOLDUTILSROOT") )
  {
    gInterpreter->AddIncludePath( gSystem->ExpandPathName("$UNFOLDUTILSROOT") );
    gSystem->Load( gSystem->ExpandPathName("$UNFOLDUTILSROOT/$CMTCONFIG/libRooUnfold.so") );
    gSystem->Load( gSystem->ExpandPathName("$UNFOLDUTILSROOT/$CMTCONFIG/libUnfoldUtils.so") );
  }


