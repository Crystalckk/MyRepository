import java.util.HashMap;
import java.util.*;
import textbook.LinkedBinaryTree;
import textbook.LinkedQueue;
import textbook.Position;

public class Assignment {

	/**
	 * Convert an arithmetic expression (in prefix notation), to a binary tree
	 * 
	 * Binary operators are +, -, * (i.e. addition, subtraction, multiplication)
	 * Anything else is assumed to be a variable or numeric value
	 * 
	 * Example: "+ 2 15" will be a tree with root "+", left child "2" and right
	 * child "15" i.e. + 2 15
	 * 
	 * Example: "+ 2 - 4 5" will be a tree with root "+", left child "2", right
	 * child a subtree representing "- 4 5" i.e. + 2 - 4 5
	 * 
	 * This method runs in O(n) time
	 * 
	 * @param expression
	 *            - an arithmetic expression in prefix notation
	 * @return BinaryTree representing an expression expressed in prefix
	 *         notation
	 * @throws IllegalArgumentException
	 *             if expression was not a valid expression
	 */
	public static LinkedBinaryTree<String> prefix2tree(String expression) throws IllegalArgumentException {
		if (expression == null) {
			throw new IllegalArgumentException("Expression string was null");
		}
		// break up the expression string using spaces, into a queue
		LinkedQueue<String> tokens = new LinkedQueue<String>();
		for (String token : expression.split(" ")) {
			tokens.enqueue(token);
		}
		// recursively build the tree
		return prefix2tree(tokens);
	}
	
	/**
	 * Recursive helper method to build an tree representing an arithmetic
	 * expression in prefix notation, where the expression has already been
	 * broken up into a queue of tokens
	 * 
	 * @param tokens
	 * @return
	 * @throws IllegalArgumentException
	 *             if expression was not a valid expression
	 */
	private static LinkedBinaryTree<String> prefix2tree(LinkedQueue<String> tokens) throws IllegalArgumentException {
		LinkedBinaryTree<String> tree = new LinkedBinaryTree<String>();

		// use the next element of the queue to build the root
		if (tokens.isEmpty()) {
			throw new IllegalArgumentException("String was not a valid arithmetic expression in prefix notation");
		}
		String element = tokens.dequeue();
		tree.addRoot(element);

		// if the element is a binary operation, we need to build the left and
		// right subtrees
		if (element.equals("+") || element.equals("-") || element.equals("*")) {
			LinkedBinaryTree<String> left = prefix2tree(tokens);
			LinkedBinaryTree<String> right = prefix2tree(tokens);
			tree.attach(tree.root(), left, right);
		}
		// otherwise, assume it's a variable or a value, so it's a leaf (i.e.
		// nothing more to do)

		return tree;
	}
	
	/**
	 * Test to see if two trees are identical (every position in the tree stores the same value)
	 * 
	 * e.g. two trees representing "+ 1 2" are identical to each other, but not to a tree representing "+ 2 1"
	 * @param a
	 * @param b
	 * @return true if the trees have the same structure and values, false otherwise
	 */
	public static boolean equals(LinkedBinaryTree<String> a, LinkedBinaryTree<String> b) {
		return equals(a, b, a.root(), b.root());
	}

	/**
	 * Recursive helper method to compare two trees
	 * @param aTree one of the trees to compare
	 * @param bTree the other tree to compare
	 * @param aRoot a position in the first tree
	 * @param bRoot a position in the second tree (corresponding to a position in the first)
	 * @return true if the subtrees rooted at the given positions are identical
	 */
	private static boolean equals(LinkedBinaryTree<String> aTree, LinkedBinaryTree<String> bTree, Position<String> aRoot, Position<String> bRoot) {
		//if either of the positions is null, then they are the same only if they are both null
		if(aRoot == null || bRoot == null) {
			return (aRoot == null) && (bRoot == null);
		}
		//first check that the elements stored in the current positions are the same
		String a = aRoot.getElement();
		String b = bRoot.getElement();
		if((a==null && b==null) || a.equals(b)) {
			//then recursively check if the left subtrees are the same...
			boolean left = equals(aTree, bTree, aTree.left(aRoot), bTree.left(bRoot));
			//...and if the right subtrees are the same
			boolean right = equals(aTree, bTree, aTree.right(aRoot), bTree.right(bRoot));
			//return true if they both matched
			return left && right;
		}
		else {
			return false;
		}
	}

	
	/**
	 * Given a tree, this method should output a string for the corresponding
	 * arithmetic expression in prefix notation, without (parenthesis) (also
	 * known as Polish notation)
	 * 
	 * Example: A tree with root "+", left child "2" and right child "15" would
	 * be "+ 2 15" Example: A tree with root "-", left child a subtree
	 * representing "(2+15)" and right child "4" would be "- + 2 15 4"
	 * 
	 * Ideally, this method should run in O(n) time
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @return prefix notation expression of the tree
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression
	 */
public static String tree2prefix(LinkedBinaryTree<String> tree) throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null){
			throw new IllegalArgumentException();
		}
		StringBuffer prefix = new StringBuffer();
		
		return tree2prefix(tree, tree.root(), prefix);
	}
	public static String tree2prefix(LinkedBinaryTree<String> tree, Position<String> p, StringBuffer prefix) throws IllegalArgumentException{
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		prefix.append(p.getElement());
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			prefix.append(" ");
			tree2prefix(tree, tree.left(p), prefix);
			prefix.append(" ");
			tree2prefix(tree, tree.right(p), prefix);
		}
		return prefix.toString();
	}

	/**
	 * Given a tree, this method should output a string for the corresponding
	 * arithmetic expression in infix notation with parenthesis (i.e. the most
	 * commonly used notation).
	 * 
	 * Example: A tree with root "+", left child "2" and right child "15" would
	 * be "(2+15)"
	 * 
	 * Example: A tree with root "-", left child a subtree representing "(2+15)"
	 * and right child "4" would be "((2+15)-4)"
	 * 
	 * Optionally, you may leave out the outermost parenthesis, but it's fine to
	 * leave them on. (i.e. "2+15" and "(2+15)-4" would also be acceptable
	 * output for the examples above)
	 * 
	 * Ideally, this method should run in O(n) time
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @return infix notation expression of the tree
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression
	 */
	public static String tree2infix(LinkedBinaryTree<String> tree) throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null){
			throw new IllegalArgumentException();
		}
		
		StringBuffer infix = new StringBuffer();
		return tree2infix(tree, tree.root(), infix);
	}
	
	public static String tree2infix(LinkedBinaryTree<String> tree, Position<String> p, StringBuffer infix){
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		if((tree.left(p) == null) && (tree.right(p) == null)){
			infix.append(p.getElement());
		}
		
		if(tree.left(p) != null){
			infix.append("(");
			tree2infix(tree, tree.left(p), infix);
			infix.append(p.getElement());
		}
		
		if(tree.right(p) != null){
			tree2infix(tree, tree.right(p), infix);
			infix.append(")");
		}
		return infix.toString();
	}
	
	/**
	 * Given a tree, this method should simplify any subtrees which can be
	 * evaluated to a single integer value.
	 * 
	 * Ideally, this method should run in O(n) time
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @return resulting binary tree after evaluating as many of the subtrees as
	 *         possible
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression
	 */
	public static LinkedBinaryTree<String> simplify(LinkedBinaryTree<String> tree) throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null){
			throw new IllegalArgumentException();
		}
		return simplify(tree, tree.root());
	}
	
	public static LinkedBinaryTree<String> simplify(LinkedBinaryTree<String> tree, Position<String> p){
		
		LinkedBinaryTree<String> linkedbinarytree = new LinkedBinaryTree<String>();
		String left = null;
		String right = null;
		LinkedBinaryTree<String> index1 = new LinkedBinaryTree<String>();
		LinkedBinaryTree<String> index2 = new LinkedBinaryTree<String>();
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		if((tree.left(p) == null) && (tree.right(p) == null)){
			linkedbinarytree.addRoot(p.getElement());
			return linkedbinarytree;
		}
		
		if(tree.left(p) != null){
			index1 = simplify(tree, tree.left(p));
			left = index1.root().getElement();
		}
		
		if(tree.right(p) != null){
			index2 = simplify(tree, tree.right(p));
			right = index2.root().getElement();
		}
		int result = 0;
		if(index1.size() == 1 && index2.size() == 1){
			if(Character.isDigit(left.charAt(0)) && Character.isDigit(right.charAt(0))){
				if(p.getElement().equals("+")){
					result = Integer.valueOf(left) + Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}else if(p.getElement().equals("-")){
					result = Integer.valueOf(left) - Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}else if(p.getElement().equals("*")){
					result = Integer.valueOf(left) * Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}
			}else{
				linkedbinarytree.addRoot(p.getElement());
				linkedbinarytree.addLeft(linkedbinarytree.root(), left);
				linkedbinarytree.addRight(linkedbinarytree.root(), right);
			}
		}else{
			linkedbinarytree.addRoot(p.getElement());
			linkedbinarytree.attach(linkedbinarytree.root(), index1, index2);
		}
		
		return linkedbinarytree;
	}


	/**
	 * This should do everything the simplify method does AND also apply the following rules:
	 *  * 1 x == x  i.e.  (1*x)==x
	 *  * x 1 == x        (x*1)==x
	 *  * 0 x == 0        (0*x)==0
	 *  * x 0 == 0        (x*0)==0
	 *  + 0 x == x        (0+x)==x
	 *  + x 0 == 0        (x+0)==x
	 *  - x 0 == x        (x-0)==x
	 *  - x x == 0        (x-x)==0
	 *  
	 *  Example: - * 1 x x == 0, in infix notation: ((1*x)-x) = (x-x) = 0
	 *  
	 * Ideally, this method should run in O(n) time (harder to achieve than for other methods!)
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @return resulting binary tree after applying the simplifications
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression
	 */
	public static LinkedBinaryTree<String> simplifyFancy(LinkedBinaryTree<String> tree) throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null){
			throw new IllegalArgumentException();
		}
		return simplifyFancy(tree, tree.root());
	}

	public static LinkedBinaryTree<String> simplifyFancy(LinkedBinaryTree<String> tree, Position<String> p){
		
		LinkedBinaryTree<String> linkedbinarytree = new LinkedBinaryTree<String>();
		String left = null;
		String right = null;
		LinkedBinaryTree<String> index1 = new LinkedBinaryTree<String>();
		LinkedBinaryTree<String> index2 = new LinkedBinaryTree<String>();
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		if((tree.left(p) == null) && (tree.right(p) == null)){
			linkedbinarytree.addRoot(p.getElement());
			return linkedbinarytree;
		}
		
		if(tree.left(p) != null){
			index1 = simplifyFancy(tree, tree.left(p));
			if(index1.size() == 1){
				left = index1.root().getElement();
			}
		}
		
		if(tree.right(p) != null){
			index2 = simplifyFancy(tree, tree.right(p));
			if(index2.size() == 1){
				right = index2.root().getElement();
			}
		}
		
		int result = 0;
		if(index1.size() == 1 && index2.size() == 1){
			if(Character.isDigit(left.charAt(0)) && Character.isDigit(right.charAt(0))){
				if(p.getElement().equals("+")){
					result = Integer.valueOf(left) + Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}else if(p.getElement().equals("-")){
					result = Integer.valueOf(left) - Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}else if(p.getElement().equals("*")){
					result = Integer.valueOf(left) * Integer.valueOf(right);
					linkedbinarytree.addRoot(String.valueOf(result));
				}
			}else{
				if(p.getElement().equals("+")){
					if(left.equals("0")){
						linkedbinarytree.addRoot(String.valueOf(right));
					}else if(right.equals("0")){
						linkedbinarytree.addRoot(String.valueOf(left));
					}else{
						linkedbinarytree.addRoot(p.getElement());
						linkedbinarytree.addLeft(linkedbinarytree.root(), left);
						linkedbinarytree.addRight(linkedbinarytree.root(), right);
					}
				}else if(p.getElement().equals("-")){
					if(right.equals("0")){
						linkedbinarytree.addRoot(String.valueOf(left));
					}else if(right.equals(left)){
						linkedbinarytree.addRoot(String.valueOf("0"));
					}else{
						linkedbinarytree.addRoot(p.getElement());
						linkedbinarytree.addLeft(linkedbinarytree.root(), left);
						linkedbinarytree.addRight(linkedbinarytree.root(), right);
					}
				}else if(p.getElement().equals("*")){
					if(left.equals("0") || right.equals("0")){
						linkedbinarytree.addRoot(String.valueOf("0"));
					}else if(left.equals("1")){
						linkedbinarytree.addRoot(String.valueOf(right));
					}else if(right.equals("1")){
						linkedbinarytree.addRoot(String.valueOf(left));
					}else{
						linkedbinarytree.addRoot(p.getElement());
						linkedbinarytree.addLeft(linkedbinarytree.root(), left);
						linkedbinarytree.addRight(linkedbinarytree.root(), right);
					}
				}
			}
		}else{
			linkedbinarytree.addRoot(p.getElement());
			linkedbinarytree.attach(linkedbinarytree.root(), index1, index2);
		}
		
		return linkedbinarytree;
	}
	

	
	/**
	 * Given a tree, a variable label and a value, this should replace all
	 * instances of that variable in the tree with the given value
	 * 
	 * Ideally, this method should run in O(n) time (quite hard! O(n^2) is easier.)
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @param variable
	 *            - a variable label that might exist in the tree
	 * @param value
	 *            - an integer value that the variable represents
	 * @return Tree after replacing all instances of the specified variable with
	 *         it's numeric value
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression, or either of the other
	 *             arguments are null
	 */
	public static LinkedBinaryTree<String> substitute(LinkedBinaryTree<String> tree, String variable, int value)
			throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null || variable == null){
			throw new IllegalArgumentException();
		}
		return substitute(tree, tree.root(), variable, value);
	}

	public static LinkedBinaryTree<String> substitute(LinkedBinaryTree<String> tree, Position<String> p, String variable, int value){
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		if(tree.left(p) != null){
			substitute(tree, tree.left(p), variable, value);
		}
		
		if(tree.right(p) != null){
			substitute(tree, tree.right(p), variable, value);
		}
		
		if(p.getElement().equals(variable)){
			tree.set(p, String.valueOf(value));
		}
		
		return tree;
	}

	/**
	 * Given a tree and a a map of variable labels to values, this should
	 * replace all instances of those variables in the tree with the
	 * corresponding given values
	 * 
	 * Ideally, this method should run in O(n) expected time
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @param map
	 *            - a map of variable labels to integer values
	 * @return Tree after replacing all instances of variables which are keys in
	 *         the map, with their numeric values
	 * @throws IllegalArgumentException
	 *             if tree was not a valid expression, or map is null, or tries
	 *             to substitute a null into the tree
	 */
	public static LinkedBinaryTree<String> substitute(LinkedBinaryTree<String> tree, HashMap<String, Integer> map)
			throws IllegalArgumentException {
		// TODO: Implement this method
		if(tree == null || map == null){
			throw new IllegalArgumentException();
		}
		return substitute(tree, tree.root(), map);
	}
	
	public static LinkedBinaryTree<String> substitute(LinkedBinaryTree<String> tree, Position<String> p, HashMap<String, Integer> map){
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				throw new IllegalArgumentException();
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				throw new IllegalArgumentException();
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				throw new IllegalArgumentException();
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				throw new IllegalArgumentException();
			}
		}
		
		if(tree.left(p) != null){
			substitute(tree, tree.left(p), map);
		}
		
		if(tree.right(p) != null){
			substitute(tree, tree.right(p), map);
		}
		
		
		for(String value: map.keySet()){
			if(value == null){
				throw new IllegalArgumentException();
			}
			
			if(p.getElement().equals(value)){
				if(map.get(value) == null){
					throw new IllegalArgumentException();
				}
				tree.set(p, String.valueOf(map.get(value)));
			}
		}
		
		return tree;
	}
	

	/**
	 * Given a tree, identify if that tree represents a valid arithmetic
	 * expression (possibly with variables)
	 * 
	 * Ideally, this method should run in O(n) expected time
	 * 
	 * @param tree
	 *            - a tree representing an arithmetic expression
	 * @return true if the tree is not null and it obeys the structure of an
	 *              arithmetic expression. Otherwise, it returns false
	 */
	public static boolean isArithmeticExpression(LinkedBinaryTree<String> tree) {
		// TODO: Implement this method
		if(tree == null){
			return false;
		}
		
		
		return isArithmeticExpression(tree, tree.root());
	}
	
	public static boolean isArithmeticExpression(LinkedBinaryTree<String> tree, Position<String> p){
		
		boolean left = true;
		boolean right = true;
		
		if(p.getElement().equals("+") || p.getElement().equals("-") || p.getElement().equals("*")){
			if(tree.left(p) == null && tree.right(p) == null){
				return false;
			}else if(tree.right(p) == null && !(tree.left(p) == null)){
				return false;
			}else if(tree.left(p) == null && !(tree.right(p) == null)){
				return false;
			}
		}else if(!p.getElement().equals("+") && !p.getElement().equals("-") && !p.getElement().equals("*")){
			if(tree.left(p) != null || tree.right(p) != null){
				return false;
			}
		}
		
		if(tree.left(p) != null){
			left = isArithmeticExpression(tree, tree.left(p));
		}
		
		if(tree.right(p) != null){
			right = isArithmeticExpression(tree, tree.right(p));
		}
		return left && right;
	}
}

