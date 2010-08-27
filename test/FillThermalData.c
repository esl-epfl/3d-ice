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

  init_thermal_data  (&stkd, &tdata, 300.00, 0.002, 0.020) ;
  fill_thermal_data  (&stkd, &tdata) ;

  int count ;
  for (count = 0; count < tdata.SV_B.Size ; count ++)
    fprintf(stdout, "%.5e\n", tdata.SV_B.Values[count]);

  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
