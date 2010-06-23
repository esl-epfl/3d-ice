#include <time.h>
#include <math.h>

#include "stack_description.h"
#include "thermal_data_iterative_crs.h"
#include "thermal_data_iterative_ccs.h"



#if defined TL_GMRES_ITERATIVE_SOLVER
int restart ;
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
main(int argc, char** argv)
{
  struct StackDescription     stkd_cc ;
  struct StackDescription     stkd_cr ;
  struct CRSThermalDataIterative tdata_cr ;
  struct CCSThermalDataIterative tdata_cc ;


  double delta_time = 0.00125 ;
  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

  int max_iter = atoi (argv[2]) ;
  printf("Using max %d iterations\n", max_iter) ;

  double tolerance = atof (argv[3]) ;
  printf("Using tolerance %.2e \n", tolerance) ;

  init_stack_description (&stkd_cc) ;
  init_stack_description (&stkd_cr) ;

  if (fill_stack_description (&stkd_cc, argv[1]) != 0)
    return EXIT_FAILURE ;

  if (fill_stack_description (&stkd_cr, argv[1]) != 0)
    return EXIT_FAILURE ;

  crs_init_thermal_data_iterative (&stkd_cr, &tdata_cr, 300.00, delta_time);
  ccs_init_thermal_data_iterative (&stkd_cc, &tdata_cc, 300.00, delta_time);

  insert_all_power_values (&stkd_cr, powers) ;
  insert_all_power_values (&stkd_cc, powers) ;

  crs_fill_thermal_data_iterative (&stkd_cr, &tdata_cr) ;
  ccs_fill_thermal_data_iterative (&stkd_cc, &tdata_cc) ;

  for (int row = 0; row < tdata_cc.Size; row++)
      for (int col = 0; col < tdata_cc.Size; col++)

          if (fabs(tdata_cc.I_Matrix_A.operator()(row, col)
                   - tdata_cr.I_Matrix_A.operator()(row, col)) > 1e-05 )

              printf ("Different %d %d %f %f\n", row, col,
                      tdata_cc.I_Matrix_A.operator()(row, col), tdata_cr.I_Matrix_A.operator()(row, col));

  crs_free_thermal_data_iterative (&tdata_cr) ;
  ccs_free_thermal_data_iterative (&tdata_cc) ;
  free_stack_description (&stkd_cc) ;
  free_stack_description (&stkd_cr) ;

  return EXIT_SUCCESS;
}
