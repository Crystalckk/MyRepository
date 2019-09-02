import java.util.*;
import java.io.*;
 
public class KNN{
	
	static void distanceCount(ArrayList<Node> train, Node test){
		ArrayList<Double> distances = new ArrayList<Double>();
		for(int i = 0; i < train.size(); i++){
			double distance = 0.0;
			for(int j = 0; j < train.get(i).value.size(); j++){
				distance += (train.get(i).value.get(j) - test.value.get(j)) * (train.get(i).value.get(j) - test.value.get(j));
			}
			distances.add(distance);
			//System.out.println(distance);
		}
		test.distance = distances;
		return;
	}
	
	static ArrayList<Pair> orderFilter(ArrayList<Pair> pair){
		if(pair.size() == 1){
			return pair;
		}
		for(int i = 1; i < pair.size(); i++){
			if(pair.get(i).distance < pair.get(i-1).distance){	
				double index1 = pair.get(i).distance;
				int index2 = pair.get(i).index;			//small in front
				pair.get(i).distance = pair.get(i-1).distance;
				pair.get(i).index = pair.get(i-1).index;
				pair.get(i - 1).distance = index1;
				pair.get(i - 1).index = index2;
			}
		}
		return pair;
	}
	
	static ArrayList<Pair> compare(Node test, int k){
		
		ArrayList<Pair> pairs = new ArrayList<Pair>();
 
		for(int i = 0; i < k; i++){
			Pair one = new Pair(test.distance.get(i), i);
			pairs.add(one);
		}
		pairs = orderFilter(pairs);
		
		for(int i = 0; i < test.distance.size(); i++){
			for(int j = 0; j < pairs.size(); j++){
				if(test.distance.get(i) < pairs.get(j).distance){
					pairs.get(pairs.size() - 1).distance = test.distance.get(i);
					pairs.get(pairs.size() - 1).index = i;
					pairs = orderFilter(pairs);
					break;
				}
			}
			
		}
		return pairs;
	}
	
	static String selectClass(ArrayList<Node> train, ArrayList<Pair> pair){
		int yes = 0;
		int no = 0;
		//System.out.println(pair.size());
		for(int i = 0; i < pair.size(); i++){
			//System.out.println(pair.get(i).index);
			int index = pair.get(i).index;
			String classValue = train.get(index).classValue;
			if(classValue.equals("yes")){
				yes++;
			} else if(classValue.equals("no")){
				no++;
			}
		}
		if(yes > no){
			return "yes";
		} else{
			return "no";
		}
	}
}
