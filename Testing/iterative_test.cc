#if defined SLU

  #define TD_INCLUDE "thermal_data_slu.h"
  #define TD_TYPE    SLUThermalData
  #define TD_INIT    slu_init_thermal_data
  #define TD_FILL    slu_fill_thermal_data
  #define TD_FREE    slu_free_thermal_data
  #define TD_MESSAGE "\n%d: SLU failed\n"
  #define TD_SOLVE   slu_solve_system

#elif defined BICG

  #define TD_INCLUDE "thermal_data_bicg.h"
  #define TD_TYPE    BICGThermalData
  #define TD_INIT    bicg_init_thermal_data
  #define TD_FILL    bicg_fill_thermal_data
  #define TD_FREE    bicg_free_thermal_data
  #define TD_MESSAGE "\n%d: BiCG failed (%d - %.5e)\n"
  #define TD_SOLVE   bicg_solve_system

#elif defined BICGSTAB

  #define TD_INCLUDE "thermal_data_bicgstab.h"
  #define TD_TYPE    BICGStabThermalData
  #define TD_INIT    bicgstab_init_thermal_data
  #define TD_FILL    bicgstab_fill_thermal_data
  #define TD_FREE    bicgstab_free_thermal_data
  #define TD_MESSAGE "\n%d: BiCGStab failed (%d - %.5e)\n"
  #define TD_SOLVE   bicgstab_solve_system

#elif defined CGS

  #define TD_INCLUDE "thermal_data_cgs.h"
  #define TD_TYPE    CGSThermalData
  #define TD_INIT    cgs_init_thermal_data
  #define TD_FILL    cgs_fill_thermal_data
  #define TD_FREE    cgs_free_thermal_data
  #define TD_MESSAGE "\n%d: CGS failed (%d - %.5e)\n"
  #define TD_SOLVE   cgs_solve_system

#elif defined GMRES

  #define TD_INCLUDE "thermal_data_gmres.h"
  #define TD_TYPE    GMRESThermalData
  #define TD_INIT    gmres_init_thermal_data
  #define TD_FILL    gmres_fill_thermal_data
  #define TD_FREE    gmres_free_thermal_data
  #define TD_MESSAGE "\n%d: QMR failed (%d - %.5e)\n"
  #define TD_SOLVE   gmres_solve_system

#elif defined IR

  #define TD_INCLUDE "thermal_data_ir.h"
  #define TD_TYPE    IRThermalData
  #define TD_INIT    ir_init_thermal_data
  #define TD_FILL    ir_fill_thermal_data
  #define TD_FREE    ir_free_thermal_data
  #define TD_MESSAGE "\n%d: IR failed (%d - %.5e)\n"
  #define TD_SOLVE   ir_solve_system

#elif defined QMR

  #define TD_INCLUDE "thermal_data_qmr.h"
  #define TD_TYPE    QMRThermalData
  #define TD_INIT    qmr_init_thermal_data
  #define TD_FILL    qmr_fill_thermal_data
  #define TD_FREE    qmr_free_thermal_data
  #define TD_MESSAGE "\n%d: QMR failed (%d - %.5e)\n"
  #define TD_SOLVE   qmr_solve_system

#else

  #error Wrong SOLVER type

#endif

#include <time.h>
#include "stack_description.h"
#include TD_INCLUDE

#if defined SLU
  #define TEMP_STRING "%.5f  %.5f  %.5f  %.5f\n"
#else
  #define TEMP_STRING "%.5f  %.5f  %.5f  %.5f"
#endif

void
print_temps
(
  struct TD_TYPE          *tdata,
  struct StackDescription *stkd ,
  double                  time
)
#if defined PRINT_TEMPS
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

  printf (TEMP_STRING, time, (tdie_1_row_099 + tdie_1_row_100)/2.0,
                             (tdie_2_row_099 + tdie_2_row_100)/2.0,
                             (tdie_3_row_099 + tdie_3_row_100)/2.0 ) ;
}
#else
{
  *tdata = *tdata ;
  *stkd  = *stkd ;
  time   = time ;
}
#endif

#if !defined SLU
  int    MAX_ITER ;
  double TOLERANCE ;
#endif

#if defined GMRES
  int RESTART ;
#endif

int
simulate
(
  struct TD_TYPE          *tdata,
  struct StackDescription *stkd ,
  double                  sim_time ,
  double                  delta_time
)
{
  int result ;

#if !defined SMALL
  static double print_time = sim_time ;
  delta_time = sim_time ;
#endif

#if defined SMALL
  for (double time = delta_time ; time <= sim_time ; time += delta_time)
  {
#endif

#if defined SLU

    result = TD_SOLVE (tdata, delta_time) ;
    if (result != 0)
    {
      printf(TD_MESSAGE, result) ;
      return result ;
    }

#else

    double tolerance = TOLERANCE ;
    int max_iter = MAX_ITER ;

  #if defined GMRES
    result = TD_SOLVE (tdata, delta_time, &tolerance, &max_iter, RESTART) ;
  #else
    result = TD_SOLVE (tdata, delta_time, &tolerance, &max_iter) ;
  #endif

    if (result != 0)
    {
      printf(TD_MESSAGE, result, max_iter, tolerance) ;
      return result ;
    }

#endif

#if defined SMALL
    print_temps (tdata, stkd, time) ;
#else
    print_temps (tdata, stkd, print_time) ;
    print_time += sim_time ;
#endif

#if defined PRINT_TEMPS && !defined SLU
    printf ("\t%d\t%e\n", max_iter, tolerance) ;
#endif

#if defined SMALL
  }
#endif

  return 0 ;
}

int
main(int argc, char** argv)
{
  struct StackDescription stkd ;
  struct TD_TYPE tdata ;

  double delta_time = 0.00125 ;
  double sim_time   = 0.10000 ;

  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

#if defined SLU
  if (argc != 2)
    {
      fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
      return EXIT_FAILURE;
    }
#elif defined GMRES
  if (argc != 5)
    {
      fprintf(stderr,
        "Usage: \"%s file.stk max_iter tolerance restart\"\n", argv[0]);
      return EXIT_FAILURE;
    }
#else
  if (argc != 4)
    {
      fprintf(stderr,
        "Usage: \"%s file.stk max_iter tolerance\"\n", argv[0]);
      return EXIT_FAILURE;
    }
#endif

#if !defined SLU
  MAX_ITER  = atoi (argv[2]) ;
  printf("Using max %d iterations\n", MAX_ITER) ;
  TOLERANCE = atof (argv[3]) ;
  printf("Using tolerance %.2e \n", TOLERANCE) ;
#endif

#if defined GMRES
  RESTART = atoi (argv[4]) ;
  printf("Using restart %d\n", RESTART) ;
#endif

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)

    return EXIT_FAILURE ;

  //print_stack_description (stdout, "", &stkd) ;

#if defined SLU
  TD_INIT (&stkd, &tdata, TL_CCS_MATRIX, 300.00, delta_time) ;
#else
  TD_INIT (&stkd, &tdata, 300.00, delta_time) ;
#endif

  //printf("-----------------------------------------------------------------\n");
  //printf("-----------------------------------------------------------------\n");
  //printf("-----------------------------------------------------------------\n");

  print_temps (&tdata, &stkd, 0.0) ;
#if defined PRINT_TEMPS && !(defined SLU || defined CULA)
  printf ("\n") ;
#endif

  clock_t time_start = clock();

  insert_all_power_values (&stkd, powers) ;
  TD_FILL (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;

  change_coolant_flow_rate (&stkd, 0.7) ;
  TD_FILL (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  TD_FILL (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;

  change_coolant_flow_rate (&stkd, 1.4) ;
  TD_FILL (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;

  printf ("sim time: %f\n\n", ( (double)clock() - time_start ) / CLOCKS_PER_SEC );

exit :

  TD_FREE (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
