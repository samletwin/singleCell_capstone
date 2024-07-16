#ifndef FUELGAUGE_H_
#define FUELGAUGE_H_
#include <Arduino.h>

//==============================================================================
// Editable settings

#define TABLE_SIZE 50


//==============================================================================


#if defined(DEBUG_SERIAL) && !defined(PRINTF) 
#define PRINTF(...) { Serial.printf(__VA_ARGS__); }
#endif


class FuelGauge
{
private:
    float _Q;    // Battery capacity in Ah
    float _R;    // Battery resistance in Ohms
    float _Vmin; // Minimum discharge voltage
    float _IL;   // Load current in A

    unsigned long _t_prev;      // previous time
    float _SOC_prev;            // initial values
    unsigned long _t;           // current time

    float SOC;      // State of charge
    float TTS;      // Time to empty
    float _SOCt;    // SOC at time t

    int _cell_count = 0;
    bool _updated_table = false;

    /* ADJUSTED FOR MOLICEL 21700 >.< */
    float _SOC_TABLE[TABLE_SIZE] = {
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

    float _OCV_TABLE[TABLE_SIZE] = {
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

    float _SOCLookup(double ocv)
    {
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

    void _updateCellCount(int cell_count){
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

public:
    FuelGauge(float capacity = 4.2f, float internal_resistance = 0.001f, float Vmin = 2.7f, float load = 1.0f, int cell_count = 1)
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
        float _SOCt = _SOCLookup(OCVt);
    }

    void initialize(float I0, float V0, float *SOC0, float *TTS0)
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

    void update(float I_k, float *SOC_k, float *TTS_k)
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
    float getSOC()
    {
        return SOC;
    }

    float getTTS()
    {
        return TTS;
    }
};

#endif // FUELGAUGE_H_