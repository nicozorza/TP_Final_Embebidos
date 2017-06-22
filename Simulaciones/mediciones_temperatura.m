clc
close all
clear all

OPAMP_GAIN=234;

medicion=[51 60 62 65 70 73 76 82 84 88 91 94 98 101 104 107 110 113 116 119 122 127 129 131 133 137 139 142 147 151 154 158 169 178];
temp=[8 10.5 11.5 13 14.5 15 16 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 42 43 46 50];
medicion=medicion.*3.3./(1024*OPAMP_GAIN);

P=polyfit(medicion,temp,1);

figure
grid minor on
hold on
plot(medicion,temp,"Linewidth",3);
hold on
plot(medicion,polyval(P,medicion),"Linewidth",3,"Color",'r');
printf("T=%f*v+%f\n\r",P(1),P(2));
legend("Curva medida","Curva aproximada");
xlabel("Tension");
ylabel("Temperatura");
