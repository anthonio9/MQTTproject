# SCTPmqtt base library

Biblioteka SCTPmqtt zawiera klasy serwera i klienta, które są podstawą do klas spełniających funkcjonalności protokołu MQTT: **broker**, **publisher**, **subscriber**.

## BaseSCTP

- BaseSCTP() - default constructor,
- BaseSCTP(int \_service) - parametrised constructor, create instance, af_family, local_port
- prepare_socket() - create socket and bind it to given port & address.
- set_options() - set socket options.
- listen() - put socket in listening mode.
- recv_sctp() - wait for incoming messages.
- send_sctp() - send message.
- echo() - continous execution of recv_sctp() followed by send_sctp().
- close() - # TODO

## ServerSCTP - TODO

- ServerSCTP()

## ClientSCTP - TODO

- ClientSCTP() - create instance, af_family, srv_address, srv_port
