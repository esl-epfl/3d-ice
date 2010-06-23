#include <time.h>

#include "stack_description.h"
#include "thermal_data_iterative_crs.h"

#ifdef SL_PROFILE
extern void print_sl_profile_data(void);
#endif

#if defined TL_GMRES_ITERATIVE_SOLVER
int restart ;
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#if defined PRINT_TEMPS
void
print_temps
(
  struct CRSThermalDataIterative *tdata,
  struct StackDescription     *stkd ,
  double                      time
)
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

  printf ("%.5f  %.5f  %.5f  %.5f",
          time, (tdie_1_row_099 + tdie_1_row_100)/2.0,
                (tdie_2_row_099 + tdie_2_row_100)/2.0,
                (tdie_3_row_099 + tdie_3_row_100)/2.0 ) ;
}

int
simulate
(
  struct CRSThermalDataIterative *tdata,
  struct StackDescription  *stkd ,
  double                   sim_time ,
  double                   delta_time,
  double                   tolerance,
  int                      max_iter
)
{
  int result, local_max_iter ;
  double local_tolerance ;
  for (double time = 0.0; time <= sim_time ; time += delta_time)
  {

    local_max_iter  = max_iter ;
    local_tolerance = tolerance ;

    result = crs_solve_system_iterative
             (
               tdata, delta_time, &local_tolerance, &local_max_iter
#if defined TL_GMRES_ITERATIVE_SOLVER
               , restart
#endif
             ) ;

    if (result != 0)
    {
      printf("\n%d: Solver failed (%d - %.5e)\n",
             result, local_max_iter, local_tolerance) ;
      return result ;
    }

    print_temps (tdata, stkd, time) ;
    printf ("\t%d\t%e\n", local_max_iter, local_tolerance) ;
  }

  return 0 ;
}

#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
main(int argc, char** argv)
{
  struct StackDescription     stkd ;
  struct CRSThermalDataIterative tdata ;

  double delta_time = 0.00125 ;
  double sim_time   = 0.10000 ;

  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

#if defined TL_GMRES_ITERATIVE_SOLVER
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

#ifndef PRINT_TEMPS
  int local_max_iter;
  double local_tolerance;
#endif

  int max_iter = atoi (argv[2]) ;
  printf("Using max %d iterations\n", max_iter) ;

  double tolerance = atof (argv[3]) ;
  printf("Using tolerance %.2e \n", tolerance) ;

#if defined TL_GMRES_ITERATIVE_SOLVER
  restart = atoi (argv[4]) ;
  printf("Using restart %d\n", restart) ;
#endif

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)

    return EXIT_FAILURE ;

#if defined TL_GMRES_ITERATIVE_SOLVER
  crs_init_thermal_data_iterative (&stkd, &tdata, 300.00, delta_time, restart);
#else
  crs_init_thermal_data_iterative (&stkd, &tdata, 300.00, delta_time);
#endif

#if defined PRINT_TEMPS
  char prefix[] = "";
  print_stack_description (stdout, prefix, &stkd) ;
  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");
  print_temps (&tdata, &stkd, 0.0) ;
  printf ("\n") ;
#endif

  clock_t time_start = clock();

  insert_all_power_values (&stkd, powers) ;
  crs_fill_thermal_data_iterative (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time, tolerance, max_iter) != 0)
    goto exit ;
#else
  int result;
  local_max_iter = max_iter;
  local_tolerance = tolerance;
  result = crs_solve_system_iterative (&tdata, sim_time,
                                       &local_tolerance, &local_max_iter
#                                      if defined TL_GMRES_ITERATIVE_SOLVER
                                       , restart
#                                      endif
                                      );
  if (result != 0)
  {
    printf("\n%d: Solver failed (%d - %.5e)\n", result, max_iter, tolerance) ;
    goto exit ;
  }
#endif

  change_coolant_flow_rate (&stkd, 0.7) ;
  crs_fill_thermal_data_iterative (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time, tolerance, max_iter) != 0)
    goto exit ;
#else
  local_max_iter = max_iter;
  local_tolerance = tolerance;
  result = crs_solve_system_iterative (&tdata, sim_time,
                                       &local_tolerance, &local_max_iter
#                                      if defined TL_GMRES_ITERATIVE_SOLVER
                                       , restart
#                                      endif
                                      );
  if (result != 0)
  {
    printf("\n%d: Solver failed (%d - %.5e)\n", result, max_iter, tolerance) ;
    goto exit ;
  }
#endif

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  crs_fill_thermal_data_iterative (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time, tolerance, max_iter) != 0)
    goto exit ;
#else
  local_max_iter = max_iter;
  local_tolerance = tolerance;
  result = crs_solve_system_iterative (&tdata, sim_time,
                                       &local_tolerance, &local_max_iter
#                                      if defined TL_GMRES_ITERATIVE_SOLVER
                                       , restart
#                                      endif
                                       );
  if (result != 0)
  {
    printf("\n%d: Solver failed (%d - %.5e)\n", result, max_iter, tolerance) ;
    goto exit ;
  }
#endif

  change_coolant_flow_rate (&stkd, 1.4) ;
  crs_fill_thermal_data_iterative (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time, tolerance, max_iter) != 0)
    goto exit ;
#else
  local_max_iter = max_iter;
  local_tolerance = tolerance;
  result = crs_solve_system_iterative (&tdata, sim_time,
                                       &local_tolerance, &local_max_iter
#                                      if defined TL_GMRES_ITERATIVE_SOLVER
                                       , restart
#                                      endif
                                      );
  if (result != 0)
  {
    printf("\n%d: Solver failed (%d - %.5e)\n", result, max_iter, tolerance) ;
    goto exit ;
  }
#endif

  printf ("sim time: %f\n", ( (double)clock() - time_start ) / CLOCKS_PER_SEC );

#ifdef SL_PROFILE
  print_sl_profile_data() ;
#endif

exit :

  crs_free_thermal_data_iterative (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
