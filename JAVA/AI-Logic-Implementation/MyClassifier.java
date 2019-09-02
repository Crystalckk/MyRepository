import java.util.*;
import java.io.*;
 
public class MyClassifier{
	public static void main(String[] args){
	
	//<-----------------------Reading File-------------------------------->//
		int counter = 1;
		String[] index = args[2].split("");
		int k = 0;
		if(index.length == 3){
			k = Integer.parseInt(index[0]);
		}
		File file = new File(args[0]);
		ArrayList<Node> trainList = new ArrayList<Node>();
		ArrayList<Node> testList = new ArrayList<Node>();
		try (BufferedReader br = new BufferedReader(new FileReader(args[0]))) {
			if (!file.exists()) {
				return;
			}
    		String line;
    		while ((line = br.readLine()) != null) {
        		String[] values = line.split(",");
				ArrayList<Double> value = new ArrayList<Double>();
				for(int i = 0; i < values.length - 1; i++){
					value.add(Double.parseDouble(values[i]));
				}
				Node one =  new Node(values[values.length-1], value, counter);
				trainList.add(one);
				counter++;
				//System.out.println(values[values.length-1]);
    		}
		}catch (Exception e) {
       		e.printStackTrace();
    	} 
		
		counter = 0;
		file = new File(args[1]);
		try (BufferedReader br = new BufferedReader(new FileReader(args[1]))) {
			if (!file.exists()) {
				return;
			}
    		String line;
    		while ((line = br.readLine()) != null) {
        		String[] values = line.split(",");
				ArrayList<Double> value = new ArrayList<Double>();
				for(int i = 0; i < values.length; i++){
					value.add(Double.parseDouble(values[i]));
				}
				Node one =  new Node("", value, counter);
				testList.add(one);
				counter++;
				//System.out.println(counter);
    		}
		}catch (Exception e) {
       		e.printStackTrace();
    	}
		
		
	// <--------------------------------------------------------------------------------->	
		for(int i = 0; i < testList.size(); i++){
			
			if(index.length == 3){
				KNN.distanceCount(trainList, testList.get(i));
				ArrayList<Pair> pairs = KNN.compare(testList.get(i), k);
				String classValue = KNN.selectClass(trainList, pairs);
				System.out.println(classValue);
			}else{
				ArrayList<Double> yesMeans = KNB.means(trainList, "yes");
				ArrayList<Double> noMeans = KNB.means(trainList, "no");
				ArrayList<Double> yesStandard = KNB.standard(trainList, yesMeans, "yes");
				ArrayList<Double> noStandard = KNB.standard(trainList, noMeans, "no");
				ArrayList<Double> yesDensit = KNB.density(testList.get(i), yesStandard, yesMeans);
				ArrayList<Double> noDensit = KNB.density(testList.get(i), noStandard, noMeans);
				String classValue = KNB.result(yesDensit, noDensit, trainList);
				System.out.println(classValue);
			}
		
		}
		
		
		
		return;
	}
	
	
	
}
