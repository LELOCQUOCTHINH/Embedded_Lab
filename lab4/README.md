# FreeRTOS Queue Exercise: Dispatcher-Worker Pattern

## 1. Objective

This project implements a common and robust inter-task communication pattern known as the **Dispatcher-Worker** model (a form of Producer-Consumer).

The goal of the exercise is to create an application with:
* One **Reception Task** (Dispatcher) that simulates receiving, classifying, and dispatching requests.
* Multiple **Functional Tasks** (Workers) that each wait for and process only the requests relevant to them.
* **Queues** as the communication channel to decouple the tasks and buffer requests.

## 2. Design and Architecture

Instead of an inefficient design where all tasks peek at a single queue, this solution uses a more robust architecture where the dispatcher routes messages to **dedicated queues** for each worker.

### Data Structures

* **`RequestType_t` (enum):** Defines the type of request (`TASK_A_REQUEST`, `TASK_B_REQUEST`, `TASK_UNKNOWN`).
* **`AppRequest_t` (struct):** The message itself, containing the `targetTask` (type) and `requestData` (payload).

### Task & Queue Flow

1.  **`app_main`** initializes two queues: `xQueueA` and `xQueueB`.
2.  **`ReceptionTask` (Dispatcher):**
    * Runs in a loop every 2 seconds.
    * Simulates receiving a request by generating a random `AppRequest_t`.
    * **Classifies** the request (Target A, Target B, or Unknown).
    * If the target is A, it sends the message to `xQueueA`.
    * If the target is B, it sends the message to `xQueueB`.
    * If the target is Unknown, it logs an **ERROR** ("raise an error and simply ignore").
3.  **`FunctionalTaskA` (Worker):**
    * Does only one thing: **blocks** on `xQueueReceive(xQueueA, ...)` waiting for data.
    * When a message arrives, it "handles" it (simulated by logging and a short delay).
    * It *never* sees or cares about messages intended for Task B.
4.  **`FunctionalTaskB` (Worker):**
    * Identical to Task A, but blocks on `xQueueReceive(xQueueB, ...)` instead.

This design is highly efficient, as the worker tasks consume 0% CPU while waiting (due to `portMAX_DELAY`) and only wake up when there is actual work for them to do.

## 3. Implementation Details

* **Queues:** Created using `xQueueCreate()`.
* **Sending:** `ReceptionTask` uses `xQueueSend()` to place a *copy* of the `AppRequest_t` struct into the appropriate queue.
* **Receiving:** `FunctionalTaskA` and `FunctionalTaskB` use `xQueueReceive()` with `portMAX_DELAY` to wait indefinitely for a message.
* **Randomization:** Uses standard C library `rand()` and `srand()`. `srand()` is seeded once in `app_main` using `esp_timer_get_time()` to ensure a different sequence of requests on each boot.

## 4. Expected Output

When you run `idf.py monitor`, you will see a clear, sequential log showing the flow of requests. The log colors help illustrate the process:

1.  **[Yellow (W)]** The `ReceptionTask` announces a new request has been generated.
2.  **[Green (I)]** The `ReceptionTask` logs that it successfully dispatched the request to a specific task queue (e.g., "Request dispatched to Task A.").
3.  **[Green (I)]** Immediately after, the corresponding `FunctionalTask` wakes up and logs that it has received the request (e.g., "Task A: Received request!").
4.  **[Green (I)]** After its simulated work delay, the task logs "Work complete."

**If an invalid request is generated:**

1.  **[Yellow (W)]** The `ReceptionTask` announces the new request.
2.  **[Red (E)]** The `ReceptionTask` logs an ERROR, indicating the request is "Unknown" and is being ignored. The functional tasks will not wake up.