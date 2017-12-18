/* Projet Tech "Un plateau toujours horizontal"
 *  Auteurs : Hallouard, Kitzinger, Pouzet
 *  Version du 15/12/2017
*/

/* Import des librairies */
#include <Wire.h>               // Librairie nécessaire à la lecture des valeurs de l'accéléromètre
#include <SparkFun_MMA8452Q.h>  // Librairie de l'accéléromètre
#include <Servo.h>              // Librairie des servomoteurs 

/* Déclaration des objets */
MMA8452Q accel ;    // L'accéléromètre
Servo myservo ;     // Le premier moteur du bas
Servo myservo2 ;    // Le deuxième moteur du bas
Servo myservo3 ;    // Le moteur du haut

float ax[4], ay[4], az[4] ; // Tableaux pour pouvoir faire une moyenne glissante
float mx, my, mz ;          // Moyennes en question
int i ;                     // Variant de boucle pour les moyennes


/* Fonction d'initialisation propre à tout programme Arduino */
void setup() {
  accel.init() ;        // Initialisation de l'accéléromètre
  myservo.attach(9) ;   // Déclaration des pins sur lesquels on branche les moteurs
  myservo2.attach(10) ;
  myservo3.attach(11) ;

  // On initialise les variables
  // (même si le plateau n'est pas nécessairement horizontal initialement)
  for (i = 0 ; i < 4 ; i++) ax[i] = ay[i] = az[i] = 0 ;
  i = 0 ;
}


/* Fonction main qui tourne en boucle propre à tout programme Arduino */
void loop() {
  
  // Lecture des valeurs de l'accéléromètre
  if (accel.available()){accel.read() ;}

  // Mise à jour des valeurs stockées
  ax[i] = accel.cx ;
  ay[i] = accel.cy ;
  az[i] = accel.cz ;
  i++ ;
  if ( i == 4) {i = 0 ;}

  // Calcul des moyennes
  moyennes() ;

  // Détection d'un décalage horizontal selon le premier axe
  if (my > 0.1) // Dans le premier sens
  {
    // On lance à la vitesse maximale les moteurs du bas
    // (qui tournent en sens inverse l'un de l'autre)
    myservo.write(0) ;
    myservo2.write(180) ;
    // Pendant la durée nécessaire au retour à l'équilibre
    delay(abs(asin(accel.cy))*1000/(1.13*2*3.1415)) ;
  }
  if (my < -0.1) // Dans l'autre sens
  {
    myservo.write(180) ;
    myservo2.write(0) ;
    delay(abs(asin(accel.cy))*1000/(1.13*2*3.1415)) ;
  }

  // Détection d'un décalage horizontal selon le deuxième axe
  if (mx < -0.1) // Dans le premier sens
  {
    // On lance à la vitesse maximale le moteur du haut
    myservo3.write(0) ;
    // Pendant la durée nécessaire au retour à l'équilibre
    delay(abs(asin(accel.cx))*1000/(1.13*2*3.1415)) ;
  }
  if (mx > 0.1) // Dans le deuxième sens
  {
    myservo3.write(180) ;
    delay(abs(asin(accel.cx))*1000/(1.13*2*3.1415)) ;
  }

  // Une fois qu'on est revenu à l'équilibre, on arrête les moteurs
  myservo.write(90) ; 
  myservo2.write(90) ;
  myservo3.write(90) ;
  // Pendant 200 ms pour que la solution soit stable
  delay(200) ;
}

/* Fonction qui calcule la moyenne glissante sur chacune des composantes de l'accéléromètre */
void moyennes()
{
  mx = my = mz = 0 ;
  for (int j = 0 ; j < 4 ; j++) {
    mx += ax[j] ;
    my += ay[j] ;
    mz += az[j] ;
  }
  mx /= 4 ;
  my /= 4 ;
  mz /= 4 ;
}

