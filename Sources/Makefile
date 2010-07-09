###############################################################################
#                                                                             #
# Sources/Makefile                                                            #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

TL_MAIN_DIR = ..

include $(TL_MAIN_DIR)/makefile.def

SOURCES = channel.c           \
          conductances.c      \
          die.c               \
          dimensions.c        \
          floorplan.c         \
          floorplan_element.c \
          layer.c             \
          material.c          \
          stack_description.c \
          stack_element.c     \
          system_matrix.c     \
          system_vector.c     \
          slu_thermal_data.c

OBJECTS = channel.o           \
          conductances.o      \
          die.o               \
          dimensions.o        \
          floorplan.o         \
          floorplan_element.o \
          layer.o             \
          material.o          \
          stack_description.o \
          stack_element.o     \
          system_matrix.o     \
          system_vector.o     \
          slu_thermal_data.o

all: $(TL_LIB_A)

$(TL_LIB_A): $(OBJECTS)
	@ar rsv $@ $?

clean:
	@rm -vf $(OBJECTS)
	@rm -vf $(TL_LIB_A)


###############################################################################

channel.o: channel.c                         \
           $(TL_INCLUDE_DIR)/channel.h       \
           $(TL_INCLUDE_DIR)/system_matrix.h

conductances.o: conductances.c                   \
                $(TL_INCLUDE_DIR)/conductances.h \
                $(TL_INCLUDE_DIR)/layer.h        \

die.o: die.c \
       $(TL_INCLUDE_DIR)/die.h

dimensions.o: dimensions.c \
              $(TL_INCLUDE_DIR)/dimensions.h
