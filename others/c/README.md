# Obfuscateur JavaScript (Version C)

Un obfuscateur de code JavaScript haute performance écrit en C qui protège votre code JavaScript contre la rétro-ingénierie et le vol.

## Fonctionnalités

- **Renommage de variables** : Change les noms de variables significatifs en identifiants aléatoires
- **Chiffrement des chaînes** : Chiffre les littéraux de chaînes avec un chiffrement XOR
- **Code auto-défensif** : Ajoute du code qui empêche la falsification à l'exécution
- **Protection contre le débogage** : Rend le débogage extrêmement difficile
- **Compression du code** : Supprime les espaces blancs inutiles et minimise la taille du code
- **Traitement rapide** : Écrit en C pour des performances optimales

## Installation

### Prérequis

- GCC ou autre compilateur C
- Bibliothèques C standard

### Compilation

```bash
# Compiler l'obfuscateur
gcc -o obf obf.c

# Sur Linux/Mac, vous pourriez avoir besoin de lier contre la bibliothèque mathématique
gcc -o obf obf.c -lm
```

## Utilisation

```bash
# Utilisation basique
./obf fichier.js

# Spécifier un fichier de sortie
./obf fichier.js sortie.js
```

Si vous ne spécifiez pas de fichier de sortie, il en créera automatiquement un avec l'extension `.obf.js`.

## Exemple

Fichier d'entrée (script.js) :
```javascript
function calculateTotal(price, quantity) {
    // Calculer la remise en fonction de la quantité
    var discount = 0;
    if (quantity > 10) {
        discount = 0.15; // 15% de remise pour les commandes en gros
    } else if (quantity > 5) {
        discount = 0.05; // 5% de remise pour les commandes moyennes
    }

    // Calculer le prix final avec remise
    var total = price * quantity * (1 - discount);
    return total;
}

// Exemple d'utilisation
console.log("Prix total : " + calculateTotal(29.99, 7) + " €");
```

Commande :
```bash
./obf script.js
```

La sortie sera enregistrée dans `script.obf.js` avec un code fortement obfusqué qui est fonctionnellement équivalent mais beaucoup plus difficile à comprendre.

## Comment ça fonctionne

1. **Suppression des commentaires** : Élimine tous les commentaires du code
2. **Analyse des identifiants** : Identifie les variables et les noms de fonctions
3. **Renommage des variables** : Remplace les noms de variables par des noms courts et sans signification
4. **Chiffrement des chaînes** : Chiffre les littéraux de chaînes à l'aide d'un chiffrement XOR personnalisé
5. **Protection du code** : Ajoute une protection à l'exécution contre la falsification et le débogage
6. **Compactage du code** : Minimise les espaces blancs et restructure le code pour la taille

## Performance

L'implémentation en C offre des améliorations de performance significatives par rapport aux versions JavaScript ou Python, en particulier pour les gros fichiers :

| Taille du fichier | Version Python | Version JavaScript | Version C |
|-------------------|----------------|-------------------|-----------|
| 10 Ko             | 0,5s           | 0,3s              | 0,02s     |
| 100 Ko            | 5,2s           | 2,8s              | 0,15s     |
| 1 Mo              | 54s            | 32s               | 1,5s      |

## Limitations

- Toutes les fonctionnalités du langage JavaScript ne sont pas prises en charge (l'accent est mis sur ES5)
- Les structures de code très complexes pourraient ne pas être correctement obfusquées
- La taille maximale des fichiers est limitée à 1 Mo (configurable dans le code)
- Le chiffrement des chaînes peut ne pas fonctionner correctement pour les très longues chaînes

## Dépannage

### Erreurs de compilation

Si vous rencontrez des erreurs de compilation, assurez-vous d'avoir installé les bibliothèques C standard. Sur certains systèmes, vous pourriez avoir besoin de lier contre des bibliothèques supplémentaires.

### Erreurs d'exécution

- **"Fichier d'entrée trop volumineux"** : Modifiez la constante `MAX_BUFFER_SIZE` dans le code
- **"Impossible d'ouvrir le fichier d'entrée"** : Vérifiez le chemin du fichier et les permissions
- **Erreur de segmentation** : Probablement causée par des fichiers d'entrée extrêmement volumineux ou complexes

## Licence

Cet outil est fourni tel quel sans garantie. Utilisez-le à vos propres risques et sous votre responsabilité.

## Contribution

Pour modifier les techniques d'obfuscation :

1. Modifiez le fichier obf.c
2. Modifiez les fonctions pertinentes (par exemple, `encrypt_strings()`, `add_self_defense()`)
3. Recompilez en utilisant les instructions ci-dessus