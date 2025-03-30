# Obfuscateur JavaScript (Version Node.js)

Un puissant obfuscateur de code JavaScript écrit en Node.js qui transforme votre code JavaScript pour le rendre extrêmement difficile à comprendre et à rétro-ingénierer.

## Fonctionnalités

- **Renommage de variables** : Transforme les noms de variables lisibles en identifiants cryptiques
- **Chiffrement des chaînes** : Chiffre les chaînes à l'aide de chiffrements XOR
- **Aplatissement du flux de contrôle** : Transforme les structures if-else en switch-case
- **Injection de code mort** : Ajoute du code sans signification pour perturber l'analyse
- **Code auto-défensif** : Empêche la modification du code obfusqué
- **Protection contre le débogage** : Rend le débogage presque impossible
- **Réorganisation des fonctions** : Randomise l'ordre des fonctions pour désorienter les lecteurs
- **Déplacement des littéraux** : Déplace les littéraux numériques vers des déclarations de variables

## Installation

1. Assurez-vous d'avoir Node.js installé
2. Téléchargez le fichier `obf.js`
3. Rendez-le exécutable (Linux/Mac) : `chmod +x obf.js`

## Utilisation

```bash
# Utilisation basique
node obf.js fichier.js

# Spécifier un fichier de sortie
node obf.js fichier.js sortie.js

# Exécution directe (si rendu exécutable)
./obf.js fichier.js
```

Si vous ne spécifiez pas de fichier de sortie, il en créera automatiquement un avec l'extension `.obf.js`.

## Exemple

Entrée (exemple.js) :
```javascript
function calculatePrice(basePrice, quantity, discount) {
    var tax = 0.07;
    var subtotal = basePrice * quantity;
    var discountAmount = subtotal * discount;
    var total = subtotal - discountAmount;
    var taxAmount = total * tax;
    return total + taxAmount;
}

console.log(calculatePrice(100, 5, 0.1));
```

Commande :
```bash
node obf.js exemple.js
```

La sortie (exemple.obf.js) sera un code JavaScript fortement obfusqué qui est fonctionnellement équivalent mais beaucoup plus difficile à comprendre.

## Comment ça fonctionne

1. **Prétraitement** : Supprime les commentaires et normalise le code
2. **Analyse** : Identifie les variables, fonctions et chaînes
3. **Transformation** : Applique plusieurs couches de techniques d'obfuscation
4. **Protection** : Ajoute un code d'auto-défense et anti-débogage
5. **Minification** : Compacte le code pour réduire la taille et la lisibilité

## Personnalisation

Vous pouvez modifier les paramètres de configuration dans la classe `JSObfuscator` :

```javascript
this.config = {
    renameVariables: true,      // Activer/désactiver le renommage des variables
    encryptStrings: true,       // Activer/désactiver le chiffrement des chaînes
    addDeadCode: true,          // Activer/désactiver l'injection de code mort
    flattenControl: true,       // Activer/désactiver l'aplatissement du flux de contrôle
    selfDefending: true,        // Activer/désactiver le code auto-défensif
    debugProtection: true,      // Activer/désactiver la protection contre le débogage
    reorderFunctions: true,     // Activer/désactiver la réorganisation des fonctions
    moveLiterals: true          // Activer/désactiver le déplacement des littéraux
};
```

## Compatibilité

- Fonctionne avec JavaScript ES5 et la plupart des fonctionnalités ES6
- Compatible avec tous les navigateurs modernes
- Compatible avec les environnements Node.js

## Limitations

- Ne parse/ne gère pas JSX ou TypeScript
- Un code très complexe pourrait ne pas être correctement obfusqué dans certains cas limites
- N'obfusque pas les expressions régulières
- Les fonctionnalités anti-débogage ne fonctionnent que dans les environnements de navigateur

## Avertissement

Le code obfusqué peut déclencher des avertissements dans certains outils d'analyse de code ou logiciels antivirus en raison de sa structure inhabituelle, qui peut ressembler à des modèles de code malveillant.

## Licence

Cet outil est fourni tel quel sans garantie. Utilisez-le à vos propres risques.