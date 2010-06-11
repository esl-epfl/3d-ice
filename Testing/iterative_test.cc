#include <time.h>

#include "stack_description.h"
#include "thermal_data_iterative.h"

//extern void print_sl_profile_data(void);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_temps
(
  struct ThermalDataIterative *tdata,
  struct StackDescription     *stkd ,
  double                      time
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

  printf ("%.5f  %.5f  %.5f  %.5f",
          time, (tdie_1_row_099 + tdie_1_row_100)/2.0,
                (tdie_2_row_099 + tdie_2_row_100)/2.0,
                (tdie_3_row_099 + tdie_3_row_100)/2.0 ) ;
}
#else
{
  // just to avoid warning messages ...
  *tdata = *tdata ;
  *stkd  = *stkd ;
  time   = time ;
}
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#if defined TL_GMRES_ITERATIVE_SOLVER
int restart ;
#endif

int
simulate
(
  struct ThermalDataIterative *tdata,
  struct StackDescription     *stkd ,
  double                      sim_time ,
  double                      delta_time,
  int                         max_iter,
  double                      tolerance
)
{
  int result ;
  int local_max_iter ;
  double local_tolerance ;

#if !defined SMALL
  static double print_time = delta_time = sim_time ;
#else
  for (double time = delta_time ; time <= sim_time ; time += delta_time)
  {
#endif

    local_max_iter  = max_iter ;
    local_tolerance = tolerance ;

    result = solve_system_iterative
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

#if defined SMALL
    print_temps (tdata, stkd, time) ;
#else
    print_temps (tdata, stkd, print_time) ;
    print_time += sim_time ;
#endif

#if defined PRINT_TEMPS
    printf ("\t%d\t%e\n", local_max_iter, local_tolerance) ;
#endif

#if defined SMALL
  }
#endif

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
main(int argc, char** argv)
{
  struct StackDescription     stkd ;
  struct ThermalDataIterative tdata ;

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

  init_thermal_data_iterative (&stkd, &tdata, 300.00, delta_time) ;

#if defined PRINT_TEMPS
  print_stack_description (stdout, "", &stkd) ;
  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");
  print_temps (&tdata, &stkd, 0.0) ;
  printf ("\n") ;
#endif

  clock_t time_start = clock();

  insert_all_power_values (&stkd, powers) ;
  fill_thermal_data_iterative (&stkd, &tdata) ;

//   solve_system_iterative
//   (
//     &tdata, delta_time, &tolerance, &max_iter
//#if defined TL_GMRES_ITERATIVE_SOLVER
//     , restart
//#endif
//   ) ;
//   print_sl_profile_data() ;

  if (simulate (&tdata, &stkd, sim_time, delta_time, max_iter, tolerance) != 0)
    goto exit ;

  change_coolant_flow_rate (&stkd, 0.7) ;
  fill_thermal_data_iterative (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time, max_iter, tolerance) != 0)
    goto exit ;

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  fill_thermal_data_iterative (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time, max_iter, tolerance) != 0)
    goto exit ;

  change_coolant_flow_rate (&stkd, 1.4) ;
  fill_thermal_data_iterative (&stkd, &tdata) ;

  if (simulate (&tdata, &stkd, sim_time, delta_time, max_iter, tolerance) != 0)
    goto exit ;

  printf ("sim time: %f\n", ( (double)clock() - time_start ) / CLOCKS_PER_SEC );

exit :

  free_thermal_data_iterative (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
