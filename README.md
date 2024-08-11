
## Proyecto de IoT para control de bomba de agua


Realice este proyecto para solucionar un problema que tenia al usar la bomba de agua de casa, en ocasiones la presión de agua disminuía y eso hacia que la bomba empezara a calentarse, demasiado. Hice un video explicando este problema y mostrando el proyecto.


[![Mira el video](https://img.youtube.com/vi/04YU2H7bteg/maxresdefault.jpg) ](https://youtu.be/04YU2H7bteg?si=JIqFpGx_y1QnmAwB)



## Características del proyecto

 

 

 - Mide la temperatura cada cierto tiempo.
 - Envía los datos recopilados a un dashboard
 - Si la temperatura alcanza cierto limite, hace sonar una alarma.
 - Si la alarma lleva un rato sonando sin que nadie apague la bomba, envía una alerta por whatsapp.

## Requisitos del proyecto

 - Un modulo ESP8266, este modulo controlara el sensor de temperatura y el buzzer, además tiene conexión Wi-Fi para enviar los datos al dashboard.
 - Un sensor de temperatura DS18B20
 - Una resistencia de 4,7 kΩ.
 - Un buzzer.

Y aquí un ejemplo de como debería ir todo armado:


<img src="https://i.imgur.com/HiL1BGB.png" alt="diagrama de conexiones" width="720"/>


**Este proyecto también utiliza la plataforma Thingsboard para el envió y muestreo de datos, y Callmebot como API para enviar los mensajes de Whatsapp**

