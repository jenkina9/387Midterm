// this sketch will allow you to bypass the Atmega chip
// and connect the Ultimate GPS directly to the USB/Serial
// chip converter.
 
// Connect VIN to +5V
// Connect GND to Ground
// Connect GPS RX (data into GPS) to Digital 0
// Connect GPS TX (data out from GPS) to Digital 1
String monthDB[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char input = ' ';
float lat,lon = 0;

//Uncomment the lat and lon you'd like to route to
//New york <<<350 5th Ave New York NY 10118
//double targLat = 40.7481628;
//double targLon = -73.9849957;
//Chicago <<<Main Street, West Chicago, DuPage County, Illinois, 60185
double targLat = 41.8820006;
double targLon = -88.1995096;
//Texas <<<61 Riesner St, Houston, TX 77002
//double targLat = 29.7646147;
//double targLon = -95.3711651;
//Florida <<<2201 NE 163rd St, North Miami Beach, FL 33160
//double targLat = 25.9264193;
//double targLon = -80.1555557;
String instr = "";

float compass,knots = 0;
double targcomp = 0;
double compDif = 0;
void setup() {
  Serial1.begin(9600);
  Serial1.println("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
  Serial1.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
  Serial1.println("$PMTK251,9600*17");
  Serial1.println("$PMTK220,200*2C");
  Serial1.println("$PMTK300,200,0,0,0,0*2F");
  
  
  

  Serial.begin(9600);
}
void loop() {
  String thing,gmt,latstr,ns,lonstr,ew,knotstr,compstr,day,month,year = "";
  boolean fix = false;
  //input = Serial.read();
  //while(input != '!'){
   // input = Serial.read();
  //}
    if(Serial1.read() == '$'){
      thing = Serial1.readStringUntil('$');
      //Serial.print('$' + thing);
     //thing = Serial1.readStringUntil('*');
     //Serial.print('$' + thing + '*');
     //Serial.print(Serial1.read());
     //Serial.println(Serial1.read());
    }
    if(thing[3] == 'M') {
      char current[thing.length()];
      thing.toCharArray(current,thing.length());
      int i = 0;
      
      //skip the letters
      while(current[i] != ','){
        i++;
      }
      i++;

      //get Time
      gmt = current[i];
      i++;
      gmt += current[i];
      gmt += ':';
      i++;
      gmt += current[i];
      i++;
      gmt += current[i];
      while(current[i] != ','){
       i++;
      }
      i++;

      //Check Fixed or not
      if(current[i] == 'A'){
        fix = true;
      }
      i += 2;

      //get Latitude
      while(current[i] != ','){
        latstr += current[i];
        i++;
      }
      i++;

      //get North/South
      if(current[i] == 'N'){
        ns = "North";
      } else if(current[i] == 'S'){
        ns = "South";
      }
      i += 2;

      //get Longitude
      while(current[i] != ','){
        lonstr += current[i];
        i++;
      }
      i++;

      //get East/West
      if(current[i] == 'E'){
        ew = "East";
      } else if(current[i] == 'W'){
        ew = "West";
      }
      i += 2;

      //get knots
      while(current[i] != ','){
        knotstr += current[i];
        i++;
      }
      i++;

      //get compass direction
      while(current[i] != ','){
        compstr += current[i];
        i++;
      }
      i++;

      day += current[i];
      i++;
      day += current[i];
      i++;

      month += current[i];
      i++;
      month += current[i];
      i++;

      month = monthDB[month.toInt() - 1];

      year += current[i];
      i++;
      year += current[i];
      i++;
      
      //Testing Converting to Decimal Degrees
      
      //latstr = "3857.5634";
      //ns = "North";
      //lonstr = "9515.9289";
      //ew = "West";
      

      lat = latstr.toInt() / 100;
      lat += (latstr.toFloat() - lat*100) / 60;
      if(ns == "South"){
        lat *= -1;
      }

      lon = lonstr.toInt() / 100;
      lon += (lonstr.toFloat() - lon*100) / 60;
      if(ew == "West"){
        lon *= -1;
      }

      //Forcing Data to test without reads
      lat = 39.5060848;
      lon = -84.7278971;

      compass = compstr.toFloat();
      knots = knotstr.toFloat();

      /* Inverted Angles, keeping incase it becomes useful
      if(targLat > lat && targLon < lon){
        targcomp = atan((lon-targLon)/(targLat-lat))*(180/3.14);
      } else if(targLat < lat && targLon < lon){
        targcomp = (atan((lat-targLat)/(lon-targLon))*(180/3.14)) + 90;
      } else if(targLat < lat && targLon > lon){
        targcomp = (atan((targLon-lon)/(lat-targLat))*(180/3.14))  + 180;
      } else if(targLat > lat && targLon > lon){
        targcomp = (atan((targLat-lat)/(targLon-lon))*(180/3.14)) + 270;
      } else {
        targcomp = 0;
      }
      */

      //Determines the angle at which the user needs turn
      if(targLat > lat && targLon < lon){
        targcomp = atan((targLat-lat)/(lon-targLon))*(180/3.14) + 270;
      } else if(targLat < lat && targLon < lon){
        targcomp = (atan((lon-targLon)/(lat-targLat))*(180/3.14)) + 180;
      } else if(targLat < lat && targLon > lon){
        targcomp = (atan((lat-targLat)/(targLon-lon))*(180/3.14))  + 90;
      } else if(targLat > lat && targLon > lon){
        targcomp = (atan((targLon-lon)/(targLat-lat))*(180/3.14));
      } else {
        targcomp = 0;
      }

      //Forcing Data
      compass = 0;

      //Accounts for users current direction to prepare to calculate turn
      compDif = targcomp - compass;
      if(compDif < 0){
        compDif += 360;
      }

      //Tells user to turn according to current path
      if(compDif <= 5 || compDif >= 355){
        instr = "Forward";
      } else if(compDif > 5 && compDif <= 45){
        instr = "Slight Right";
      } else if(compDif > 45 && compDif <= 90){
        instr = "Right";
      } else if(compDif > 90 && compDif <= 270){
        instr = "Behind";
      } else if(compDif > 270 && compDif <= 315){
        instr = "Left";
      } else if(compDif > 315 && compDif < 355){
        instr = "Slight Left";
      } else {
        instr = "Error";
      }
      
      //Forcing Data
      fix = true;

      //Gives time,date,location,speed,and direction when user enters . to serial monitor
      //Important one is the last one, which gives directions
      if(Serial.read() == '.'){
        Serial.println("Time: " + gmt + " GMT");
        Serial.println(month + " " + day + ',' + " " + year);
        if(fix){
         Serial.print(lat,7);
         Serial.println(' ' + ns);
         Serial.print(lon,7);
         Serial.println(' ' + ew);
         Serial.print("Speed: ");
         Serial.print(knots,2);
         Serial.println(" Knots");
         Serial.print("Direction ");
         Serial.println(compass,4);
         Serial.println(targcomp,4);
         //Serial.println(lat,7);
         //Serial.println(lon,7);
         //Serial.println(targLat,7);
         //Serial.println(targLon,7);
         Serial.println(instr);
        } else {
          Serial.println("Could not find fix with Satelite");
        }
      }
      input = ' ';
     
    }
  }
