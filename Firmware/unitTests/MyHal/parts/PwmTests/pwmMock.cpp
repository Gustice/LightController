#include "TestUtils.h"
#include "iPwmInclude.h"

esp_err_t ledc_timer_config(const ledc_timer_config_t* ledc_conf) { return ESP_OK; }

esp_err_t ledc_channel_config(const ledc_channel_config_t* ledc_conf) { return ESP_OK; }

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty) { 
    pulsedOutSignal = duty;
    return ESP_OK; 
    }

esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel) { 
    return ESP_OK; 
    }