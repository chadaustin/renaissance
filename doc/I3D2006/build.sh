die() {
    echo "Error"
    exit
}

latex paper.tex  || die
bibtex paper     || die
latex paper.tex  || die
latex paper.tex  || die
dvipdf paper.dvi || die