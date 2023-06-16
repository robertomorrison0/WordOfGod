#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.style"), aligned (8)))
#else
# define SECTION
#endif

static const SECTION union { const guint8 data[2393]; const double alignment; void * const ptr;}  style_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\344\000\000\000\000\000\000\050\007\000\000\000"
  "\000\000\000\000\001\000\000\000\003\000\000\000\004\000\000\000"
  "\005\000\000\000\006\000\000\000\007\000\000\000\120\002\104\340"
  "\001\000\000\000\344\000\000\000\020\000\114\000\364\000\000\000"
  "\370\000\000\000\113\120\220\013\003\000\000\000\370\000\000\000"
  "\004\000\114\000\374\000\000\000\000\001\000\000\114\335\037\100"
  "\004\000\000\000\000\001\000\000\012\000\166\000\020\001\000\000"
  "\322\005\000\000\324\265\002\000\377\377\377\377\322\005\000\000"
  "\001\000\114\000\324\005\000\000\330\005\000\000\031\044\061\045"
  "\000\000\000\000\330\005\000\000\013\000\114\000\344\005\000\000"
  "\360\005\000\000\201\175\357\064\004\000\000\000\360\005\000\000"
  "\011\000\166\000\000\006\000\000\266\007\000\000\220\067\222\167"
  "\004\000\000\000\266\007\000\000\016\000\166\000\310\007\000\000"
  "\130\011\000\000\162\157\142\145\162\164\157\155\157\162\162\151"
  "\163\157\156\057\004\000\000\000\157\162\147\057\000\000\000\000"
  "\142\165\151\154\144\145\162\056\165\151\000\000\000\000\000\000"
  "\177\056\000\000\001\000\000\000\170\332\355\132\113\163\342\070"
  "\020\276\363\053\130\335\025\146\016\133\265\007\140\212\274\046"
  "\251\245\102\212\170\046\265\047\227\154\067\240\305\110\056\111"
  "\340\260\277\176\345\007\013\044\074\054\133\166\062\251\315\051"
  "\030\253\273\325\217\257\277\226\350\176\173\131\204\355\025\010"
  "\111\071\353\241\257\027\137\120\033\230\317\003\312\246\075\364"
  "\303\271\305\177\240\157\375\126\227\062\005\142\102\174\350\267"
  "\332\372\257\313\275\277\301\127\155\032\364\120\114\131\300\143"
  "\324\366\103\042\145\017\015\202\370\071\173\222\275\232\276\356"
  "\317\150\030\154\077\357\212\310\227\175\127\363\113\376\202\366"
  "\337\111\337\213\004\217\100\250\165\233\221\005\364\020\027\024"
  "\230\042\112\333\213\372\332\160\105\175\022\166\073\233\267\316"
  "\013\230\301\113\104\130\200\372\216\130\202\311\302\125\341\205"
  "\277\141\334\276\270\270\150\143\174\340\313\003\276\070\342\023"
  "\355\312\041\220\111\010\012\245\236\016\363\017\207\027\037\062"
  "\331\047\014\063\262\242\123\242\000\173\304\237\237\067\276\220"
  "\250\011\027\061\021\101\171\151\023\036\006\130\315\004\310\131"
  "\362\137\304\103\352\257\121\177\160\375\354\336\216\206\327\256"
  "\163\067\276\171\272\113\376\173\034\015\357\257\376\162\037\006"
  "\316\217\361\140\130\110\327\011\017\233\146\237\365\154\054\143"
  "\354\361\364\270\003\022\200\270\044\002\025\223\220\112\361\164"
  "\171\352\362\316\167\240\003\020\143\140\072\032\124\205\072\111"
  "\226\112\161\046\015\344\245\062\103\316\347\313\150\047\270\240"
  "\223\043\317\327\156\047\373\326\300\304\116\156\243\301\222\127"
  "\201\311\166\023\323\140\172\262\140\212\271\071\003\064\047\021"
  "\151\050\353\250\151\250\255\004\141\062\044\212\170\241\176\270"
  "\006\355\363\047\032\200\107\204\111\376\354\273\055\263\334\304"
  "\321\146\232\212\153\350\166\012\046\166\012\226\062\333\370\101"
  "\300\264\130\061\272\314\237\174\301\303\020\202\067\075\312\246"
  "\332\343\352\207\124\252\004\151\122\074\347\032\060\126\024\142"
  "\327\053\204\075\326\032\233\251\340\125\135\202\045\204\332\075"
  "\032\073\361\202\007\272\264\030\147\325\344\113\265\016\301\174"
  "\135\026\337\044\110\271\141\072\040\020\340\120\007\013\165\112"
  "\230\321\051\151\107\132\013\202\307\262\120\041\124\055\175\223"
  "\072\262\133\367\347\305\235\025\123\242\311\157\351\324\043\231"
  "\102\211\146\237\323\237\004\260\121\377\226\204\322\050\131\337"
  "\020\252\144\003\250\064\216\101\104\004\121\134\024\111\320\242"
  "\146\276\117\134\336\221\174\331\102\320\217\301\342\244\042\102"
  "\375\352\074\316\112\233\275\114\167\137\246\247\356\273\165\105"
  "\045\365\112\221\276\032\274\131\301\253\307\022\236\372\074\231"
  "\352\026\172\207\123\216\043\001\053\312\227\022\313\365\302\113"
  "\046\262\046\211\250\141\334\077\005\337\277\342\114\151\004\173"
  "\037\276\337\160\111\072\174\072\015\041\057\314\224\376\252\364"
  "\211\253\021\027\134\257\164\301\036\317\350\174\250\300\031\066"
  "\126\114\351\103\272\210\346\256\053\250\106\213\233\050\025\233"
  "\220\232\106\056\245\267\265\304\114\103\320\004\004\060\037\044"
  "\016\100\316\025\217\360\204\357\227\310\347\367\157\100\304\334"
  "\115\106\242\012\076\116\106\211\023\216\216\201\250\031\010\354"
  "\353\166\044\060\243\323\231\072\124\043\306\203\310\231\370\006"
  "\124\106\041\131\143\117\044\032\031\310\137\243\331\130\211\360"
  "\043\217\222\211\177\267\204\242\374\121\165\037\373\332\223\071"
  "\354\355\112\266\011\167\046\003\213\055\312\337\370\331\107\355"
  "\147\040\125\162\252\106\342\153\120\303\377\160\276\300\174\251"
  "\154\170\341\044\134\155\372\353\353\121\073\005\245\152\216\063"
  "\306\214\312\030\362\311\342\117\331\377\341\117\302\327\152\126"
  "\377\007\155\222\165\335\013\370\331\200\124\054\342\325\056\006"
  "\314\372\221\365\376\143\275\337\330\341\054\043\315\041\064\137"
  "\313\070\113\176\376\111\071\163\171\376\374\243\261\212\127\327"
  "\073\251\331\013\102\231\033\233\336\367\234\263\173\101\304\224"
  "\262\354\314\015\365\277\376\136\007\351\310\165\100\222\007\365"
  "\150\330\170\337\166\033\252\104\356\012\235\367\176\167\376\164"
  "\107\343\373\233\007\147\340\334\217\036\334\237\067\143\347\376"
  "\252\330\057\004\232\354\321\047\116\104\340\105\375\244\220\147"
  "\151\162\021\210\354\250\072\075\037\050\255\066\121\146\323\121"
  "\147\022\330\343\232\225\054\164\016\177\151\120\251\342\121\303"
  "\032\067\130\320\244\316\014\033\032\321\150\175\306\152\174\346"
  "\072\247\060\026\044\312\257\242\023\170\171\036\017\036\335\347"
  "\321\370\332\275\272\033\214\233\060\300\133\116\046\245\216\003"
  "\314\221\347\062\123\225\235\304\352\317\226\225\036\074\010\117"
  "\325\324\341\107\073\244\336\026\331\066\302\345\245\220\134\340"
  "\377\056\232\336\134\053\327\246\031\002\132\366\052\273\164\101"
  "\157\166\131\107\101\333\213\377\166\270\173\367\051\263\232\213"
  "\266\026\124\032\064\355\034\076\233\217\014\125\316\241\053\014"
  "\274\255\112\333\314\031\165\066\262\266\012\246\120\211\371\271"
  "\125\347\057\071\216\057\337\321\337\072\275\142\117\301\366\353"
  "\156\147\347\267\344\377\002\153\260\275\255\000\050\165\165\141"
  "\171\051\057\000\001\000\000\000\142\151\142\154\145\142\157\154"
  "\154\163\057\000\002\000\000\000\006\000\000\000\005\000\000\000"
  "\163\164\171\154\145\056\143\163\163\000\000\000\000\000\000\000"
  "\246\001\000\000\000\000\000\000\167\151\156\144\157\167\040\173"
  "\012\040\040\040\040\142\141\143\153\147\162\157\165\156\144\055"
  "\143\157\154\157\162\072\040\162\147\142\050\061\071\066\054\040"
  "\061\070\067\054\040\061\064\070\051\073\012\040\040\040\040\143"
  "\157\154\157\162\072\040\162\147\142\050\063\066\054\040\063\066"
  "\054\040\063\066\051\073\012\175\012\012\164\145\170\164\166\151"
  "\145\167\040\173\040\012\040\040\040\040\142\141\143\153\147\162"
  "\157\165\156\144\055\143\157\154\157\162\072\040\162\147\142\050"
  "\061\071\066\054\040\061\070\067\054\040\061\064\070\051\073\012"
  "\040\040\040\040\143\157\154\157\162\072\040\162\147\142\050\063"
  "\066\054\040\063\066\054\040\063\066\051\073\012\175\012\012\163"
  "\164\171\154\145\137\160\157\160\157\166\145\162\040\173\040\012"
  "\040\040\040\040\142\141\143\153\147\162\157\165\156\144\055\143"
  "\157\154\157\162\072\040\162\147\142\050\061\071\066\054\040\061"
  "\070\067\054\040\061\064\070\051\073\012\040\040\040\040\143\157"
  "\154\157\162\072\040\162\147\142\050\063\066\054\040\063\066\054"
  "\040\063\066\051\073\012\040\040\040\040\012\175\012\012\142\157"
  "\170\040\173\040\012\040\040\040\040\012\040\040\040\040\142\141"
  "\143\153\147\162\157\165\156\144\055\143\157\154\157\162\072\040"
  "\162\147\142\050\061\071\066\054\040\061\070\067\054\040\061\064"
  "\070\051\073\012\040\040\040\040\143\157\154\157\162\072\040\162"
  "\147\142\050\063\066\054\040\063\066\054\040\063\066\051\073\012"
  "\175\012\012\043\156\141\166\151\147\141\164\151\157\156\137\141"
  "\162\162\157\167\173\012\040\040\040\040\143\157\154\157\162\072"
  "\040\162\147\142\050\063\066\054\040\063\066\054\040\063\066\051"
  "\073\012\012\175\012\012\154\141\142\145\154\040\173\012\040\040"
  "\040\040\163\151\172\145\072\040\061\066\073\012\175\012\000\000"
  "\050\165\165\141\171\051\163\164\171\154\145\055\144\141\162\153"
  "\056\143\163\163\000\000\000\000\200\001\000\000\000\000\000\000"
  "\167\151\156\144\157\167\040\173\012\040\040\040\040\142\141\143"
  "\153\147\162\157\165\156\144\055\143\157\154\157\162\072\040\162"
  "\147\142\050\063\067\054\040\063\067\054\040\063\067\051\073\012"
  "\175\012\012\164\145\170\164\166\151\145\167\040\173\040\012\040"
  "\040\040\040\143\157\154\157\162\072\040\162\147\142\050\062\065"
  "\065\054\040\062\065\065\054\040\062\065\065\051\073\012\040\040"
  "\040\040\057\052\040\142\141\143\153\147\162\157\165\156\144\055"
  "\143\157\154\157\162\072\040\162\147\142\050\060\054\040\060\054"
  "\040\060\051\073\040\052\057\012\040\040\040\040\142\141\143\153"
  "\147\162\157\165\156\144\055\143\157\154\157\162\072\040\162\147"
  "\142\050\060\054\040\060\054\040\060\051\073\012\175\012\012\163"
  "\164\171\154\145\137\160\157\160\157\166\145\162\040\173\040\012"
  "\040\040\040\040\143\157\154\157\162\072\040\162\147\142\050\060"
  "\054\040\060\054\040\060\051\073\012\040\040\040\040\142\141\143"
  "\153\147\162\157\165\156\144\055\143\157\154\157\162\072\040\162"
  "\147\142\050\060\054\040\060\054\040\060\051\073\012\040\040\040"
  "\040\012\175\012\012\142\157\170\040\173\040\012\040\040\040\040"
  "\012\040\040\040\040\143\157\154\157\162\072\040\162\147\142\050"
  "\062\065\065\054\040\062\065\065\054\040\062\065\065\051\073\012"
  "\040\040\040\040\057\052\040\142\141\143\153\147\162\157\165\156"
  "\144\055\143\157\154\157\162\072\040\162\147\142\050\060\054\040"
  "\060\054\040\060\051\073\040\052\057\012\040\040\040\040\142\141"
  "\143\153\147\162\157\165\156\144\055\143\157\154\157\162\072\040"
  "\162\147\142\050\060\054\040\060\054\040\060\051\073\012\175\012"
  "\000\000\050\165\165\141\171\051" };

static GStaticResource static_resource = { style_resource_data.data, sizeof (style_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_MODULE_EXPORT
GResource *style_get_resource (void);
GResource *style_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_CONSTRUCTOR_H__
#define __G_CONSTRUCTOR_H__

/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
/* Visual studio 2008 and later has _Pragma */

/*
 * Only try to include gslist.h if not already included via glib.h,
 * so that items using gconstructor.h outside of GLib (such as
 * GResources) continue to build properly.
 */
#ifndef __G_LIB_H__
#include "gslist.h"
#endif

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined (_MSC_VER)

#define G_HAS_CONSTRUCTORS 1

/* Pre Visual studio 2008 must use #pragma section */
#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _wrapper(void) { _func(); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (*p)(void) = _func ## _wrapper;

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _constructor(void) { atexit (_func); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */
#endif /* __G_CONSTRUCTOR_H__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(styleresource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(styleresource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(styleresource_destructor)
#endif
G_DEFINE_DESTRUCTOR(styleresource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void styleresource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void styleresource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
