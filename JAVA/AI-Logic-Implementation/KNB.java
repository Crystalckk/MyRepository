import java.util.*;
import java.io.*;
 
public class KNB{
	
 static ArrayList<Double> means(ArrayList<Node> train, String classValue){
	 ArrayList<Double> means = new ArrayList<Double>();
	 int counter = 0;
	 for(int i = 0; i < train.get(0).value.size(); i++){
		 means.add(0.0);
	 }
	 for(int i = 0; i < train.size(); i++){
		 if(classValue.equals(train.get(i).classValue)){
			 for(int j = 0; j < train.get(i).value.size(); j++){
			 double index = means.get(j) + train.get(i).value.get(j);
			 means.set(j, index);
			 }
			 counter++;
		 }
	 }
	 for(int i = 0; i < means.size(); i++){
		double index = means.get(i) / counter;
		means.set(i, index);
	 }
	 return means;
 }
	
	static ArrayList<Double> standard(ArrayList<Node> train, ArrayList<Double> means, String classValue){
		ArrayList<Double> standard = new ArrayList<Double>();
		int counter = 0;
	 	for(int i = 0; i < train.get(i).value.size(); i++){
		 standard.add(0.0);
	 	}
	 	for(int i = 0; i < train.size(); i++){
			if(classValue.equals(train.get(i).classValue)){
				for(int j = 0; j < train.get(i).value.size(); j++){
					double index = Math.pow(train.get(i).value.get(j) - means.get(j), 2);
					index = standard.get(j) + index;
			 		standard.set(j, index);
		 		}
				counter++;
			}
	 	}	
		for(int i = 0; i < standard.size(); i++){
			double index = standard.get(i) / (counter - 1);
			index = Math.sqrt(index);
			standard.set(i, index);
	 	}
	 	return standard;
	}
	
	static ArrayList<Double> density(Node test, ArrayList<Double> standard, ArrayList<Double> means){
		ArrayList<Double> densit = new ArrayList<Double>();
		for(int i = 0; i <  test.value.size(); i++){
		 densit.add(0.0);
	 	}
		for(int i = 0; i < test.value.size(); i++){
			double base = (1/(Math.sqrt(2*Math.PI)*standard.get(i)));
  			double pow = -(Math.pow((test.value.get(i)-means.get(i)), 2)/(2*Math.pow(standard.get(i), 2)));
			//System.out.println("standard is: "+standard.get(i));
			//System.out.println("means is: "+means.get(i));
			//System.out.println("test is: "+test.value.get(i));
			//System.out.println("result is: " + base +" / " + pow +" = " +(double)base*Math.exp(pow));
			densit.set(i, (double)base*Math.exp(pow));
		}
		return densit;
	}
	
	static String result(ArrayList<Double> yesDensit, ArrayList<Double> noDensit, ArrayList<Node> train){
		double pYes = 0.0;
		int counter = 0;
		double pNo = 0.0;
		double pE = 1.0;
		double pF = 1.0;
		for(int i = 0; i <  train.size(); i++){
			if(train.get(i).classValue.equals("yes")){
				counter++;
			}
		}
		if(counter == 0){
			counter = 1;
		}else if(counter == train.size()){
			counter -= 1;
		}
		pYes = counter / (double)train.size();
		pNo = 1 - pYes;
		
		for(int i = 0; i < yesDensit.size() ; i++){
			pE *= yesDensit.get(i);
			//System.out.println(pE);
			pF *= noDensit.get(i);
		}
		pYes = pE*pYes;
		pNo = pF*pNo;
		//System.out.println(pYes);
		//System.out.println(pNo);
		if(pYes > pNo){
			return "yes";
		}else{
			return "No";
		}
	}
}
