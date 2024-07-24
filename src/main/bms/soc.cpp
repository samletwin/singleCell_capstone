
#include "soc.h"
#if defined(DEBUG_SERIAL) && !defined(PRINTF) 
#define PRINTF(...) { Serial.printf(__VA_ARGS__); }
#endif


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------

FuelGauge::FuelGauge(float capacity = 4.2f, float internal_resistance = 0.001f, float Vmin = 2.7f, float load = 1.0f, int cell_count = 4)
{
    this->_Q = capacity;
    this->_R = internal_resistance*float(cell_count);
    this->_Vmin = Vmin*float(cell_count);
    this->_IL = load;

    // Initialize previous values
    this->_t_prev = 0;
    this->_SOC_prev = 0.0f;

    // Update cell count
    this->_updateCellCount(cell_count);

    // Initial TTS
    float OCVt = _Vmin - _IL * _R;
    this->_SOCt = _SOCLookup(OCVt);
}

void FuelGauge::initialize(float I0, float V0, float *SOC0, float *TTS0)
{
    // Initial SOC
    float OCV0 = V0 - I0 * _R;
    *SOC0 = _SOCLookup(OCV0);

    printf("%f %f %f %f %f\n", *SOC0, OCV0, V0, I0, _R);
    float SOCd = *SOC0 - _SOCt;
    *TTS0 = SOCd * _Q / _IL;

    // Update previous values
    _t_prev = millis() / 1000; // seconds
    _SOC_prev = *SOC0;
}

void FuelGauge::update(float I_k, float *SOC_k, float *TTS_k)
{
    unsigned long curtime = millis();
    unsigned long delta_k = (curtime / 1000) - _t_prev; // seconds

    float SOC_km1 = _SOC_prev;                  // SOC at k-1
    I_k = I_k / 3600 * delta_k;                 // convert to A/s

    // SOC at k
    *SOC_k = SOC_km1 + I_k * float(delta_k) / _Q;
    // printf("D1:%f  %f  %f  %f  %d  %d\n", ( I_k * float(delta_k) / _Q), SOC_km1, *SOC_k, I_k, curtime, delta_k);

    _SOC_prev = *SOC_k;   
    _t_prev = curtime / 1000; // seconds            


    // TTS at [k]
    float SOCd = *SOC_k - _SOCt;
    *TTS_k = SOCd * _Q / _IL;

    // Update SOC
    SOC = *SOC_k;
    TTS = *TTS_k;

    // printf("D2: %f %f %f %f\n", _SOCt, SOCd, _Q, _IL);
}

float FuelGauge::getSOC()
{
    return this->SOC;
}

float FuelGauge::getTTS()
{
    return this->TTS;
}

//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------

float FuelGauge::_SOCLookup(double ocv){
    int i;
    // Find closest x values in the table (not the best approach)
    for (i = 0; i < TABLE_SIZE - 1; i++)
    {
        if (ocv >= _OCV_TABLE[i] && ocv <= _OCV_TABLE[i + 1])
        {
            break;
        }
    }
    // Perform linear interpolation
    float x0 = _OCV_TABLE[i];
    float x1 = _OCV_TABLE[i + 1];
    float y0 = _SOC_TABLE[i];
    float y1 = _SOC_TABLE[i + 1];
    float soc = y0 + (y1 - y0) * (ocv - x0) / (x1 - x0);
    return soc;
}

void FuelGauge::_updateCellCount(int cell_count){
    if(cell_count == _cell_count){
        return;
    }
    if (_updated_table == true){
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            float ocv = _OCV_TABLE[i];
            _OCV_TABLE[i] = ocv / float(_cell_count);
        }
        _R = _R / float(_cell_count);
        _Vmin = _Vmin / float(_cell_count);
    }
    for(int i = 0;i < TABLE_SIZE; i++){
        float ocv = _OCV_TABLE[i];
        _OCV_TABLE[i] = float(cell_count) * ocv;
    }
    _R = _R * float(cell_count);
    _Vmin = _Vmin * float(cell_count);
    _updated_table = true;

    _cell_count = cell_count;
}


