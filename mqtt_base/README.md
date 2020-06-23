# mqtt_base

## How to run mqtt examples?

Start with compilation:

```sh
$ make
```

2. Start **mqtt_broker**.

```sh
$ ./mqtt_broker
```

3. Then, start **mqtt_subscriber** in a new terminal window.

```sh
$ ./mqtt_subscriber
```

4. At the end, run **mqtt_publisher1** or **mqtt_publisher2** or both.

```sh
$ ./mqtt_publisher1
$ ./mqtt_publisher2
```


## Examples

Example use case of the library is presented in the following files:
* [mqtt_broker.cpp](mqtt_broker.cpp) - basic broker setup.
* [mqtt_subscriber.cpp](mqtt_subscriber.cpp) - client subscribing two topics with a callback to each topic.
* [mqtt_publisher1.cpp](mqtt_publisher1.cpp) - client publishing on topic no1.
* [mqtt_publisher2.cpp](mqtt_publisher2.cpp) - client publishing on topic no2.

Enjoy ;)


