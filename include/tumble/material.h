#ifndef TUMBLE_MATERIAL_H
#define TUMBLE_MATERIAL_H
#include "tumble/body.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Material presets: density, restitution and friction triples. */
typedef struct { float density, restitution, friction; } tmb_material;
tmb_material tmb_material_wood(void);
tmb_material tmb_material_steel(void);
tmb_material tmb_material_rubber(void);
tmb_material tmb_material_ice(void);
tmb_material tmb_material_stone(void);
void tmb_body_apply_material(tmb_body *b, tmb_material m);
#ifdef __cplusplus
}
#endif
#endif
