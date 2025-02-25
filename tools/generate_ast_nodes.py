import os
import sys
import json
import abc
from pprint import pprint
from typing import Dict, List

def indent(level: int, block:str) -> str:
    ret: str = ""
    for line in block.splitlines():
        ret += " "*level + line + "\n"
    return ret

class Generator:
    def generate(self) -> str:
        """
            Generate the output according to the spec
        """
    def generateHeader(self) -> str:
        """
            Generate the output header according to the spec
        """
class FileContentsGenerator(Generator):
    def __init__(self, spec:Dict):
        self.spec   = spec

    def generate(self):
        ret: str = ""
        ret += BannerGenerator().generate()
        ret += '\n'
        if "headers" in self.spec:
            ret += HeaderGenerator(self.spec).generate()            
            ret += "\n"*2
        if "classes" in self.spec:
            for m_cls in self.spec["classes"]:
                ret += ClassGenerator(m_cls).generate()
                ret += "\n"*2
        if "functions" in self.spec:
            for func in self.spec["functions"]:
                ret += FuncGenerator(func).generate()
                ret += "\n"*2
        return ret

    def generateHeader(self):
        ret: str = ""
        ret += BannerGenerator().generateHeader()
        ret += '\n'
        ret += "#ifndef __%s_H__\n"%(self.spec["filename"].upper())
        ret += "#define __%s_H__\n"%(self.spec["filename"].upper())
        if "headers" in self.spec:
            ret += HeaderGenerator(self.spec).generateHeader()            
            ret += "\n"*2
        if "classes" in self.spec:
            for m_cls in self.spec["classes"]:
                ret += ClassGenerator(m_cls).generateHeader()
                ret += "\n"*2
        if "functions" in self.spec:
            for func in self.spec["functions"]:
                ret += FuncGenerator(func).generateHeader()
                ret += "\n"*2
        ret += "#endif"
        return ret

class BannerGenerator(Generator):
    def generate(self):
        return ("/*\n"
                " * This file is generated by './tools/generate_ast_node.py'.\n"
                " * DO NOT EDIT MANUALLY!!, Add your spec to './tools/expr_spec.json' and it will be auto generated.\n"
                "*/")
    def generateHeader(self):
        return ("/*\n"
                " * This file is generated by './tools/generate_ast_node.py'.\n"
                " * DO NOT EDIT MANUALLY!!, Add your spec to './tools/expr_spec.json' and it will be auto generated.\n"
                "*/")

class HeaderGenerator(Generator):
    def __init__(self, spec:List):
        self.spec   = spec
    def generate(self):
        ret: str = ""
        ret += "#include <%s>\n"%(self.spec["headerlocation"].split("/")[-1]+"/"+self.spec["filename"]+".h")
        return ret
    def generateHeader(self):
        ret: str = ""
        if "headers" in self.spec: 
            for h in self.spec["headers"]:
                ret += f"#include <{h}>\n"
        return ret

class ClassGenerator(Generator):
    def __init__(self, spec:Dict):
        self.spec   = spec

    def generate(self):
        ret:str = ""
        fnSpec: Dict = {
            "name": self.spec["name"],
        }
        if "init_args" in self.spec:
            fnSpec["args"] = self.spec["init_args"]
        fnGen = FuncGenerator(fnSpec, self.spec["name"])
        ret += indent(0, fnGen.generate())
        if "functions" in self.spec:
            for fnSpec in self.spec["functions"]:
                ret += indent(0, FuncGenerator(fnSpec, self.spec["name"]).generate())
        return ret
    
    def generateHeader(self):
        ret:str = ""
        ret += "class %s"%(self.spec["name"])
        if "base" in self.spec:
            ret += ": public %s"%(self.spec["base"])
        ret += "{\n"
        ret += "public:\n"
        if "init_args" in self.spec:
            for init_arg in self.spec["init_args"]:
                ret += indent(4, "%s %s;"%(init_arg[0], init_arg[1]))

        fnSpec: Dict = {
            "name": self.spec["name"],
        }
        if "init_args" in self.spec:
            fnSpec["args"] = self.spec["init_args"]
        fnGen = FuncGenerator(fnSpec, self.spec["name"])
        ret += indent(4, fnGen.generateHeader())
        if "functions" in self.spec:
            for fnSpec in self.spec["functions"]:
                ret += indent(4, FuncGenerator(fnSpec, self.spec["name"]).generateHeader())
        ret += "};"
        return ret

class FuncGenerator(Generator):
    def __init__(self, spec:Dict, className:str=None):
        self.spec: Dict = spec
        self.className = className
    def generate(self):
        ret: str = ""
        if "virtual" in self.spec and self.spec["virtual"] == "pure":
            return ret
        if "returntype" in self.spec:
            ret += "%s "%(self.spec["returntype"])
        if self.className:
            ret += "%s::"%(self.className)
        ret += "%s("%(self.spec["name"])
        if "args" in self.spec:
            for arg in self.spec["args"]:
                ret += "%s %s,"%(arg[0], arg[1])
            ret = ret[:-1]
        ret += "){"
        body = ""
        
        if "body" in self.spec:
            body = self.spec["body"]
        elif ("returntype" not in self.spec and
               self.spec["name"][0] != "~" and
               "args" in self.spec):
            for arg in self.spec["args"]:
                body += "this->%s = %s;\n"%(arg[1], arg[1])

        if body != "":
            ret += "\n"
            ret += indent(4, body)
        ret += "}"
        return ret
    
    def generateHeader(self):
        ret: str = ""
        if "virtual" in self.spec:
            ret += "virtual "
        if "returntype" in self.spec:
            ret += "%s "%(self.spec["returntype"])
        ret += "%s("%(self.spec["name"])
        if "args" in self.spec:
            for arg in self.spec["args"]:
                ret += "%s %s,"%(arg[0], arg[1])
            ret = ret[:-1]
        ret += ")"
        if "virtual" in self.spec and self.spec["virtual"] == "pure":
            ret += " = 0"
        ret += ";"
        return ret

def generateBaseClass(base: str, ) -> str:
    ret: str = ""
    ret += "class %s {};"%(base)
    return ret

def generateSubClass(spec:str) -> str:
    indentLevel = 0
    def indent(line:str):
        return " "*indentLevel+line

    spec_list = spec.split(";")
    baseName = spec_list[0]
    className = spec_list[1]
    
    arg_list = []
    for i in spec_list[2:]:
        arg_list.append(i.split(":"))

    ret: str = indent("class %s: public %s{\n"%(className, baseName))
    indentLevel+=4
    for arg in arg_list:
        ret += indent(f"{arg[1]} {arg[0]};\n")
    ret += indent("%s("%(className))
    for arg in arg_list:
        ret += f"{arg[1]} {arg[0]},"
    ret += "){\n"
    indentLevel += 4
    for arg in arg_list:
        ret += indent(f"this->{arg[0]} = {arg[0]};\n")
    indentLevel -= 4
    ret += indent("}\n")
    ret += indent("void accept(ExprVisior* visitor){\n")
    indentLevel += 4
    ret += indent("visitor.visit(this);\n")
    indentLevel -= 4
    ret += indent("}\n")
    indentLevel -= 4
    ret += indent("};\n")
    return ret

if __name__ == "__main__":
    if(len(sys.argv) < 3):
        print(f"Usage: {sys.argv[0]} [header|body] class_spec.json")
        exit(1)
    if(sys.argv[1] is "header" or sys.argv[1] is "body"):
        print(f"Usage: {sys.argv[0]} [header|body] class_spec.json")
        print("The first argument should be: 'header' or 'body'")
        exit(1)
    if not sys.argv[2].endswith(".json"):
        print(f"Usage: {sys.argv[0]} [header|body] class_spec.json")
        print(f"class spec file, '{sys.argv[2]}' should be a json file.")
        exit(1)
    spec_file_path = os.path.dirname(__file__)+f"/{sys.argv[2]}"
    if not os.path.exists(spec_file_path):
        print(f"Doesn't exist: {spec_file_path}")
        exit(1)
    with open(spec_file_path, "r") as spec_file:
        spec = json.load(spec_file)
        fileGen = FileContentsGenerator(spec)
        if sys.argv[1] == "body":
            print(fileGen.generate())
        else:
            print(fileGen.generateHeader())
