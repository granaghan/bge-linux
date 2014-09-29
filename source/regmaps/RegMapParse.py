
# -----------------------------------------------------------------------------
# calc.py
#
# A simple calculator with variables -- all in one file.
# -----------------------------------------------------------------------------
import re
import json
import lex
import yacc

class Parser:
   """
   Base class for a lexer/parser that has the rules defined as methods
   """
   tokens = ()
   precedence = ()

   def __init__(self, **kw):
      self.debug = kw.get('debug', 0)
      self.names = { }
      try:
         modname = os.path.split(os.path.splitext(__file__)[0])[1] + "_" + self.__class__.__name__
      except:
         modname = "parser"+"_"+self.__class__.__name__
      self.debugfile = modname + ".dbg"
      self.tabmodule = modname + "_" + "parsetab"
      #print self.debugfile, self.tabmodule

      # Build the lexer and parser
      self.lexer = lex.lex(module=self, debug=self.debug)
      self.yacc = yacc.yacc(module=self,
              debug=self.debug,
              debugfile=self.debugfile,
              tabmodule=self.tabmodule)

   def __del__(self):
      print "deleting"

   def parse(self, input):
      self.yacc.parse(input)

   def run(self):
      while 1:
         try:
            s = raw_input('RegMap > ')
         except EOFError:
            break
         if not s: continue
         yacc.parse(s)

class RegMap(Parser):
   def __init__(self, **kw):
      Parser.__init__(self)
      self.registers = []
      self.currentFieldOffset = 0
   # Compiler Contexts
   contextNone = 0
   contextRegister = 1
   contextStrings = ["none", "register"];
   currentContext = 0

   # dictionary of functions
   aliases = {}

   # Lexer
   reserved = {
     'register':'REGISTER',
     'field':'FIELD'}
   tokens = ['HEX', 'NAME', 'NUMBER', 'NEWLINE'] + reserved.values()

   # Tokens
   #t_SET    = r'set'
   #t_WRITE   = r'write'
   #t_WAIT   = r'wait'
   #t_CALL   = r'call'
   #t_END    = r'end'

   def t_HEX(self, t):
     r'0x[0-9A-Fa-f]+'
     try:
       t.value = int(t.value,16)
     except ValueError:
       print("Integer value too large %d", t.value)
       t.value = 0
     return t

   def t_NAME(self, t):
     r'[a-zA-Z_][a-zA-Z0-9_]*'
     if t.value in self.reserved:
       t.type = self.reserved[t.value]
     return t

   def t_NUMBER(self, t):
     r'\d+'
     try:
       t.value = int(t.value)
     except ValueError:
       print("Integer value too large %d", t.value)
       t.value = 0
     return t

   # Ignored characters
   t_ignore = " \t"

   def t_comment(self, t):
      r"[ ]*[#][^\n]*"  # \043 is '#'
      pass

   def t_NEWLINE(self, t):
     r'\n+'
     t.lexer.lineno += t.value.count("\n")

   def t_error(self, t):
     print("Illegal character '%s'" % t.value[0])
     t.lexer.skip(1)

   # # Parsing rules
   def requireContext(self, context):
     if self.currentContext is not context:
       print("ERROR " + self.getCompilerLocation() + ": Operation not valid outside of " + self.contextStrings[context] + " context.")
       print("Current context is " + self.contextStrings[self.currentContext] + ".")

   def getCompilerLocation(self):
     return "[L" + str(self.lexer.lineno) + ",C" + str(self.lexer.lexpos) + "]"

   def p_statement_register(self, p):
     'statement : REGISTER NAME NUMBER'
     #self.requireContext(self.contextNone)
     reg = {'name':p[2],'offset':p[3]}
     reg['fields'] = []
     self.currentFieldOffset = 0
     self.registers.append(reg)
     self.currentContext = self.contextRegister

   def p_statement_registerhex(self, p):
     'statement : REGISTER NAME HEX'
     #self.requireContext(self.contextNone)
     reg = {'name':p[2],'offset':p[3]}
     reg['fields'] = []
     self.currentFieldOffset = 0
     self.registers.append(reg)
     self.currentContext = self.contextRegister

   def p_statement_field(self, p):
     'statement : FIELD NAME NUMBER'
     self.requireContext(self.contextRegister)
     field = {'name':p[2],'width':p[3],'offset':self.currentFieldOffset}
     self.currentFieldOffset += p[3]
     self.registers[-1]['fields'].append(field)

   def p_statement_emptyline(self, p):
     'statement : '
     self.currentContext = self.contextNone

   def p_value_number(self, p):
      'value : NUMBER'
      p[0] = str(p[1])

   def p_value_hex(self, p):
      'value : HEX'
      p[0] = str(int(p[1],16))

   def p_error(self, t):
      print t
      print("Syntax error at " + self.getCompilerLocation()) # + ": '%s'" % t.value)

   def printRegisters(self):
      print json.dumps(self.registers)

import sys
if __name__ == '__main__':
   regmap = RegMap()
   filename = sys.argv[1]
   for line in open(filename):
      regmap.parse(line)
   regmap.printRegisters()