$(bindir)/%: % $(bindir)
	$(INSTALL_PROGRAM) $(INST_PROG_FLAGS) $< $@
$(includedir)/%: % $(includedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(libdir)/%: % $(libdir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(sbindir)/%: % $(sbindir)
	$(INSTALL_PROGRAM) $(INST_DATA_FLAGS) $< $@
$(libexecdir)/%: % $(libexecdir)
	$(INSTALL_PROGRAM) $(INST_DATA_FLAGS) $< $@
$(sysconfdir)/%: % $(sysconfdir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(localstatedir)/%: % $(localstatedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(sharedstatedir)/%: % $(sharedstatedir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(mandir)/%: % $(mandir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(man1dir)/%: % $(man1dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(man5dir)/%: % $(man5dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@
$(man8dir)/%: % $(man8dir)
	$(INSTALL_DATA) $(INST_DATA_FLAGS) $< $@

$(bindir) $(includedir) $(libdir) $(sbindir) $(libexecdir) $(sysconfdir) $(localstatedir) $(sharedstatedir) $(mandir) $(man1dir) $(man5dir) $(man8dir):
	@$(MKINSTDIRS) $@
