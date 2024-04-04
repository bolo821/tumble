#include "tumble/filter.h"
tmb_filter tmb_filter_default(void){ tmb_filter f={0x0001u,0xffffffffu,0}; return f; }
int tmb_filter_should_collide(tmb_filter a, tmb_filter b){
    if (a.group != 0 && a.group == b.group) return a.group > 0;
    return (a.mask & b.category) != 0 && (b.mask & a.category) != 0;
}
