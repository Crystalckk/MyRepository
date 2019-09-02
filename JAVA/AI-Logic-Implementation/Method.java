import java.util.*;
public class Method{
	static ArrayList<String> path = new ArrayList<String>();
	static ArrayList<Node> fringe = new ArrayList<Node>();
	static ArrayList<Node> expanded = new ArrayList<Node>();
	static ArrayList<String> fringeValue = new ArrayList<String>();
	static int n = 0;
	static int isFound = 0;
	static int DEEP = 0;
	static int isEnd = 0;
	
	public static ArrayList<String> findPath(Node one){
		if(one.parent == null){
			path.add(one.value);
			return path;
		}else{
			findPath(one.parent);
			path.add(one.value);
		}
		return path;
	}
	
	public static boolean checkObstacle(ArrayList<String> input, String value){
		for(int i= 2; i < input.size(); i++){
			if(value.equals(input.get(i))){
				return false;
			}
		}
		return true;
	}
	
	public static void calculate(Node parent, int index, ArrayList<Node> next){
		String value = parent.value;
		StringBuffer str = new StringBuffer(value); 
		//System.out.println(value+" yessss");
		if(value.charAt(index) == '0'){
			str.setCharAt(index,'1');
			next.add(new Node(str.toString(), parent, index));
				
		}else if(value.charAt(index) == '1'){
			str.setCharAt(index,'0');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'2');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '2'){
			str.setCharAt(index,'1');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'3');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '3'){
			str.setCharAt(index,'2');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'4');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '4'){
			str.setCharAt(index,'3');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'5');
				next.add(new Node(str.toString(), parent, index));
				
		}else if(value.charAt(index) == '5'){
			str.setCharAt(index,'4');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'6');
				next.add(new Node(str.toString(), parent, index));
 
		}else if(value.charAt(index) == '6'){
			str.setCharAt(index,'5');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'7');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '7'){
			str.setCharAt(index,'6');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'8');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '8'){
			str.setCharAt(index,'7');
				next.add(new Node(str.toString(), parent, index));
			str.setCharAt(index,'9');
				next.add(new Node(str.toString(), parent, index));
			
		}else if(value.charAt(index) == '9'){
			str.setCharAt(index,'8');
				next.add(new Node(str.toString(), parent, index));	
		}
	}
	
	public static ArrayList<Node> findChild(Node parent){
		ArrayList<Node> next = new ArrayList<Node>();
		if(parent.index == 4){
			calculate(parent, 0, next);
			calculate(parent, 1, next);
			calculate(parent, 2, next);
		}else if(parent.index == 0){
			calculate(parent, 1, next);
			calculate(parent, 2, next);
		}else if(parent.index == 1){
			calculate(parent, 0, next);
			calculate(parent, 2, next);
		}else if(parent.index == 2){
			calculate(parent, 0, next);
			calculate(parent, 1, next);
		}
		return next;
	}
	
	public static ArrayList<String> BFS(ArrayList<String> input, Node current){
			ArrayList<Node> next = findChild(current);
			for(int i = 0; i < next.size(); i++){
				if(fringe.size() < 1000){
					if(!checkObstacle(input, next.get(i).value)){
						continue;
					}
					int flag = 0;
					for(int j = 0; j < fringe.size(); j++){
						if(fringe.get(j).value.equals(next.get(i).value)){
								if(fringe.get(j).index == next.get(i).index){
									flag = 1;
									break;	
								}
						}
					}
					if(flag == 0){
						fringe.add(next.get(i));
						fringeValue.add(next.get(i).value);
					}
					if(next.get(i).value.equals(input.get(1))){
						isFound = 1;
						return fringeValue;
					}
				}else{
					return fringeValue;	
				}
			}
			n++;
			if(n < fringe.size()){
				BFS(input, fringe.get(n));
			}
			return fringeValue;
		
	}
	
// <-----------------------------------------DFS------------------------------------------------------------------>
	
	public static ArrayList<String> DFS(ArrayList<String> input, Node current){
		ArrayList<Node> next = findChild(current);
			for(int i = 0; i < next.size(); i++){
				if(fringe.size() < 1000){
					if(!checkObstacle(input, next.get(i).value)){
						continue;
					}
					int flag = 0;
					for(int j = 0; j < fringe.size(); j++){
						if(fringe.get(j).value.equals(next.get(i).value)){
							if(fringe.get(j).index == next.get(i).index){
								flag = 1;
								break;	
							}
						}
					}
					if(flag == 0){
						fringe.add(next.get(i));
						fringeValue.add(next.get(i).value);
						n++;
					}else{
						continue;
					}
					if(next.get(i).value.equals(input.get(1))){
						isFound = 1;
						return fringeValue;
					}
					if(n < fringe.size()){
						DFS(input, fringe.get(n));
						if(isFound == 1){
							return fringeValue;
						}
					}
				}else{
					return fringeValue;	
				}
			}
		return fringeValue;	
	}
	
// <-------------------------------------IDS------------------------------------------------------------------>
	
	public static ArrayList<String> IDS(ArrayList<String> input, ArrayList<String> indexValue, ArrayList<Node> index, ArrayList<Node> subtrack, Node current, int deepth){
		if(deepth >= DEEP){
			return indexValue;
		}
		ArrayList<Node> next = findChild(current);
			for(int i = 0; i < next.size(); i++){
				if(index.size() < 1000){
					if(!checkObstacle(input, next.get(i).value)){
						continue;
					}
					int flag = 0;
					for(int j = 0; j < subtrack.size(); j++){
						if(subtrack.get(j).value.equals(next.get(i).value)){
							if(subtrack.get(j).index == next.get(i).index){
								flag = 1;
								break;	
							}
						}
					}
					if(flag == 0){
						if(deepth < DEEP){
							index.add(next.get(i));
							n++;	
						}
						subtrack.add(next.get(i));
						indexValue.add(next.get(i).value);
					}else{
						continue;
					}
					if(next.get(i).value.equals(input.get(1))){
						isFound = 1;
						return indexValue;
					}
					
					if(n < index.size()){
						if(deepth < DEEP - 1){
							int deepthNext = deepth + 1;
							IDS(input, indexValue, index, subtrack, index.get(n), deepthNext);	
						}
							if(isFound == 1){
								return indexValue;
							}
					}
				}else{
					return indexValue;	
				}
			}
		return indexValue;	
	}
	
// <-----------------------------------Hill----------------------------------------------------------------------->
 
	public static ArrayList<Node> Hill(ArrayList<String> input, Node current){
		ArrayList<Node> next = findChild(current);
		next = Node.childWithHeuristic(input, next);
		for(int i = 0; i < next.size(); i++){
			//System.out.println(next.get(i).value);
			fringe.add(next.get(i));
		}
		boolean notFind = true;
			while(notFind && expanded.size() < 1000 && fringe.size() != 0){
				if(expanded.size() < 1000){
					int index = compareChildDistance(fringe);
					//System.out.println(current.value+": "+current.distance+" ------ "+fringe.get(index).distance+": "+fringe.get(index).value);
					if(fringe.get(index).distance >= current.distance && !current.value.equals(input.get(0))){
						//System.out.println("RETURNED!!");
						isEnd = 1;
						return expanded;
					}
					if(!checkObstacle(input, fringe.get(index).value)){
						fringe.remove(index);
						continue;
					}
					int flag = 0;
					for(int j = 0; j < expanded.size(); j++){
						if(expanded.get(j).value.equals(fringe.get(index).value)){
								if(expanded.get(j).index == fringe.get(index).index){
									flag = 1;
									fringe.remove(index);
									break;	
								}
						}
					}
					if(flag == 0){
						//System.out.println(fringe.get(index).value+" ADD!!");
						expanded.add(fringe.get(index));
						fringe.remove(index);
					}else{
						return expanded;
					}
					if(expanded.get(expanded.size() - 1).value.equals(input.get(1))){
						//System.out.println("FINDED!!");
						isFound = 1;
						notFind = false;
						return expanded;
					}
					if(isFound == 0 && fringe != null){
						//System.out.println("EXPANDED!  "+expanded.get(expanded.size() - 1).value);
						Hill(input, expanded.get(expanded.size() - 1));
						if(isFound == 1 || isEnd == 1){
							return expanded;
						}
					}
				}else{
					return expanded;	
				}
			}
			
			return expanded;
	}
// <-----------------------------------Greedy----------------------------------------------------------------------->	
	
	public static int compareChildDistance(ArrayList<Node> children){
		int min = children.get(0).distance;
		int index = 0;
		for(int j = 0; j < children.size(); j++){
			//System.out.println(children.get(j).distance);
			if(min > children.get(j).distance){
				min = children.get(j).distance;
				index = j;
			}else if(min == children.get(j).distance){
				index = j;
			}
		}
		return index;
	}
	public static ArrayList<Node> Greedy(ArrayList<String> input, Node current){
		ArrayList<Node> next = findChild(current);
		next = Node.childWithHeuristic(input, next);
		
		for(int i = 0; i < next.size(); i++){
			//System.out.println(next.get(i).value);
			fringe.add(next.get(i));
		}
		boolean notFind = true;
			while(notFind && expanded.size() < 1000 && fringe.size() != 0){
				if(expanded.size() < 1000){
					int index = compareChildDistance(fringe);
					//System.out.println(index+" ------ "+fringe.get(index).value);
					if(!checkObstacle(input, fringe.get(index).value)){
						fringe.remove(index);
						continue;
					}
					int flag = 0;
					for(int j = 0; j < expanded.size(); j++){
						if(expanded.get(j).value.equals(fringe.get(index).value)){
								if(expanded.get(j).index == fringe.get(index).index){
									flag = 1;
									fringe.remove(index);
									break;	
								}
						}
					}
					if(flag == 0){
						//System.out.println(fringe.get(index).value+" ADD!!");
						expanded.add(fringe.get(index));
						fringe.remove(index);
					}else{
						return expanded;
					}
					if(expanded.get(expanded.size() - 1).value.equals(input.get(1))){
						//System.out.println("FINDED!!");
						isFound = 1;
						notFind = false;
						return expanded;
					}
					if(isFound == 0 && fringe != null){
						Greedy(input, expanded.get(expanded.size() - 1));
						if(isFound == 1){
							return expanded;
						}
					}
				}else{
					return expanded;	
				}
			}
			
			return expanded;
	}
// <-----------------------------------A----------------------------------------------------------------------->	
	public static ArrayList<Node> A(ArrayList<String> input, Node current){
		ArrayList<Node> next = Node.childWithHeuristic(input, current);
		for(int i = 0; i < next.size(); i++){
			//System.out.println(next.get(i).value);
			fringe.add(next.get(i));
		}
		boolean notFind = true;
			while(notFind && expanded.size() < 1000 && fringe.size() != 0){
				if(expanded.size() < 1000){
					int index = compareChildDistance(fringe);
					//System.out.println(index+" ------ "+fringe.get(index).value);
					if(!checkObstacle(input, fringe.get(index).value)){
						fringe.remove(index);
						continue;
					}
					int flag = 0;
					for(int j = 0; j < expanded.size(); j++){
						if(expanded.get(j).value.equals(fringe.get(index).value)){
								if(expanded.get(j).index == fringe.get(index).index){
									flag = 1;
									fringe.remove(index);
									break;	
								}
						}
					}
					if(flag == 0){
						//System.out.println(fringe.get(index).value+" ADD!!");
						expanded.add(fringe.get(index));
						fringe.remove(index);
					}else{
						return expanded;
					}
					if(expanded.get(expanded.size() - 1).value.equals(input.get(1))){
						//System.out.println("FINDED!!");
						isFound = 1;
						notFind = false;
						return expanded;
					}
					if(isFound == 0 && fringe != null){
						//System.out.println("EXPANDED!  "+expanded.get(expanded.size() - 1).value);
						A(input, expanded.get(expanded.size() - 1));
						if(isFound == 1){
							return expanded;
						}
					}
				}else{
					return expanded;	
				}
			}
			
			return expanded;
	}
	
}
