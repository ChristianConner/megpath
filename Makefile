all:
	$(MAKE) -C shared
	$(MAKE) -C threeMatrix
	$(MAKE) -C distributed
	$(MAKE) -C threaded
	$(MAKE) -C openMp
	$(MAKE) -C hybrid
	$(MAKE) -C standard

clean:
	$(MAKE) -C shared clean
	$(MAKE) -C threeMatrix clean
	$(MAKE) -C distributed clean
	$(MAKE) -C threaded clean
	$(MAKE) -C openMp clean
	$(MAKE) -C hybrid clean
	$(MAKE) -C standard clean
