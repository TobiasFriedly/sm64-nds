
define validate-option
  ifeq ($$(filter $($(1)),$(2)),)
    $$(error Value of $(1) must be one of the following: $(2))
  endif
  ifneq ($$(words $($(1))),1)
    $$(error Value of $(1) must be one of the following: $(2))
  endif
endef

find-command = $(shell which $(1) 2>/dev/null)
