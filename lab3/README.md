# FreeRTOS Scheduler Simulation on ESP-IDF

## 1. Objective

This project demonstrates the three core scheduling algorithms of FreeRTOS by precisely controlling the kernel's configuration. The goal is to observe and understand the behavior of:

1.  **Prioritized Pre-emptive Scheduling (Without Time Slicing)**
2.  **Prioritized Pre-emptive Scheduling (With Time Slicing)**
3.  **Co-operative Scheduling**

## 2. How the Simulation Works

The simulation uses three tasks and an idle hook to compete for CPU time on Core 0.

* **`HighPrioTask` (Priority 1):** The "reporter" task that runs every 5 seconds.
* **`LowPrioTask` (Priority 0):** A "busy-loop" task that increments a counter.
* **`IdleTask` (Priority 0):** The default system idle task, monitored via a hook.

## 3. How to Run Each Scenario

To configure each scenario, you must **find and manually edit the `FreeRTOSConfig.h` file** for your project.

In this file, you will override the two key configuration constants:
* `configUSE_PREEMPTION`
* `configUSE_TIME_SLICING`

---

### Scenario 1: Pre-emptive (WITHOUT Time Slicing)

In this mode, `LowPrioTask` (Prio 0) will run and **starve** `IdleTask` (also Prio 0) because time-slicing is OFF.

**Configuration:**
1.  Open `FreeRTOSConfig.h`.
2.  Find or add the following lines to turn **OFF** time-slicing but keep preemption ON.
    ```h
    #define configUSE_PREEMPTION 1
    #define configUSE_TIME_SLICING 0
    ```
3.  Save the file and re-build the project.

---

### Scenario 2: Pre-emptive (WITH Time Slicing) - Default Mode

This is the standard, default behavior. Both preemption and time-slicing are ON. The two Prio 0 tasks will share CPU time.

**Configuration:**
1.  Open `FreeRTOSConfig.h`.
2.  Ensure the following lines are defined and set to 1:
    ```h
    #define configUSE_PREEMPTION 1
    #define configUSE_TIME_SLICING 1
    ```
3.  Save the file and re-build the project.

---

### Scenario 3: Co-operative Scheduling

In this mode, we **turn OFF preemption**. The scheduler can no longer force a task to stop.

**Configuration:**
1.  Open `FreeRTOSConfig.h`.
2.  Set `configUSE_PREEMPTION` to 0. The value of `configUSE_TIME_SLICING` no longer matters.
    ```h
    #define configUSE_PREEMPTION 0
    #define configUSE_TIME_SLICING 1 // This no longer has any effect
    ```
3.  Save the file and re-build the project.
4.  **Important:** For this scenario, you must also modify the C code. `LowPrioTask` must be a "co-operative" task, so you **must add `taskYIELD();`** inside its `while(1)` loop.
