clc
close all
clear all


T=[74 70 66 60 56 53 50 47 30 13 10 8];

termocupla=[3.4 3.01 2.83 2.56 2.47 2.3 2.2 2.05 1.4 0.6 0.53 0.43]./1000;
pt100=[110.4 107.5 106.6 104.6 103.3 102.5 101.3 100.1 94.4 87.7 87.3 87];

figure
grid minor on
hold on
plot(termocupla,T,'Color','r','Linewidth',3);
xlabel("Tension [mV]");
ylabel("Temperatura");

figure
grid minor on
hold on
plot(pt100,T,'Color','r','Linewidth',3);
xlabel("Resistencia [Ohm]");
ylabel("Temperatura");

p1=polyfit(termocupla,T,1);
p2=polyfit(pt100,T,1);

printf("Termocupla: T(v)=%.3f*v+%.3f\n\r",p1(1),p1(2));
printf("PT100: T(r)=%.3f*r+%.3f\n\r",p2(1),p2(2));
