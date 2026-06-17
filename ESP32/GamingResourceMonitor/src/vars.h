#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_CPU_PERCENTAGE_BAR_VALUE = 0,
    FLOW_GLOBAL_VARIABLE_CPU_PERCENTAGE_VALUE = 1,
    FLOW_GLOBAL_VARIABLE_RAM_PERCENTAGE_VALUE = 2,
    FLOW_GLOBAL_VARIABLE_RAM_PERCENTAGE_BAR_VALUE = 3,
    FLOW_GLOBAL_VARIABLE_VRAM_PERCENTAGE_VALUE = 4,
    FLOW_GLOBAL_VARIABLE_VRAM_PERCENTAGE_BAR_VALUE = 5
};

// Native global variables

extern int32_t get_var_cpu_percentage_bar_value();
extern void set_var_cpu_percentage_bar_value(int32_t value);
extern const char *get_var_cpu_percentage_value();
extern void set_var_cpu_percentage_value(const char *value);
extern const char *get_var_ram_percentage_value();
extern void set_var_ram_percentage_value(const char *value);
extern int32_t get_var_ram_percentage_bar_value();
extern void set_var_ram_percentage_bar_value(int32_t value);
extern const char *get_var_vram_percentage_value();
extern void set_var_vram_percentage_value(const char *value);
extern int32_t get_var_vram_percentage_bar_value();
extern void set_var_vram_percentage_bar_value(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/