#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>
#include <map>
#include <vector>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

struct MethodDeclaration {
    Symbol return_type;
    std::vector<Symbol> argument_types;

    static MethodDeclaration from_method_class(method_class * method);
    bool has_same_args(MethodDeclaration & other);
    bool matches(std::vector<Symbol>& args);
    std::vector<Symbol> get_undeclared_types(SymbolTable<Symbol, Class__class> & types);
};

using MethodDeclarations_ = std::vector<MethodDeclaration>;
using MethodDeclarations = MethodDeclarations_*;

class ClassTable {
private:
  int semant_errors;
  Classes install_basic_classes(Classes classes);
  ostream& error_stream;
  SymbolTable<Symbol, Class__class> classes_table;
  SymbolTable<Symbol, Entry> symbol_table;
  SymbolTable<Symbol, MethodDeclarations_> method_table;
  
  std::map<Symbol, SymbolTable<Symbol, MethodDeclarations_>> method_tables_by_classes;
  std::map<Symbol, SymbolTable<Symbol, Entry>> symbol_tables_by_classes;

  /* Function for declaring a new class. Followed by a call to a type_check_class() function. */
  void decl_class(class__class * current_class);

  /* Perform type checking on a class__class object. First, check whether a type check is necessary by looking at the class name. 
  Then, get the features of the class by calling current_class->get_features(). On attr and method type features, perform type checking 
  by calling type_check_attr() and type_check_method() functions. Lastly, call the same function recursively on all children of current_class.
  */
  void type_check_class(class__class * current_class);

  /* Function for type checking attribute feature type. If the current attribute is an expression, type check it by calling type_check_expression() method. */
  void type_check_attr(attr_class* current_attr, class__class * current_class);

  /* Function for type checking method feature type. Sample implementation is given in semant.cc */
  void type_check_method(method_class* current_method, class__class * current_class);

  /* Function for type checking an expression. Get the expression type by calling expr->expression_type() and in a case statement call the 
  appropriate type_check function for the expression type. 
  For straight forward expression types, simply return its type. Eg.: For ExpressionType::int_const, assign final_type = Int; */
  Symbol type_check_expression(Expression expr, class__class* current_class, Symbol expected_type);

  /* Type check fucntions to be called from type_check_expression() function. */
  Symbol type_check_assign(assign_class* assign, class__class* current_class);
  Symbol type_check_static_dispatch(static_dispatch_class* static_dispatch, class__class* current_class);
  Symbol type_check_dispatch(dispatch_class* dispatch, class__class* current_class); /* Sample implementation is given in semant.cc */
  Symbol type_check_cond(cond_class* cond, class__class* current_class);
  Symbol type_check_typcase(typcase_class* typcase, class__class* current_class);
  Symbol type_check_block(block_class*  block, class__class* current_class);
  Symbol type_check_object(object_class* object, class__class* current_class);
  Symbol type_check_let(let_class* let, class__class* current_class); /* Sample implementation is given in semant.cc */
  Symbol type_check_new_(new__class* new_, class__class* current_class);
  void type_check_loop(loop_class* loop, class__class* current_class);
  void type_check_eq(eq_class* eq, class__class* current_class); /* Sample implementation is given in semant.cc */

  /* Check whether an attribute declaration is valid. Eg. Not a redefinition. Called by decl_class() function. */
  void decl_attr(attr_class* current_attr, class__class* current_class);

  /* Check whether a method declaration is valid. Called by decl_class() function.*/
  void decl_method(method_class* current_method, class__class* current_class);

  /* Establish the descendant status between two symbols. A sample implementation is given in semant.cc */
  bool is_descendant(Symbol desc, Symbol ancestor, class__class* current_class);

  Symbol handle_dispatch(
    Expression expr,
    Symbol name,
    Expressions arguments,
    Symbol dispatch_type,
    class__class* current_class);

  Symbol type_union(Symbol t1, Symbol t2, class__class* current_class);
public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);
};


#endif

