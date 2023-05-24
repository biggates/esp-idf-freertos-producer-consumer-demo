#include <string.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

static const char* TAG = "demo";

/** 设定 QUEUE_ITEM_TYPE 为 int 类型 */
typedef uint32_t QUEUE_ITEM_t;

/** 多少毫秒生成一个数据 */
#define CONSUMER_INTERVAL_MS (500)
/** 多少毫秒消费一个数据 */
#define PRODUCER_INTERVAL_MS (4000)

/** 全局计数器(用于生成递增的 item) */
static int s_counter = 0;

/** 全局队列 */
static QueueHandle_t s_queue = NULL;

/**
 * @brief 生产一个数据
 *
 * @return QUEUE_ITEM_t*
 */
static QUEUE_ITEM_t* produce_item(void) {
    QUEUE_ITEM_t* p = calloc(1, sizeof(QUEUE_ITEM_t));
    *p              = (s_counter++);
    return p;
}

static char s_consumer_print_buffer[1024];
/**
 * @brief 消费若干个数据
 *
 * @param pItemArray 数组
 * @param length 数组的长度
 */
static void consumer(const QUEUE_ITEM_t* pItemArray, size_t length) {
    if (pItemArray != NULL) {
        // 将元素内容拼装成字符串
        memset(s_consumer_print_buffer, 0, sizeof(s_consumer_print_buffer));
        char* p = s_consumer_print_buffer;
        for (int i = 0; i < length; i++) {
            p += sprintf(p, "%lu,", pItemArray[i]);
        }
        if (p > s_consumer_print_buffer) {
            *(p - 1) = '\0';
        }
        ESP_LOGI(TAG, "consumer: will consume %s", s_consumer_print_buffer);
    }
}

/**
 * @brief 生产者 task, 每间隔 CONSUMER_INTERVAL_MS 生产一个数据
 */
static void producer_task(void* arg) {
    TickType_t ticks_before = xTaskGetTickCount();
    while (true) {
        // 生产一个数据
        QUEUE_ITEM_t* pItem = produce_item();
        // 入队和打印
        xQueueSend(s_queue, pItem, 0);
        ESP_LOGI(TAG, "queue size=%2u after enqueue %3lu", uxQueueMessagesWaiting(s_queue), *pItem);
        free(pItem);

        // 生成一次数据的间隔
        xTaskDelayUntil(&ticks_before, pdMS_TO_TICKS(CONSUMER_INTERVAL_MS));
    }
}

/**
 * @brief 消费者 task, 每间隔 PRODUCER_INTERVAL_MS 取一批数据
 */
static void consumer_task(void* arg) {
    // 首先先等待一下
    vTaskDelay(pdMS_TO_TICKS(PRODUCER_INTERVAL_MS));

    TickType_t    ticks_before = xTaskGetTickCount();
    // 计算每一批要取多少个数据
    const int     item_length  = PRODUCER_INTERVAL_MS / CONSUMER_INTERVAL_MS;
    QUEUE_ITEM_t* items        = calloc(1, sizeof(QUEUE_ITEM_t) * item_length);
    while (true) {
        memset(items, 0, sizeof(QUEUE_ITEM_t) * item_length);
        // 在循环中每次取 1 个元素，共取 item_length 个元素出来
        for (int i = 0; i < item_length; i++) {
            bool received = xQueueReceive(s_queue, &items[i], pdMS_TO_TICKS(CONSUMER_INTERVAL_MS));
            if (received) {
                // 如果取到了就打印一下
                ESP_LOGI(TAG, "queue size=%2u after dequeue %3lu", uxQueueMessagesWaiting(s_queue), items[i]);
            } else {
                // 如果没取到就再取一次
                i--;
            }
        }
        consumer(items, item_length);

        // 接收一次数据的间隔
        xTaskDelayUntil(&ticks_before, pdMS_TO_TICKS(PRODUCER_INTERVAL_MS));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "hello world!\n");

    // 建队列
    s_queue = xQueueCreate(10, sizeof(QUEUE_ITEM_t));

    // 分别建生产者和消费者任务
    xTaskCreate(producer_task, "producer", 2048, NULL, 5, NULL);
    xTaskCreate(consumer_task, "consumer", 2048, NULL, 5, NULL);
}
