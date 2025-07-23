# Irrigation system 

progetto simulato di un sistema di irrigazione, sviluppato in C, basato su FreeRTOS, con integrazione MicroROS per la pubblicazione dello stato del sistema 

Il sistema raccoglie dati da sensori virtuali di:
- temperatura
- umidità
- pioggia

La logica decide se attivare l'irrigazione sulla base delle condizioni meteo

- **SensorTask**: simula sensori di temperatura, umidità e pioggia.
- **IrrigatorTask**: decide se attivare l'irrigazione in base ai dati e a soglie definite
- **RosTask**: gestisce la pubblicazione dello stato di irrigazione su un topic ROS2 tramite micro-ros, sia periodicamente, sia a seguito di eventi.
- **FreeRTOS**: gestione dei task concorrenti.
- **CMake**: per la build del progetto.