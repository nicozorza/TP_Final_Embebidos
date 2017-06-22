clc
close all
clear all



T=0:10:300;	%Vector de temperatura
Tension=[0 0.397  0.798  1.203 1.611 2.022 2.436 2.850 3.266  3.681 4.095 4.508 4.919 5.327 5.733 6.137 6.539  6.939 7.338 7.737 8.137 8.537 8.938 9.341 9.745 10.151  10.560 10.969  11.381 11.793  12.207]./1000;	%Vector de tensiones

Tension=Tension(1:length(T));

P=polyfit(Tension,T,1);

aprox=Tension.*P(1)+P(2);
figure
grid minor on
hold on
plot(Tension, T,"Linewidth",3);
hold on
plot(Tension, aprox,"Linewidth",3,'Color','r');
title("Grafico de la termocupla");
xlabel("Diferencia de potencial [V]");
ylabel("Temperatura [°C]");
legend("Valores de tabla","Valores aproximados");

printf("Termocupla: T(v)=%.3f * v + %.3f\n\r",P(1),P(2));

T=0:10:50;	%Vector de temperatura
resistencia=[100.00 103.90 107.79 111.67 115.54 119.40 123.24 127.07 130.89 134.70 138.50 142.29 146.06 149.83 153.58 157.32 161.05 164.76 168.47 172.16 175.84 179.51 183.17 186.82 190.46 194.08 197.69 201.29 204.88 208.46 212.03];

resistencia=resistencia(1:length(T));

P=polyfit(resistencia,T,1);

aprox=resistencia.*P(1)+P(2);
figure
grid minor on
hold on
plot(resistencia, T,"Linewidth",3);
hold on
plot(resistencia, aprox,"Linewidth",3,'Color','r');
title("Grafico del PT100");
xlabel("Resistencia [ohm]");
ylabel("Temperatura [°C]");
legend("Valores de tabla","Valores aproximados");

printf("PT100: T(R)=%.3f * R + %.3f\n\r",P(1),P(2));
