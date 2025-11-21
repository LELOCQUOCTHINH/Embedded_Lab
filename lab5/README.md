# FreeRTOS Software Timer Demo: Shared Callback Function (ESP-IDF)

## Overview

This project demonstrates the efficient use of **FreeRTOS Software Timers** within the **ESP-IDF** environment. Specifically, it illustrates how to utilize a **single shared callback function** to handle multiple timer instances.

By assigning unique IDs to each timer, the system can differentiate between them within the shared callback context (Daemon Task). This approach allows for distinct behaviors and execution limits without the overhead of creating separate function pointers for each timer, optimizing code maintenance and memory usage.

## Features

* **Multiple Timers:** Creates two independent software timers running in parallel.
* **Shared Callback:** Uses a single `vSharedTimerCallback` function to handle logic for both timers.
* **Timer ID Management:** Demonstrates the use of `pvTimerID` and `pvTimerGetTimerID()` to identify the calling timer.
* **Execution Limiting:** Implements logic to automatically stop a timer after a specific number of executions.
* **State Transition:** Shows the transition from the *Running* state to the *Dormant* state using `xTimerStop()`.

## Technical Details

### Timer Logic

| Timer Name | Period | Type | Max Executions | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **Timer_Ahihi** | 2000 ms | Auto-reload | 10 | Prints "ahihi" and increments its specific counter. |
| **Timer_Ihaha** | 3000 ms | Auto-reload | 5 | Prints "ihaha" and increments its specific counter. |

### Key API Functions Used

* `xTimerCreate()`: Instantiates the timers with specific IDs.
* `xTimerStart()`: Transitions timers from the *Dormant* to the *Running* state.
* `pvTimerGetTimerID()`: Retrieves the ID inside the callback to determine the active timer.
* `xTimerStop()`: Stops the timer when the execution limit is reached.

## Configuration

You can easily adjust the timer identifiers, periods, and execution limits by modifying the macros defined at the top of the source file.

```c
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

```

## Expected Output

Upon running the application, the monitor will display logs similar to the following sequence:

```
--- Starting Software Timer Demo ---
Timers created and started successfully.
ahihi - Count: 1 (Time: 2010 ms)
ihaha - Count: 1 (Time: 3010 ms)
ahihi - Count: 2 (Time: 4010 ms)
ihaha - Count: 2 (Time: 6010 ms)
ahihi - Count: 3 (Time: 6010 ms)
ahihi - Count: 4 (Time: 8010 ms)
ihaha - Count: 3 (Time: 9010 ms)
ahihi - Count: 5 (Time: 10010 ms)
ihaha - Count: 4 (Time: 12010 ms)
ahihi - Count: 6 (Time: 12010 ms)
ahihi - Count: 7 (Time: 14010 ms)
ihaha - Count: 5 (Time: 15010 ms)
>> Timer 'ihaha' reached limit (5). Stopping timer.
ahihi - Count: 8 (Time: 16010 ms)
ahihi - Count: 9 (Time: 18010 ms)
ahihi - Count: 10 (Time: 20010 ms)
>> Timer 'ahihi' reached limit (10). Stopping timer.
```