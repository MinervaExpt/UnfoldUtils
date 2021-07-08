#ifndef UNFOLDUTILSDICT_H 
#define UNFOLDUTILSDICT_H 1

// Include files for UnfoldUtils dictionary.

/** @file UnfoldUtilsDict.h
 *  
 *
 *  @author Jeremy Wolcott <jwolcott@fnal.gov>
 *  @date   2014-06-04
 */
// ============================================================================
// UnfoldUtils
// ============================================================================

// here we need to include all the header files
// for the classes we want to make dictionaries of

#include <vector>

#include "RooUnfold/RooUnfold.h"
#include "RooUnfold/RooUnfoldBayes.h"
#include "RooUnfold/RooUnfoldBinByBin.h"
//#include "../RooUnfold/RooUnfoldDagostini.h"  # isn't built by default in RooUnfold anyhow
#include "RooUnfold/RooUnfoldErrors.h"
#include "RooUnfold/RooUnfoldInvert.h"
#include "RooUnfold/RooUnfoldParms.h"
#include "RooUnfold/RooUnfoldResponse.h"
#include "RooUnfold/RooUnfoldSvd.h"
#include "RooUnfold/RooUnfoldTUnfold.h"

#include "MinervaUnfold/MnvUnfold.h"
#include "MinervaUnfold/MnvResponse.h"

// this garbage is necessary so that gccxml is able to create dictionaries for these custom containers
// see: http://root.cern.ch/root/roottalk/roottalk10/0035.html
#ifdef __GCCXML__
template class std::map<std::string, std::string>;
template class std::map<std::string,int>;
#endif

#endif // UNFOLDUTILSDICT_H

