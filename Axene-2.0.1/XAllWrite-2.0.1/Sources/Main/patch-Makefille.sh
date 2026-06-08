#!/bin/tcsh -fb
foreach item ( `fgrep ".o \" Makefile` )
  if(${item} != '\') then
    set folder = `dirname ${item}`
    if("${folder}" != ".") then
      set folder = `echo ${folder} | cut -d/ -f2`
      set file = `basename ${item} | cut -d. -f1`
      echo ${file}
      sh -c "exec sed 's/^${file}.o :/..\/${folder}\/&/' Makefile-depend" > tmp
      mv tmp ../../programme/Main/Makefile-depend
    endif
  endif
end

