function y=pid( err , err_acum , err_prev , kp , ki , kd )
	
	P_term=kp*err;			%Termino proporcional
	I_term=ki*( err_acum+err );	%Termino integral
	D_term=kd*( err - err_prev );	%Termino derivativo
	
	y=-(P_term+I_term+D_term);		%Salida del controlador

endfunction
