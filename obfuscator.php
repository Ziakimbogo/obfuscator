<?php
/**
 * JavaScript Code Obfuscator
 *
 * Un outil puissant pour obfusquer le code JavaScript afin de le protéger
 * contre le reverse engineering et l'inspection du code source.
 */

class JsObfuscator {
    // Configuration par défaut
    private $config = [
        'rename_variables' => true,           // Renommer les variables/fonctions
        'string_encryption' => true,          // Chiffrer les chaînes de caractères
        'dead_code' => true,                  // Insérer du code mort
        'control_flow_flattening' => true,    // Modifier les structures de contrôle
        'self_defending' => true,             // Empêcher la modification du code
        'debug_protection' => true,           // Protection contre le débogage
        'function_reordering' => true,        // Réorganiser les fonctions
        'literal_movement' => true,           // Déplacer les littéraux dans des variables
        'mangle_names' => true,               // Rendre les noms incompréhensibles
        'remove_comments' => true,            // Supprimer les commentaires
        'compact_code' => true,               // Réduire la taille du code
    ];

    private $reservedWords = [
        'abstract', 'arguments', 'await', 'boolean', 'break', 'byte', 'case', 'catch', 'char',
        'class', 'const', 'continue', 'debugger', 'default', 'delete', 'do', 'double', 'else',
        'enum', 'eval', 'export', 'extends', 'false', 'final', 'finally', 'float', 'for',
        'function', 'goto', 'if', 'implements', 'import', 'in', 'instanceof', 'int', 'interface',
        'let', 'long', 'native', 'new', 'null', 'package', 'private', 'protected', 'public',
        'return', 'short', 'static', 'super', 'switch', 'synchronized', 'this', 'throw', 'throws',
        'transient', 'true', 'try', 'typeof', 'var', 'void', 'volatile', 'while', 'with', 'yield',
        'Array', 'Date', 'eval', 'function', 'hasOwnProperty', 'Infinity', 'isFinite', 'isNaN',
        'isPrototypeOf', 'length', 'Math', 'NaN', 'name', 'Number', 'Object', 'prototype', 'String',
        'toString', 'undefined', 'valueOf', 'window', 'document'
    ];

    private $identifiers = [];
    private $strings = [];
    private $stringMap = [];
    private $functionMap = [];
    private $variableMap = [];

    private $varCounter = 0;
    private $funcCounter = 0;
    private $stringCounter = 0;

    public function __construct(array $customConfig = []) {
        $this->config = array_merge($this->config, $customConfig);
    }

    public function obfuscate($js) {
        if (empty($js)) {
            return '';
        }

        // Prétraitement
        $js = $this->removeComments($js);

        $this->extractIdentifiers($js);

        $this->extractStrings($js);

        if ($this->config['rename_variables']) {
            $js = $this->renameIdentifiers($js);
        }

        if ($this->config['string_encryption']) {
            $js = $this->encryptStrings($js);
        }

        if ($this->config['literal_movement']) {
            $js = $this->moveLiterals($js);
        }

        if ($this->config['control_flow_flattening']) {
            $js = $this->flattenControlFlow($js);
        }

        if ($this->config['function_reordering']) {
            $js = $this->reorderFunctions($js);
        }

        if ($this->config['dead_code']) {
            $js = $this->insertDeadCode($js);
        }

        $js = $this->compactCode($js);

        if ($this->config['self_defending']) {
            $js = $this->addSelfDefense($js);
        }

        if ($this->config['debug_protection']) {
            $js = $this->addDebugProtection($js);
        }

        return $js;
    }

    private function removeComments($js) {
        if (!$this->config['remove_comments']) {
            return $js;
        }

        $js = preg_replace('/\/\/.*$/m', '', $js);

        $js = preg_replace('/\/\*[\s\S]*?\*\//', '', $js);

        return $js;
    }

    private function extractIdentifiers($js) {
        preg_match_all('/(?:var|let|const)\s+([a-zA-Z_$][a-zA-Z0-9_$]*)/i', $js, $varMatches);

        preg_match_all('/function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)/i', $js, $funcMatches);


        $this->identifiers = array_merge(
            isset($varMatches[1]) ? $varMatches[1] : [],
            isset($funcMatches[1]) ? $funcMatches[1] : []
        );

        $this->identifiers = array_diff($this->identifiers, $this->reservedWords);
        $this->identifiers = array_unique($this->identifiers);
    }

    private function extractStrings($js) {
        preg_match_all('/\'((?:\\\\.|[^\\\\\'])*)\'/', $js, $singleQuoteMatches);

        preg_match_all('/"((?:\\\\.|[^\\\\"])*)"/s', $js, $doubleQuoteMatches);

        $this->strings = array_merge(
            isset($singleQuoteMatches[0]) ? $singleQuoteMatches[0] : [],
            isset($doubleQuoteMatches[0]) ? $doubleQuoteMatches[0] : []
        );
    }

    private function renameIdentifiers($js) {
        if (!$this->config['mangle_names']) {
            return $js;
        }

        foreach ($this->identifiers as $id) {
            if (strlen($id) <= 2 || in_array($id, $this->reservedWords)) {
                continue;
            }

            $newName = $this->generateVarName();

            $js = preg_replace('/\b' . preg_quote($id, '/') . '\b(?!\s*:)/', $newName, $js);
        }

        return $js;
    }

    private function encryptStrings($js) {
        if (!$this->config['string_encryption']) {
            return $js;
        }

        $decoder = $this->generateVarName();
        $key = mt_rand(1, 50);

        $decoderFunction = "function $decoder(s){var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^$key);return r}";

        foreach ($this->strings as $string) {
            $content = substr($string, 1, -1);

            if (strlen($content) <= 2) {
                continue;
            }

            $encrypted = '';
            for ($i = 0; $i < strlen($content); $i++) {
                $encrypted .= '\\x' . dechex(ord($content[$i]) ^ $key);
            }

            $replacement = "$decoder(\"$encrypted\")";

            $js = str_replace($string, $replacement, $js);
        }

        $js = $decoderFunction . ";" . $js;

        return $js;
    }

    private function moveLiterals($js) {
        if (!$this->config['literal_movement']) {
            return $js;
        }

        preg_match_all('/\b(\d+)\b/', $js, $matches);

        if (isset($matches[1]) && count($matches[1]) > 0) {
            $nums = array_unique($matches[1]);
            $numVars = [];

            foreach ($nums as $num) {
                if ((int)$num < 10) {
                    continue;
                }

                $varName = $this->generateVarName();
                $numVars[$num] = $varName;
            }

            $declarations = '';
            foreach ($numVars as $num => $var) {
                $declarations .= "var $var=$num;";
            }
            if (!empty($declarations)) {
                foreach ($numVars as $num => $var) {
                    $js = preg_replace('/\b' . $num . '\b/', $var, $js);
                }

                $js = $declarations . $js;
            }
        }

        return $js;
    }

    private function flattenControlFlow($js) {
        if (!$this->config['control_flow_flattening']) {
            return $js;
        }

        preg_match_all('/if\s*\((.*?)\)\s*{(.*?)}\s*else\s*{(.*?)}/s', $js, $matches, PREG_SET_ORDER);

        foreach ($matches as $match) {
            $condition = $match[1];
            $ifBlock = $match[2];
            $elseBlock = $match[3];

            $switchVar = $this->generateVarName();
            $case1 = mt_rand(1, 100);
            $case2 = mt_rand(101, 200);

            $replacement = "var $switchVar = $condition ? $case1 : $case2; switch($switchVar) { case $case1: $ifBlock break; case $case2: $elseBlock break; }";

            $js = str_replace($match[0], $replacement, $js);
        }

        return $js;
    }

    private function reorderFunctions($js) {
        if (!$this->config['function_reordering']) {
            return $js;
        }

        preg_match_all('/function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)\s*\([^)]*\)\s*{(?:[^{}]*|(?R))*}/i', $js, $matches, PREG_SET_ORDER);

        if (count($matches) <= 1) {
            return $js;
        }

        $functions = [];
        foreach ($matches as $match) {
            $functions[] = $match[0];
            $js = str_replace($match[0], '/* FUNCTION_PLACEHOLDER */', $js, $count);
        }

        shuffle($functions);

        foreach ($functions as $function) {
            $js = preg_replace('/\/\* FUNCTION_PLACEHOLDER \*\//', $function, $js, 1);
        }

        return $js;
    }
    private function insertDeadCode($js) {
        if (!$this->config['dead_code']) {
            return $js;
        }

        $deadCodeSamples = [
            "if(false){console.log('Dead code');}",
            "var __d=Date.now();if(__d<0){throw new Error('Time travel detected');}",
            "try{if(typeof window==='undefined'&&typeof window.__z==='string'){}}catch(e){}",
            "function __zzz(){return Math.random()<0?true:false}if(__zzz()){}",
            "var __counter=0;while(__counter<0){__counter++;console.log('Never runs');}",
        ];

        preg_match_all('/[;}]/', $js, $matches, PREG_OFFSET_CAPTURE);

        if (count($matches[0]) === 0) {
            return $js;
        }

        $inserted = 0;
        $positions = $matches[0];
        shuffle($positions);

        $result = $js;
        foreach ($positions as $pos) {
            if ($inserted >= 3) break;

            $position = $pos[1] + 1;
            $deadCode = $deadCodeSamples[array_rand($deadCodeSamples)];

            $result = substr_replace($result, $deadCode, $position, 0);
            $inserted++;
        }

        return $result;
    }
    private function compactCode($js) {
        if (!$this->config['compact_code']) {
            return $js;
        }

        $js = preg_replace('/\s+/', ' ', $js);
        $js = preg_replace('/\s*([{}:;,=+\-*\/&|<>!?()[\]])\s*/', '$1', $js);
        $js = preg_replace('/\s*\n\s*/', '', $js);
        $js = preg_replace('/;}/', '}', $js);

        return $js;
    }

    private function addSelfDefense($js) {
        if (!$this->config['self_defending']) {
            return $js;
        }
        $checkFuncName = $this->generateVarName();
        $originalCode = $this->generateVarName();
        $interval = $this->generateVarName();

        $codeHash = md5($js);

        $selfDefense = <<<EOD
(function(){
    var $originalCode = '$codeHash';
    function $checkFuncName(){
        var currentCode = document.currentScript.text.replace(/\\s+/g,'');
        var hash = '';
        for(var i=0;i<currentCode.length;i++){
            hash = ((hash<<5)-hash)+currentCode.charCodeAt(i);
            hash = hash & hash;
        }
        hash = hash.toString(16);
        if(hash !== $originalCode){
            document.body.innerHTML = '';
            window.location.href = 'about:blank';
        }
    }
    var $interval = setInterval($checkFuncName, 2000);
})();
EOD;

        return $selfDefense . $js;
    }

    private function addDebugProtection($js) {
        if (!$this->config['debug_protection']) {
            return $js;
        }

        $debugProtection = <<<EOD
(function(){
    setInterval(function(){
        debugger;
    }, 100);

    var start = new Date();
    debugger;
    var end = new Date();

    if(end - start > 100){
        document.body.innerHTML = '';
        window.location.href = 'about:blank';
    }
})();
EOD;

        return $debugProtection . $js;
    }

    private function generateVarName() {
        $this->varCounter++;
        return '_' . base_convert($this->varCounter, 10, 36);
    }
}

if (PHP_SAPI === 'cli' && isset($argv) && basename($argv[0]) === basename(__FILE__)) {
    if (count($argv) < 3) {
        echo "Usage: php " . basename(__FILE__) . " input.js output.js\n";
        exit(1);
    }

    $inputFile = $argv[1];
    $outputFile = $argv[2];

    if (!file_exists($inputFile)) {
        echo "Erreur: Le fichier d'entrée n'existe pas.\n";
        exit(1);
    }

    $js = file_get_contents($inputFile);
    $obfuscator = new JsObfuscator();
    $result = $obfuscator->obfuscate($js);

    file_put_contents($outputFile, $result);
    echo "Obfuscation terminée. Résultat sauvegardé dans $outputFile\n";
}

if (!isset($argv) && PHP_SAPI !== 'cli') {
    if (isset($_POST['action']) && $_POST['action'] === 'obfuscate' && isset($_POST['js_code'])) {
        $obfuscator = new JsObfuscator();
        $result = $obfuscator->obfuscate($_POST['js_code']);

        header('Content-Type: application/javascript');
        echo $result;
        exit;
    }
}
?>