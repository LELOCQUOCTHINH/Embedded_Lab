# FreeRTOS Software Timer Demo: Shared Callback Function

## Overview

This project demonstrates the efficient use of **FreeRTOS Software Timers**. Specifically, it illustrates how to utilize a **single shared callback function** to handle multiple timer instances within an embedded system.

By assigning unique IDs to each timer, the system can differentiate between them within the shared callback context (Daemon Task). This approach allows for distinct behaviors and execution limits without the overhead of creating separate function pointers for each timer, optimizing code maintenance and memory usage.

## Features

* **Multiple Timers:** Creates two independent software timers running in parallel.
* **Shared Callback:** Uses a single `vSharedTimerCallback` function to handle logic for both timers, reducing code duplication.
* **Timer ID Management:** Demonstrates the use of `pvTimerID` and `pvTimerGetTimerID()` to identify the calling timer.
* **Execution Limiting:** Implements logic to automatically stop a timer after a specific number of executions (simulating a finite task).
* **State Transition:** Shows the transition from the *Running* state to the *Dormant* state using `xTimerStop()`.

## Technical Details

### Timer Configuration

| Timer Name | Period | Type | Max Executions | Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **Timer_Ahihi** | 2000 ms (2s) | Auto-reload | 10 | Prints "ahihi" and increments its specific counter. |
| **Timer_Ihaha** | 3000 ms (3s) | Auto-reload | 5 | Prints "ihaha" and increments its specific counter. |

### Key API Functions Used

* `xTimerCreate()`: Instantiates the timers with specific IDs (`pvTimerID`).
* `xTimerStart()`: Transitions timers from the *Dormant* to the *Running* state.
* `pvTimerGetTimerID()`: Retrieves the ID inside the callback to determine the active timer.
* `xTimerStop()`: Stops the timer when the execution limit is reached.

## Expected Output

Upon running the application, the serial monitor/console will display logs similar to the following sequence:

```text
--- Starting Software Timer Demo ---
Timers created and started successfully.
ahihi - Count: 1 (Time: 2000 ms)
ihaha - Count: 1 (Time: 3000 ms)
ahihi - Count: 2 (Time: 4000 ms)
ahihi - Count: 3 (Time: 6000 ms)
ihaha - Count: 2 (Time: 6000 ms)
...
ihaha - Count: 5 (Time: 15000 ms)
>> Timer 'ihaha' reached limit (5). Stopping timer.
...
ahihi - Count: 10 (Time: 20000 ms)
>> Timer 'ahihi' reached limit (10). Stopping timer.