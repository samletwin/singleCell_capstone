#ifndef MAIN_CFG_H
#define MAIN_CFG_H

// #define USE_WEBSERVER  
#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL
#define PRINT_LN(test) Serial.println(test);
#define PRINT(test) Serial.print(test);
#else
#define PRINT_LN(test) 
#define PRINT(test) 
#endif

#endif /* MAIN_CFG_H */