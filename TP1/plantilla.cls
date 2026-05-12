\NeedsTeXFormat{LaTeX2e}

\LoadClass[journal, a4paper, 10pt]{IEEEtran}

\RequirePackage[utf8]{inputenc}
\RequirePackage[main=spanish]{babel}
\RequirePackage{mathptmx}
\RequirePackage{graphicx}
\RequirePackage{amsmath, amsfonts}
\RequirePackage{xfrac}
\RequirePackage{float}
\RequirePackage{enumitem}
\RequirePackage{booktabs}
\RequirePackage{tikz}
\RequirePackage{pgfplots}
\RequirePackage{circuitikz}
\RequirePackage{hyperref}
\RequirePackage{cuted}
\RequirePackage{caption}
\RequirePackage[table]{xcolor}

\usetikzlibrary{plotmarks}
\captionsetup{font=footnotesize, justification=centering, labelsep=period}
\pgfplotsset{compat=1.18}

\renewcommand{\title}[1]{\gdef\@title{\fontsize{24}{28}\selectfont\bfseries #1}}

\hypersetup{
    colorlinks,
    citecolor=black,
    filecolor=black,
    linkcolor=black,
    urlcolor=black
}

\makeatletter

\newcommand{\materia}[1]{\def\@materia{#1}}
\newcommand{\comision}[1]{\def\@comision{#1}}
\newcommand{\titulo}[1]{
  \title{
    \vspace{1.5cm}
    \fontsize{24}{28}\selectfont\bfseries #1
  }
}
\newcommand{\autores}[1]{
  \author{
    \vspace{0.25cm}
    
    \fontsize{11}{13}\selectfont\bfseries\itshape #1 \\[0.3cm]
    
    \fontsize{10}{12}\selectfont
    \@materia\, 4R2,
    Ingeniería Electrónica, Facultad Regional Córdoba \\
    Universidad Tecnológica Nacional
  }
}

\makeatother

\newcommand{\ohm}{\,\Omega}
\newcommand{\kohm}{\,\mathrm{k}\Omega}
\newcommand{\Mohm}{\,\mathrm{M}\Omega}
\newcommand{\nF}{\,\mathrm{nF}}
\newcommand{\uF}{\,\mathrm{\mu F}}
\newcommand{\mF}{\,\mathrm{mF}}
\newcommand{\F}{\,\mathrm{F}}
\newcommand{\uV}{\,\mathrm{\mu V}}
\newcommand{\mV}{\,\mathrm{mV}}
\newcommand{\V}{\,\mathrm{V}}
\newcommand{\kV}{\,\mathrm{kV}}
\newcommand{\uA}{\,\mathrm{\mu A}}
\newcommand{\mA}{\,\mathrm{mA}}
\newcommand{\A}{\,\mathrm{A}}
\newcommand{\mH}{\,\mathrm{mH}}
\newcommand{\uH}{\,\mathrm{\mu H}}
\newcommand{\nH}{\,\mathrm{nH}}
\newcommand{\Hz}{\,\mathrm{Hz}}
\newcommand{\W}{\,\mathrm{W}}
\newcommand{\mW}{\,\mathrm{mW}}
\newcommand{\kW}{\,\mathrm{kW}}
\newcommand{\J}{\,\mathrm{J}}
\newcommand{\mJ}{\,\mathrm{mJ}}
\newcommand{\kJ}{\,\mathrm{kJ}}
\newcommand{\Wh}{\,\mathrm{Wh}}
\newcommand{\kWh}{\,\mathrm{kWh}}
\newcommand{\Celsius}{\,^\circ\mathrm{C}}
\newcommand{\Kelvin}{\,\mathrm{K}}
\newcommand{\s}{\,\mathrm{s}}
\newcommand{\ms}{\,\mathrm{ms}}
\newcommand{\us}{\,\mathrm{\mu s}}
\newcommand{\ns}{\,\mathrm{ns}}
\newcommand{\percent}{\,\%}


\endinput
