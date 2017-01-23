# --------------------------------------------- #
# Makefile for Adversarial Jets Framework       #
# Luke de Oliveira (lukedeoliveira@lbl.gov      #
# Created 1/21/17                               #
# --------------------------------------------- #

# set directories
TARGDIR    := src
EXECDIR    := bin
EXECUTABLE := src/bin/jet-image-maker
EXTERNALS  := simulations

all: $(EXTERNALS)

simulations:
	@echo "Building $@"
	@$(MAKE) -C $(TARGDIR)
	@mkdir $(EXECDIR)
	@cp $(EXECUTABLE) $(EXECDIR)

.PHONY : clean rmdep purge $(EXTERNALS)

clean:
	@$(MAKE) $@ -C $(TARGDIR)

purge:
	@rm -rf $(EXECDIR)
	@$(MAKE) $@ -C $(TARGDIR)

rmdep: 
	@$(MAKE) $@ -C $(TARGDIR)