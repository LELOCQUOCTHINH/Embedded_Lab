#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h" // For esp_random()

static const char *TAG = "QueueExercise";

/* ---------------------------------------------------- */
/* 1. Define the Application Request Structure & Types  */
/* ---------------------------------------------------- */

// Define what kind of requests we can process
typedef enum {
    TASK_A_REQUEST, // A request for Functional Task A
    TASK_B_REQUEST, // A request for Functional Task B
    TASK_UNKNOWN    // An invalid request
} RequestType_t;

// This is the message structure that will be sent in the queues
typedef struct {
    RequestType_t targetTask;   // Which task is this message for?
    int           requestData;  // The actual data payload (e.g., a number)
} AppRequest_t;


/* ---------------------------------------------------- */
/* 2. Global Queue Handles                            */
/* ---------------------------------------------------- */

// We create one dedicated queue for each functional task
static QueueHandle_t xQueueA;
static QueueHandle_t xQueueB;


/* ---------------------------------------------------- */
/* 3. The Functional Tasks (The "Workers")              */
/* ---------------------------------------------------- */

/**
 * @brief Functional Task A
 * This task waits ONLY on its own queue (xQueueA) and processes requests.
 */
void FunctionalTaskA(void *pvParameters)
{
    AppRequest_t receivedRequest;

    while(1) {
        // Wait forever (portMAX_DELAY) until a message arrives on xQueueA
        if (xQueueReceive(xQueueA, &receivedRequest, portMAX_DELAY) == pdPASS) {
            
            ESP_LOGI(TAG, "Task A: Received request! Handling data: %d", receivedRequest.requestData);
            
            // Simulate doing "work" on the request
            vTaskDelay(pdMS_TO_TICKS(1000)); // Task A is fast
            ESP_LOGI(TAG, "Task A: Work complete.");
        }
    }
}

/**
 * @brief Functional Task B
 * This task waits ONLY on its own queue (xQueueB) and processes requests.
 */
void FunctionalTaskB(void *pvParameters)
{
    AppRequest_t receivedRequest;

    while(1) {
        // Wait forever (portMAX_DELAY) until a message arrives on xQueueB
        if (xQueueReceive(xQueueB, &receivedRequest, portMAX_DELAY) == pdPASS) {
            
            ESP_LOGI(TAG, "Task B: Received request! Handling data: %d", receivedRequest.requestData);
            
            // Simulate doing "work" on the request
            vTaskDelay(pdMS_TO_TICKS(5000)); // Task B is slower
            ESP_LOGI(TAG, "Task B: Work complete.");
        }
    }
}


/* ---------------------------------------------------- */
/* 4. The Reception Task (The "Dispatcher")             */
/* ---------------------------------------------------- */

/**
 * @brief Reception Task
 * This task simulates receiving requests, classifying them,
 * and dispatching them to the correct functional task queue.
 */
void ReceptionTask(void *pvParameters)
{
    int requestCounter = 0; // To make the data unique

    while(1) {
        // 1. "Receive requests from somewhere"
        // We will simulate this by randomly generating a request type
        AppRequest_t request;
        request.requestData = requestCounter++;
        
        // 2. "Classify these tasks"
        int randomTask = rand() % 3; // 0, 1, or 2
        if (randomTask == 0) {
            request.targetTask = TASK_A_REQUEST;
        } else if (randomTask == 1) {
            request.targetTask = TASK_B_REQUEST;
        } else {
            request.targetTask = TASK_UNKNOWN;
        }

        ESP_LOGW(TAG, "Reception: New request generated (Target: %d, Data: %d)", 
                 request.targetTask, request.requestData);

        // 3. "Send them to a queue" (This is the dispatch logic)
        
        if (request.targetTask == TASK_A_REQUEST) {
            // Send to Queue A
            if (xQueueSend(xQueueA, &request, pdMS_TO_TICKS(100)) != pdPASS) {
                ESP_LOGE(TAG, "Reception: FAILED to send to Queue A (it might be full).");
            } else {
                ESP_LOGI(TAG, "Reception: Request dispatched to Task A.");
            }
        } 
        else if (request.targetTask == TASK_B_REQUEST) {
            // Send to Queue B
            if (xQueueSend(xQueueB, &request, pdMS_TO_TICKS(100)) != pdPASS) {
                ESP_LOGE(TAG, "Reception: FAILED to send to Queue B (it might be full).");
            } else {
                ESP_LOGI(TAG, "Reception: Request dispatched to Task B.");
            }
        } 
        else {
            // 4. "If no functional task receives... raise an error"
            // This is our implementation of that requirement.
            ESP_LOGE(TAG, "Reception: ERROR! Unknown request type (%d). Ignoring request.", request.targetTask);
        }

        // Wait 2 seconds before generating the next request
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


/* ---------------------------------------------------- */
/* 5. Main Function (Setup)                             */
/* ---------------------------------------------------- */

void app_main(void)
{
    // Seed the random number generator
    srand(4);
    // Create the queues
    // xQueueCreate( uxQueueLength, uxItemSize );
    xQueueA = xQueueCreate(5, sizeof(AppRequest_t)); // Queue for Task A, holds 5 items
    xQueueB = xQueueCreate(5, sizeof(AppRequest_t)); // Queue for Task B, holds 5 items

    if (xQueueA == NULL || xQueueB == NULL) {
        ESP_LOGE(TAG, "Fatal Error: Failed to create queues!");
        return;
    }

    ESP_LOGI(TAG, "Queues created successfully.");

    // Create the tasks
    // Use the same priority for all tasks for this example
    xTaskCreate(ReceptionTask,   "ReceptionTask",   2048, NULL, 5, NULL);
    xTaskCreate(FunctionalTaskA, "FunctionalTaskA", 2048, NULL, 5, NULL);
    xTaskCreate(FunctionalTaskB, "FunctionalTaskB", 2048, NULL, 5, NULL);
}