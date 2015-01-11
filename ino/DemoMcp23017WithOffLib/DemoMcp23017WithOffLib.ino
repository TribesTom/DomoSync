#include <Wire.h>

#include "Adafruit_MCP23017.h"

// Test de base d'ecriture d'état pour un "MCP23017 I/O expander"
// Domaine publique (code d'origine)!

// Connectez la broche #12 du MCP23017 sur Arduino broche Analogique A5 (Horloge I2C, SCL)
// Connectez la broche #13 du MCP23017 sur Arduino broche Analogique A4 (Données I2C, SDA)
// Connectez la broche #15, 16 et 17 du MCP23017 sur DNG (Sélection de l'adresse)
// Connectez la broche #9 du MCP23017 sur 5V (Alimentation)
// Connectez la broche #10 du MCP23017 sur GND (Masse commune)

// Sortie #0 (GPA0) est sur la Broche 21 du MCP, vous pouvez y connecter une LED 
// ou autre choses (lui même raccordé vers la masse/GND)

// Ajout MCHobby:
// - Connectez la broche #18 du MCP23017 sur 5V (désactiver la ligne Reset)
// - Activer la broche 13 pour faire clignoter la LED sur la carte Arduino au même rythme
//   (sert de témoin d'exécution et permet de détecter les erreurs de montage.
// - Utiliser un délai de pause de 1 seconde au lieu de 100ms
//
Adafruit_MCP23017 mcp;
  
void setup() {
  mcp.begin(); // Utilise l'adresse par défaut qui est 0

  // Définir la sortie 13 comme sortie
  pinMode( 13, OUTPUT );

  // Définir la sortie #0 (GPA 0, broche 21) comme sortie
  for(int i=0;i<16;i++ ) mcp.pinMode(i, OUTPUT); 
  
}
void loop() {
  
  for(int i=0;i<16;i++ ) mcp.digitalWrite(i,HIGH);
  digitalWrite(13,HIGH);
  delay(5000);
  for(int j=0;j<16;j++ ) mcp.digitalWrite(j,LOW);
  digitalWrite(13,LOW);
  delay(5000);
  }

