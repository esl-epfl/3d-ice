#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd  ;
  ThermalData      tdata ;

  int cell1, cell2;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;
  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  //print_all_floorplans (stdout, "Before  ", &stkd) ;

  init_thermal_data  (&stkd, &tdata, 300.00, 0.002, 0.020) ;
  fill_thermal_data  (&stkd, &tdata) ;

  //print_dimensions (stdout, "dim: ",     stkd.Dimensions) ;
  //print_channel    (stdout, "channel: ", stkd.Channel) ;

  cell1 = get_cell_offset_in_stack (stkd.Dimensions, 0, 51, 2) ;
  cell2 = get_cell_offset_in_stack (stkd.Dimensions, 4, 51, 2) ;

  do {

    printf ("%d: %8.4f -- %d:%8.4f\n", cell1, tdata.Temperatures[cell1],
                                       cell2, tdata.Temperatures[cell2]) ;

  } while (emulate_time_slot (&stkd, &tdata) == 0 ) ;


  //print_all_floorplans (stdout, "After  ", &stkd) ;

  free_thermal_data (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
