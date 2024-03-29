#include "tumble/material.h"
tmb_material tmb_material_wood(void){ tmb_material m={0.7f,0.3f,0.5f}; return m; }
tmb_material tmb_material_steel(void){ tmb_material m={7.8f,0.2f,0.4f}; return m; }
tmb_material tmb_material_rubber(void){ tmb_material m={1.1f,0.8f,0.9f}; return m; }
tmb_material tmb_material_ice(void){ tmb_material m={0.9f,0.1f,0.05f}; return m; }
tmb_material tmb_material_stone(void){ tmb_material m={2.6f,0.2f,0.6f}; return m; }
void tmb_body_apply_material(tmb_body *b, tmb_material m){
    b->restitution=m.restitution; b->friction=m.friction;
    if(b->type==TMB_BODY_DYNAMIC) tmb_body_set_density(b, m.density);
}
