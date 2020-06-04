# MQTTproject
Basic MQTT implementation.

Implementacja uproszczonego protokołu typu MQTT - SCTP, TCP, UDP.

## Aktualności

W folderze src/sctp_mqtt pojawiła się pierwsza klasa mająca byc podstawą dla reszty działań - **BaseSCTP**.

## Wykonawcy

Antoni Jankowski
Natalia Skawińska

## Opis projektu

Poniższe zestawienie to opis typowej architektury protokołu MQTT. 

### Planowane moduły programowe

1. Broker

	Broker jest serwerem z którym łączą się klienci(Publisher i Subscriber), aby za jego pośrednictwem komunikować się.

2. Publisher

	Publikuje informacje na dany temat, za pomocą brokera odpowiedni subskrybujący są powiadamiani o nowej wiadmości. 

3. Subscriber

	Nasłuchuje komunikatów od na dany temat.

### Protokoły sieciowe

W aktualnym planie komunikacja będzie oparta o TCP, ze względu na potwierdzenia które są już częścią protokołu.
Bierzemy po uwagę również SCTP, jednak ta kwestia podlega konsultacji.

### Adresacja

Komunikacja oparta o adresację prywatną, dowolnie IPv4 lub IPv6.

Zakres portów, na których aplikacja brokera ma otwarte gniazda:
7733:7734


