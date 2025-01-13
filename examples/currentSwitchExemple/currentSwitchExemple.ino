/*
  CurrentSwitch est une bibliothèque pour utiliser une sonde de courant non invasive comme preuve de fonctionnement.
  Ce code utilise un capteur SCT-013-000, avec l'interface Arduino et la bibliothèque emonLib originale :
  https://github.com/openenergymonitor/EmonLib

  Au lieu de lire le courant réel avec emonLib, ce code renvoie 'true' si un courant est détecté.
  Cela est utile comme preuve de fonctionnement et est beaucoup plus léger que la bibliothèque emonLib.

  Copyright (c) 31/07/2015
  Par Nitrof

  Ce logiciel est distribué sous la licence MIT. Voir les détails de la licence ci-dessous.
*/

#include <TimerOne.h>          // Bibliothèque pour utiliser Timer1 (période d'échantillonnage)
#include <currentSwitch.h>     // Bibliothèque pour la gestion du capteur de courant

// Définition des broches et paramètres
#define INPUT1 0              // Broche d'entrée analogique pour le capteur de courant
#define CURRENT_SCALE 100     // Échelle du capteur de courant
#define CURRENT_THRESHOLD 10  // Seuil du courant pour la détection

#define SECTOR_FREQUENCY 60   // Fréquence secteur (60 Hz dans la plupart des pays)


// Création de l'instance de CurrentSwitch pour surveiller le courant
CurrentSwitch test1(ANALOG_RESO_10, CURRENT_SCALE, CURRENT_THRESHOLD);  // Initialisation avec résolution 10 bits et seuil de courant

void setup() {
  Serial.begin(9600);  // Initialisation de la communication série

  // Initialisation de la minuterie pour la gestion de l'échantillonnage du courant
  Timer1.initialize(test1.currentSamplingPeriod(SECTOR_FREQUENCY));  // Période d'échantillonnage basée sur la fréquence secteur
  Timer1.attachInterrupt(readCurrentISR);  // Attache l'interruption pour lire les données de courant
}

void loop() {
  // Vérification de l'état du capteur et impression du résultat sur le moniteur série
  if (test1.workProof()) {  // Vérifie si le capteur détecte un courant
    Serial.println("Entrée fonctionnelle");
  } else {
    Serial.println("Entrée non fonctionnelle");
  }

  // Vérifie si le courant a été détecté (augmentation de la consommation)
  if (test1.rised()) {
    Serial.println("La charge a démarré !");
  }

  // Vérifie si le courant a cessé (diminution de la consommation)
  if (test1.dropped()) {
    Serial.println("La charge s'est arrêtée !");
  }

  // Vérification de l'état du changement de courant (montée ou descente)
  if (test1.changed() == RISE) {
    Serial.println("La charge a démarré !");
  } else if (test1.changed() == DROP) {
    Serial.println("La charge s'est arrêtée !");
  }

  // Délai de 1 seconde pour éviter un envoi trop rapide de données sur le moniteur série
  delay(1000);
}

// Fonction d'interruption pour lire les données de courant
void readCurrentISR() {
  // Lit la valeur du capteur à chaque interruption
  test1.read(analogRead(INPUT1));  // Lecture de la valeur analogique du capteur de courant
  // D'autres instances de CurrentSwitch peuvent être ajoutées pour lire plusieurs capteurs si nécessaire
}
