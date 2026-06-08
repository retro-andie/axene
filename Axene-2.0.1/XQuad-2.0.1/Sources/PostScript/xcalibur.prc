%%BeginProcSet: XCaliburDict 1 1
%% ---------------- XCALIBUR POSTSCRIPT PROCSET 1.0 ----------------
%% Copyright (C) 1994-1996 Xcalibur <xcalibur@axene.org>
%% Last update Wed Apr  9 14:08:49 1997 Gudul
%% -----------------------------------------------------------------

/XCaliburDict 100 dict def
XCaliburDict begin

/bd{bind def}bind def
/ld{load def}bd
/dd{XCaliburDict 3 1 roll put}bd
/xdd{exch dd}bd
/dg{XCaliburDict exch get}bd

% Variables
/_fc{}def		% fill color
/_sc{}def		% stroke color
/$ctm matrix def	% ctm
/$cs 0 def		% char space
/$ws 0 def		% word space
/$tf false def		% text object flag
/$riz 0 def		% text rize
/$siz 0 def		% -(font size)/1000
/$tr 0 def		% text rendering mode
/$tl 0 def		% text leading
/$tm{}def		% textmatrix
/$scl 1 def		% text scaling
%/$mc 1 string def	% tmp tj variable
/$is{}def		% image string

% Color procs
/setrgbcolor where
{pop}{/setrgbcolor{.11 mul exch .59 mul add exch .3 mul add setgray}bd}ifelse
/setcmykcolor where
{pop}{/setcmykcolor{1 sub 4 1 roll 3{3 index add neg dup 0 lt{pop 0}if
3 1 roll}repeat setrgbcolor pop}bd}ifelse
/_so{load dup type /arraytype eq{aload length}{1}ifelse}bd
/_g{/setgray _so 1 add array astore cvx}bd
/_k{/setcmykcolor _so 4 add array astore cvx}bd
/_rg{/setrgbcolor _so 3 add array astore cvx}bd
/g{_g /_fc xdd}bd
/G{_g /_sc xdd}bd
/k{_k /_fc xdd}bd
/K{_k /_sc xdd}bd
/rg{_rg /_fc xdd}bd
/RG{_rg /_sc xdd}bd

% Filling & clipping procs
/W/clip ld
/W*/eoclip ld
/f{_fc fill}bd
/f*{_fc eofill}bd
/S{_sc stroke}bd
/s{h S}bd
/b{h q f Q S}bd
/B{q f Q S}bd
/b*{h q f* Q S}bd
/B*{q f* Q S}bd
/n/newpath ld

% Graphic context procs
/d/setdash ld
/w/setlinewidth ld
/M/setmiterlimit ld
/i/setflat ld
/j/setlinejoin ld
/J/setlinecap ld
/q/gsave ld
/Q/grestore ld
/cm{matrix astore concat}bd
/cp/currentpoint ld

% Drawing procs
/setstrokeadjust where{pop false setstrokeadjust}{}ifelse
/rm/rmoveto ld
/l/lineto ld
/m/moveto ld
/c/curveto ld
/v{cp 6 2 roll c}bd
/y{2 copy c}bd
/h/closepath ld
/re{4 2 roll m 1 index 0 rlineto 0 exch rlineto neg 0 rlineto h}bd

% Image procs
/colorimage where{pop}{% colorimage emulation (8bits color samples only :-( )
/_glue{2{aload length dup 2 add -1 roll}repeat add array astore cvx}bd
/$c2g[{}{}{dup length 3 idiv 0 1 2 index 1 sub{dup 3 mul 3 index 1 index get .3
mul 4 index 2 index 1 add get .59 mul add 4 index 2 index 2 add get .11 mul
add round cvi exch pop 3 index 3 1 roll put}for 0 exch getinterval}% RGB->Gray
{dup length 4 idiv 0 1 2 index 1 sub{dup 4 mul 255 4 index 2 index get .3
mul 5 index 3 index 1 add get .59 mul add 5 index 3 index 2 add get .11 mul
add 5 index 3 index 3 add get add round cvi dup 255 gt{pop dup}if sub exch
pop 3 index 3 1 roll put}for 0 exch getinterval}% CMYK->Gray
]bd
/colorimage{exch pop $c2g exch 1 sub get exch _glue image}bd}ifelse 
/ASCIIHexDecode{currentfile $is readhexstring pop}bd
/DeviceCMYK 4 def
/DeviceRGB 3 def
/DeviceGray 1 def
/BI{save mark}bd
/EI/restore ld
/ID{counttomark 2 idiv dup dict begin {def}repeat pop Width dup
BitsPerComponent ColorSpace dg mul mul 7 add 8 idiv string /$is xdd
Height BitsPerComponent[Width 0 0 Height 0 0] Filter dg false ColorSpace dg
end colorimage{currentfile read{62 eq{exit}if}{exit}ifelse}loop}bd

% Text procs (render mode 0 only :-( )
/_dtm{$ctm setmatrix $tm concat 0 $riz translate $scl 1 scale}bd
/_stm{_dtm 0 0 m}bd
/_su{$ws 0 32 $cs 0 6 5 roll}bd
/BT{/$tf true dd $ctm currentmatrix pop /$tm matrix dd _stm}bd
/ET{/$tf false dd $ctm setmatrix n}bd
/Tm{$tm astore pop _stm}bd
/Td{matrix translate $tm $tm concatmatrix pop _stm}bd
/TL{neg /$tl xdd}bd
/TD{dup /$tl xdd Td}bd
/Tf{dup 1000 div neg /$siz xdd exch findfont exch scalefont setfont}bd
/Tz{100 div /$scl xdd $tf{_dtm}if}bd
/Tc{/$cs xdd}bd
/Tw{/$ws xdd}bd
/Ts{/$riz xdd cp _dtm m}bd
/Tr{/$tr xdd}bd
/T*{0 $tl Td}bd
/Tj{_fc _su awidthshow}bd
/TJ{{dup type /stringtype ne{$siz mul 0 rm}{Tj}ifelse}forall}bd
/"{exch Tc exch Tw T* Tj}bd
/'{T* Tj}bd

% Font encoding
/FE{findfont dup length dict begin{1 index/FID ne{def}{pop pop}ifelse}forall
/FontName exch def dup type /nulltype ne{/Encoding exch def}{pop}ifelse
FontName currentdict end definefont pop}bd 
/XcEncoding[
% 00x
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef
% 01x
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef
% 02x
/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef/.notdef
% 03x
/.notdef/.notdef/emdash/endash/florin/OE/guilsinglleft/guilsinglright
% 04x
/space/exclam/quotedbl/numbersign/dollar/percent/ampersand/quoteright
% 05x
/parenleft/parenright/asterisk/plus/comma/minus/period/slash
% 06x
/zero/one/two/three/four/five/six/seven
% 07x
/eight/nine/colon/semicolon/less/equal/greater/question
% 10x
/at/A/B/C/D/E/F/G
% 11x
/H/I/J/K/L/M/N/O
% 12x
/P/Q/R/S/T/U/V/W
% 13x
/X/Y/Z/bracketleft/backslash/bracketright/asciicircum/underscore
% 14x
/quoteleft/a/b/c/d/e/f/g
% 15x
/h/i/j/k/l/m/n/o
% 16x
/p/q/r/s/t/u/v/w
% 17x
/x/y/z/braceleft/bar/braceright/asciitilde/.notdef
% 20x
/perthousand/trademark/fraction/bullet/dagger/daggerdbl/ellipsis/florin
% 21x (notdef)
/quotedblbase/quotedblleft/quotedblright/quotesinglbase/quotesingle/fi/fl/oe
% 22x (notdef)
/dotlessi/grave/acute/circumflex/tilde/macron/breve/dotaccent
% 23x (notdef)
/dieresis/.notdef/ring/cedilla/.notdef/hungarumlaut/ogonek/caron
% 24x
/space/exclamdown/cent/sterling/currency/yen/brokenbar/section
% 25x
/dieresis/copyright/ordfeminine/guillemotleft/logicalnot/hyphen/registered/macron
% 26x
/ring/plusminus/twosuperior/threesuperior/acute/mu/paragraph/periodcentered
% 27x
/cedilla/onesuperior/ordmasculine/guillemotright/onequarter/onehalf/threequarters/questiondown
% 30x
/Agrave/Aacute/Acircumflex/Atilde/Adieresis/Aring/AE/Ccedilla
% 31x
/Egrave/Eacute/Ecircumflex/Edieresis/Igrave/Iacute/Icircumflex/Idieresis
% 32x
/Eth/Ntilde/Ograve/Oacute/Ocircumflex/Otilde/Odieresis/multiply
% 33x
/Oslash/Ugrave/Uacute/Ucircumflex/Udieresis/Yacute/Thorn/germandbls
% 34x
/agrave/aacute/acircumflex/atilde/adieresis/aring/ae/ccedilla
% 35x
/egrave/eacute/ecircumflex/edieresis/igrave/iacute/icircumflex/idieresis
% 36x
/eth/ntilde/ograve/oacute/ocircumflex/otilde/odieresis/divide
% 37x
/oslash/ugrave/uacute/ucircumflex/udieresis/yacute/thorn/ydieresis
]def

%%EndProcSet

