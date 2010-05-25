#include <stdlib.h>

#include "stack_description.h"
#include "thermal_data_qmr.h"

#define MAX_ITER  15000
#define TOLERANCE 1e-03

void
qmr_print_temps (struct QMRThermalData *tdata, struct StackDescription *stkd, double time)
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
  struct QMRThermalData  tdata ;

  double delta_time = 0.00125 ;
  double sim_time ;

  int max_iter ;
  double tolerance ;

  int result ;

#ifdef DETAILS
  double time ;
  sim_time = delta_time ;
#else
  sim_time = 0.10 ;
#endif

  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

  if (argc != 2)
    {
      fprintf(stderr, "Error: missing input file parameter.\n");
      return EXIT_FAILURE;
    }

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  print_stack_description (stdout, "", &stkd) ;

  qmr_init_thermal_data (&stkd, &tdata, 300.00, delta_time) ;

  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");
  printf("-----------------------------------------------------------------\n");

  /*
   *  Insert first set of power values and update
   */

  insert_all_power_values (&stkd, powers) ;
  qmr_fill_thermal_data (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    qmr_print_temps (&tdata, &stkd, time) ;
#else
    qmr_print_temps (&tdata, &stkd, 0.0) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;
    result = qmr_solve_system (&tdata, sim_time, &tolerance, &max_iter ) ;
    if (result != 0)
    {
        printf("\n%d: QMR failed (%d - %.5e)\n",result, max_iter, tolerance) ;
        goto exit ;
    }

    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Change coolant flow rate and update
   */

  change_coolant_flow_rate (&stkd, 0.7) ;
  qmr_fill_thermal_data (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    qmr_print_temps (&tdata, &stkd, time) ;
#else
    qmr_print_temps (&tdata, &stkd, 0.10) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;
    result = qmr_solve_system (&tdata, sim_time, &tolerance, &max_iter ) ;
    if (result != 0)
    {
        printf("\n%d: QMR failed (%d - %.5e)\n",result, max_iter, tolerance) ;
        goto exit ;
    }

    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Insert second set of power values and update
   */

  powers[1] = 1.5 ;
  insert_all_power_values (&stkd, powers) ;
  qmr_fill_thermal_data (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    qmr_print_temps (&tdata, &stkd, time) ;
#else
    qmr_print_temps (&tdata, &stkd, 0.20) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;
    result = qmr_solve_system (&tdata, sim_time, &tolerance, &max_iter ) ;
    if (result != 0)
    {
        printf("\n%d: QMR failed (%d - %.5e)\n",result, max_iter, tolerance) ;
        goto exit ;
    }

    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#endif

  /*
   *  Change coolant flow rate and update
   */

  change_coolant_flow_rate (&stkd, 1.4) ;
  qmr_fill_thermal_data (&stkd, &tdata) ;

  /*
   *  Solve for 0.1 seconds
   */

#ifdef DETAILS
  for (time = 0.00 ; time < 0.10 ; time += delta_time)
  {
    qmr_print_temps (&tdata, &stkd, time) ;
#else
    qmr_print_temps (&tdata, &stkd, 0.30) ;
#endif

    tolerance = TOLERANCE ;
    max_iter = MAX_ITER ;
    result = qmr_solve_system (&tdata, sim_time, &tolerance, &max_iter ) ;
    if (result != 0)
    {
        printf("\n%d: QMR failed (%d - %.5e)\n",result, max_iter, tolerance) ;
        goto exit ;
    }

    printf ("\t%d\t%e\n", max_iter, tolerance) ;

#ifdef DETAILS
  }
#else
    qmr_print_temps (&tdata, &stkd, 0.40) ;
#endif

exit :

  qmr_free_thermal_data (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
