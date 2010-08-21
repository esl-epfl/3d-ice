#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd ;
  ThermalData      tdata ;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;
  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  print_all_floorplans (stdout, "Before  ", &stkd) ;

  init_thermal_data  (&stkd, &tdata, 300.00, 0.05, 0.10) ;
  fill_thermal_data  (&stkd, &tdata) ;

  while (emulate_time_slot (&stkd, &tdata) == 0 ) ;

  print_all_floorplans (stdout, "After  ", &stkd) ;

  free_thermal_data  (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
