#include <pthread.h>
#include <cstdio>

#include <TMatrixD.h>


#include "matrix_mult_mt.h"


#define MAX_THREAD 8
//#define i_am_dan

// multi-threaded implementation
void* multiply_on_thread(void* data)
{

    input_data* input_ptr = (input_data*) data;
    const double* a_array = input_ptr->a_array_;
    const double* b_array = input_ptr->b_array_;
    double* c_array       = input_ptr->c_array_;

    int a_nrow = input_ptr->a_nrow_;
    int a_ncol = input_ptr->a_ncol_;
        //int b_nrow = input_ptr->b_nrow_;
        //int b_ncol = input_ptr->b_ncol_;
    int start  = input_ptr->start_;
    int end    = input_ptr->end_;

   
    //printf("matrix multiply\n");
    //printf("result matrix is %d x %d\n", a_nrow, b_ncol);
    //printf("\t   multiply_mt: start = %d   end = %d\n", start, end);

      
    int c_ncol = a_nrow;
    
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < a_nrow; ++j) {
            double c_ij = 0.0;
            for (int k = 0; k < a_ncol; ++k) {
                    //c_ij += (*(a_array + i*a_ncol + k)) * (*(b_array + k*b_ncol + j));
                c_ij += (*(a_array + i*a_ncol + k)) * (*(b_array + j*a_ncol + k));

            }
            
            *(c_array + i*c_ncol + j) = c_ij;
        }

    }
    
    return NULL;
    
}


// matrix multiplication where matrix data are stored in arrays
void multiply_mt_impl(const double* a_array, const double* b_array, double* c_array,
                      const int a_nrow, const int a_ncol, const int b_nrow, const int b_ncol)
{

    double breaks[MAX_THREAD+1];
    for (int i = 0; i < MAX_THREAD; ++i) {
        breaks[i] = i * int(a_nrow / MAX_THREAD);
    }

    breaks[MAX_THREAD] = MAX_THREAD * int(a_nrow / MAX_THREAD) + a_nrow % MAX_THREAD;

    
    	// declaring four threads 
    pthread_t threads[MAX_THREAD]; 
    
	// Creating four threads, each evaluating its own part 
    for (int i = 0; i < MAX_THREAD; i++) { 
        int start = breaks[i];
        int end   = breaks[i + 1];

        
        input_data* data_i = new input_data;
        data_i->a_array_ = a_array;
        data_i->b_array_ = b_array;
        data_i->c_array_ = c_array;
        data_i->a_nrow_ = a_nrow;
        data_i->a_ncol_ = a_ncol;
        data_i->b_nrow_ = b_nrow;
        data_i->b_ncol_ = b_ncol;
        data_i->start_ = start;
        data_i->end_   = end;

        
            //printf("start = %d  end = %d\n", start, end);
        
        pthread_create(&threads[i], NULL, multiply_on_thread, (void*)(data_i)); 


    } 
    
	// joining and waiting for all threads to complete 
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);	 
    }


}

// matrix multiplication using root TMatrixD objects
TMatrixD multiply_mt(const TMatrixD& a, const TMatrixD& b)
{
    
#ifdef i_am_dan

    printf("use multiply_mt with %d threads\n", MAX_THREAD);

    TMatrixD B(TMatrixD::kTransposed, b);
    
    int a_nrow = a.GetNrows();
    int a_ncol = a.GetNcols();
    int b_nrow = b.GetNrows();
    int b_ncol = b.GetNcols();
    const double* a_array = a.GetMatrixArray();
    const double* b_array = B.GetMatrixArray();
    double* c_array = new double[a_nrow * b_ncol];

    
    multiply_mt_impl(a_array, b_array, c_array, a_nrow, a_ncol, b_nrow, b_ncol);

    TMatrixD result(a_nrow, b_ncol, c_array);

    delete[] c_array; // root TMatrixD constructor copies the array
    
    return result;

#else

    TMatrixD result(a, TMatrixD::kMult, b);

    return result;

#endif
    
}

void ABAT_MT(const TMatrixD& a, const TVectorD& b, TMatrixD& c)
{
  // Fills C such that C = A * B * A^T, where B is a diagonal matrix specified by the vector.
  // Note that C cannot be the same object as A.
  TMatrixD d (TMatrixD::kTransposed, a);
  d.NormByColumn (b, "M");
  c = multiply_mt(a,d);
  
}

