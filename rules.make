$(bindir)/%: % $(bindir)
	$(INSTALL_PROGRAM) $(INST_PROG_FLAGS) $(filter-out $(bindir),$<) $@
$(includedir)/%: % $(includedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(includedir),$<) $@
$(includedir)/%: $(srcdir)/% $(includedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(includedir),$<) $@
$(libdir)/%: % $(libdir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(libdir),$<) $@
$(sbindir)/%: % $(sbindir)
	$(INSTALL_PROGRAM) $(INST_PROG_FLAGS) $(filter-out $(sbindir),$<) $@
$(libexecdir)/%: % $(libexecdir)
	$(INSTALL_PROGRAM) $(INST_PROG_FLAGS) $(filter-out $(libexecdir),$<) $@
$(sysconfdir)/%: % $(sysconfdir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(sysconfdir),$<) $@
$(localstatedir)/%: % $(localstatedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(localstatedir),$<) $@
$(sharedstatedir)/%: % $(sharedstatedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(sharedstatedir),$<) $@
$(mandir)/%: % $(mandir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(mandir),$<) $@
$(man1dir)/%: % $(man1dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(man1dir),$<) $@
$(man5dir)/%: % $(man5dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(man5dir),$<) $@
$(man8dir)/%: % $(man8dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $(filter-out $(man8dir),$<) $@

$(bindir) $(includedir) $(libdir) $(sbindir) $(libexecdir) $(sysconfdir) $(localstatedir) $(sharedstatedir) $(mandir) $(man1dir) $(man5dir) $(man8dir):
	@$(MKINSTDIRS) $@
