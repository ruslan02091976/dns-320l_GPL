#!/bin/sh
ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes ./configure --host=${TARGET_HOST} --with-gnu-ld --prefix=/usr --enable-libdbi --libdir=/lib
make clean
make