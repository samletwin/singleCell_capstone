#include "soc.h"
#include <stdio.h>
#include <stdlib.h>
#include "soc_cfg.h"
#include <Arduino.h>

// Declare OCV and SOC tables as static constants
static const float SOC_TABLE[TABLE_SIZE] = {
    0, 0.00502512562814070, 0.0100502512562814, 0.0150753768844221, 0.0201005025125628, 
    0.0251256281407035, 0.0301507537688442, 0.0351758793969849, 0.0402010050251256, 
    0.0452261306532663, 0.0552763819095477, 0.0653266331658292, 0.0804020100502513, 
    0.0904522613065327, 0.100502512562814, 0.110552763819095, 0.125628140703518, 
    0.155778894472362, 0.170854271356784, 0.216080402010050, 0.276381909547739, 
    0.296482412060302, 0.326633165829146, 0.346733668341709, 0.402010050251256, 
    0.427135678391960, 0.452261306532663, 0.472361809045226, 0.552763819095477, 
    0.567839195979900, 0.603015075376884, 0.628140703517588, 0.653266331658292, 
    0.703517587939699, 0.723618090452261, 0.743718592964824, 0.788944723618090, 
    0.809045226130653, 0.824120603015075, 0.834170854271357, 0.849246231155779, 
    0.904522613065327, 0.924623115577890, 0.939698492462312, 0.954773869346734, 
    0.964824120603015, 0.974874371859297, 0.984924623115578, 0.994974874371859, 1
};

static const float OCV_TABLE[TABLE_SIZE] = {
    2.50606500000000, 2.70541134123973, 2.81760417353781, 2.89805620877589, 2.96025354599690, 
    3.00979099408924, 3.05139068066065, 3.08724874804176, 3.11857037279292, 3.14608817286485, 
    3.19237755852895, 3.23007561967661, 3.27939790148126, 3.30931056376016, 3.33570535786419, 
    3.35713178775930, 3.38358296362730, 3.42937088597665, 3.44628552499160, 3.49020296280636, 
    3.55959617508305, 3.57805723035749, 3.60307618371238, 3.61848295836797, 3.65746436166502, 
    3.67639094369603, 3.69723006658122, 3.71544649611300, 3.79216347015583, 3.80764014485851, 
    3.84706863447781, 3.87174019396256, 3.89304880898118, 3.92898500000000, 3.94653444180267, 
    3.96747892476240, 4.02188199772211, 4.04323329485654, 4.05677526559970, 4.06365415585129, 
    4.06967310092295, 4.08088476183897, 4.08714904857348, 4.09425789705177, 4.10503575414873, 
    4.11549669108686, 4.12973985963703, 4.14922684045340, 4.17557066349685, 4.19316500000000
};

// Global state
static struct {
    float Q;    // Battery capacity in Ah
    float R;    // Battery resistance in Ohms
    float Vmin; // Minimum discharge voltage
    float IL;   // Load current in A

    uint32_t t_prev;      // previous time
    float SOC_prev;       // initial values
    uint32_t t;           // current time

    float SOC;      // State of charge
    float SOCt;     // SOC at time t

    int cell_count;
    int updated_table;
} soc_state;

// Private function prototypes
static float SOCLookup(double ocv);
static void updateCellCount(int cell_count);

// Public Functions

void soc_init(void)
{
    soc_state.Q = SOC_CAPACITY;
    soc_state.R = SOC_INTERNAL_RESISTANCE * (float)SOC_CELL_COUNT;
    soc_state.Vmin = SOC_VMIN * (float)SOC_CELL_COUNT;
    soc_state.IL = SOC_LOAD;

    // Initialize previous values
    soc_state.t_prev = 0;
    soc_state.SOC_prev = 0.0f;

    // Update cell count
    updateCellCount(SOC_CELL_COUNT);

    // Initial SOCt
    float OCVt = soc_state.Vmin - soc_state.IL * soc_state.R;
    soc_state.SOCt = SOCLookup(OCVt);
}

float soc_initialize(float I0, float V0)
{
    // Initial SOC
    float OCV0 = V0 - I0 * soc_state.R;
    float SOC0 = SOCLookup(OCV0);

    printf("%f %f %f %f %f\n", SOC0, OCV0, V0, I0, soc_state.R);

    // Update previous values
    soc_state.t_prev = 0; // Assuming we start at time 0
    soc_state.SOC_prev = SOC0;

    return SOC0;
}

float soc_update(float I_k)
{
    uint32_t currentTime_s = millis() / 1000; // You'll need to implement a function to get current time in seconds
    uint32_t delta_k = currentTime_s - soc_state.t_prev;

    float SOC_km1 = soc_state.SOC_prev;
    I_k = I_k / 3600 * delta_k;

    // SOC at k
    float SOC_k = SOC_km1 + I_k * (float)delta_k / soc_state.Q;

    soc_state.SOC_prev = SOC_k;   
    soc_state.t_prev = currentTime_s;

    // Update SOC
    soc_state.SOC = SOC_k;

    return SOC_k;
}

float soc_get()
{
    return soc_state.SOC;
}

// Private Functions

/* Since OCV and SOC table are pre sorted, perform a binary search.*/
static float SOCLookup(double ocv){
    int left = 0;
    int right = TABLE_SIZE - 1;
    int mid;

    while (left <= right) {
        mid = left + (right - left) / 2;

        if (OCV_TABLE[mid] == ocv) {
            return SOC_TABLE[mid];
        }

        if (OCV_TABLE[mid] < ocv) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    // If we're here, we didn't find an exact match. Interpolate between the two closest values.
    int lower = (left > 0) ? left - 1 : 0;
    int upper = (left < TABLE_SIZE) ? left : TABLE_SIZE - 1;

    float x0 = OCV_TABLE[lower];
    float x1 = OCV_TABLE[upper];
    float y0 = SOC_TABLE[lower];
    float y1 = SOC_TABLE[upper];

    return y0 + (y1 - y0) * (ocv - x0) / (x1 - x0);
}

static void updateCellCount(int cell_count){
    if(cell_count == soc_state.cell_count){
        return;
    }
    if (soc_state.updated_table == 1){
        soc_state.R = soc_state.R / (float)soc_state.cell_count;
        soc_state.Vmin = soc_state.Vmin / (float)soc_state.cell_count;
    }
    soc_state.R = soc_state.R * (float)cell_count;
    soc_state.Vmin = soc_state.Vmin * (float)cell_count;
    soc_state.updated_table = 1;

    soc_state.cell_count = cell_count;
}