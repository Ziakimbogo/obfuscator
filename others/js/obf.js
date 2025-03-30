#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

class JSObfuscator {
    constructor() {
        this.config = {
            renameVariables: true,
            encryptStrings: true,
            addDeadCode: true,
            flattenControl: true,
            selfDefending: true,
            debugProtection: true,
            reorderFunctions: true,
            moveLiterals: true
        };

        this.varCounter = 0;
        this.reservedWords = [
            'abstract', 'arguments', 'await', 'boolean', 'break', 'byte', 'case', 'catch', 'char',
            'class', 'const', 'continue', 'debugger', 'default', 'delete', 'do', 'double', 'else',
            'enum', 'eval', 'export', 'extends', 'false', 'final', 'finally', 'float', 'for',
            'function', 'goto', 'if', 'implements', 'import', 'in', 'instanceof', 'int', 'interface',
            'let', 'long', 'native', 'new', 'null', 'package', 'private', 'protected', 'public',
            'return', 'short', 'static', 'super', 'switch', 'synchronized', 'this', 'throw', 'throws',
            'transient', 'true', 'try', 'typeof', 'var', 'void', 'volatile', 'while', 'with', 'yield',
            'Array', 'Date', 'eval', 'function', 'hasOwnProperty', 'Infinity', 'isFinite', 'isNaN',
            'length', 'Math', 'NaN', 'name', 'Number', 'Object', 'prototype', 'String', 'undefined'
        ];

        this.identifiers = [];
        this.strings = [];
    }

    generateVarName() {
        this.varCounter++;
        return '_' + this.varCounter.toString(36);
    }

    removeComments(js) {
        return js
            .replace(/\/\/.*$/gm, '')
            .replace(/\/\*[\s\S]*?\*\//g, '');
    }

    extractIdentifiers(js) {
        const varMatches = js.match(/(?:var|let|const)\s+([a-zA-Z_$][a-zA-Z0-9_$]*)/g) || [];
        const funcMatches = js.match(/function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)/g) || [];

        const varNames = varMatches.map(match => match.replace(/(?:var|let|const)\s+/, ''));
        const funcNames = funcMatches.map(match => match.replace(/function\s+/, ''));

        this.identifiers = [...new Set([...varNames, ...funcNames])];
        this.identifiers = this.identifiers.filter(id => !this.reservedWords.includes(id));
    }

    extractStrings(js) {
        const singleQuotes = js.match(/'((?:\\.|[^\\'])*)'/g) || [];
        const doubleQuotes = js.match(/"((?:\\.|[^\\"])*)"/g) || [];

        this.strings = [...singleQuotes, ...doubleQuotes];
    }

    renameIdentifiers(js) {
        if (!this.config.renameVariables) {
            return js;
        }

        for (const id of this.identifiers) {
            if (id.length <= 2 || this.reservedWords.includes(id)) {
                continue;
            }

            const newName = this.generateVarName();
            const regex = new RegExp(`\\b${id}\\b(?!\\s*:)`, 'g');
            js = js.replace(regex, newName);
        }

        return js;
    }

    encryptStrings(js) {
        if (!this.config.encryptStrings) {
            return js;
        }

        const decoder = this.generateVarName();
        const key = Math.floor(Math.random() * 50) + 1;

        const decoderFunc = `function ${decoder}(s){var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^${key});return r}`;

        for (const string of this.strings) {
            if (string.length <= 3) continue;

            const content = string.substring(1, string.length - 1);
            if (content.length <= 2) continue;

            let encrypted = '';
            for (let i = 0; i < content.length; i++) {
                const charCode = content.charCodeAt(i) ^ key;
                encrypted += '\\x' + charCode.toString(16).padStart(2, '0');
            }

            const quote = string[0];
            js = js.replace(string, `${decoder}(${quote}${encrypted}${quote})`);
        }

        return decoderFunc + ';' + js;
    }

    moveLiterals(js) {
        if (!this.config.moveLiterals) {
            return js;
        }

        const numMatches = js.match(/\b(\d+)\b/g) || [];
        const nums = [...new Set(numMatches)];
        const numVars = {};

        for (const num of nums) {
            if (parseInt(num) < 10) continue;

            const varName = this.generateVarName();
            numVars[num] = varName;
        }

        let declarations = '';
        for (const [num, varName] of Object.entries(numVars)) {
            declarations += `var ${varName}=${num};`;
            const regex = new RegExp(`\\b${num}\\b`, 'g');
            js = js.replace(regex, varName);
        }

        return declarations + js;
    }

    flattenControlFlow(js) {
        if (!this.config.flattenControl) {
            return js;
        }

        const ifElseRegex = /if\s*\((.*?)\)\s*{(.*?)}\s*else\s*{(.*?)}/gs;
        let match;
        let result = js;

        while ((match = ifElseRegex.exec(js)) !== null) {
            const condition = match[1];
            const ifBlock = match[2];
            const elseBlock = match[3];

            const switchVar = this.generateVarName();
            const case1 = Math.floor(Math.random() * 100) + 1;
            const case2 = Math.floor(Math.random() * 100) + 101;

            const replacement = `var ${switchVar}=${condition}?${case1}:${case2};switch(${switchVar}){case ${case1}:${ifBlock}break;case ${case2}:${elseBlock}break;}`;
            result = result.replace(match[0], replacement);
        }

        return result;
    }

    reorderFunctions(js) {
        if (!this.config.reorderFunctions) {
            return js;
        }

        const funcRegex = /function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)\s*\([^)]*\)\s*{(?:[^{}]*|(?:(?!{).)*{(?:[^{}]*|(?:(?!{).)*{[^{}]*})*})*}/g;
        const functions = [];
        let match;

        while ((match = funcRegex.exec(js)) !== null) {
            functions.push(match[0]);
        }

        if (functions.length <= 1) {
            return js;
        }

        let result = js;
        for (const func of functions) {
            result = result.replace(func, '/* FUNCTION_PLACEHOLDER */');
        }

        functions.sort(() => Math.random() - 0.5);

        for (const func of functions) {
            result = result.replace('/* FUNCTION_PLACEHOLDER */', func);
        }

        return result;
    }

    addDeadCode(js) {
        if (!this.config.addDeadCode) {
            return js;
        }

        const deadCodeSamples = [
            "if(false){console.log('Dead code');}",
            "var __d=Date.now();if(__d<0){throw new Error('Time travel detected');}",
            "try{if(typeof window==='undefined'&&typeof window.__z==='string'){}}catch(e){}",
            "function __zzz(){return Math.random()<0?true:false}if(__zzz()){}",
            "var __counter=0;while(__counter<0){__counter++;console.log('Never runs');}"
        ];

        const positions = [];
        const semicolons = /[;}]/g;
        let match;

        while ((match = semicolons.exec(js)) !== null) {
            positions.push(match.index);
        }

        if (positions.length === 0) {
            return js;
        }

        let result = js;
        let offset = 0;
        const insertCount = Math.min(3, positions.length);

        for (let i = 0; i < insertCount; i++) {
            const randomIndex = Math.floor(Math.random() * positions.length);
            const position = positions[randomIndex];
            positions.splice(randomIndex, 1);

            const deadCode = deadCodeSamples[Math.floor(Math.random() * deadCodeSamples.length)];
            result = result.slice(0, position + offset + 1) + deadCode + result.slice(position + offset + 1);

            offset += deadCode.length;
        }

        return result;
    }

    compactCode(js) {
        return js
            .replace(/\s+/g, ' ')
            .replace(/\s*([{}:;,=+\-*\/&|<>!?()[\]])\s*/g, '$1')
            .replace(/\s*\n\s*/g, '')
            .replace(/;}/g, '}');
    }

    addSelfDefense(js) {
        if (!this.config.selfDefending) {
            return js;
        }

        const checkFuncName = this.generateVarName();
        const originalCode = this.generateVarName();
        const interval = this.generateVarName();

        const codeHash = crypto.createHash('md5').update(js).digest('hex');

        const selfDefense = `
(function(){
    var ${originalCode}='${codeHash}';
    function ${checkFuncName}(){
        var currentCode=document.currentScript.text.replace(/\\s+/g,'');
        var hash='';
        for(var i=0;i<currentCode.length;i++){
            hash=((hash<<5)-hash)+currentCode.charCodeAt(i);
            hash=hash&hash;
        }
        hash=hash.toString(16);
        if(hash!==${originalCode}){
            document.body.innerHTML='';
            window.location.href='about:blank';
        }
    }
    var ${interval}=setInterval(${checkFuncName},2000);
})();`;

        return selfDefense + js;
    }

    addDebugProtection(js) {
        if (!this.config.debugProtection) {
            return js;
        }

        const debugProtection = `
(function(){
    setInterval(function(){
        debugger;
    },100);

    var start=new Date();
    debugger;
    var end=new Date();

    if(end-start>100){
        document.body.innerHTML='';
        window.location.href='about:blank';
    }
})();`;

        return debugProtection + js;
    }

    obfuscate(js) {
        js = this.removeComments(js);

        this.extractIdentifiers(js);
        this.extractStrings(js);

        if (this.config.renameVariables) {
            js = this.renameIdentifiers(js);
        }

        if (this.config.encryptStrings) {
            js = this.encryptStrings(js);
        }

        if (this.config.moveLiterals) {
            js = this.moveLiterals(js);
        }

        if (this.config.flattenControl) {
            js = this.flattenControlFlow(js);
        }

        if (this.config.reorderFunctions) {
            js = this.reorderFunctions(js);
        }

        if (this.config.addDeadCode) {
            js = this.addDeadCode(js);
        }

        js = this.compactCode(js);

        if (this.config.selfDefending) {
            js = this.addSelfDefense(js);
        }

        if (this.config.debugProtection) {
            js = this.addDebugProtection(js);
        }

        return js;
    }
}

function main() {
    const args = process.argv.slice(2);

    if (args.length < 1) {
        console.log(`Usage: ${process.argv[1]} input.js [output.js]`);
        process.exit(1);
    }

    const inputFile = args[0];
    const outputFile = args[1] || path.basename(inputFile, path.extname(inputFile)) + '.obf.js';

    if (!fs.existsSync(inputFile)) {
        console.error(`Error: Input file '${inputFile}' not found`);
        process.exit(1);
    }

    try {
        const jsCode = fs.readFileSync(inputFile, 'utf8');

        const obfuscator = new JSObfuscator();
        const obfuscated = obfuscator.obfuscate(jsCode);

        fs.writeFileSync(outputFile, obfuscated, 'utf8');
        console.log(`Obfuscation complete. Output saved to '${outputFile}'`);
    } catch (err) {
        console.error(`Error: ${err.message}`);
        process.exit(1);
    }
}

main();