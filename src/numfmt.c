#include "tumble/numfmt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
size_t tmb_fmt_float(float v, char *buf, size_t cap){
    for (int p=1;p<=9;p++){ char t[32]; int n=snprintf(t,sizeof(t),"%.*g",p,(double)v);
        if(n<0||(size_t)n>=sizeof(t)) continue;
        if((float)strtod(t,NULL)==v){ if((size_t)n+1>cap) return 0; memcpy(buf,t,(size_t)n+1); return (size_t)n; } }
    int n=snprintf(buf,cap,"%.9g",(double)v); return n<0?0:(size_t)n;
}
size_t tmb_fmt_int(long long v, char *buf, size_t cap){ int n=snprintf(buf,cap,"%lld",v); return n<0?0:(size_t)n; }
int tmb_parse_float(const char *s, size_t n, float *out){
    char b[64]; if(n==0||n>=sizeof(b)) return -1; memcpy(b,s,n); b[n]='\0';
    char *e; float v=strtof(b,&e); if(e==b) return -1; *out=v; return 0;
}
int tmb_parse_int(const char *s, size_t n, long long *out){
    char b[32]; if(n==0||n>=sizeof(b)) return -1; memcpy(b,s,n); b[n]='\0';
    char *e; long long v=strtoll(b,&e,10); if(e==b||*e!='\0') return -1; *out=v; return 0;
}
