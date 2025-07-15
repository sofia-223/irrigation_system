IRRIGATION SYSTEM

progetto simulato di un sistema di irrigazione, sviluppato in C, basato su FreeRTOS

- **SensorTask**: simula sensori di temperatura, umidità e pioggia.
- **LogicTask**: decide se attivare l'irrigazione in base ai dati.
- **RosTask**: simula la pubblicazione dello stato su ROS2.
- **FreeRTOS**: gestione dei task concorrenti.
- **CMake**: per la build del progetto.