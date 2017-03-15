/*
* Ejercicio 2 del TP de Cola de mensajes
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <signal.h>

#define MENSAJE "DATA PARA PROCESO"
#define MQ_PATH "/MQ_TD3" 

void sig_user1(int a){
   
   write (0, "\nTerminando proceso...\n", sizeof("\nTerminando proceso...\n"));
    
   exit(0);
}

int main() {
   
   int err, leido;
   char buff[1024];   
   mqd_t mqd; 
   struct mq_attr attr, attr_rcv;
   
    signal(SIGUSR1, sig_user1);

   err = mq_unlink(MQ_PATH);

   attr.mq_msgsize = sizeof(buff);
   attr.mq_maxmsg = 5;

   mqd = mq_open(MQ_PATH, O_RDONLY | O_CREAT, 0666, &attr); 
   if (mqd < 0) {
      printf ("error en mq_open()");      
      return -1 ;}

   printf("Cola de mensajes creada\n");
   
   while(1) {
      
      err = mq_send(mqd, MENSAJE, strlen(MENSAJE)+1, 1);  //strlen nos da la longitud de una cadena

      if(err == -1){
         printf ("error en mq_send()");
         return -1;}
 
      printf("Mensaje enviado (%d)\n", err);

      sleep(3);

   }

   err = mq_close(mqd);
   if (( err < 0 )){
      printf ("error en mq_close()");
      return -1;   }

   printf("Cola de mensajes cerrada (%d)\n", err);
   
   err = mq_unlink(MQ_PATH);
   if(err == -1){
      printf ("error en mq_unlink()");
      return -1;    }

   printf("Cola de mensajes eliminada (%d)\n", err);
      

   return 0;
}