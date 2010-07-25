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
	cd ./flex    ; make ;
	cd ./bison   ; make ;
	cd ./sources ; make ;

clean:
	cd ./flex    ; make clean ;
	cd ./bison   ; make clean ;
	cd ./sources ; make clean ;

