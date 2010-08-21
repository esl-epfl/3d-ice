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

  init_thermal_data  (&stkd, &tdata, 300.00, 0.1, 1.0) ;

  fill_thermal_data (&stkd, &tdata) ;

  free_thermal_data  (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
