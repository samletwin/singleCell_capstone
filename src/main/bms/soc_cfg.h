#ifndef SOC_CFG_H
#define SOC_CFG_H

//==============================================================================
// Editable settings

#define TABLE_SIZE 50
// #define DEBUG_SERIAL

// Configuration values for SOC initialization
#define SOC_CAPACITY            4.2f    // Battery capacity in Ah
#define SOC_INTERNAL_RESISTANCE 0.001f // Internal resistance in Ohms
#define SOC_VMIN                2.7f    // Minimum discharge voltage
#define SOC_LOAD                0.0f    // Load current in A
#define SOC_CELL_COUNT          1       // Number of cells

#endif // SOC_CFG_H