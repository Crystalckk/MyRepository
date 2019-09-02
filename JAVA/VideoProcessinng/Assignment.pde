import processing.video.*; 
import java.util.*;
import ddf.minim.*;

Movie m;
Movie bg;
Minim minim;
AudioPlayer[] song;
PImage pre;
PImage remove;
PImage refer;
PImage[] food;
Bubble b1;
Bubble b2;
Bubble b3;
Bubble b4;
int framenumber = 1;
float threshold = 25;
int flag1 = 1;
int flag2 = 1;
int index = 0;

void setup() { 
  size(480, 320); 
  //frameRate(120); 
  m = new Movie(this, sketchPath("monkey (option1).mov")); 
  bg = new Movie(this, sketchPath("background.mov"));
  minim = new Minim(this);
  song = new AudioPlayer[7];
  for(int i = 0; i < 7; i++){
    song[i] = minim.loadFile("sound.wav");
  }
  food = new PImage[5];
  food[0] = loadImage("cake1.jpg");
  food[1] = loadImage("humberger.jpg");
  food[2] = loadImage("pizza.jpg");
  food[3] = loadImage("chick.jpg");
  food[4] = loadImage("bee.jpg");
  b1 = new Bubble(100, 30, 50, 5);
  b2 = new Bubble(400, 300, 50, -4);
  b3 = new Bubble(150, 50, 50, 5);
  b4 = new Bubble(300, 300, 50, -6);
  pre = createImage(480, 380, RGB);
  //m.frameRate(120); 
  //bg.frameRate(120);
  framenumber = 0;
  m.play(); 
  bg.play();  
} 

void draw() { 
   float time = m.time();
   float duration = m.duration();
   List<Integer> coordListX = new ArrayList<Integer>();
   List<Integer> coordListY = new ArrayList<Integer>();
   int count = 0;
   float avgX = 0;
  float avgY = 0;

   
 //Phase 1, begin to divide and save frames
  if (m.available()){
    background(0, 0, 0);
    m.read(); 
    bg.read();
  }
    if(time < duration) {
    System.out.printf("Extracting Frame: %d ...\n", framenumber);
    framenumber++;
  }else{
    exit();
  }
  
// remove background
  pre.loadPixels();
  remove = m.copy();
  refer = m.copy();
  remove.loadPixels();
   for (int x = 0; x < m.width; x++)
            for (int y = 0; y < m.height; y++){
              int mloc = x + y * m.width;
                  color mc = m.pixels[mloc];
                  if ( blue(mc) > 85 && green(mc) < 190){ 
                     remove.pixels[mloc] = -1;
    }
  }                   
    remove.updatePixels();
//

//make the image shown on window
    refer.loadPixels();
   for (int i = 0; i < m.width; i++ ) {
    for (int j = 0; j < m.height; j++ ) {
      int loc = i + j * m.width;
      if(remove.pixels[loc] != -1){
      color current = remove.pixels[loc];
      float r1 = red(current);
      float g1 = green(current);
      float b1 = blue(current);
      color prev = pre.pixels[loc];
      float r2 = red(prev);
      float g2 = green(prev);
      float b2 = blue(prev);
      float distance = disqt(r1, g1, b1, r2, g2, b2); 
      if (distance > threshold*threshold) {
        coordListX.add(i);
        coordListY.add(j);
        }
      }else{
        // replace background since frame 100
        if(framenumber > 50){
          refer.pixels[loc] = bg.pixels[loc];
        }
      }
    }  
  }
  // replace marionette from frame 300 to 600
  if(framenumber > 100 && framenumber < 230){
    Marionette(remove, refer, bg);
  }
 
  //when fod hits marionette, change the color of monkey, and play sound!
  if(flag1 == 0){
    hitReflection(remove, refer);
    if(index < 7){
     song[index].play();
     index++;
    }
    flag2 = 10;
  }
  
  if(flag2 > 0){
    hitReflection(remove, refer);
    flag2--;
  }
  //update pixels and show image on window
   refer.updatePixels();
   image(refer,0,0);
  //text string when food hitsb marionette
   if(flag2 > 0){
     String s = "Lose 1 kg!";
     fill(255, 255, 0);
     textSize(30);
     text(s, 50, 50);
   }
   //launch different food during different frame range
   if(framenumber > 350 && framenumber < 450){
     flag1 = b1.accsece(remove, food[0]);
   }else if(framenumber > 450 && framenumber < 550){
     flag1 = b2.accsece(remove, food[1]);
   }else if(framenumber > 250 && framenumber < 350){
    flag1 = b3.accsece(remove, food[2]); 
   }else if(framenumber > 550 && framenumber < 650){
    flag1 = b4.accsece(remove, food[3]); 
   }else if(framenumber > 650){
     for (int x = 0; x < remove.width; x++){
     for (int y = 0; y < remove.height; y++){
      int mloc = x + y * remove.width;
      color mc = remove.pixels[mloc];
      if(remove.pixels[mloc] != -1 && red(mc) > 220){
         avgX += x;
         avgY += y;
         count++;
      }
     }         
   }
    if (count > 0) { 
    avgX = avgX / count;
    avgY = avgY / count;
    fill(255, 255,0);
    strokeWeight(4.0);
    stroke(0);
    image(food[4], avgX - 100, avgY - 100, 30, 30);
  }
 }
   
   //draw red mark to track motion of maritonette
   redMark(coordListX, coordListY, framenumber);
   saveFrame("frames/####.png");
//
   
} 

//read movie and save previous frame
void movieEvent(Movie m, Movie bg) {
    pre.copy(m, 0, 0, m.width, m.height, 0, 0, pre.width, pre.height);
    pre.updatePixels();
    m.read();
    bg.read();
  }

//distance algorithm 
float disqt(float x1, float y1, float z1, float x2, float y2, float z2) {
  float d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1);
  return d;
}

//method for drawing red marks for motion tracking
void redMark(List<Integer> coordListX, List<Integer> coordListY, int framenumber){
  if(framenumber > 0 && framenumber < 100){
   for(int x = 0; x < coordListX.size(); x++){
     point(coordListX.get(x).floatValue(), coordListY.get(x).floatValue());
     stroke(255,153,204);
     strokeWeight(3);
     x = x+10;
     }
  }
}

//for replace marionette by own created character
 void Marionette(PImage remove, PImage refer, PImage bg){
   for (int x = 0; x < remove.width; x++){
     for (int y = 0; y < remove.height; y++){
       int mloc = x + y * remove.width;
         color mc = remove.pixels[mloc];
          if (remove.pixels[mloc] != -1 && red(mc) < 220){ 
              refer.pixels[mloc] = bg.pixels[mloc];
          }else if(remove.pixels[mloc] != -1 && red(mc) > 220){
              refer.pixels[mloc] = color(0);
          }
      }         
   }
}

//the reflection of marionette when hits food picture, it's color changed to yellow
void hitReflection(PImage remove, PImage refer){
   for (int x = 0; x < remove.width; x++){
     for (int y = 0; y < remove.height; y++){
       int mloc = x + y * remove.width;
          if (remove.pixels[mloc] != -1){ 
              refer.pixels[mloc] = color(225, 225, 0);;
          }
      }         
   }
}
