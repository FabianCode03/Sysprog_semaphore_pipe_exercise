TARGETS=random
CFLAGS+=-Wall -Wextra -pthread -O2
LDFLAGS+=-pthread

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	rm -f $(TARGETS)

%: %.c
