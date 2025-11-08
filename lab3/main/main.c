#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "esp_log.h"
#include "esp32_perfmon.h"

/*
 * ===================================================================================
 * SCENARIO CONFIGURATION
 *
 * Uncomment the line below ONLY when testing Scenario 3 (Co-operative Scheduling).
 * This forces LowPrioTask to "co-operate" by yielding.
 *
 * For Scenarios 1 & 2 (Pre-emptive), this line MUST be commented out.
 * ===================================================================================
 */
#define SCENARIO_COOPERATIVE 1
// ===================================================================================

static const char *TAG = "SchedulerDemo";

/*
 * Global counters.
 * We use 'volatile' to ensure the compiler does not optimize away
 * reads/writes to these variables. See explanation below.
 */
volatile uint32_t g_high_prio_runs = 0;
volatile uint32_t g_low_prio_runs = 0;
volatile uint32_t g_idle_runs = 0;
volatile uint32_t idle_core0_counter = 0;
volatile uint32_t idle_core1_counter = 0;

/**
 * @brief Idle Hook for CPU 0.
 * This function is called by the FreeRTOS Idle Task (Priority 0) at CPU0
 * for counting the number execution of idle task when no other task execute.
 */
bool idle_counter_cpu0()
{
    idle_core0_counter++;
    return false; // Keep the hook running
}

/**
 * @brief Idle Hook for CPU 0.
 * This function is called by the FreeRTOS Idle Task (Priority 0) at CPU1
 * for counting the number execution of idle task when no other task execute.
 */
bool idle_counter_cpu1()
{
    idle_core1_counter++;
    return false; // Keep the hook running
}

/**
 * @brief Idle Hook for CPU 0.
 * This function is called by the FreeRTOS Idle Task (Priority 0)
 * when no other task is ready to run on CPU 0.
 */
bool my_idle_hook_cpu0()
{
    g_idle_runs++;
    return false; // Keep the hook running
}

/**
 * @brief Low Priority Task (Priority 0).
 * This task competes directly with the Idle Task.
 */
void LowPrioTask(void *pvParameters)
{
    while (1)
    {
        // This task just spins, incrementing its counter
        g_low_prio_runs++;

        // For Scenario 3, we must manually yield the CPU
        #if defined(SCENARIO_COOPERATIVE)
        taskYIELD();
        #endif

        /*
         * In Pre-emptive modes (Scenario 1 & 2), this task is a "busy-loop".
         * It never blocks or yields.
         */
    }
}

/**
 * @brief High Priority Task (Priority 1).
 *
 * This task has a higher priority. It will pre-empt the Prio 0 tasks.
 * It also acts as the "reporter", printing the stats every second.
 */
void HighPrioTask(void *pvParameters)
{
    while (1)
    {
        // 1. Reset counters to start a new 1-second measurement cycle
        g_high_prio_runs = 0;
        g_low_prio_runs = 0;
        g_idle_runs = 0;

        // 2. Mark that this task itself has run once in this cycle
        g_high_prio_runs = 1;

        ESP_LOGI(TAG, "--- [HighPrioTask REPORTING] ---");

        // 3. Block for 5 second. This yields the CPU to lower-priority tasks.
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // 4. After 5 second, this task wakes up (pre-empting others) and prints the results.
        ESP_LOGI(TAG, "Results from the last 5 second:");
        ESP_LOGI(TAG, "  High Prio (Prio 1): %lu runs", g_high_prio_runs);
        ESP_LOGI(TAG, "  Low Prio  (Prio 0): %lu runs", g_low_prio_runs);
        ESP_LOGI(TAG, "  Idle Task (Prio 0): %lu runs\n", g_idle_runs);
    }
}

void app_main(void)
{
    // esp_register_freertos_idle_hook_for_cpu(idle_counter_cpu0, 0);
    // esp_register_freertos_idle_hook_for_cpu(idle_counter_cpu1, 1);

    // while(1)
    // {
    //     vTaskDelay(5000 / portTICK_PERIOD_MS);
    //     printf("Execution number of idle task at CPU 0 is %ld \n", idle_core0_counter);
    //     printf("Execution number of idle task at CPU 1 is %ld \n", idle_core1_counter);
    //     idle_core0_counter = 0;
    //     idle_core1_counter = 0;
    // }
    printf("configUSE_TIME_SLICING: %d \n", configUSE_TIME_SLICING);
    printf("configUSE_PREEMPTION: %d \n", configUSE_PREEMPTION);
    // perfmon_start();
    // Register the idle hook for CPU 0
    esp_register_freertos_idle_hook_for_cpu(my_idle_hook_cpu0, 0);

    /*
     * Create the tasks.
     * We PIN THEM TO CORE 0. This is critical.
     * It ensures they both compete with the Idle Task of Core 0.
     * (By default, app_main might be running on Core 1).
     */
    xTaskCreatePinnedToCore(LowPrioTask, "LowPrio", 2048, NULL, tskIDLE_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(HighPrioTask, "HighPrio", 2048, NULL, 1, NULL, 0);
}