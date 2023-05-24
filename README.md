# esp-idf-freertos-producer-consumer-demo
a producer-consumer example using FreeRTOS Queue in esp-idf

## 可配置项目

* `QUEUE_ITEM_t` 是一个抽象的元素，在此项目中实际的实现是 `uint32_t`
* `CONSUMER_INTERVAL_MS` 生产者多少毫秒生产一个数据
* `PRODUCER_INTERVAL_MS` 消费者多少毫秒消费一批数据

## 主要内容

* `producer_task` 不断地调用 `producer()` 生产数据并插入到队列中
* `consumer_task` 在计算出每次取多少数据之后，就不断地从队列中取数据，并调用 `consumer()` 将他们消费掉

## 输出

当 `CONSUMER_INTERVAL_MS=500`, `PRODUCER_INTERVAL_MS=4000` 时，生产者每 500 ms 生产一个数据，消费者每 4000 ms 消费一批数据。预期为: 消费者每次消费 8 个数据。实际输出为:

```
I (318) demo: queue size= 1 after enqueue   0
I (818) demo: queue size= 2 after enqueue   1
I (1318) demo: queue size= 3 after enqueue   2
I (1818) demo: queue size= 4 after enqueue   3
I (2318) demo: queue size= 5 after enqueue   4
I (2818) demo: queue size= 6 after enqueue   5
I (3318) demo: queue size= 7 after enqueue   6
I (3818) demo: queue size= 8 after enqueue   7
I (4318) demo: queue size= 7 after dequeue   0
I (4318) demo: queue size= 6 after dequeue   1
I (4318) demo: queue size= 5 after dequeue   2
I (4318) demo: queue size= 4 after dequeue   3
I (4318) demo: queue size= 5 after enqueue   8
I (4328) demo: queue size= 4 after dequeue   4
I (4328) demo: queue size= 3 after dequeue   5
I (4338) demo: queue size= 2 after dequeue   6
I (4338) demo: queue size= 1 after dequeue   7
I (4348) demo: consumer: will consume 0,1,2,3,4,5,6,7
I (4818) demo: queue size= 2 after enqueue   9
I (5318) demo: queue size= 3 after enqueue  10
I (5818) demo: queue size= 4 after enqueue  11
I (6318) demo: queue size= 5 after enqueue  12
I (6818) demo: queue size= 6 after enqueue  13
I (7318) demo: queue size= 7 after enqueue  14
I (7818) demo: queue size= 8 after enqueue  15
I (8318) demo: queue size= 7 after dequeue   8
I (8318) demo: queue size= 6 after dequeue   9
I (8318) demo: queue size= 5 after dequeue  10
I (8318) demo: queue size= 4 after dequeue  11
I (8318) demo: queue size= 5 after enqueue  16
I (8328) demo: queue size= 4 after dequeue  12
I (8328) demo: queue size= 3 after dequeue  13
I (8338) demo: queue size= 2 after dequeue  14
I (8338) demo: queue size= 1 after dequeue  15
I (8348) demo: consumer: will consume 8,9,10,11,12,13,14,15
I (8818) demo: queue size= 2 after enqueue  17
```
