#include "stack_description.h"
#include "thermal_data_iterative_ccs.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
main(int argc, char** argv)
{
  struct StackDescription     stkd ;
  struct CCSThermalDataIterative tdata ;

  double delta_time = 0.00125 ;
  double powers [] = { 1.5, 0.3, 1.2, 1.5} ;

  if (argc != 2)
  {
    fprintf(stderr,
      "Usage: \"%s file.stk\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  ccs_init_thermal_data_iterative (&stkd, &tdata, 300.00, delta_time);

  insert_all_power_values (&stkd, powers) ;
  ccs_fill_thermal_data_iterative (&stkd, &tdata) ;

  MV_Vector_double r(tdata.Size);
  r = tdata.I_Matrix_A * tdata.Temperatures;
  std::cout << r << std::endl;

//  for (int c = 0; c < tdata.Size ; c++)
//    if (tdata.I_Matrix_A.operator()(1402, c) != 0)
//        printf ("%d %.5e\n", c, tdata.I_Matrix_A.operator()(1402, c));

  ccs_free_thermal_data_iterative (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
