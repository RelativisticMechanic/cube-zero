CFLAGS = -Wall -Wextra -I../../include 

OBJ = $(shell for i in *.c ; do echo $${i%.c}.o ; done )

LIBNAME = libenet.a

all: $(OBJ)

	$(AR) -cvq $(LIBNAME) $(OBJ)

clean:

	$(RM) $(OBJ) $(LIBNAME)

