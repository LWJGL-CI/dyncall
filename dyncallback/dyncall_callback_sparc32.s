/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_sparc32.s
 Description: Callback Thunk - Implementation for Sparc 32-bit
 License:

   Copyright (c) 2007-2016 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

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

/* input:
	$i0    -> thunk
	$i0+24 -> cb handler
	$i0+28 -> userdata
*/

.text
.globl dcCallbackThunkEntry

/* Called by thunk - thunk stores pointer to DCCallback */
/* in %g1, and pointer to called function in %g2        */
dcCallbackThunkEntry:

	/* Prolog. */
	/* Frame size of 80b comes from needing storage space for the following: */
	/*   DCargs(sparc_req_reg_save_area:64 + argptr:4) + pad:4 + retval:8 */
	save %sp, -80, %sp

	/* Spill register args. */
	st   %i0, [ %fp + 68 ]  /* reg arg 0 */
	st   %i1, [ %fp + 72 ]  /* reg arg 1 */
	st   %i2, [ %fp + 76 ]  /* reg arg 2 */
	st   %i3, [ %fp + 80 ]  /* reg arg 3 */
	st   %i4, [ %fp + 84 ]  /* reg arg 4 */
	st   %i5, [ %fp + 88 ]  /* reg arg 5 */
	add  %fp, 68, %l0
	st   %l0, [ %sp + 64 ]  /* init arg_ptr */

	/* Zero retval store (for data < word size). */
	st   %g0, [ %sp + 72 ]
	st   %g0, [ %sp + 76 ]

	/* Prepare callback handler call. */
	mov  %g1, %o0           /* Param 0 = DCCallback*, %g1 holds ptr to thunk */
	add  %sp, 64, %o1       /* Param 1 = DCArgs* (ptr to struct with args ptr */
	add  %sp, 72, %o2       /* Param 2 = results ptr to 8b of local stack data */
	ld   [ %g1 + 28 ], %o3  /* Param 3 = userdata ptr */

	ld   [ %g1 + 24 ], %l0
	call %l0
	nop

	/* Put retval in %i0/%i1 (to be in caller's %o0/%o1), and %f0/%f1. */
	ld   [ %sp + 72 ], %i0
	ld   [ %sp + 76 ], %i1
	ld   [ %sp + 72 ], %f0
	ld   [ %sp + 76 ], %f1

	/* Epilog. */
	restore                 /* unshift reg window */
	retl                    /* Return from proc. -- jmpl %i7 + 8, %g0 */
	nop

