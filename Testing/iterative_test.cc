#include <stdlib.h>
#include <time.h>

#ifdef TD_BICG
  #define TD_INCLUDE "thermal_data_bicg.h"
  #define TD_TYPE    BICGThermalData
  #define TD_INIT    bicg_init_thermal_data
  #define TD_FILL    bicg_fill_thermal_data
  #define TD_FREE    bicg_free_thermal_data
  #define TD_MESSAGE "\n%d: BiCG failed (%d - %.5e)\n"

  #ifdef TL_DIAGONAL_PRECONDITIONER
    #define TD_SOLVE bicg_diag_pre_solve_system
  #endif
  #ifdef TL_ILU_PRECONDITIONER
    #define TD_SOLVE bicg_ilu_pre_solve_system
  #endif
#endif

#ifdef TD_BICGSTAB
  #define TD_INCLUDE "thermal_data_bicgstab.h"
  #define TD_TYPE    BICGStabThermalData
  #define TD_INIT    bicgstab_init_thermal_data
  #define TD_FILL    bicgstab_fill_thermal_data
  #define TD_FREE    bicgstab_free_thermal_data
  #define TD_MESSAGE "\n%d: BiCGStab failed (%d - %.5e)\n"

  #ifdef TL_DIAGONAL_PRECONDITIONER
    #define TD_SOLVE bicgstab_diag_pre_solve_system
  #endif
  #ifdef TL_ILU_PRECONDITIONER
    #define TD_SOLVE bicgstab_ilu_pre_solve_system
  #endif
#endif

#ifdef TD_CGS
  #define TD_INCLUDE "thermal_data_cgs.h"
  #define TD_TYPE    CGSThermalData
  #define TD_INIT    cgs_init_thermal_data
  #define TD_FILL    cgs_fill_thermal_data
  #define TD_FREE    cgs_free_thermal_data
  #define TD_MESSAGE "\n%d: CGS failed (%d - %.5e)\n"

  #ifdef TL_DIAGONAL_PRECONDITIONER
    #define TD_SOLVE cgs_diag_pre_solve_system
  #endif
  #ifdef TL_ILU_PRECONDITIONER
    #define TD_SOLVE cgs_ilu_pre_solve_system
  #endif
#endif

#ifdef TD_GMRES
  #define TD_INCLUDE "thermal_data_gmres.h"
  #define TD_TYPE    GMRESThermalData
  #define TD_INIT    gmres_init_thermal_data
  #define TD_FILL    gmres_fill_thermal_data
  #define TD_FREE    gmres_free_thermal_data
  #define TD_MESSAGE "\n%d: QMR failed (%d - %.5e)\n"

  #ifdef TL_DIAGONAL_PRECONDITIONER
    #define TD_SOLVE gmres_diag_pre_solve_system
  #endif
  #ifdef TL_ILU_PRECONDITIONER
    #define TD_SOLVE gmres_ilu_pre_solve_system
  #endif
#endif

#ifdef TD_IR
  #define TD_INCLUDE "thermal_data_ir.h"
  #define TD_TYPE    IRThermalData
  #define TD_INIT    ir_init_thermal_data
  #define TD_FILL    ir_fill_thermal_data
  #define TD_FREE    ir_free_thermal_data
  #define TD_MESSAGE "\n%d: IR failed (%d - %.5e)\n"
  #define TD_SOLVE   ir_ilu_pre_solve_system
#endif

#ifdef TD_QMR
  #define TD_INCLUDE "thermal_data_qmr.h"
  #define TD_TYPE    QMRThermalData
  #define TD_INIT    qmr_init_thermal_data
  #define TD_FILL    qmr_fill_thermal_data
  #define TD_FREE    qmr_free_thermal_data
  #define TD_MESSAGE "\n%d: QMR failed (%d - %.5e)\n"

  #ifdef TL_DIAGONAL_PRECONDITIONER
    #define TD_SOLVE qmr_diag_pre_solve_system
  #endif
  #ifdef TL_ILU_PRECONDITIONER
    #define TD_SOLVE qmr_ilu_pre_solve_system
  #endif
#endif

#include "stack_description.h"
#include TD_INCLUDE

void
print_temps (struct TD_TYPE *tdata, struct StackDescription *stkd, double time)
{
  int column ;
  int ncolumns = get_number_of_columns(stkd->Dimensions) ;

  int die_1_row_099 = get_cell_offset_in_stack (stkd->Dimensions,  3,  99, 0) ;
  int die_1_row_100 = get_cell_offset_in_stack (stkd->Dimensions,  3, 100, 0) ;
  int die_2_row_099 = get_cell_offset_in_stack (stkd->Dimensions,  7,  99, 0) ;
  int die_2_row_100 = get_cell_offset_in_stack (stkd->Dimensions,  7, 100, 0) ;
  int die_3_row_099 = get_cell_offset_in_stack (stkd->Dimensions, 11,  99, 0) ;
  int die_3_row_100 = get_cell_offset_in_stack (stkd->Dimensions, 11, 100, 0) ;

  double tdie_1_row_099 = 0.0, tdie_1_row_100 = 0.0 ;
  double tdie_2_row_099 = 0.0, tdie_2_row_100 = 0.0 ;
  double tdie_3_row_099 = 0.0, tdie_3_row_100 = 0.0 ;

  for (column = 0; column < ncolumns; column++)
  {
    tdie_1_row_099 += tdata->Temperatures[die_1_row_099 + column] ;
    tdie_1_row_100 += tdata->Temperatures[die_1_row_100 + column] ;
    tdie_2_row_099 += tdata->Temperatures[die_2_row_099 + column] ;
    tdie_2_row_100 += tdata->Temperatures[die_2_row_100 + column] ;
    tdie_3_row_099 += tdata->Temperatures[die_3_row_099 + column] ;
    tdie_3_row_100 += tdata->Temperatures[die_3_row_100 + column] ;
  }

  tdie_1_row_099 /= ncolumns ;
  tdie_1_row_100 /= ncolumns ;
  tdie_2_row_099 /= ncolumns ;
  tdie_2_row_100 /= ncolumns ;
  tdie_3_row_099 /= ncolumns ;
  tdie_3_row_100 /= ncolumns ;

  printf ("%.5f  %.5f  %.5f  %.5f", time, (tdie_1_row_099 + tdie_1_row_100)/2.0,
                                          (tdie_2_row_099 + tdie_2_row_100)/2.0,
                                          (tdie_3_row_099 + tdie_3_row_100)/2.0 ) ;
}

int
main(int argc, char** argv)
{
  struct StackDescription stkd ;
  struct TD_TYPE tdata ;

  double delta_time = 0.00125 ;
  double sim_time ;

  int max_iter, MAX_ITER ;
  double tolerance, TOLERANCE ;

  int result ;

#ifdef DETAILS
  double time ;
  sim_time = delta_time ;
#else
  sim_time = 0.10 ;
#endif

  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

#ifdef TD_GMRES
  if (argc != 5)
    {
      fprintf(stderr, "Usage: \"%s file.stk max_iter tolerance restart\"\n", argv[0]);
      return EXIT_FAILURE;
    }
  int RESTART = atoi (argv[4]) ;
#else
  if (argc != 4)
    {
      fprintf(stderr, "Usage: \"%s file.stk max_iter tolerance\"\n", argv[0]);
      return EXIT_FAILURE;
    }
#endif

  MAX_ITER  = atoi (argv[2]) ;
  TOLERANCE = atof (argv[3]) ;

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  print_stack_description (stdout, "", &stkd) ;

  TD_INIT (&stkd, &tdata, 300.00, delta_time) ;

#ifdef TD_GMRES
  printf("Using restart %d\n", RESTART) ;
#endif
  printf("Using max %d iterations\n", MAX_ITER) ;
  printf("Using tolerance %.2e \n", TOLERANCE) ;

  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");

  clock_t time_start = clock();

  /*
   *  Insert first set of power values and update
   */

  insert_all_power_values (&stkd, powers) ;
  TD_FILL (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    print_temps (&tdata, &stkd, time) ;
#else
    print_temps (&tdata, &stkd, 0.0) ;
    printf ("\n") ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;

    result = TD_SOLVE
             (
               &tdata, sim_time, &tolerance, &max_iter
#ifdef TD_GMRES
               ,RESTART
#endif
             ) ;

    if (result != 0)
    {
        printf(TD_MESSAGE, result, max_iter, tolerance) ;
        goto exit ;
    }

#ifndef DETAILS
    print_temps (&tdata, &stkd, 0.10) ;
#endif
    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Change coolant flow rate and update
   */

  change_coolant_flow_rate (&stkd, 0.7) ;
  TD_FILL (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    print_temps (&tdata, &stkd, time) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;

    result = TD_SOLVE
             (
               &tdata, sim_time, &tolerance, &max_iter
#ifdef TD_GMRES
               ,RESTART
#endif
             ) ;

    if (result != 0)
    {
        printf(TD_MESSAGE, result, max_iter, tolerance) ;
        goto exit ;
    }

#ifndef DETAILS
    print_temps (&tdata, &stkd, 0.20) ;
#endif
    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Insert second set of power values and update
   */

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  TD_FILL (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    print_temps (&tdata, &stkd, time) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;

    result = TD_SOLVE
             (
               &tdata, sim_time, &tolerance, &max_iter
#ifdef TD_GMRES
               ,RESTART
#endif
             ) ;

    if (result != 0)
    {
        printf(TD_MESSAGE, result, max_iter, tolerance) ;
        goto exit ;
    }

#ifndef DETAILS
    print_temps (&tdata, &stkd, 0.30) ;
#endif
    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Change coolant flow rate and update
   */

  change_coolant_flow_rate (&stkd, 1.4) ;
  TD_FILL (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    print_temps (&tdata, &stkd, time) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;

    result = TD_SOLVE
             (
               &tdata, sim_time, &tolerance, &max_iter
#ifdef TD_GMRES
               ,RESTART
#endif
             ) ;

    if (result != 0)
    {
        printf(TD_MESSAGE, result, max_iter, tolerance) ;
        goto exit ;
    }

#ifndef DETAILS
    print_temps (&tdata, &stkd, 0.40) ;
#endif
    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  printf ("sim time: %f\n", ( (double)clock() - time_start ) / CLOCKS_PER_SEC );

exit :

  TD_FREE (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
