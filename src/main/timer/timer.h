#ifndef TIMER_H
#define TIMER_H

#include "esp_timer.h"


esp_timer_handle_t create_timer(int timer_num, int64_t interval_us, esp_timer_cb_t callback, bool continuous);
void delete_timer(esp_timer_handle_t timer_handle);

#endif /*TIMER_H*/