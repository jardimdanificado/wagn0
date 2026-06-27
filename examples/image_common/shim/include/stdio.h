// Minimal stdio.h for wasm32 -nostdlib builds.
// gifdec.c includes <stdio.h> for fprintf; posix_shim.h provides
// fprintf as a no-op macro, so this header just needs to exist.

#ifndef _STDIO_H
#define _STDIO_H

#define stderr 0
#define fprintf(file, ...) ((void)0)

#endif
