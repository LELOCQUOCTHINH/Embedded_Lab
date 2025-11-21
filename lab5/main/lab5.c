/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/* ---------------------------------------------------
 * Definitions and Constants
 * --------------------------------------------------- */

/* ID definitions to distinguish between the two timers. 
   These are used as identifiers when retrieving the timer ID in the callback. */
#define TIMER_ID_AHIHI    0
#define TIMER_ID_IHAHA    1

/* Constants for the timer periods (converted to ticks). */
#define TIMER_PERIOD_AHIHI   pdMS_TO_TICKS( 2000 )
#define TIMER_PERIOD_IHAHA   pdMS_TO_TICKS( 3000 )

/* Execution limits for each timer. */
#define MAX_EXEC_COUNT_AHIHI  10
#define MAX_EXEC_COUNT_IHAHA  5

/* ---------------------------------------------------
 * Global Variables
 * --------------------------------------------------- */

/* Handles to reference the software timers. */
TimerHandle_t xTimerAhihi = NULL;
TimerHandle_t xTimerIhaha = NULL;

/* Array to track the execution count for each timer.
   Index 0 corresponds to TIMER_ID_AHIHI.
   Index 1 corresponds to TIMER_ID_IHAHA. */
static uint32_t ulExecutionCounts[2] = { 0, 0 };

/* ---------------------------------------------------
 * Callback Function Implementation
 * --------------------------------------------------- */

/**
 * @brief Shared Callback Function for multiple software timers.
 * * This function is executed in the context of the RTOS Daemon Task (Timer Service Task).
 * It retrieves the ID of the calling timer to determine which action to take.
 * * @param xTimer The handle of the timer that expired.
 */
void vSharedTimerCallback( TimerHandle_t xTimer )
{
    /* Retrieve the unique ID assigned to the timer.
       We cast the void pointer back to uint32_t to use it as an index/identifier. */
    uint32_t ulTimerID = ( uint32_t ) pvTimerGetTimerID( xTimer );

    /* ---------------------------------------------------
     * Logic for Timer 1 ("ahihi")
     * --------------------------------------------------- */
    if( ulTimerID == TIMER_ID_AHIHI )
    {
        /* Increment the specific counter for this timer */
        ulExecutionCounts[TIMER_ID_AHIHI]++;

        /* Perform the required action */
        printf( "ahihi - Count: %lu (Time: %lu ms)\n", 
                (unsigned long)ulExecutionCounts[TIMER_ID_AHIHI], 
                (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS) );

        /* Check if the limit has been reached */
        if( ulExecutionCounts[TIMER_ID_AHIHI] >= MAX_EXEC_COUNT_AHIHI )
        {
            printf( ">> Timer 'ahihi' reached limit (%d). Stopping timer.\n", MAX_EXEC_COUNT_AHIHI );
            
            /* Stop the timer. xBlockTime is set to 0 (non-blocking) 
               as we are inside a callback (Timer Service Task context). */
            xTimerStop( xTimer, 0 );
        }
    }
    /* ---------------------------------------------------
     * Logic for Timer 2 ("ihaha")
     * --------------------------------------------------- */
    else if( ulTimerID == TIMER_ID_IHAHA )
    {
        /* Increment the specific counter for this timer */
        ulExecutionCounts[TIMER_ID_IHAHA]++;

        /* Perform the required action */
        printf( "ihaha - Count: %lu (Time: %lu ms)\n", 
                (unsigned long)ulExecutionCounts[TIMER_ID_IHAHA], 
                (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS) );

        /* Check if the limit has been reached */
        if( ulExecutionCounts[TIMER_ID_IHAHA] >= MAX_EXEC_COUNT_IHAHA )
        {
            printf( ">> Timer 'ihaha' reached limit (%d). Stopping timer.\n", MAX_EXEC_COUNT_IHAHA );
            
            /* Stop the timer explicitly, transitioning it to the Dormant state. */
            xTimerStop( xTimer, 0 );
        }
    }
}

/* ---------------------------------------------------
 * Main Application Entry Point
 * --------------------------------------------------- */
void app_main( void )
{
    printf( "--- Starting Software Timer Demo ---\n" );

    /* * 1. Create the first timer ("ahihi")
     * - Period: 2000ms
     * - Auto-reload: pdTRUE (It repeats automatically until stopped)
     * - ID: TIMER_ID_AHIHI (0)
     */
    xTimerAhihi = xTimerCreate(
                    "Timer_Ahihi",          /* Text name for debugging. */
                    TIMER_PERIOD_AHIHI,        /* Timer period in ticks. */
                    pdTRUE,                 /* Auto-reload enabled. */
                    ( void * ) TIMER_ID_AHIHI, /* ID used in the callback. */
                    vSharedTimerCallback    /* The shared callback function. */
                  );

    /* * 2. Create the second timer ("ihaha")
     * - Period: 3000ms
     * - Auto-reload: pdTRUE
     * - ID: TIMER_ID_IHAHA (1)
     */
    xTimerIhaha = xTimerCreate(
                    "Timer_Ihaha",          /* Text name. */
                    TIMER_PERIOD_IHAHA,        /* Period. */
                    pdTRUE,                 /* Auto-reload enabled. */
                    ( void * ) TIMER_ID_IHAHA, /* ID used in the callback. */
                    vSharedTimerCallback    /* The shared callback function. */
                  );

    /* * 3. Start the timers
     * Check if handles were created successfully (heap memory was sufficient).
     */
    if( ( xTimerAhihi != NULL ) && ( xTimerIhaha != NULL ) )
    {
        /* Start timers with 0 block time (non-blocking). */
        xTimerStart( xTimerAhihi, 0 );
        xTimerStart( xTimerIhaha, 0 );
        
        printf( "Timers created and started successfully.\n" );
    }
    else
    {
        printf( "Error: Failed to create timers (Insufficient Heap).\n" );
    }

    /* Note: In a typical FreeRTOS application (not ESP-IDF app_main), 
       you would call vTaskStartScheduler() here. 
       In ESP-IDF, the scheduler is already running. */
}