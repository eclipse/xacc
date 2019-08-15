# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = minorminer
SOURCEDIR     = source
BUILDDIR      = build-python

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile cpp

cpp:
	# This will generate the XML dump of the docstrings from the c++
	doxygen Doxyfile

	# This will (only as needed) create new rst files when new c++ classes/files/namespaces
	# appear. At the moment, they are all being regenerated every time since
	# source/cpp is in the gitignore.
	python -m breathe.apidoc -o source/cpp build-cpp/xml/

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
