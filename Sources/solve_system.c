
#include "slu_ddefs.h"

#include "system_matrix.h"
#include "system_vector.h"

int
solve_system
(
  SystemMatrix *SM_A,
  SystemVector *SV_X,
  SystemVector *SV_B,
  double delta_time,
  double total_time
)
{
  SuperMatrix A, B, L, U, X;

  SuperLUStat_t     stat;
  superlu_options_t options;
  mem_usage_t       mem_usage;

  set_default_options (&options);

  options.PrintStat       = NO ;
  options.Equil           = NO ;
  options.SymmetricMode   = YES ;
  options.ColPerm         = MMD_AT_PLUS_A ;
  options.DiagPivotThresh = 0.001 ;

  StatInit (&stat) ;

  int         info ;
  int         *perm_r, *perm_c, *etree ;
  double      rpg, rcond, ferr, berr;
  double      *R, *C ;

  char        equed[1];

  if ( !(perm_r = intMalloc(SM_A->Size)) )
    ABORT("Malloc fails for perm_r[].");

  if ( !(perm_c = intMalloc(SM_A->Size)) )
    ABORT("Malloc fails for perm_c[].");

  if ( !(etree  = intMalloc(SM_A->Size)) )
    ABORT("Malloc fails for etree[].");

  if ( !(R = doubleMalloc(SM_A->Size)) )
    ABORT("Malloc fails for R[].");

  if ( !(C = doubleMalloc(SM_A->Size)) )
    ABORT("Malloc fails for C[].");

  dCreate_CompCol_Matrix  /* Matrix A */
  (
    &A, SM_A->Size, SM_A->Size, SM_A->NNz,
    SM_A->Values, SM_A->Rows, SM_A->Columns,
    SLU_NC, SLU_D, SLU_GE
  ) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &B, SV_B->Size, 1,
    SV_B->Values, SV_B->Size,
    SLU_DN, SLU_D, SLU_GE
  );

  dCreate_Dense_Matrix  /* Vector X */
  (
    &X, SV_X->Size, 1,
    SV_X->Values, SV_X->Size,
    SLU_DN, SLU_D, SLU_GE
  );

  for ( ; total_time > 0 ; total_time -= delta_time )
  {
    dgssvx(&options, &A, perm_c, perm_r, etree, equed, R, C,
           &L, &U, NULL, 0, &B, &X, &rpg, &rcond, &ferr, &berr,
           &mem_usage, &stat, &info);

    options.Fact = FACTORED ;
  }

  StatFree(&stat);

  free (perm_r);
  free (perm_c);
  free (etree);
  free (R);
  free (C);

  return info ;
}
