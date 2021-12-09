#ifndef NEWLIB_H
#define NEWLIB_H

#ifdef __cplusplus
    extern "C" {
#endif

extern int heap_used;
extern int heap_total;

void write_impl(char *ptr, int len);

#ifdef __cplusplus
    }
#endif

#endif // NEWLIB_H
