#include <time.h>

#include "stack_description.h"
#include "thermal_data_direct.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#if defined PRINT_TEMPS
void
print_temps
(
  struct ThermalDataDirect *tdata,
  struct StackDescription  *stkd ,
  double                  time
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

  printf ("%.5f  %.5f  %.5f  %.5f\n",
          time, (tdie_1_row_099 + tdie_1_row_100)/2.0,
                (tdie_2_row_099 + tdie_2_row_100)/2.0,
                (tdie_3_row_099 + tdie_3_row_100)/2.0 ) ;
}

int
simulate
(
  struct ThermalDataDirect *tdata,
  struct StackDescription  *stkd ,
  double                   sim_time ,
  double                   delta_time
)
{
  int result ;
  for (double time = 0.0 ; time <= sim_time ; time += delta_time)
  {
    result = solve_system_direct (tdata, delta_time) ;
    if (result != 0)
    {
      printf("%d: Solver failed\n", result) ;
      return result ;
    }
    print_temps (tdata, stkd, time) ;
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
  struct StackDescription  stkd ;
  struct ThermalDataDirect tdata ;

  double delta_time = 0.00125 ;
  double sim_time   = 0.10000 ;

  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)

    return EXIT_FAILURE ;

  init_thermal_data_direct (&stkd, &tdata, 300.00, delta_time) ;

#if defined PRINT_TEMPS
  char prefix[] = "";
  print_stack_description (stdout, prefix, &stkd) ;
  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");
  print_temps (&tdata, &stkd, 0.0) ;
#endif

  clock_t time_start = clock();

  insert_all_power_values (&stkd, powers) ;
  fill_thermal_data_direct (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;
#else
  int result ;
  if ((result = solve_system_direct (&tdata, sim_time)) != 0)
  {
    printf("%d: Solver failed\n", result) ;
    goto exit ;
  }
#endif

  change_coolant_flow_rate (&stkd, 0.7) ;
  fill_thermal_data_direct (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;
#else
  if ((result = solve_system_direct (&tdata, sim_time)) != 0)
  {
    printf("%d: Solver failed\n", result) ;
    goto exit ;
  }
#endif

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  fill_thermal_data_direct (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;
#else
  if ((result = solve_system_direct (&tdata, sim_time)) != 0)
  {
    printf("%d: Solver failed\n", result) ;
    goto exit ;
  }
#endif

  change_coolant_flow_rate (&stkd, 1.4) ;
  fill_thermal_data_direct (&stkd, &tdata) ;

#ifdef PRINT_TEMPS
  if (simulate (&tdata, &stkd, sim_time, delta_time) != 0)
    goto exit ;
#else
  if ((result = solve_system_direct (&tdata, sim_time)) != 0)
  {
    printf("%d: Solver failed\n", result) ;
    goto exit ;
  }
#endif

  printf ("sim time: %f\n", ( (double)clock() - time_start ) / CLOCKS_PER_SEC );

exit :

  free_thermal_data_direct (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
