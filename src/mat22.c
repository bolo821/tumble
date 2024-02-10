#include "tumble/mat22.h"
#include <math.h>
tmb_mat22 tmb_mat22_identity(void){ tmb_mat22 m={{1,0},{0,1}}; return m; }
tmb_mat22 tmb_mat22_rotation(float r){ float c=cosf(r),s=sinf(r); tmb_mat22 m={{c,s},{-s,c}}; return m; }
tmb_mat22 tmb_mat22_scale(float sx,float sy){ tmb_mat22 m={{sx,0},{0,sy}}; return m; }
tmb_vec2 tmb_mat22_apply(tmb_mat22 m, tmb_vec2 v){ return tmb_v2(m.a.x*v.x+m.b.x*v.y, m.a.y*v.x+m.b.y*v.y); }
tmb_mat22 tmb_mat22_mul(tmb_mat22 m, tmb_mat22 n){ tmb_mat22 r; r.a=tmb_mat22_apply(m,n.a); r.b=tmb_mat22_apply(m,n.b); return r; }
tmb_mat22 tmb_mat22_transpose(tmb_mat22 m){ tmb_mat22 r={{m.a.x,m.b.x},{m.a.y,m.b.y}}; return r; }
float tmb_mat22_det(tmb_mat22 m){ return m.a.x*m.b.y - m.b.x*m.a.y; }
tmb_mat22 tmb_mat22_inverse(tmb_mat22 m, int *ok){
    float d=tmb_mat22_det(m);
    if (d>-1e-20f && d<1e-20f){ if(ok)*ok=0; return tmb_mat22_identity(); }
    float inv=1.0f/d; if(ok)*ok=1;
    tmb_mat22 r={{m.b.y*inv,-m.a.y*inv},{-m.b.x*inv,m.a.x*inv}}; return r;
}
tmb_vec2 tmb_mat22_solve(tmb_mat22 m, tmb_vec2 rhs){
    int ok; tmb_mat22 inv=tmb_mat22_inverse(m,&ok); return tmb_mat22_apply(inv,rhs);
}
