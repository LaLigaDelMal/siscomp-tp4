#include <linux/module.h>	/* Requerido por todos los módulos */
#include <linux/kernel.h>	/* Definición de KERN_INFO */
#include "../include/libGPIO.h"

MODULE_LICENSE("GPL"); 	/*  Licencia del modulo */
MODULE_DESCRIPTION("Super duper módulo para Raspberry Pi");
MODULE_AUTHOR("La liga del mal");

/* Función que se invoca cuando se carga el módulo en el kernel */
int modulo_lin_init(void){
	printk(KERN_INFO "Super duper Module LOADED\n");

	GPIOExport(PIN_24);
	
	return 0;
}

void modulo_lin_clean(void){

	GPIOUnexport(PIN_24);
	printk(KERN_INFO "Super duper Module UNLOADED\n");
}

/* Declaración de funciones init y exit */
module_init(modulo_lin_init);
module_exit(modulo_lin_clean);

