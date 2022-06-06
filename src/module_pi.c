#include <linux/module.h>	/* Requerido por todos los módulos */
#include <linux/kernel.h>	/* Definición de KERN_INFO */

MODULE_LICENSE("GPL"); 	/*  Licencia del modulo */
MODULE_DESCRIPTION("Super duper módulo para Raspberry Pi");
MODULE_AUTHOR("La liga del mal");


//Verbose definitions
#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

/* Función que se invoca cuando se carga el módulo en el kernel */
int modulo_lin_init(void){
	printk(KERN_INFO "Super duper Module LOADED\n");

	/* Devolver 0 para indicar una carga correcta del módulo */
	return 0;
}

void modulo_lin_clean(void){
	printk(KERN_INFO "Super duper Module UNLOADED\n");
}

/* Declaración de funciones init y exit */
module_init(modulo_lin_init);
module_exit(modulo_lin_clean);