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
	cd ./bison   ; make ;
	cd ./Sources ; make ;

clean:
	cd ./Flex    ; make clean ;
	cd ./bison   ; make clean ;
	cd ./Sources ; make clean ;

