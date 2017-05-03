A="callf ellipsis malloc_wx plain plain_c++ suite suite2 suite3 suite_floats thunk callback_plain call_suite callback_suite dynload_plain" # syscall nm resolve_self
for i in $A ; do
  $i/$i
done

