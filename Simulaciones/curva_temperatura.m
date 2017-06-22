clc
close all
clear all

temp=[20 60 120 150 165 170 180 220 180];
time=[0 30 60 90 120 150 180 210 240];

figure
grid minor on
hold on
plot(time,temp,"Linewidth",3);
xlabel("Tiempo [s]");
ylabel("Temperatura [°C]");

N=5;
t=0:5:240;
p=polyfit(time, temp,N);
figure
grid minor on
hold on
plot(t,polyval(p,t),"Linewidth",3);
xlabel("Tiempo [s]");
ylabel("Temperatura [°C]");

for i=t
	%printf(" %.2f, \r\n",25+polyval(p,i)/2);
	printf(" %.2f, \r\n",polyval(p,i));
endfor



