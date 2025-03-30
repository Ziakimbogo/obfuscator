# Obfuscateur JavaScript (Version Python)

Un puissant obfuscateur de code JavaScript écrit en Python qui rend votre code JavaScript plus difficile à comprendre, à rétro-ingénierer et à voler.

## Fonctionnalités

- **Renommage de variables** : Transforme les noms de variables significatifs en identifiants courts et aléatoires
- **Chiffrement des chaînes** : Chiffre les littéraux de chaînes pour qu'ils ne puissent pas être facilement lus
- **Aplatissement du flux de contrôle** : Rend le flux du programme plus difficile à suivre
- **Injection de code mort** : Ajoute du code non pertinent pour confondre les ingénieurs en rétro-ingénierie
- **Code auto-défensif** : Empêche la modification du code obfusqué
- **Protection contre le débogage** : Rend le débogage presque impossible
- **Réorganisation des fonctions** : Randomise l'ordre des définitions de fonctions
- **Déplacement des littéraux** : Déplace les littéraux numériques vers des déclarations de variables

## Installation

Aucune installation nécessaire, assurez-vous simplement d'avoir Python 3.x installé sur votre système.

## Utilisation

```bash
# Utilisation basique
python obf.py fichier.js

# Spécifier un fichier de sortie
python obf.py fichier.js sortie.js

# Rendre exécutable (Linux/Mac)
chmod +x obf.py
./obf.py fichier.js
```

Si vous ne spécifiez pas de fichier de sortie, il en créera automatiquement un avec l'extension `.obf.js`.

## Exemple

Entrée (original.js) :
```javascript
function calculateTotal(price, quantity) {
    let discount = 0;
    if (quantity > 10) {
        discount = 0.1;
    } else if (quantity > 5) {
        discount = 0.05;
    }
    return price * quantity * (1 - discount);
}
```

Commande :
```bash
python obf.py original.js
```

Sortie (original.obf.js) :
```javascript
function _x1(s){var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^17);return r}(function(){setInterval(function(){debugger;},100);var s=new Date();debugger;var e=new Date();if(e-s>100){document.body.innerHTML='';window.location='about:blank';}})();(function(){var _x2='a5f7d21e';function _x3(){var c=document.currentScript.text.replace(/\s+/g,'');var h='';for(var i=0;i<c.length;i++){h=((h<<5)-h)+c.charCodeAt(i);h=h&h;}h=h.toString(16);if(h!==_x2){document.body.innerHTML='';window.location='about:blank';}}var _x4=setInterval(_x3,2000);})();function _x5(_x6,_x7){var _x8=0;var _x9=_x7>10?45:67;switch(_x9){case 45:_x8=_x1('\x7d\x7d');break;case 67:var _x10=_x7>5?22:89;switch(_x10){case 22:_x8=_x1('\x7d\x7c');break;case 89:break;}}return _x6*_x7*(1-_x8);}if(false){console.log('Dead code');}
```

## Comment ça fonctionne

1. Supprime les commentaires et les espaces blancs
2. Extrait les identifiants et les chaînes
3. Renomme les variables, fonctions et paramètres
4. Chiffre les littéraux de chaînes
5. Transforme les structures de contrôle
6. Injecte du code mort
7. Ajoute une protection contre le débogage
8. Ajoute des mécanismes d'auto-défense
9. Compacte le code

## Personnalisation

Vous pouvez modifier le dictionnaire `config` dans la classe `JSObfuscator` pour activer/désactiver des techniques d'obfuscation spécifiques :

```python
self.config = {
    'rename_vars': True,         # Activer/désactiver le renommage des variables
    'encrypt_strings': True,     # Activer/désactiver le chiffrement des chaînes
    'add_dead_code': True,       # Activer/désactiver l'injection de code mort
    'flatten_control': True,     # Activer/désactiver l'aplatissement du flux de contrôle
    'self_defend': True,         # Activer/désactiver le code auto-défensif
    'debug_protect': True,       # Activer/désactiver la protection contre le débogage
    'reorder_funcs': True,       # Activer/désactiver la réorganisation des fonctions
    'move_literals': True        # Activer/désactiver le déplacement des littéraux
}
```

## Limitations

- Ne gère pas toutes les syntaxes JavaScript (comme les fonctionnalités ES6+)
- Les fichiers très volumineux peuvent causer des problèmes de performance
- Certaines techniques de débogage avancées pourraient contourner les protections
- Les structures de code extrêmement complexes pourraient ne pas être correctement obfusquées

## Licence

Cet outil est fourni tel quel sans garantie. Utilisez-le à vos propres risques.