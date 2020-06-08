# Protocol used for communication between clients and broker

```
msg = {
    "topic": "topic_name",
    "client_type": int(SUBSCRIBER|PUBLISHER),
    "data_type": string(int|string|float),
    "data_length": int(1314523),
    "data": "example_data"
}

```
