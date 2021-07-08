#include <cstdio>
#include <iterator>
#include <algorithm>


#include <TDecompChol.h>
#include <TRandom1.h>


#include "UnfoldCovmxAsErrorBand.h"


using PlotUtils::MnvH1D;

// remove rows, columns whose all entries are zero
TMatrixD RemoveZeroRows(const TMatrixD& covmx, std::set<int>& zero_index_vec)
{

        //print_matrix_ij(covmx, "covmx original");
    
    int N = covmx.GetNrows();
    for (int i = 0; i < N; ++i) {
        bool row_is_zero = true;
        for (int j = 0; j < N; ++j) {
            if (covmx(i,j) != 0) row_is_zero = false;
        }

        if (row_is_zero) zero_index_vec.insert(i);
    }

        //std::cout << "zero-row index:" << std::endl;
        //std::copy(zero_index_vec.begin(), zero_index_vec.end(),
        //      std::ostream_iterator<int>(std::cout, " "));
        //std::cout << std::endl;

    int n = zero_index_vec.size();
    TMatrixD covmx_nonzero(N-n, N-n);

    int i0 = 0;
    for (int i = 0; i < N; ++i) {

        if (zero_index_vec.find(i) != zero_index_vec.end()) {
            ++i0;
                //printf("skip row %d %d\n", i,i0);
            continue;
        }

        int j0 = 0;
        for (int j = 0; j < N; ++j) {
            if (zero_index_vec.find(j) != zero_index_vec.end()) {
                ++j0;
                continue;
            }

            covmx_nonzero(i-i0,j-j0) = covmx(i,j);        
        }

    }

        //print_matrix_ij(covmx_nonzero, "covmx_nonzero");
    
    return covmx_nonzero;
    
}


std::vector<TMatrixD> GenerateCorrelatedErrors(const TMatrixD& covmx, int nthrows)
{
    
    const double tolerance = 0.0;
    TDecompChol* decomp = new TDecompChol(covmx, tolerance);
    bool successful = decomp->Decompose();
    if (!successful) {
        std::cerr << "Cannot decompose" << std::endl;
        exit(1);
    }
    
    TMatrixD U = decomp->GetU();
        //std::cout << "U matrix" << std::endl;
        //U.Print();
    
        // invert the matrix
    TMatrixD UT(TMatrixD::kTransposed, U);
        //std::cout << "Transposed matrix " << std::endl;
        //UT.Print();
    
    std::vector<TRandom1*> generators;
    for (int i = 0; i < covmx.GetNrows(); ++i) {
        generators.push_back(new TRandom1(i * 1e6 + 12345));
    }
    
    std::vector<TMatrixD> correlated_data;
    for (int trial = 0; trial < nthrows; ++trial) {
        
        TMatrixD random_vector(covmx.GetNrows(),1);
        
        for (int i = 0; i < covmx.GetNrows(); ++i) {
            random_vector(i,0) = generators[i]->Gaus(0.0, 1.0);
        }
        
        TMatrixD product = UT * random_vector;
            //printf("nthrow %3d %10.4f %10.4f\n", trial, product(0,0), product(1,0));
        correlated_data.push_back(product);

    }

    return correlated_data;
}

MnvH1D* UnfoldCovmxAsErrorBand(const MnvH1D* mnvh1d_unfolded, const TMatrixD& covmx)
{

    std::set<int> row_index_vec;
    TMatrixD nonzero_row_covmx = RemoveZeroRows(covmx,row_index_vec);
     
    const int nthrows = 500;
    std::vector<TMatrixD> correlated_norm_vec = GenerateCorrelatedErrors(nonzero_row_covmx, nthrows);

    std::vector<TH1D*> universe_histograms;
    TH1D h1d_cv = mnvh1d_unfolded->GetCVHistoWithStatError();
    for (int univ = 0; univ < nthrows; ++univ) {
        TMatrixD& mat = correlated_norm_vec[univ];
            //print_matrix_ij(mat, Form("mat %3d", univ));
        TH1D* h1d_univ = (TH1D*) h1d_cv.Clone(Form("unfolded_data_univ_%3d", univ));
        int i0 = 0;
        for (int i = 0; i < covmx.GetNrows(); ++i) {
            int bin = i;
            double content_cv   = h1d_cv.GetBinContent(bin);
            if (row_index_vec.find(i) == row_index_vec.end()) {
                    //printf("\tfill bin %2d access col %d\n", i+1, i-i0);
                double content_univ = content_cv + mat(i-i0,0);
                h1d_univ->SetBinContent(bin, content_univ);
            } else {
                h1d_univ->SetBinContent(bin, content_cv);
                ++i0;
            }
        }
        
        universe_histograms.push_back(h1d_univ);
    }

    MnvH1D* mnvh1d_unfolded_with_error_band = new MnvH1D(*mnvh1d_unfolded);
    
    mnvh1d_unfolded_with_error_band->AddVertErrorBand("Unfold_error", universe_histograms);
    
    return mnvh1d_unfolded_with_error_band;
        
}
