#!/usr/bin/env python3
import sys
import re
import random
import hashlib
import base64
import os

class JSObfuscator:
    def __init__(self):
        self.config = {
            'rename_vars': True,
            'encrypt_strings': True,
            'add_dead_code': True,
            'flatten_control': True,
            'self_defend': True,
            'debug_protect': True,
            'reorder_funcs': True,
            'move_literals': True
        }

        self.var_counter = 0
        self.reserved_words = [
            'abstract', 'arguments', 'await', 'boolean', 'break', 'byte', 'case', 'catch', 'char',
            'class', 'const', 'continue', 'debugger', 'default', 'delete', 'do', 'double', 'else',
            'enum', 'eval', 'export', 'extends', 'false', 'final', 'finally', 'float', 'for',
            'function', 'goto', 'if', 'implements', 'import', 'in', 'instanceof', 'int', 'interface',
            'let', 'long', 'native', 'new', 'null', 'package', 'private', 'protected', 'public',
            'return', 'short', 'static', 'super', 'switch', 'synchronized', 'this', 'throw', 'throws',
            'transient', 'true', 'try', 'typeof', 'var', 'void', 'volatile', 'while', 'with', 'yield'
        ]
        self.identifiers = []
        self.strings = []

    def gen_var_name(self):
        self.var_counter += 1
        return '_' + base64.b32encode(str(self.var_counter).encode()).decode().replace('=', '').lower()[:5]

    def remove_comments(self, js):
        js = re.sub(r'\/\/.*$', '', js, flags=re.MULTILINE)
        js = re.sub(r'\/\*[\s\S]*?\*\/', '', js)
        return js

    def extract_identifiers(self, js):
        var_matches = re.findall(r'(?:var|let|const)\s+([a-zA-Z_$][a-zA-Z0-9_$]*)', js)
        func_matches = re.findall(r'function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)', js)

        self.identifiers = list(set(var_matches + func_matches))
        self.identifiers = [id for id in self.identifiers if id not in self.reserved_words]

    def extract_strings(self, js):
        single_quotes = re.findall(r'\'((?:\\\\.|[^\\\\\'])*)\'', js)
        double_quotes = re.findall(r'"((?:\\\\.|[^\\\\"])*)"', js)

        self.strings = single_quotes + double_quotes

    def rename_identifiers(self, js):
        for id in self.identifiers:
            if len(id) <= 2 or id in self.reserved_words:
                continue

            new_name = self.gen_var_name()
            js = re.sub(r'\b' + re.escape(id) + r'\b(?!\s*:)', new_name, js)

        return js

    def encrypt_strings(self, js):
        if not self.config['encrypt_strings']:
            return js

        decoder = self.gen_var_name()
        key = random.randint(1, 50)

        decoder_func = f"function {decoder}(s){{var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^{key});return r}}"

        for string in self.strings:
            if len(string) <= 2:
                continue

            encrypted = ''
            for char in string:
                encrypted += '\\x' + format(ord(char) ^ key, '02x')

            js = js.replace(f"'{string}'", f"{decoder}('{encrypted}')")
            js = js.replace(f'"{string}"', f'{decoder}("{encrypted}")')

        return decoder_func + ";" + js

    def move_literals(self, js):
        if not self.config['move_literals']:
            return js

        num_matches = re.findall(r'\b(\d+)\b', js)
        nums = list(set(num_matches))
        num_vars = {}

        for num in nums:
            if int(num) < 10:
                continue

            var_name = self.gen_var_name()
            num_vars[num] = var_name

        declarations = ''
        for num, var in num_vars.items():
            declarations += f"var {var}={num};"
            js = re.sub(r'\b' + num + r'\b', var, js)

        return declarations + js

    def flatten_control_flow(self, js):
        if not self.config['flatten_control']:
            return js

        patterns = re.finditer(r'if\s*\((.*?)\)\s*{(.*?)}\s*else\s*{(.*?)}', js, re.DOTALL)

        for match in patterns:
            condition = match.group(1)
            if_block = match.group(2)
            else_block = match.group(3)

            switch_var = self.gen_var_name()
            case1 = random.randint(1, 100)
            case2 = random.randint(101, 200)

            replacement = f"var {switch_var}={condition}?{case1}:{case2};switch({switch_var}){{case {case1}:{if_block}break;case {case2}:{else_block}break;}}"
            js = js.replace(match.group(0), replacement)

        return js

    def reorder_functions(self, js):
        if not self.config['reorder_funcs']:
            return js

        func_matches = re.finditer(r'function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)\s*\([^)]*\)\s*{(?:[^{}]*|(?R))*}', js, re.DOTALL)
        functions = [match.group(0) for match in func_matches]

        if len(functions) <= 1:
            return js

        for func in functions:
            js = js.replace(func, '/* FUNCTION_PLACEHOLDER */', 1)

        random.shuffle(functions)

        for func in functions:
            js = js.replace('/* FUNCTION_PLACEHOLDER */', func, 1)

        return js

    def add_dead_code(self, js):
        if not self.config['add_dead_code']:
            return js

        dead_code = [
            "if(false){console.log('x');}",
            "var __d=Date.now();if(__d<0){}",
            "try{if(typeof window==='undefined'&&window.__z==='x'){}}catch(e){}",
            "function __x(){return Math.random()<0}if(__x()){}",
            "var __c=0;while(__c<0){__c++;}"
        ]

        positions = [m.start() for m in re.finditer(r'[;}]', js)]

        if not positions:
            return js

        result = js
        inserted = 0
        random.shuffle(positions)

        for pos in positions[:3]:
            code = random.choice(dead_code)
            result = result[:pos+1] + code + result[pos+1:]
            inserted += 1

        return result

    def compact_code(self, js):
        js = re.sub(r'\s+', ' ', js)
        js = re.sub(r'\s*([{}:;,=+\-*\/&|<>!?()[\]])\s*', r'\1', js)
        js = re.sub(r';}', '}', js)
        return js

    def add_self_defense(self, js):
        if not self.config['self_defend']:
            return js

        check_func = self.gen_var_name()
        original_code = self.gen_var_name()
        interval = self.gen_var_name()

        code_hash = hashlib.md5(js.encode()).hexdigest()

        defense = f"""
(function(){{
var {original_code}='{code_hash}';
function {check_func}(){{
var c=document.currentScript.text.replace(/\\s+/g,'');
var h='';
for(var i=0;i<c.length;i++){{
h=((h<<5)-h)+c.charCodeAt(i);
h=h&h;
}}
h=h.toString(16);
if(h!=={original_code}){{
document.body.innerHTML='';
window.location='about:blank';
}}
}}
var {interval}=setInterval({check_func},2000);
}})();
"""
        return defense + js

    def add_debug_protection(self, js):
        if not self.config['debug_protect']:
            return js

        protection = """
(function(){
setInterval(function(){
debugger;
},100);
var s=new Date();
debugger;
var e=new Date();
if(e-s>100){
document.body.innerHTML='';
window.location='about:blank';
}
})();
"""
        return protection + js

    def obfuscate(self, js):
        js = self.remove_comments(js)

        self.extract_identifiers(js)
        self.extract_strings(js)

        if self.config['rename_vars']:
            js = self.rename_identifiers(js)

        if self.config['encrypt_strings']:
            js = self.encrypt_strings(js)

        if self.config['move_literals']:
            js = self.move_literals(js)

        if self.config['flatten_control']:
            js = self.flatten_control_flow(js)

        if self.config['reorder_funcs']:
            js = self.reorder_functions(js)

        if self.config['add_dead_code']:
            js = self.add_dead_code(js)

        js = self.compact_code(js)

        if self.config['self_defend']:
            js = self.add_self_defense(js)

        if self.config['debug_protect']:
            js = self.add_debug_protection(js)

        return js

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} input.js [output.js]")
        sys.exit(1)

    input_file = sys.argv[1]

    if len(sys.argv) >= 3:
        output_file = sys.argv[2]
    else:
        base_name = os.path.splitext(input_file)[0]
        output_file = f"{base_name}.obf.js"

    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found")
        sys.exit(1)

    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            js_code = f.read()

        obfuscator = JSObfuscator()
        obfuscated = obfuscator.obfuscate(js_code)

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(obfuscated)

        print(f"Obfuscation complete. Output saved to '{output_file}'")

    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()