#include "pixel.h"
//#include "all.h"
//#include "pixelDecls.h"


/* !BEGIN!: Do not edit below this line. */

PixelStubs pixelStubs = {
    TCL_STUB_MAGIC,
    NULL,
    pmap_new, /* 0 */
    pmap_dup, /* 1 */
    row_blend, /* 2 */
    row_set, /* 3 */
    pmap_clr, /* 4 */
    pmap_cut, /* 5 */
    pmap_paste, /* 6 */
    pmap_paste_ref, /* 7 */
    pmap_patch, /* 8 */
    pmap_compose, /* 9 */
    pmap_free, /* 10 */
    pmap_filter, /* 11 */
    pmap_dropshadow, /* 12 */
    pmap_rotate, /* 13 */
    digest_region, /* 14 */
    put_pixel, /* 15 */
    get_pixel, /* 16 */
    rgb2hsv, /* 17 */
    hsv2rgb, /* 18 */
    do_dirty_tricks, /* 19 */
    init_2d, /* 20 */
    scale_pmap, /* 21 */
    bezier_arc_length, /* 22 */
    asm_pelset, /* 23 */
    NULL, /* 24 */
    box, /* 25 */
    line, /* 26 */
    line_aa, /* 27 */
    line_aa_osa, /* 28 */
    bezier, /* 29 */
    quadshade, /* 30 */
    Tcl_SetPMAPObj, /* 31 */
    Tcl_NewPMAPObj, /* 32 */
    Tcl_GetPMAPFromObj, /* 33 */
    read_vfs_file, /* 34 */
    read_vfs_file2, /* 35 */
    rle_decode, /* 36 */
    rle_encode, /* 37 */
};

/* !END!: Do not edit above this line. */
