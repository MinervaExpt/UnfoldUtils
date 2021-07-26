# UnfoldUtils

MINERvA's ancient fork of [RooUnfold](https://gitlab.cern.ch/RooUnfold/RooUnfold).  Just like RooUnfold, this package provides a [ROOT](https://root.cern/)-centric framework for unsmearing experimental results given a smearing matrix.  On MINERvA, we almost always estimate our smearing matrices using a Monte Carlo simulation.  This fork adds a different treatment of systematic uncertainties, paralellized unfolding to power high dimensional analyses, and direct support for the [MAT](https://github.com/MinervaExpt/MAT), MINERvA's systematics format, on top of RooUnfold itself.

## Installation
If you are a MINERvA collaborator, just install [MAT-MINERvA](https://github.com/MinervaExpt/MAT-MINERvA) instead.  It installs this package too and keeps their builds synchronized.

### CMake build (for non-MINERvA experiments)
Requires at least CMake 3.

```
git clone git@github.com:MinervaExpt/UnfoldUtils.git
mkdir -p opt/build && cd opt/build
cmake ../../MAT -DCMAKE_INSTALL_PREFIX=`pwd`/.. -DCMAKE_BUILD_TYPE=Release
make install #-j 4
```

## How to use UnfoldUtils

### Learn to unfold histograms

MINERvA's data preservation project provides a complete example of how to do an analysis using the MINERvA Analysis Toolkit:
- Install this package
- [MAT-MINERvA]() extends the MAT with MINERvA-specific systematic uncertainties
- The [2021 MINERvA 101 Tutorial](https://github.com/MinervaExpt/MINERvA-101-Cross-Section) walks you through a simple inclusive cross section analysis in detail
- [Exercise C](https://github.com/MinervaExpt/MINERvA-101-Cross-Section/wiki/Exercise-C%3A-Warping-Studies) in particular focuses on how we validate our unfolding procedure for a given analysis

### Use it in your own analysis
- CMake: Works automatically if you install in the same `opt` prefix.  Just create a separate `buildYourPackage` area under `opt`.
- Makefile: Your package can link against libUnfoldUtils in `opt/lib`.  Headers to include are in `opt/include`.
- PyROOT: UnfoldUtils automatically generates python bindings thanks to ROOT.  Use the `.rootlogon.C` in the interpreter instructions below, and `from ROOT import PlotUtils`.
- ROOT's CLING Interpreter: Install this `.rootlogon.C` in either your home area or your working area
```
{
  if( gSystem->Getenv("PLOTUTILSROOT") )
  {
    string newpath = string(gROOT->GetMacroPath()) + ":" + string("${PLOTUTILSROOT}/../bin" );
    gROOT->SetMacroPath( newpath.c_str() );
    gInterpreter->AddIncludePath( "${PLOTUTILSROOT}/../include" );
    gInterpreter->AddIncludePath( "${PLOTUTILSROOT}/../include/PlotUtils" );
    std::vector<std::string> packages = { "MAT", "MAT-MINERvA" };
    for(const std::string& package: packages)
    {
      gSystem->Load( gSystem->ExpandPathName(("$PLOTUTILSROOT/lib" + package + ".so").c_str()) );
    }
  }
}
```

## Contributing

- Commit directly to the main branch for now, effectively the way CVS used to work.  If we get more breaking changes than we can handle, the respository maintainers will move to a pull-request-powered contribution workflow.
- Use `git merge` to fold in changes to your working area and other branches.  We're keeping the commit procedure as simple as possible at the cost of a little commit history bloat.
- For big changes that you want to collaborate on, create a new branch with "feature/" at the beginning of its name.
