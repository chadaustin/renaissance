#
# Shell to be used
#

#
# Variables to customize this makefile
#
# Tex documents and tex directory and host where to 'tex'
#
#

DOCUS=	miragl.tex

DOCUSDVI= $(DOCUS:.tex=.dvi)

DOCUSPS= $(DOCUS:.tex=.ps)

DOCUSPDF= $(DOCUS:.tex=.pdf)

ALLDOCUS= 

FIGNAMES=

FIGS= $(FIGNAMES:=.fig)
EPSS= $(FIGS:.fig=.eps)

#
# Only Tex suffixes
#

.SUFFIXES:
.SUFFIXES: .dvi .tex .eps .fig .ps .tex,v .fig,v .aux .bib .pdf

.PATH.fig,v: RCS
.PATH.tex,v: RCS


#
# Texing rules
#


.tex.dvi:
		latex $*.tex || ( rm $@; false; )
		bibtex $(@:.dvi=)
		latex $*.tex || ( rm $@; false; )
		latex $*.tex || ( rm $@; false; )

.tex.aux:
		latex $*.tex || ( rm $@; false; )
                
#		rsh $(TEXHOST) "source .tex; cd $(TEXDOCS); latex $*.tex;";\
#		rsh $(TEXHOST) "source .tex; alias";


.fig.eps:
	fig2dev -L ps $*.fig $*.eps

# sed ...  has to be done because of a bug in fig2dev
#	@sed "s/findfont 0\.000/findfont 8/" $*.eps > $*.eps.1
#	@mv -f $*.eps.1 $*.eps			# still needed ?


.epsZ.eps:
	f=$*; mv $$f $${f%.epsZ}.eps.Z; \
	uncompress $${f%.epsZ}.eps.Z


.dvi.ps:
	dvips -o $*.ps $*.dvi

.ps.pdf:
	ps2pdf -dMaxSubsetPct=100 \
         -dCompatibilityLevel=1.3 \
         -dSubsetFonts=true \
         -dEmbedAllFonts=true \
         -dAutoFilterColorImages=false \
         -dAutoFilterGrayImages=false \
         -dColorImageFilter=/FlateEncode \
         -dGrayImageFilter=/FlateEncode \
         -dMonoImageFilter=/FlateEncode \
         $*.ps $*.pdf

#
# Service dependencies
#

default: $(DOCUSPDF)

pics: $(EPSS)

clean:
	rm -f *.ps *.eps *.dvi *.log *.toc *.aux *.tmp \
			*.bbl *.blg *.lot *.lof \;

compress:
	for f in *.eps; \
	do \
		compress $f; \
	done

uncompress:
	for f in *.Z; \
	do \
		uncompress $f; \
	done

depend:
	sed '/^#DONT EDIT/q' makefile > makefile.new
#	echo 'for i do \n echo "$$i: $$i,v" \n done \n' |\
#		sh -s $(FIGS) $(ALLDOCUS) main.tex >> makefile.new
	echo 'for i do \n echo "$$i.eps: $$i.fig" \n done \n' |\
		sh -s $(FIGNAMES) >> makefile.new
	mv makefile.new makefile

small: $(DOCUSPS)
	quarto -r -4 main.ps > xxtmp.ps
	mv xxtmp.ps main.ps	
#
# Explicit Dependencies
#

miragl.pdf: miragl.ps
miragl.ps: ${EPSS} miragl.dvi 
miragl.dvi: ${EPSS} ${ALLDOCUS} miragl.aux cits.bib
# main.dvi: $(DOCUS:.tex=.aux)

