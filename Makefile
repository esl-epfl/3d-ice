#******************************************************************************
#*                                                                            *
#* Makefile                                                                   *
#*                                                                            *
#* EPFL-STI-IEL-ESL                                                           *
#* Bâtiment ELG, ELG 130                                                      *
#* Station 11                                                                 *
#* 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
#******************************************************************************

.PHONY: all clean

all:
	cd ./Flex    ; make ;
	cd ./Bison   ; make ;
	cd ./Sources ; make ;

clean:
	cd ./Flex    ; make clean ;
	cd ./Bison   ; make clean ;
	cd ./Sources ; make clean ;

