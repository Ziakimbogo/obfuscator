# Obfuscateur JavaScript en PHP

Un outil puissant et flexible écrit en PHP pour obfusquer votre code JavaScript, le rendant plus difficile à comprendre et à copier.

## Fonctionnalités

- **Renommage de variables** : Transforme les noms significatifs en identifiants courts et aléatoires
- **Chiffrement des chaînes** : Chiffre les chaînes de caractères avec un algorithme XOR
- **Aplatissement du flux de contrôle** : Transforme les structures if-else en switch-case
- **Injection de code mort** : Insère du code qui ne s'exécute jamais pour dérouter l'analyse
- **Protection anti-débogage** : Empêche l'utilisation des outils de développement
- **Code auto-défensif** : Détecte les tentatives de modification du code
- **Réorganisation des fonctions** : Mélange l'ordre des déclarations de fonctions
- **Déplacement des littéraux** : Remplace les nombres par des variables
- **Compactage du code** : Réduit la taille et améliore les performances

## Installation

Aucune installation nécessaire, téléchargez simplement le fichier `obfuscator.php` sur votre serveur.

## Utilisation

### En ligne de commande

```bash
# Utilisation basique
php obfuscator.php input.js output.js
```

### En tant que classe PHP dans votre code

```php
<?php
require_once 'obfuscator.php';

$js_code = file_get_contents('monscript.js');
$obfuscator = new JsObfuscator();
$obfuscated_code = $obfuscator->obfuscate($js_code);

file_put_contents('monscript.obf.js', $obfuscated_code);
?>
```

### En tant qu'API web

Créez un formulaire HTML :

```html
<form action="obfuscator.php" method="post">
    <textarea name="js_code">// Votre code JavaScript ici</textarea>
    <input type="hidden" name="action" value="obfuscate">
    <button type="submit">Obfusquer</button>
</form>
```

## Personnalisation

Vous pouvez configurer les options d'obfuscation en passant un tableau au constructeur :

```php
<?php
$config = [
    'rename_variables' => true,          // Renommer les variables et fonctions
    'string_encryption' => true,         // Chiffrer les chaînes de caractères
    'dead_code' => false,                // Désactiver l'insertion de code mort
    'control_flow_flattening' => true,   // Aplatir le flux de contrôle
    'self_defending' => true,            // Protection contre la modification du code
    'debug_protection' => true,          // Protection contre le débogage
    'function_reordering' => true,       // Réorganiser les fonctions
    'literal_movement' => true,          // Déplacer les littéraux dans des variables
    'mangle_names' => true,              // Rendre les noms incompréhensibles
    'remove_comments' => true,           // Supprimer les commentaires
    'compact_code' => true,              // Réduire la taille du code
];

$obfuscator = new JsObfuscator($config);
$obfuscated = $obfuscator->obfuscate($js_code);
?>
```

## Exemple

### Code original (input.js)

```javascript
function calculateDiscount(price, quantity) {
    // Calculer le prix avec remise
    var discount = 0;

    if (quantity > 10) {
        discount = 0.15;  // 15% de remise pour les grandes quantités
    } else if (quantity > 5) {
        discount = 0.05;  // 5% de remise pour les moyennes quantités
    }

    return price * (1 - discount);
}

// Calculer le prix total
function calculateTotal(unitPrice, quantity) {
    var discountedPrice = calculateDiscount(unitPrice, quantity);
    return discountedPrice * quantity;
}

console.log("Total pour 7 articles à 25€: " + calculateTotal(25, 7) + "€");
```

### Code obfusqué

```javascript
function _b(s){var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^14);return r}(function(){setInterval(function(){debugger;},100);var s=new Date();debugger;var e=new Date();if(e-s>100){document.body.innerHTML='';window.location.href='about:blank';}})();(function(){var _c='d947ba6bc394fd7ae13928ed5769fd9c';function _d(){var c=document.currentScript.text.replace(/\s+/g,'');var h='';for(var i=0;i<c.length;i++){h=((h<<5)-h)+c.charCodeAt(i);h=h&h;}h=h.toString(16);if(h!==_c){document.body.innerHTML='';window.location.href='about:blank';}}var _e=setInterval(_d,2000);})();var _f=15;var _g=10;var _h=5;function _i(_j,_k){var _l=0;var _m=_k>_g?23:47;switch(_m){case 23:_l=0.15;break;case 47:var _o=_k>_h?81:92;switch(_o){case 81:_l=0.05;break;case 92:break;}}return _j*(1-_l);}function _p(_q,_k){var _r=_i(_q,_k);return _r*_k;}if(false){console.log(_b('\r\x07\x06\x0c\x1c\x1f\x0f\x0b\x1c'));}console.log(_b('\x0b\x14\x17\x0b\x0f\x1f\x63\x1d\x14\x16\x05\x63\x08\x63\x0f\x05\x17\x10\x0c\x1f\x08\x15\x63\t\x63\x03\x01\x0d\x3a\x63')+_p(_f,7)+_b('\x0d'));
```

## Interface Web

Vous pouvez également créer une interface web complète pour l'obfuscateur en créant un fichier HTML qui appelle le script. Voici un exemple simple :

```html
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Obfuscateur JavaScript</title>
    <style>
        body { font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }
        textarea { width: 100%; height: 300px; }
        .container { display: flex; gap: 20px; }
        .col { flex: 1; }
        button { padding: 10px 20px; background: #4CAF50; color: white; border: none; cursor: pointer; }
    </style>
</head>
<body>
    <h1>Obfuscateur JavaScript</h1>
    <form action="obfuscator.php" method="post" target="_blank">
        <div class="container">
            <div class="col">
                <h3>Code JavaScript original</h3>
                <textarea name="js_code" placeholder="Collez votre code JavaScript ici..."></textarea>
            </div>
        </div>
        <input type="hidden" name="action" value="obfuscate">
        <button type="submit">Obfusquer</button>
    </form>
</body>
</html>
```

## Limitations

- Ne prend pas en charge tous les aspects de la syntaxe JavaScript moderne (ES6+)
- L'aplatissement du flux de contrôle est simplifié et ne fonctionne que sur des structures if-else basiques
- Les protections anti-débogage et d'auto-défense ne fonctionnent que dans les navigateurs

## Considérations de sécurité

L'obfuscation n'est pas une mesure de sécurité complète. Elle rend le code plus difficile à comprendre mais n'est pas infaillible. Pour une sécurité optimale :

1. N'incluez jamais de secrets (clés API, mots de passe) dans le code JavaScript
2. Implémentez toujours l'authentification et l'autorisation côté serveur
3. Utilisez l'obfuscation comme une couche de protection supplémentaire, pas comme seule mesure

## Licence

Ce code est fourni "tel quel", sans garantie d'aucune sorte. Vous êtes libre de l'utiliser et de le modifier selon vos besoins.