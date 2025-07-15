# Irrigation system 

progetto simulato di un sistema di irrigazione, sviluppato in C, basato su FreeRTOS

Il sistema raccoglie dati da sensori virtuali di:
- temperatura
- umidità
- pioggia

La logica decide se attivare l'irrigazione sulla base delle condizioni meteo

- **SensorTask**: simula sensori di temperatura, umidità e pioggia.
- **LogicTask**: decide se attivare l'irrigazione in base ai dati.
- **RosTask**: simula la pubblicazione dello stato su ROS2.
- **FreeRTOS**: gestione dei task concorrenti.
- **CMake**: per la build del progetto.