clc
close all
clear all

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
dt=0.01;	%Paso de tiempo
t_final=3;	%Instante de tiempo final
t=0:dt:t_final;	%Vector de tiempos
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
kp=0.1;		%Ganancia proporcional
ki=0.1;		%Ganancia integral
kd=0.1;		%Ganancia derivativa
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

x=[zeros(1,5) ones(1,length(t)-5)];	%Escalon de entrada


err=rand*3;		%Error
err_acum=err;	%Error acumulado (integral)
err_prev=err;	%Error del paso previo (derivativo)
for i=1:1:length(t)

	y(i)=pid( err , err_acum , err_prev , kp , ki , kd );
	err_prev=err;
	err=y(i)-x(i);
	err_acum+=err;

endfor

figure
grid minor on
hold on
plot(t,y,'Linewidth',3,'Color','b');
hold on
plot(t,x,'Linewidth',3,'Color','r');
xlabel('Tiempo');
ylabel('Amplitud');
legend("Respuesta controlada","Referencia de entrada");

