/*

 Package: dyncall
 Library: test
 File: test/dynload_plain/dynload_plain.c
 Description:
 License:

   Copyright (c) 2017 Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/


#include "../../dynload/dynload.h"
#include "../common/platformInit.h"

#include <string.h>
#if defined(DC_WINDOWS)
#  include <io.h>
#  define F_OK 0
#else
#  include <unistd.h>
#endif


int main(int argc, char* argv[])
{
  int r = 0, i;
  void* p;
  DLLib* pLib;
  DLSyms* pSyms;
  const char* path = NULL;
  const char* clibs[] = { // hacky/lazy list of some clib paths per platform
    "/lib/libc.so",
    "/lib/libc.so.6",
    "/lib/libc.so.7",
    "/lib64/libc.so",
    "/lib64/libc.so.6",
    "/lib64/libc.so.7",
    "/lib32/libc.so",
    "/lib32/libc.so.6",
    "/lib32/libc.so.7",
	"/usr/lib/system/libsystem_c.dylib",
    "/usr/lib/libc.dylib",
    "\\ReactOS\\system32\\msvcrt.dll",
    "C:\\ReactOS\\system32\\msvcrt.dll",
    "\\Windows\\system32\\msvcrt.dll",
    "C:\\Windows\\system32\\msvcrt.dll"
  };


  for(i=0; i<(sizeof(clibs)/sizeof(const char*)); ++i) {
    if(access(clibs[i], F_OK) != -1) {
      path = clibs[i];
      break;
    }
  }

  if(path) {
    printf("using clib to test at: %s\n", path);
    ++r;

    // dl*Library tests
    // --------
    pLib = dlLoadLibrary(path); // check if we can load a lib
    if(pLib) {
      printf("pLib handle: %p\n", pLib);
      ++r;

      p = dlFindSymbol(pLib, "printf"); // check if we can lookup a symbol
      printf("printf at: %p\n", p);
      r += (p != NULL);

      dlFreeLibrary(pLib);
    }
    else
      printf("unable to open library %s\n", path);


    // dlSyms* tests (intentionally after freeing lib above, as they work standalone)
    // --------
    pSyms = dlSymsInit(path); // check if we can iterate over symbols - init
    if(pSyms) {
      int n;
      const char* name;

      printf("pSyms handle: %p\n", pSyms);
      ++r;

      n = dlSymsCount(pSyms); // check if there are some syms to iterate over
      printf("num of libc symbols: %d\n", n);
      r += (n > 0);

      for(i=0; i<n; ++i) {
        name = dlSymsName(pSyms, i);
        if(name && strcmp(name, "printf") == 0) { // check if we find "printf" also in iterated symbols
          ++r;
          break;
        }
      }
      printf("printf symbol found by iteration: %d\n", i<n);

      name = (i<n) ? dlSymsName(pSyms, i) : NULL;
      r += (name && strcmp(name, "printf") == 0); // check if we can lookup "printf" by index
      printf("printf symbol name by index: %s\n", name?name:"");

      pLib = dlLoadLibrary(path); // check if we can resolve ptr -> name,
      if(pLib) {                  // need to lookup by name again, first
        p = dlFindSymbol(pLib, "printf");
        name = dlSymsNameFromValue(pSyms, p);
        printf("printf symbol name by its own address (%p): %s\n", p, name?name:"");
        r += (name && strcmp(name, "printf") == 0);
        dlFreeLibrary(pLib);
      }

      dlSymsCleanup(pSyms);
    }
    else
      printf("dlSymsInit failed\n");
  }

  // All worked if we got a score of 6 right ones
  r = (r == 8);
  printf("result: dynload_plain: %d\n", r);
  return !r;
}

