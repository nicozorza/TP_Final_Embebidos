clc
close all
clear all

temp=[150 150 240 200];
time=[0 100 240 250];

figure
grid minor on
hold on
plot(time,temp,"Linewidth",3);
xlabel("Tiempo [s]");
ylabel("Temperatura [°C]");

N=4;
t=0:5:250;
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



