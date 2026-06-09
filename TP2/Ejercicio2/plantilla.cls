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

% ========================================================
% CONFIGURACIÓN DE CÓDIGO FUENTE (C/C++ PARA MICROCONTROLADORES)
% ========================================================
\RequirePackage{listings}
\RequirePackage{xcolor}

% Definición de colores
\definecolor{codegreen}{rgb}{0,0.6,0}
\definecolor{codegray}{rgb}{0.5,0.5,0.5}
\definecolor{codepurple}{rgb}{0.58,0,0.82}
\definecolor{backcolour}{rgb}{0.95,0.95,0.92}

% Configuración global del paquete listings
\lstset{
    language=C,
    backgroundcolor=\color{backcolour},
    commentstyle=\color{codegreen}\itshape,
    keywordstyle=\color{blue}\bfseries,
    numberstyle=\tiny\color{codegray},
    stringstyle=\color{codepurple},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,         
    breaklines=true,
    captionpos=b,                    
    keepspaces=true,                 
    numbers=left,                    
    numbersep=5pt,                  
    showspaces=false,                
    showstringspaces=false,
    showtabs=false,                  
    tabsize=4,
    frame=single,
    rulecolor=\color{black!30},
    % Soporte para tildes y eñes en comentarios
    literate={á}{{\'a}}1 {é}{{\'e}}1 {í}{{\'i}}1 {ó}{{\'o}}1 {ú}{{\'u}}1
             {Á}{{\'A}}1 {É}{{\'E}}1 {Í}{{\'I}}1 {Ó}{{\'O}}1 {Ú}{{\'U}}1
             {ñ}{{\~n}}1 {Ñ}{{\~N}}1
}
% ========================================================
% ========================================================
% LENGUAJE PERSONALIZADO PARA ARM ASSEMBLER
% ========================================================
\lstdefinelanguage{ARMasm}{
    % Directivas de ensamblador y mnemónicos comunes de ARM Cortex-M
    morekeywords={.syntax, .cpu, .thumb, .global, .section, .word, .thumb_func, .equ, bl, b, ldr, str, mov, cmp, beq, bne, push, pop},
    sensitive=false, % El ensamblador suele no ser sensible a mayúsculas/minúsculas
    % Definición de comentarios típicos en ARM GNU Assembler
    morecomment=[l]{//},
    morecomment=[s]{/*}{*/},
    morecomment=[l]{@},
    morecomment=[l]{;},
    morestring=[b]",
    % Le damos un color morado a las instrucciones para diferenciarlo del C (opcional)
    keywordstyle=\color{codepurple}\bfseries
}
% ========================================================
% LENGUAJE PERSONALIZADO PARA LINKER SCRIPTS (.ld)
% ========================================================
\lstdefinelanguage{Linker}{
    % Palabras reservadas típicas de GNU Linker
    morekeywords={MEMORY, SECTIONS, KEEP, ORIGIN, LENGTH, ALIGN, ENTRY, PROVIDE, DISCARD},
    sensitive=true, % Sensible a mayúsculas y minúsculas
    % Los linker scripts usan comentarios estilo C
    morecomment=[s]{/*}{*/},
    morestring=[b]",
    % Mantenemos el azul para las palabras clave
    keywordstyle=\color{blue}\bfseries
}
% ========================================================
% LENGUAJE PERSONALIZADO PARA MAKEFILE
% ========================================================
\lstdefinelanguage{MyMake}{
    % Palabras clave o nombres de variables comunes que quieras resaltar en azul
    morekeywords={all, clean, .PHONY, CC, MACH, CFLAGS, LDFLAGS, TARGET},
    sensitive=true,
    % Los comentarios en Makefile usan el numeral (#)
    morecomment=[l]{\#},
    morestring=[b]",
    morestring=[b]',
    % Resaltar variables $(...) y ${...} en color morado
    moredelim=[s][\color{codepurple}]{\$(}{)},
    moredelim=[s][\color{codepurple}]{\$\{}{\}},
    % Estilo de las palabras clave
    keywordstyle=\color{blue}\bfseries
}

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
