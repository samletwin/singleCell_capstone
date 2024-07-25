#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Function prototypes
void soc_init(void);
float soc_initialize(float I0, float V0);
float soc_update(float I_k);
float soc_get(void);


#ifdef __cplusplus
} /* extern C */
#endif

#endif // SOC_H