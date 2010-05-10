
#include "slu_ddefs.h"
#include "thermal_data.h"

int
solve_system
(
  ThermalData *tdata,
  double total_time
)
{
  for
  ( ;
    tdata->SLU_Info != 0 && total_time > 0 ;
    total_time -= tdata->delta_time
  )
  {
    dgssvx
    (
      &tdata->SLU_Options,
      &tdata->SLUMatrix_A,
      tdata->SLU_PermutationMatrixC,
      tdata->SLU_PermutationMatrixR,
      tdata->SLU_Etree,
      tdata->SLU_equed,
      tdata->SLU_RowsScaleFactors,
      tdata->SLU_ColumnsScaleFactors,
      &tdata->SLUMatrix_L,
      &tdata->SLUMatrix_U,
      NULL, 0,
      &tdata->SLUMatrix_B,
      &tdata->SLUMatrix_X,
      &tdata->SLU_rpg,
      &tdata->SLU_rcond,
      &tdata->SLU_ferr,
      &tdata->SLU_berr,
      &tdata->SLU_MemUsage,
      &tdata->SLU_Stat,
      &tdata->SLU_Info
    );

    tdata->SLU_Options.Fact = FACTORED ;
  }
//    dgstrs
//    (
//      NOTRANS,
//      &tdata->SLUMatrix_L,
//      &tdata->SLUMatrix_U,
//      tdata->SLU_PermutationMatrixC,
//      tdata->SLU_PermutationMatrixR,
//      &tdata->SLUMatrix_B,
//      &tdata->SLU_Stat,
//      &tdata->SLU_Info
//    ) ;

  return tdata->SLU_Info ;
}
