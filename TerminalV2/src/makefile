CC :=gcc
LDFLAGS :=
C_SOURCES :=$(wildcard *.c)
C_EXECUTABLE :=$(C_SOURCES:.c=)
 
all:$(C_EXECUTABLE)

$(C_EXECUTABLE):$(C_SOURCES)
		$(CC) $< $(LDFLAGS) $(CFLAGS) -o $@

$(CPP_EXECUTABLE):$(CPP_SOURCES)
		$(CPP) $< $(LDFLAGS) $(CFLAGS) -o $@

clean:
		rm -rf $(EXECUTABLE)