#include <sys/stat.h>
#include <errno.h>

/**
 * manage the heap
 */
extern char _sheap;                 /* start of the heap */
extern char _eheap;                 /* end of the heap */
static caddr_t heap_top = (caddr_t)&_sheap + 4;

int errno;

caddr_t _sbrk(int incr)
{
//    unsigned int state = disableIRQ();
    caddr_t res = heap_top;

    if (((incr > 0) && ((heap_top + incr > &_eheap) || (heap_top + incr < res))) ||
        ((incr < 0) && ((heap_top + incr < &_sheap) || (heap_top + incr > res)))) {
        errno = ENOMEM;
        res = (void *) -1;
    }
    else {
        heap_top += incr;
    }

//    restoreIRQ(state);
    return res;
}
