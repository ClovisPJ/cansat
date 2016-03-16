void setup() {
  size(400,400,P3D);
  background(255);
  lights();
  frameRate(1);
}

void draw() {
  background(255);
  translate(width/2, height/2, 0);
  String lines[] = loadStrings("data.csv");
  String data[] = split(lines[lines.length-1], ',');
  //rotateX(map(mouseX, 0, 500, 0, 2*PI));
  //rotateY(map(mouseY, 0, 500, 0, 2*PI));
  rotateX(-16*PI/8);
  rotateY(PI/4);
  cylinder(50, 200, 32);
  arrow(float(data[0])*30, float(data[1])*30, float(data[2])*30);
/*  textSize(10);
  text(data[0], 10, 10);
  text(data[1], 20, 10);
  text(data[2], 30, 10);
*/
}

void cylinder(float radius, float height, float res) {
  float angle = (2*PI)/res;
  fill(0,0,255);
  strokeWeight(1);
  stroke(1);
  pushMatrix();
  beginShape(TRIANGLE_STRIP);
  for (int i = 0; i <= res; i++) {
    float x = radius * cos(i * angle);
    float y = radius * sin(i * angle);
    vertex( x, y, height/2);
    vertex( x, y, -height/2);    
  }
  endShape(CLOSE);  
  popMatrix();
}

void arrow(float x, float y, float z) {
  float t = z;
  z = y;
  y = t;
  float mag = pow( pow(x,2) + pow(y,2) + pow(z,2), 0.5);
  float arg1 = atan2(y, x);
  float arg2 = atan2(z, y);
  pushMatrix();

  stroke(255,0,0);
  fill(255,0,0);
  strokeWeight(10);

  rotateX(arg2);
  rotateZ(arg1);
  beginShape();
  line(0,0,0,mag,0,0);
  endShape(CLOSE);  

  float radius = 10;
  float height = 30;
  float angle = (2*PI)/10;
  noStroke();

  beginShape(TRIANGLE_STRIP);
  translate(mag, 0, 0);
  rotateY(-PI/2);
  for (int i = 0; i <= 10; i++) {
    float xcoordinate = radius * cos(i * angle);
    float ycoordinate = radius * sin(i * angle);
    vertex( xcoordinate, ycoordinate, height/2);
    vertex( 0, 0, -height/2);    
  }
  endShape(CLOSE);  
  popMatrix();
}
