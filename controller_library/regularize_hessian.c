#include "regularize_hessian.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <dlfcn.h>

// Forward declare LAPACK routine for Eigenvalue decomposition of a real-symmetric matrix
// void dsyev_(const char *jobz, const char *uplo, int *n, double *a, int *lda, double *w, double *work, int *lwork, int *info);
// // Forward declare BLAS routine for general matrix-matrix multiplication
// int dgemm_(const char *transa, const char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc);
// Forward declare BLAS routine for scalar-vector multiplication
// int daxpy_(int *n, double *da, double *dx, int *incx, double *dy, int *incy);

int regularize_hessian_calculate_mem_size(int n) {
    // Q: n x n
    // Ld: n
    // QL: n x n
    return (sizeof(regularize_hessian_mem) + n*sizeof(double) + 2*n*n*sizeof(double));
}

regularize_hessian_mem *regularize_hessian_assign_mem(int n, void *raw_memory) {
    char *c_ptr = (char *) raw_memory;

    regularize_hessian_mem *out = (regularize_hessian_mem *) c_ptr;
    c_ptr += sizeof(regularize_hessian_mem);

    out->Q = (double *) c_ptr;
    c_ptr += n*n*sizeof(double);

    out->Ld = (double *) c_ptr;
    c_ptr += n*sizeof(double);

    out->QL = (double *) c_ptr;
    c_ptr += n*n*sizeof(double);

    assert(c_ptr == raw_memory + regularize_hessian_calculate_mem_size(n));

    int flag = RTLD_DEEPBIND | RTLD_LAZY;
    void *blas_handle = dlopen("/usr/lib/libblas.so", flag);
    void *lapack_handle = dlopen("/usr/lib/liblapack.so", flag);
    out->blas_daxpy = dlsym(blas_handle,"daxpy_");
    out->blas_dsyev = dlsym(lapack_handle,"dsyev_");
    out->blas_dgemm = dlsym(blas_handle,"dgemm_");

    return out;
}

regularize_hessian_mem *regularize_hessian_create_mem(int n) {
    int size = regularize_hessian_calculate_mem_size(n);
    void *mem = malloc(size);
    regularize_hessian_mem *out = regularize_hessian_assign_mem(n, mem);
    return out;
}

int regularize_hessian(double *H, int n, double eps, regularize_hessian_mem *mem) {
    // Aliases
    double *Q = mem->Q;
    double *Ld = mem->Ld;
    double *QL = mem->QL;

    // Copy input
    memcpy(Q, H, n*n*sizeof(double));

    // Eigenvalue decomposition
    const char jobz = 'V'; // Eigenvalues and eigenvectors
    const char uplo = 'U'; // Upper triangular
    int info;
    int lwork = 10 + (64 + 2) * n;
    double dsyev_work [lwork];
    mem->blas_dsyev(&jobz, &uplo, &n, Q, &n, Ld, dsyev_work, &lwork, &info);

    // printf("Eigenvalues: ");
    // for (int i=0; i<n; ++i) printf("%.10f\t", Ld[i]);
    // printf("\n");
    
    // Project eigenvalues to positive orthant
    for (int i=0; i<n; ++i) Ld[i] = fmax(Ld[i], eps);
    // for (int i=0; i<n; ++i) Ld[i] = fabs(Ld[i]);


    // Compute QL=Q*L
    int incx = 1;
    int incy = 1;
    memset(QL, 0, n*n*sizeof(double));
    for (int i=0; i<n; ++i){
        mem->blas_daxpy(&n, Ld+i, Q+i*n, &incx, QL+i*n, &incy);
    }

    // Compute QLQT=QL*Q'
    const char transa = 'n';
    const char transb = 't';
    double alpha = 1;
    double beta = 0;
    mem->blas_dgemm(&transa, &transb, &n, &n, &n, &alpha, QL, &n, Q, &n, &beta, H, &n);
    // Issues with blas+blasfeo, write tripple for-loop as fallback
    // for (int i=0; i<n; ++i) {
    //     for (int ii=0; ii<n; ++ii) {
    //         H[ii*n+i] = 0;
    //         for (int iii=0; iii<n; ++iii) {
    //             H[ii*n+i] += QL[iii*n+i]*Q[iii*n+ii];
    //         }
    //     }
    // }


    return 0;
}
